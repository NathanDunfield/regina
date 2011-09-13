
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
#include "regina-config.h"
#include "packet/npacket.h"

// UI includes:
#include "eventids.h"
#include "flatbutton.h"
#include "packeteditiface.h"
#include "packetmanager.h"
#include "packetui.h"
#include "packetwindow.h"
#include "reginapart.h"

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstdguiitem.h>
#include <ktoolbar.h>
#include <qboxlayout.h>
#include <qevent.h>
#include <qlabel.h>
#include <qlinkedlist.h>
#include <qtreewidget.h>
#include <qwhatsthis.h>

using regina::NPacket;

QLinkedList<KAction*> PacketUI::noActions;

PacketHeader::PacketHeader(NPacket* pkt, QWidget* parent) 
        : QFrame(parent), packet(pkt) {
    QBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    icon = new QLabel();
    icon->setPixmap(PacketManager::iconSmall(packet, true));
    icon->setMargin(2); // Leave *some* space, however tiny.
    layout->addWidget(icon);

    title = new QLabel(packet->getFullName().c_str());
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title, 1);

    setFrameStyle(QFrame::Box | QFrame::Sunken);
    // setMidLineWidth(1);
}

void PacketHeader::refresh() {
    title->setText(packet->getFullName().c_str());
    icon->setPixmap(PacketManager::iconSmall(packet, true));
}

ErrorPacketUI::ErrorPacketUI(regina::NPacket* newPacket,
        PacketPane* newEnclosingPane, const QString& errorMessage) :
        PacketReadOnlyUI(newEnclosingPane), packet(newPacket) {
    QString msg = errorMessage;
    msg += "\n\nPlease mail\n";
    msg += PACKAGE_BUGREPORT;
    msg += "\nfor further assistance.";

    label = new QLabel(msg, 0);
    label->setAlignment(Qt::AlignCenter);
}

regina::NPacket* ErrorPacketUI::getPacket() {
    return packet;
}

QWidget* ErrorPacketUI::getInterface() {
    return label;
}

QString ErrorPacketUI::getPacketMenuText() const {
    return i18n("&Unknown Packet");
}

void ErrorPacketUI::refresh() {
}

DefaultPacketUI::DefaultPacketUI(regina::NPacket* newPacket,
        PacketPane* newEnclosingPane) :
        ErrorPacketUI(newPacket, newEnclosingPane,
        i18n("Packets of type %1\nare not yet supported.").arg(
        newPacket->getPacketTypeName().c_str())) {
}

