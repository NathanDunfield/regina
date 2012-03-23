
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  KDE User Interface                                                    *
 *                                                                        *
 *  Copyright (c) 1999-2011, Ben Burton                                   *
 *  For further details contact Ben Burton (bab@debian.org).              *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or         *
 *  modify it under the terms of the GNU General Public License as        *
 *  published by the Free Software Foundation; either version 2 of the    *
 *  License, or (at your option) any later version.                       *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful, but   *
 *  WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *  General Public License for more details.                              *
 *                                                                        *
 *  You should have received a copy of the GNU General Public             *
 *  License along with this program; if not, write to the Free            *
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,       *
 *  MA 02110-1301, USA.                                                   *
 *                                                                        *
 **************************************************************************/

/* end stub */

// Regina core includes:
#include "file/nxmlfile.h"
#include "packet/ncontainer.h"

// UI includes:
#include "reginafilter.h"
#include "packettreeview.h"
#include "packetui.h"
#include "reginamain.h"
#include "reginapart.h"
#include "reginasupport.h"

#include <QBoxLayout>
#include <QColor>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QTextDocument>
#include <QTreeView>
#include <QTreeWidget>
#include <QUrl>
#include <QWhatsThis>

ReginaPart::ReginaPart(ReginaMain *parent,
        const QStringList& /*args*/) :
        parent(parent), packetTree(0), 
        dockedPane(0), dirty(false),
        supportingDock(ReginaPrefSet::global().useDock) {

    // Set up our widgets and actions.
    setupWidgets();
    setupActions();
    // Send Packet Tree menu to parent
    parent->plugTreeMenu(treeMenu);

    // Initialise the packet tree.
    initPacketTree();

    // Other tidying up.
    setModified(false);
    updateTreeActions();

    connect(&ReginaPrefSet::global(), SIGNAL(preferencesChanged()),
        this, SLOT(updatePreferences()));
}

ReginaPart::~ReginaPart() {
    // Make an emergency closure of any remaining packet panes.
    QLinkedList<PacketPane*> panes = allPanes;
    QLinkedList<PacketPane*>::iterator i;
    for (i=panes.begin() ; i!=panes.end(); i++)
        delete *i;

    // Delete the visual tree before the underlying packets so that
    // we don't get a flood of change events.
    delete treeView;

    // Finish cleaning up.
    if (packetTree)
        delete packetTree;
    // Delete all actions
    allActions.clear();
}

void ReginaPart::setModified(bool modified) {
    dirty = modified;
    actSave->setEnabled(modified);
}

bool ReginaPart::closeUrl() {
    if (! closeAllPanes())
        return false;
    consoles.closeAllConsoles();

    while (dirty) {
        QMessageBox msgBox(QMessageBox::Information, tr("Information"),
            tr("The data file has been modified."),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            widget());
        msgBox.setInformativeText(tr("Do you want to save your changes?"));
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();

        if (ret == QMessageBox::Cancel)
            return false;
        if (ret == QMessageBox::Discard)
            break;

        fileSave();
    }

    return true;
}

void ReginaPart::ensureVisibleInTree(regina::NPacket* packet) {
    PacketTreeItem* item = treeView->find(packet);
    if (item)
        treeView->scrollToItem(item);
}

void ReginaPart::view(PacketPane* newPane) {
    // Decide whether to dock or float.
    bool shouldDock;

    if (supportingDock) {
        if (dockedPane) {
            shouldDock = ! dockedPane->isDirty();
        } else
            shouldDock = true;
    } else
        shouldDock = false;

    // Display the new pane.
    if (shouldDock) {
        dock(newPane);
        newPane->setFocus();
    } else
        newPane->floatPane();

    // Add it to the list of currently managed panes.
    allPanes.append(newPane);
}

