
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  KDE User Interface                                                    *
 *                                                                        *
 *  Copyright (c) 1999-2013, Ben Burton                                   *
 *  For further details contact Ben Burton (bab@debian.org).              *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or         *
 *  modify it under the terms of the GNU General Public License as        *
 *  published by the Free Software Foundation; either version 2 of the    *
 *  License, or (at your option) any later version.                       *
 *                                                                        *
 *  As an exception, when this program is distributed through (i) the     *
 *  App Store by Apple Inc.; (ii) the Mac App Store by Apple Inc.; or     *
 *  (iii) Google Play by Google Inc., then that store may impose any      *
 *  digital rights management, device limits and/or redistribution        *
 *  restrictions that are required by its terms of service.               *
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
#include "packeteditiface.h"
#include "packetmanager.h"
#include "packetui.h"
#include "packetwindow.h"
#include "reginamain.h"
#include "reginaprefset.h"
#include "reginasupport.h"

#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QEvent>
#include <QFrame>
#include <QLabel>
#include <QLinkedList>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QToolBar>
#include <QToolButton>
#include <QTreeWidget>
#include <QWhatsThis>

using regina::NPacket;

// Hard-code the header size for now.
namespace {
    const int headerSize = 22;
};

QLinkedList<QAction*> PacketUI::noActions;

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
    return QObject::tr("&Unknown Packet");
}

void ErrorPacketUI::refresh() {
}

DefaultPacketUI::DefaultPacketUI(regina::NPacket* newPacket,
        PacketPane* newEnclosingPane) :
        ErrorPacketUI(newPacket, newEnclosingPane,
        newEnclosingPane->tr("Packets of type %1\nare not yet supported.").arg(
        newPacket->getPacketTypeName().c_str())) {
}