PacketPane::PacketPane(ReginaPart* newPart, NPacket* newPacket,
        QWidget* parent) : QWidget(parent),
        part(newPart), frame(0), dirty(false), dirtinessBroken(false),
        emergencyClosure(false), emergencyRefresh(false), isCommitting(false),
        editCut(0), editCopy(0), editPaste(0) {
    // Initialise a vertical layout with no padding or spacing.
    QBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Should we allow both read and write?
    readWrite = part->isReadWrite() && newPacket->isPacketEditable();

    // Create the actions first, since PacketManager::createUI()
    // might want to modify them.
    actCommit = part->actionCollection()->addAction("packet_editor_commit");
    actCommit->setText(i18n("Co&mmit"));
    actCommit->setIcon(KIcon("dialog-ok"));
    actCommit->setEnabled(false);
    actCommit->setToolTip(i18n("Commit changes to this packet"));
    actCommit->setWhatsThis(i18n("Commit any changes you have made inside "
        "this packet viewer.  Changes you make will have no effect elsewhere "
        "until they are committed."));
    connect(actCommit,SIGNAL(triggered()),this,SLOT(commit()));
    actRefresh = part->actionCollection()->addAction("packet_editor_refresh");
    actRefresh->setText(i18n("&Refresh"));
    actRefresh->setIcon(KIcon("view-refresh"));
    actRefresh->setToolTip(i18n("Discard any changes and refresh this "
        "packet viewer"));
    actRefresh->setWhatsThis(i18n("Refresh this viewer to show the most "
        "recent state of the packet.  Any changes you mave made inside this "
        "viewer that have not been committed will be discarded."));
    connect(actRefresh,SIGNAL(triggered()), this, SLOT(refresh()));
    actDockUndock = part->actionCollection()->addAction("packet_editor_dock");
    actDockUndock->setText(i18n("Un&dock"));
    actDockUndock->setIcon(KIcon("mail-attachment"));
    actDockUndock->setToolTip(i18n("Dock / undock this packet viewer"));
    actDockUndock->setWhatsThis(i18n("Dock or undock this packet viewer.  "
        "A docked viewer sits within the main window, to the right of "
        "the packet tree.  An undocked viewer floats in its own window."));
    connect(actDockUndock,SIGNAL(triggered()),this, SLOT(floatPane()));
    actClose = part->actionCollection()->addAction("packet_editor_close");
    actClose->setText(i18n("&Close"));
    actClose->setIcon(KIcon("window-close"));
    actClose->setToolTip(i18n("Close this packet viewer"));
    actClose->setWhatsThis(i18n("Close this packet viewer.  Any changes "
        "that have not been committed will be discarded."));
    connect(actClose,SIGNAL(triggered()), this, SLOT(close()));

    // Set up the header and dock/undock button.
    QBoxLayout* headerBox = new QHBoxLayout();
    headerBox->setSpacing(0);

    header = new PacketHeader(newPacket);
    header->setWhatsThis(i18n("This shows the label of the packet "
        "being viewed, as well as its packet type."));
    headerBox->addWidget(header, 1);

    dockUndockBtn = new FlatToolButton();
    dockUndockBtn->setCheckable(true);
    dockUndockBtn->setIcon(KIcon("mail-attachment"));
    dockUndockBtn->setText(i18n("Dock or undock this packet viewer"));
    dockUndockBtn->setChecked(true);
    dockUndockBtn->setWhatsThis(i18n("Dock or undock this packet viewer.  "
        "A docked viewer sits within the main window, to the right of "
        "the packet tree.  An undocked viewer floats in its own window."));
    headerBox->addWidget(dockUndockBtn);
    connect(dockUndockBtn, SIGNAL(toggled(bool)), this, SLOT(floatPane()));

    layout->addLayout(headerBox);

    // Set up the main interface component.
    mainUI = PacketManager::createUI(newPacket, this);
    QWidget* mainUIWidget = mainUI->getInterface();
    layout->addWidget(mainUIWidget, 1);
    setFocusProxy(mainUIWidget);

    // Set up the footer buttons and other actions.
    KToolBar* footer = new KToolBar(false, true);
    footer->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    footer->addAction(actCommit);
    footer->addAction(actRefresh);
    footer->addSeparator();
    footer->addAction(actClose);
    layout->addWidget(footer);

    // Set up the packet type menu.
    packetTypeMenu = new KActionMenu(mainUI->getPacketMenuText(), this);

    const QLinkedList<KAction*>& packetTypeActions(
        mainUI->getPacketTypeActions());
    if (! packetTypeActions.isEmpty()) {
        for (QLinkedListIterator<KAction*> it(packetTypeActions) ;
                it.hasNext(); ) {
            packetTypeMenu->addAction( it.next() );  
        }
        packetTypeMenu->addSeparator();
    }

    packetTypeMenu->addAction(actCommit);
    packetTypeMenu->addAction(actRefresh);
    packetTypeMenu->addSeparator();
    packetTypeMenu->addAction(actDockUndock);
    packetTypeMenu->addAction(actClose);

    // Register ourselves to listen for various events.
    newPacket->listen(this);
}

PacketPane::~PacketPane() {
    delete mainUI;
    delete actCommit;
    delete actRefresh;
    delete actDockUndock;
    delete actClose;
    delete packetTypeMenu;
}

void PacketPane::setDirty(bool newDirty) {
    if (dirtinessBroken || dirty == newDirty)
        return;

    dirty = newDirty;

    actCommit->setEnabled(dirty);
    actRefresh->setText(dirty ? i18n("&Discard") : i18n("&Refresh"));
    actRefresh->setIcon(dirty ? KIcon("dialog-cancel") : KIcon("view-refresh"));
}