void ReginaPart::dock(PacketPane* newPane) {
    // Get rid of the currently docked pane by whatever means necessary.
    if (! closeDockedPane())
        dockedPane->floatPane();

    newPane->setParent(dockArea);
    static_cast<QBoxLayout*>(dockArea->layout())->addWidget(newPane, 1);
    dockedPane = newPane;
    
    parent->plugPacketMenu(newPane->createPacketTypeMenu(true));

    newPane->registerEditOperations(actCut, actCopy, actPaste);

    // Don't plug the full KTextEditor::View GUI; there's way too much stuff
    // that we don't want (like Save and Save-As, for instance).
    // factory()->addClient(doc->views().front());
}

void ReginaPart::isClosing(PacketPane* closingPane) {
    parent->unplugPacketMenu();
    allPanes.removeAll(closingPane);
}

void ReginaPart::aboutToUndock(PacketPane* undockedPane) {
    parent->unplugPacketMenu();
    undockedPane->deregisterEditOperations();

    if (dockedPane == undockedPane) {
        dockedPane = 0;
    }
}

bool ReginaPart::initData(regina::NPacket* usePacketTree,
        const QString& useLocalFilename,
        const QString& useDisplayName) {
    if (packetTree) {
        delete packetTree;
        setModified(false);
    }

    localFile = useLocalFilename;
    displayName = useDisplayName;
    packetTree = usePacketTree;

    if (packetTree) {
        treeView->fill(packetTree);
        // Expand the first level.
        if (treeView->invisibleRootItem()->child(0)->child(0))
            treeView->scrollToItem(
                treeView->invisibleRootItem()->child(0)->child(0));

        if (! displayName.isEmpty())
            parent->setWindowTitle(displayName);
        else
            parent->setWindowTitle(localFile);

        if (! localFile.isEmpty())
            parent->addRecentFile();

        return true;
    } else {
        initPacketTree();
        return false;
    }
}

bool ReginaPart::saveFile() {
    // Does the user have some work that still needs to be committed?
    while (hasUncommittedChanges()) {
        QMessageBox msgBox(QMessageBox::Information, tr("Information"),
            tr("Some of your packets have changes that are "
            "not yet committed."));
        msgBox.setInformativeText("<qt>Uncommitted changes will "
            "not be saved to file.<p>"
            "Do you wish to commit all of your changes now?</qt>");
        QPushButton* commit = msgBox.addButton(tr("Commit All"),
            QMessageBox::AcceptRole);
        QPushButton* discard = msgBox.addButton(tr("Discard All"),
            QMessageBox::DestructiveRole);
        QPushButton* cancel = msgBox.addButton(tr("Cancel"),
            QMessageBox::RejectRole);
        msgBox.setDefaultButton(commit);
        msgBox.exec();

        if (msgBox.clickedButton() == commit)
            commitAllChanges();
        else if (msgBox.clickedButton() == discard)
            discardAllChanges();
        else
            return false;
    }

    if (regina::writeXMLFile(static_cast<const char*>(
            QFile::encodeName(localFile)), packetTree)) {
        setModified(false);
        return true;
    } else {
        ReginaSupport::warn(widget(),
            tr("<qt>I could not save the data file <tt>%1</tt>.</qt>").
                arg(Qt::escape(localFile)),
            tr("Please check that you have permissions to write "
                "to this file."));
        return false;
    }
}

void ReginaPart::fileSave() {
    if (localFile.isEmpty())
        fileSaveAs();
    else
        saveFile();
}

