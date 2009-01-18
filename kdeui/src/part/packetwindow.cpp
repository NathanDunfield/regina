
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  KDE User Interface                                                    *
 *                                                                        *
 *  Copyright (c) 1999-2009, Ben Burton                                   *
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

// UI includes:
#include "packetui.h"
#include "packetwindow.h"
#include "reginapart.h"

#include <kaction.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kstdaction.h>

PacketWindow::PacketWindow(PacketPane* newPane, QWidget* parent) :
        KMainWindow(parent, "Packet#",
        WType_TopLevel | WDestructiveClose | WStyle_ContextHelp),
        heldPane(newPane) {
    // Resize ourselves nicely.
    if (! initialGeometrySet())
        resize(400, 400);

    // Set up our actions.
    setInstance(ReginaPart::factoryInstance());

    if (newPane->hasTextComponent()) {
        KAction* cut = KStdAction::cut(0, 0, actionCollection());
        KAction* copy = KStdAction::copy(0, 0, actionCollection());
        KAction* paste = KStdAction::paste(0, 0, actionCollection());
        KAction* undo = KStdAction::undo(0, 0, actionCollection());
        KAction* redo = KStdAction::redo(0, 0, actionCollection());

        newPane->registerEditOperation(cut, PacketPane::editCut);
        newPane->registerEditOperation(copy, PacketPane::editCopy);
        newPane->registerEditOperation(paste, PacketPane::editPaste);
        newPane->registerEditOperation(undo, PacketPane::editUndo);
        newPane->registerEditOperation(redo, PacketPane::editRedo);
    }

    createGUI("packetwindow.rc", false);

    QPtrList<KAction> typeActions;
    typeActions.append(newPane->getPacketTypeMenu());
    plugActionList("packet_type_menu", typeActions);

    // Set up the widgets.
    newPane->reparent(this, QPoint(0, 0));
    setCentralWidget(newPane);
    newPane->show();
}

bool PacketWindow::queryClose() {
    return heldPane->queryClose();
}

#include "packetwindow.moc"