void PacketPane::setDirtinessBroken() {
    dirtinessBroken = true;
    dirty = readWrite;

    actCommit->setEnabled(dirty);
    actRefresh->setText(dirty ? i18n("&Discard / Refresh") : i18n("&Refresh"));
    actRefresh->setIcon(KIcon("view-refresh"));
}

bool PacketPane::setReadWrite(bool allowReadWrite) {
    if (allowReadWrite)
        if (! (mainUI->getPacket()->isPacketEditable() && part->isReadWrite()))
            return false;

    if (readWrite == allowReadWrite)
        return true;

    // We are changing the status and we are allowed to.
    readWrite = allowReadWrite;

    mainUI->setReadWrite(allowReadWrite);
    updateClipboardActions();
    if (dirtinessBroken) {
        // Update the UI according to the new value of readWrite.
        setDirtinessBroken();
    }

    emit readWriteStatusChanged(readWrite);

    return true;
}

bool PacketPane::queryClose() {
    if ((! emergencyClosure) && dirty) {
        QString msg = (dirtinessBroken ?
            i18n("This packet might contain changes that have not yet been "
                 "committed.  Are you sure you wish to close this packet "
                 "now and discard these changes?") :
            i18n("This packet contains changes that have not yet been "
                 "committed.  Are you sure you wish to close this packet "
                 "now and discard these changes?"));
        if (KMessageBox::warningContinueCancel(this, msg,
                mainUI->getPacket()->getPacketLabel().c_str(),
                KStandardGuiItem::discard()) == KMessageBox::Cancel)
            return false;
    }

    // We are definitely going to close the pane.  Do some cleaning up.
    part->isClosing(this);
    return true;
}

void PacketPane::registerEditOperations(KAction* actCut, KAction* actCopy,
        KAction* actPaste) {
    deregisterEditOperations();

    editCut = actCut;
    editCopy = actCopy;
    editPaste = actPaste;

    PacketEditIface* iface = mainUI->getEditIface();
    if (iface) {
        connect(iface, SIGNAL(statesChanged()), this,
            SLOT(updateClipboardActions()));

        if (editCut)
            connect(editCut, SIGNAL(triggered()), iface, SLOT(cut()));
        if (editCopy)
            connect(editCopy, SIGNAL(triggered()), iface, SLOT(copy()));
        if (editPaste)
            connect(editPaste, SIGNAL(triggered()), iface, SLOT(paste()));
    }

    updateClipboardActions();
}

void PacketPane::deregisterEditOperations() {
    PacketEditIface* iface = mainUI->getEditIface();
    if (iface) {
        disconnect(iface, SIGNAL(statesChanged()), this,
            SLOT(updateClipboardActions()));

        if (editCut)
            disconnect(editCut, SIGNAL(triggered()), iface, SLOT(cut()));
        if (editCopy)
            disconnect(editCopy, SIGNAL(triggered()), iface, SLOT(copy()));
        if (editPaste)
            disconnect(editPaste, SIGNAL(triggered()), iface, SLOT(paste()));
    }

    if (editCut) {
        editCut->setEnabled(false);
        editCut = 0;
    }
    if (editCopy) {
        editCopy->setEnabled(false);
        editCopy = 0;
    }
    if (editPaste) {
        editPaste->setEnabled(false);
        editPaste = 0;
    }
}

void PacketPane::packetWasChanged(regina::NPacket*) {
    // Assume it's this packet.

    // Ignore this if we're responsible for the event.
    if (isCommitting)
        return;

    header->refresh();

    if (dirty) {
        QString msg = (dirtinessBroken ?
            i18n("This packet has been changed from within a script or "
                 "another interface.  However, this interface might contain "
                 "changes that have not yet been committed.  Do you wish "
                 "to refresh this interface to reflect the changes "
                 "that have been made elsewhere?") :
            i18n("This packet has been changed from within a script or "
                 "another interface.  However, this interface contains "
                 "changes that have not yet been committed.  Do you wish "
                 "to refresh this interface to reflect the changes "
                 "that have been made elsewhere?"));
        if (KMessageBox::warningYesNo(this, msg,
                mainUI->getPacket()->getPacketLabel().c_str()) ==
                KMessageBox::No)
            return;
    }

    mainUI->refresh();
    setDirty(false); // Just in case somebody forgot.
}