void ReginaPart::fileSaveAs() {
    // Pass parent, not widget(), to QFileDialog::getSaveFileName.
    // For some reason, on MacOS the file dialog disappears immediately
    // if widget() is used [20 Feb 2012].
    QString file = QFileDialog::getSaveFileName(parent,
        tr("Save Data File"), QString(), tr(FILTER_REGINA));

    if (file.isEmpty())
        return;

    // Do we need to add an extension?
    // Qt seems to handle this for us automatically.
    /*
    if (QFileInfo(file).suffix().isEmpty())
        file += ReginaAbout::regDataExt;
    */

    // Does this file already exist?
    // Don't warn the user; Qt seems to do this for us.
    /*
    if (QFileInfo(file).exists()) {
        QMessageBox msgBox(QMessageBox::Information,
            tr("Information"),
            tr("A file with this name already exists."),
            QMessageBox::Save | QMessageBox::Cancel,
            widget());
        msgBox.setInformativeText(tr("Are you sure you wish to "
            "overwrite it?"));
        msgBox.setDefaultButton(QMessageBox::Cancel);
        if (msgBox.exec() != QMessageBox::Save)
            return;
    }
    */

    // Go ahead and save it.
    if (localFile != file) {
        localFile = file;
        parent->setWindowTitle(localFile);
    }

    saveFile();

    parent->addRecentFile();
}

void ReginaPart::packetView(regina::NPacket* packet, bool makeVisibleInTree,
        bool selectInTree) {
    view(new PacketPane(this, packet));

    if (makeVisibleInTree || selectInTree) {
        PacketTreeItem* item = treeView->find(packet);
        if (! item) {
            // We cannot find the item in the tree.
            // Perhaps this is because the packet was just created and
            // the tree has not been refreshed yet?
            // Force a refresh now and try again.
            if (packet->getTreeParent()) {
                PacketTreeItem* parent =
                    treeView->find(packet->getTreeParent());
                if (parent) {
                    parent->refreshSubtree();
                    item = treeView->find(packet);
                }
            }
        }
        if (item) {
            if (makeVisibleInTree)
                treeView->scrollToItem(item);
            if (selectInTree)
                treeView->setCurrentItem(item);
        }
    }
}

void ReginaPart::packetView() {
    regina::NPacket* packet = checkPacketSelected();
    if (packet)
        packetView(packet, false);
}

void ReginaPart::packetRename() {
    regina::NPacket* packet = checkPacketSelected();
    if (! packet)
        return;

    bool ok;
    QString suggest = packet->getPacketLabel().c_str();
    while (true) {
        QString newLabel = QInputDialog::getText(widget(),
            tr("Rename Packet"), tr("New label:"), QLineEdit::Normal,
                suggest, &ok).trimmed();
        if ((! ok) || (newLabel == packet->getPacketLabel().c_str()))
            return;

        // Has this label already been used?
        if (packetTree->findPacketLabel(newLabel.toAscii().constData())) {
            ReginaSupport::info(widget(),
                tr("Another packet is already using this label."),
                tr("Each packet in your data file must have its own "
                    "unique label.  I will suggest a different label "
                    "that is not in use."));
            suggest = packetTree->makeUniqueLabel(
                newLabel.toAscii().constData()).c_str();
        } else {
            // It's a unique label; we can rename it!
            packet->setPacketLabel(newLabel.toAscii().constData());
            return;
        }
    }
}

void ReginaPart::packetDelete() {
    regina::NPacket* packet = checkPacketSelected();
    if (! packet)
        return;
    if (! packet->getTreeParent()) {
        ReginaSupport::info(widget(),
            tr("You cannot delete the root of the packet tree."),
            tr("You may delete any other packet except for this one."));
        return;
    }

    QMessageBox msgBox(widget());
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(tr("Warning"));
    QPushButton* delBtn;
    if (packet->getFirstTreeChild()) {
        msgBox.setText(tr("<qt>You are about to delete the packet <i>%1</i> "
            "and all of its children.</qt>").
            arg(Qt::escape(packet->getPacketLabel().c_str())));
        delBtn = msgBox.addButton(tr("Delete All"),
            QMessageBox::AcceptRole);
    } else {
        msgBox.setText(tr("<qt>You are about to delete the packet "
            "<i>%1</i>.</qt>").
            arg(Qt::escape(packet->getPacketLabel().c_str())));
        delBtn = msgBox.addButton(tr("Delete"),
            QMessageBox::AcceptRole);
    }
    msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
    msgBox.setDefaultButton(delBtn);
    msgBox.setInformativeText("Are you sure?");
    msgBox.exec();
    if (msgBox.clickedButton() != delBtn)
        return;

    delete packet;
}