PacketPane::PacketPane(ReginaMain* newMainWindow, NPacket* newPacket,
        QWidget* parent) : QWidget(parent),
        mainWindow(newMainWindow), frame(0),
        editCut(0), editCopy(0), editPaste(0) {
    // Initialise a vertical layout with no padding or spacing.
    QBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Should we allow both read and write?
    readWrite = newPacket->isPacketEditable();

    // Create the actions first, since PacketManager::createUI()
    // might want to modify them.
    actDockUndock = new QAction(this);
    actDockUndock->setText(tr("Un&dock"));
    actDockUndock->setIcon(ReginaSupport::themeIcon("mail-attachment"));
    actDockUndock->setToolTip(tr("Dock / undock this packet viewer"));
    actDockUndock->setWhatsThis(tr("Dock or undock this packet viewer.  "
        "A docked viewer sits within the main window, to the right of "
        "the packet tree.  An undocked viewer floats in its own window."));
    actDockUndock->setVisible(ReginaPrefSet::global().useDock);
    actDockUndock->setEnabled(ReginaPrefSet::global().useDock);
    connect(actDockUndock,SIGNAL(triggered()),this, SLOT(floatPane()));

    actClose = new QAction(this);
    actClose->setText(tr("&Close"));
    actClose->setIcon(ReginaSupport::themeIcon("window-close"));
    actClose->setShortcuts(QKeySequence::Close);
    actClose->setToolTip(tr("Close this packet viewer"));
    actClose->setWhatsThis(tr("Close this packet viewer."));
    connect(actClose,SIGNAL(triggered()), this, SLOT(close()));

    // Set up the header and dock/undock button.
    QBoxLayout* headerBox = new QHBoxLayout();
    headerBox->setSpacing(0);

    headerBox->addStretch(1);

    headerIcon = new QLabel();
    headerIcon->setPixmap(PacketManager::icon(newPacket, true).
        pixmap(headerSize));
    headerIcon->setMargin(2); // Leave *some* space, however tiny.
    headerIcon->setWhatsThis(tr("This shows the label of the packet "
        "being viewed, as well as its packet type."));
    headerBox->addWidget(headerIcon);

    headerBox->addSpacing((headerSize / 2 /* shrug */));

    headerTitle = new QLabel(newPacket->getFullName().c_str());
    headerTitle->setAlignment(Qt::AlignCenter);
    headerTitle->setWhatsThis(tr("This shows the label of the packet "
        "being viewed, as well as its packet type."));
    headerBox->addWidget(headerTitle);

    headerBox->addStretch(1);

    dockUndockBtn = new QToolButton();
    dockUndockBtn->setCheckable(true);
    dockUndockBtn->setIcon(ReginaSupport::themeIcon("mail-attachment"));
    dockUndockBtn->setText(tr("Dock or undock this packet viewer"));
    dockUndockBtn->setChecked(true);
    dockUndockBtn->setWhatsThis(tr("Dock or undock this packet viewer.  "
        "A docked viewer sits within the main window, to the right of "
        "the packet tree.  An undocked viewer floats in its own window."));
    dockUndockBtn->setVisible(ReginaPrefSet::global().useDock);
    dockUndockBtn->setEnabled(ReginaPrefSet::global().useDock);
    headerBox->addWidget(dockUndockBtn);
    connect(dockUndockBtn, SIGNAL(toggled(bool)), this, SLOT(floatPane()));

    layout->addLayout(headerBox);

    QFrame* separator = new QFrame();
    separator->setFrameStyle(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    layout->addWidget(separator);

    // Set up the main interface component.
    mainUI = PacketManager::createUI(newPacket, this);
    QWidget* mainUIWidget = mainUI->getInterface();
    layout->addWidget(mainUIWidget, 1);
    setFocusProxy(mainUIWidget);

    // Register ourselves to listen for various events.
    newPacket->listen(this);
}

PacketPane::~PacketPane() {
    delete mainUI;
    delete actDockUndock;
    delete actClose;
}

void PacketPane::supportDock(bool shouldSupport) {
    actDockUndock->setEnabled(shouldSupport);
    actDockUndock->setVisible(shouldSupport);
    dockUndockBtn->setEnabled(shouldSupport);
    dockUndockBtn->setVisible(shouldSupport);
}

void PacketPane::fillPacketTypeMenu(QMenu* menu) {
    const QLinkedList<QAction*>& packetTypeActions(
        mainUI->getPacketTypeActions());
    if (! packetTypeActions.isEmpty()) {
        for (QLinkedListIterator<QAction*> it(packetTypeActions) ;
                it.hasNext(); ) {
            menu->addAction( it.next() );  
        }
        menu->addSeparator();
    }

    menu->addAction(actDockUndock);
    menu->addAction(actClose);
}

bool PacketPane::setReadWrite(bool allowReadWrite) {
    if (allowReadWrite)
        if (! (mainUI->getPacket()->isPacketEditable()))
            return false;

    if (readWrite == allowReadWrite)
        return true;

    // We are changing the status and we are allowed to.
    readWrite = allowReadWrite;

    mainUI->setReadWrite(allowReadWrite);
    updateClipboardActions();

    return true;
}

bool PacketPane::queryClose() {
    // Now that changes are automatically synced with the calculation
    // engine immediately, there is no reason not to close.
    // Just do some cleaning up.
    mainWindow->isClosing(this);
    return true;
}

void PacketPane::registerEditOperations(QAction* actCut, QAction* actCopy,
        QAction* actPaste) {
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
    refreshHeader();
    mainUI->refresh();
}

void PacketPane::packetWasRenamed(regina::NPacket*) {
    // Assume it's this packet.
    refreshHeader();

    if (frame)
        frame->setWindowTitle(getPacket()->getHumanLabel().c_str());
}

void PacketPane::packetToBeDestroyed(regina::NPacket*) {
    // Assume it's this packet.
    close();
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
        refreshHeader();
}

bool PacketPane::close() {
    // Let whoever owns us handle the entire close event.
    // We'll come back to this class when they call queryClose().
    if (frame)
        return frame->close();
    else
        return mainWindow->closeDockedPane();
}

void PacketPane::dockPane() {
    if (! frame)
        return;

    // The packet pane is currently floating.

    mainWindow->dock(this);
    frame->hide();

    // Use deleteLater() instead of delete; otherwise we get occasional
    // crashes when the event loop cleans up the departing window.
    frame->deleteLater();
    frame = 0;

    dockUndockBtn->setChecked(true);
    actDockUndock->setText(tr("Un&dock"));
    disconnect(dockUndockBtn, SIGNAL(toggled(bool)), this, SLOT(dockPane()));
    connect(dockUndockBtn, SIGNAL(toggled(bool)), this, SLOT(floatPane()));
    disconnect(actDockUndock, SIGNAL(triggered()), this, SLOT(dockPane()));
    connect(actDockUndock, SIGNAL(triggered()), this, SLOT(floatPane()));
}

void PacketPane::floatPane() {
    if (frame)
        return;

    // The packet pane is currently docked.
    mainWindow->aboutToUndock(this);
    frame = new PacketWindow(this, mainWindow);

    dockUndockBtn->setChecked(false);
    actDockUndock->setText(tr("&Dock"));
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

void PacketPane::refreshHeader() {
    regina::NPacket* packet = mainUI->getPacket();
    headerTitle->setText(packet->getFullName().c_str());
    headerIcon->setPixmap(PacketManager::icon(packet, true).
        pixmap(headerSize));
}

void PacketPane::customEvent(QEvent* evt) {
    int type = evt->type();
    if (type == EVT_PANE_SET_READONLY)
        setReadWrite(false);
    else if (type == EVT_PANE_SET_READWRITE)
        setReadWrite(true);
    else if (type == EVT_REFRESH_HEADER)
        refreshHeader();
}