void PacketPane::packetWasRenamed(regina::NPacket*) {
    // Assume it's this packet.
    header->refresh();
}

void PacketPane::packetToBeDestroyed(regina::NPacket*) {
    // Assume it's this packet.
    closeForce();
}

void PacketPane::childWasAdded(regina::NPacket* packet, regina::NPacket*) {
    // Assume it's this packet.
    // Watch out though.  We may not be in the GUI thread.
    // Better do it all through Qt events.
    if (packet->isPacketEditable() != readWrite)
        QApplication::postEvent(this, new QEvent(
            readWrite ? (QEvent::Type)EVT_PANE_SET_READONLY : (QEvent::Type)EVT_PANE_SET_READWRITE));
    QApplication::postEvent(this, new QEvent((QEvent::Type)EVT_REFRESH_HEADER));
}

void PacketPane::childWasRemoved(regina::NPacket* packet, regina::NPacket*,
        bool inParentDestructor) {
    // Assume it's this packet.
    if (packet->isPacketEditable() != readWrite)
        setReadWrite(!readWrite);
    if (! inParentDestructor)
        header->refresh();
}

void PacketPane::refresh() {
    header->refresh();

    if ((! emergencyRefresh) && dirty) {
        QString msg = (dirtinessBroken ?
            i18n("This packet might contain changes that have not yet been "
                 "committed.  Are you sure you wish to discard these "
                 "changes?") :
            i18n("This packet contains changes that have not yet been "
                 "committed.  Are you sure you wish to discard these "
                 "changes?"));
        if (KMessageBox::warningContinueCancel(this, msg,
                mainUI->getPacket()->getPacketLabel().c_str(),
                KStandardGuiItem::discard()) != KMessageBox::Continue)
            return;
    }

    emergencyRefresh = false;
    mainUI->refresh();
    setDirty(false); // Just in case somebody forgot.
}

void PacketPane::refreshForce() {
    emergencyRefresh = true;
    refresh();
}

bool PacketPane::commit() {
    if (dirty) {
        if (! mainUI->getPacket()->isPacketEditable()) {
            KMessageBox::sorry(this, i18n("<qt>This packet may not be "
                "edited at the present time.  Because of this, your "
                "changes cannot be committed.<p>"
                "This is generally due to a tight relationship shared "
                "with some other packet in the tree.  For instance, a "
                "triangulation containing a normal surface list may "
                "not be edited, since the normal surfaces are stored "
                "as coordinates relative to the triangulation.<p>"
                "As a workaround for this problem, you might wish to try "
                "cloning this packet and editing the clone instead.</qt>"));
            return false;
        }

        if (! readWrite) {
            KMessageBox::sorry(this, i18n("This packet is read-only.  "
                "No changes may be committed."));
            return false;
        }

        isCommitting = true;

        {
            NPacket::ChangeEventSpan span(mainUI->getPacket());
            mainUI->commit();
        }

        setDirty(false); // Just in case somebody forgot.
        isCommitting = false;
    }

    return true;
}

bool PacketPane::commitToModify() {
    if (! mainUI->getPacket()->isPacketEditable()) {
        KMessageBox::sorry(this, i18n("<qt>This packet may not be "
            "modified at the present time.<p>"
            "This is generally due to a tight relationship shared "
            "with some other packet in the tree.  For instance, a "
            "triangulation containing a normal surface list may "
            "not be edited, since the normal surfaces are stored "
            "as coordinates relative to the triangulation.<p>"
            "As a workaround for this problem, you might wish to try "
            "cloning this packet and editing the clone instead.</qt>"));
        return false;
    }

    if (! readWrite) {
        KMessageBox::sorry(this, i18n("This packet is read-only, and "
            "so may not be modified."));
        return false;
    }

    return commit();
}