void ReginaPart::subtreeRefresh() {
    if (! checkSubtreeSelected())
        return;

    // Refresh the tree itself.
    PacketTreeItem* item = dynamic_cast<PacketTreeItem*>(
        treeView->selectedItems().first());
    item->refreshSubtree();

    // Refresh any relevant packet panes.
    regina::NPacket* subtree = item->getPacket();
    for (QLinkedList<PacketPane *>::iterator it = allPanes.begin();
            it != allPanes.end(); it++) {
        PacketPane * pane = (*it);
        if (subtree->isGrandparentOf(pane->getPacket()))
            pane->refresh();
    }
}

void ReginaPart::clonePacket() {
    regina::NPacket* packet = checkPacketSelected();
    if (! packet)
        return;
    if (! packet->getTreeParent()) {
        ReginaSupport::info(widget(),
            tr("You cannot clone the top-level packet in the tree."),
            tr("Any other packet in the tree may be cloned except "
            "for this one."));
        return;
    }

    regina::NPacket* ans = packet->clone(false, false);

    treeView->selectPacket(ans, true);
    packetView(ans, false);
}

void ReginaPart::cloneSubtree() {
    regina::NPacket* packet = checkSubtreeSelected();
    if (! packet)
        return;
    if (! packet->getTreeParent()) {
        ReginaSupport::info(widget(),
            tr("You cannot clone the entire tree."),
            tr("Any subtree may be cloned, however."));
        return;
    }

    regina::NPacket* ans = packet->clone(true, false);

    treeView->selectPacket(ans, true);
    packetView(ans, false);
}

void ReginaPart::newCensus() {
    ReginaSupport::info(widget(),
        tr("Census creation is only available from the command line."), 
        tr("<qt>See the command-line program <i>tricensus</i>, which "
        "supports a rich set of features and is suitable for "
        "long census calculations.</qt>"));
}

void ReginaPart::pythonConsole() {
    consoles.launchPythonConsole(widget(), packetTree,
        treeView->selectedPacket());
}

void ReginaPart::floatDockedPane() {
    // Delegate the entire procedure to PacketPane::floatPane().
    // Processing will return to this class when PacketPane calls
    // ReginaPart::aboutToUndock().
    if (dockedPane)
        dockedPane->floatPane();
}

bool ReginaPart::closeDockedPane() {
    // Is there anything to close?
    if (! dockedPane)
        return true;

    // Are we allowed to close it?
    if (! dockedPane->queryClose())
        return false;

    // Close it.  Note that queryClose() has already done the
    // deregistration for us.
    PacketPane* closedPane = dockedPane;
    aboutToUndock(dockedPane);

    // At this point dockedPane is already 0.
    delete closedPane;
    return true;
}

bool ReginaPart::closeAllPanes() {
    // Copy the list since the original list will be modified as panes
    // are closed.
    QLinkedList<PacketPane *> panes = allPanes;

    // Try to close each pane in return, returning false if a pane
    // refuses.
    for (QLinkedList<PacketPane *>::iterator it = panes.begin(); 
            it != panes.end() ; it++) {
        if (! (*it)->close())
            return false;
    }

    return true;
}

bool ReginaPart::hasUncommittedChanges() {
    QLinkedList<PacketPane *> panes = allPanes;
    for (QLinkedList<PacketPane *>::iterator it = panes.begin(); 
            it != panes.end() ; it++) {
        if ((*it)->isDirty())
            return true;
    }
    return false;
}

void ReginaPart::commitAllChanges() {
    QLinkedList<PacketPane *> panes = allPanes;
    for (QLinkedList<PacketPane *>::iterator it = panes.begin(); 
            it != panes.end() ; it++) {
        if ((*it)->isDirty())
            (*it)->commit();
    }
}

