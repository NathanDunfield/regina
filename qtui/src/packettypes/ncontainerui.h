
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  KDE User Interface                                                    *
 *                                                                        *
 *  Copyright (c) 1999-2014, Ben Burton                                   *
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

/*! \file ncontainerui.h
 *  \brief Provides an interface for viewing container packets.
 */

#ifndef __NCONTAINERUI_H
#define __NCONTAINERUI_H

#include "packet/npacketlistener.h"

#include "../packetui.h"

class QLabel;

namespace regina {
    class NContainer;
    class NPacket;
};

/**
 * A packet interface for viewing containers.
 *
 * Note that this interface is always read-only.
 */
class NContainerUI : public PacketReadOnlyUI, public regina::NPacketListener {
    private:
        /**
         * Packet details
         */
        regina::NContainer* container;

        /**
         * Internal components
         */
        QWidget* ui;
        QLabel* children;
        QLabel* descendants;

    public:
        /**
         * Constructor.
         */
        NContainerUI(regina::NContainer* packet, PacketPane* newEnclosingPane);

        /**
         * PacketUI overrides.
         */
        regina::NPacket* getPacket();
        QWidget* getInterface();
        QString getPacketMenuText() const;
        void refresh();

        /**
         * NPacketListener overrides.
         */
        void childWasAdded(regina::NPacket* packet, regina::NPacket* child);
        void childWasRemoved(regina::NPacket* packet, regina::NPacket* child,
            bool inParentDestructor);
};

#endif