bool PacketPane::tryCommit() {
    if (dirty) {
        if (! mainUI->getPacket()->isPacketEditable()) {
            if (KMessageBox::warningContinueCancel(this,
                    i18n("<qt>This packet may not be edited at the present "
                    "time.  Because of this I cannot commit your recent "
                    "changes, and I will have to work from an old copy "
                    "of the packet instead.<p>"
                    "This is generally due to a tight relationship shared "
                    "with some other packet in the tree.  For instance, a "
                    "triangulation containing a normal surface list may "
                    "not be edited, since the normal surfaces are stored "
                    "as coordinates relative to the triangulation.<p>"
                    "Do you wish to continue this operation using an old "
                    "copy of the packet?</qt>"))
                    != KMessageBox::Continue)
                return false;
        } else if (! readWrite) {
            if (KMessageBox::warningContinueCancel(this,
                    i18n("<qt>This packet is read-only, but you appear "
                    "to have made changes that have not yet been committed.  "
                    "I cannot commit these changes for you, and so I will "
                    "have to work from an old copy of the packet instead.<p>"
                    "Do you wish to continue this operation using an old "
                    "copy of the packet?</qt>"))
                    != KMessageBox::Continue)
                return false;
        } else {
            isCommitting = true;

            {
                NPacket::ChangeEventSpan span(mainUI->getPacket());
                mainUI->commit();
            }

            setDirty(false); // Just in case somebody forgot.
            isCommitting = false;
        }
    }

    return true;
}

bool PacketPane::close() {
    // Let whoever owns us handle the entire close event.
    // We'll come back to this class when they call queryClose().
    if (frame)
        return frame->close();
    else
        return part->closeDockedPane();
}

void PacketPane::closeForce() {
    emergencyClosure = true;
    close();
}

void PacketPane::dockPane() {
    if (! frame)
        return;

    // The packet pane is currently floating.
    // Be sure to unplug the action list so it doesn't get destroyed.
    frame->unplugActionList("packet_type_menu");
    part->dock(this);
    delete frame;
    frame = 0;

    dockUndockBtn->setChecked(true);
    actDockUndock->setText(i18n("Un&dock"));
    disconnect(dockUndockBtn, SIGNAL(toggled(bool)), this, SLOT(dockPane()));
    connect(dockUndockBtn, SIGNAL(toggled(bool)), this, SLOT(floatPane()));
    disconnect(actDockUndock, SIGNAL(triggered()), this, SLOT(dockPane()));
    connect(actDockUndock, SIGNAL(triggered()), this, SLOT(floatPane()));
}

void PacketPane::floatPane() {
    if (frame)
        return;

    // The packet pane is currently docked.
    part->aboutToUndock(this);
    frame = new PacketWindow(this);

    dockUndockBtn->setChecked(false);
    actDockUndock->setText(i18n("&Dock"));
    disconnect(dockUndockBtn, SIGNAL(toggled(bool)), this, SLOT(floatPane()));
    connect(dockUndockBtn, SIGNAL(toggled(bool)), this, SLOT(dockPane()));
    disconnect(actDockUndock, SIGNAL(triggered()), this, SLOT(floatPane()));
    connect(actDockUndock, SIGNAL(triggered()), this, SLOT(dockPane()));

    frame->show();
}

void PacketPane::updateClipboardActions() {
    PacketEditIface* iface = mainUI->getEditIface();

    if (editCut)
        editCut->setEnabled(iface ? iface->cutEnabled() : false);
    if (editCopy)
        editCopy->setEnabled(iface ? iface->copyEnabled() : false);
    if (editPaste)
        editPaste->setEnabled(iface ? iface->pasteEnabled() : false);
}

void PacketPane::customEvent(QEvent* evt) {
    switch (evt->type()) {
        case EVT_PANE_SET_READONLY:
            setReadWrite(false); break;
        case EVT_PANE_SET_READWRITE:
            setReadWrite(true); break;
        case EVT_REFRESH_HEADER:
            header->refresh(); break;
        default:
            break;
    }
}