void ReginaPart::discardAllChanges() {
    QLinkedList<PacketPane *> panes = allPanes;
    for (QLinkedList<PacketPane *>::iterator it = panes.begin(); 
            it != panes.end() ; it++) {
        if ((*it)->isDirty())
            (*it)->refreshForce();
    }
}

void ReginaPart::updateTreeActions() {
    QLinkedList<QAction *>::iterator it;
    for (it = treeGeneralEditActions.begin(); 
            it != treeGeneralEditActions.end(); it++)
        (*it)->setEnabled(true);

    bool enable = ! treeView->selectedItems().isEmpty(); 

    for (it = treePacketViewActions.begin(); 
            it != treePacketViewActions.end(); it++)
        (*it)->setEnabled(enable);

    for (it = treePacketEditActions.begin(); 
            it != treePacketEditActions.end(); it++)
        (*it)->setEnabled(enable);
}

void ReginaPart::setupWidgets() {
    splitter = new QSplitter();
    splitter->setWhatsThis(tr(
        "<qt>Each piece of information stored in a data file "
        "is a <i>packet</i>: this include triangulations, normal surface "
        "lists, text items and so on.<p>"
        "Packets within a data file are arranged in a tree-like structure, "
        "which you should see on the left-hand side of the window.  "
        "If you click on a packet in the tree, it will open up in the "
        "right-hand side of the window where you can edit it or view "
        "detailed information.</qt>"));

    // Set up the packet tree viewer.
    treeView = new PacketTreeView(this, splitter);
    treeView->setSizePolicy(QSizePolicy(
        QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    // Leave the stretch factors at the default of zero.
    connect(treeView, SIGNAL(itemSelectionChanged()), this,
        SLOT(updateTreeActions()));

    // Make sure the tree area doesn't shrink too far.
    // Removed this for the KDE4 port, things seem fine without it.
    // treeLayout->addStrut(150);

    // Set up the docking area.
    dockArea = new QWidget(splitter);
    QBoxLayout* dockLayout = new QVBoxLayout(dockArea);
    dockLayout->setContentsMargins(0, 0, 0, 0);
    if (! supportingDock)
        dockArea->hide();

    QSizePolicy qpol(
        QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    qpol.setHorizontalStretch(5);
    qpol.setVerticalStretch(5);
    dockArea->setSizePolicy(qpol);

    // Make sure the docking area gets some space even when there's
    // nothing in it.
    dockLayout->addStrut(100);
}

void ReginaPart::initPacketTree() {
    if (packetTree)
        delete packetTree;
    packetTree = new regina::NContainer();
    packetTree->setPacketLabel(tr("Container").toAscii().constData());

    // Update the visual representation.
    treeView->fill(packetTree);

    parent->setWindowTitle(tr("Untitled"));
}

regina::NPacket* ReginaPart::checkPacketSelected() {
    regina::NPacket* p = treeView->selectedPacket();
    if (p)
        return p;
    ReginaSupport::info(widget(),
        tr("Please select a packet to work with."));
    return 0;
}

regina::NPacket* ReginaPart::checkSubtreeSelected() {
    regina::NPacket* p = treeView->selectedPacket();
    if (p)
        return p;
    ReginaSupport::info(widget(),
        tr("Please select a packet to work with."));
        // Remove all the information about subtrees; it's clear anyway.
    return 0;
}

const QUrl ReginaPart::url() {
    return QUrl::fromLocalFile(localFile);
}

void ReginaPart::updatePreferences() {
    if (ReginaPrefSet::global().useDock == supportingDock)
        return;

    supportingDock = ReginaPrefSet::global().useDock;

    if (! supportingDock)
        floatDockedPane();

    dockArea->setVisible(supportingDock);

    for (QLinkedList<PacketPane *>::iterator it = allPanes.begin();
            it != allPanes.end(); it++)
        (*it)->supportDock(supportingDock);
}

