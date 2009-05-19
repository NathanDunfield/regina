
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
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

/*! \file packet/npacketlistener.h
 *  \brief Deals with objects that can listen for packet events.
 */

#ifndef __NPACKETLISTENER_H
#ifndef __DOXYGEN
#define __NPACKETLISTENER_H
#endif

#include <set>

namespace regina {

class NPacket;

/**
 * \weakgroup packet
 * @{
 */

/**
 * An object that can be registered to listen for packet events.
 *
 * A packet listener can be registered to listen for events on a
 * packet by calling NPacket::listen().
 *
 * Each time that one of the events listed in this class occurs,
 * the packet will call the appropriate routine for all registered
 * packet listeners.
 *
 * Note that these events are mutually exclusive, i.e., any event will
 * cause at most one routine to be called for each listener.  For
 * instance, if a packet is renamed then packetWasRenamed() will be
 * called but packetWasChanged() will not.
 *
 * No guarantees are made as to the order in which the different packet
 * listeners are notified of an event.
 *
 * Note that when a listener is destroyed, it is automatically unregistered
 * from any packets to which it is currently listening.  Similarly, when
 * a packet is destroyed all listeners are automatically unregistered.
 *
 * When using multiple threads, there are restrictions upon what any thread
 * other than the main thread may do.  If these restrictions are properly
 * adhered to, packet listeners may assume that no routines other than
 * childWasAdded() will be called from a non-main thread.  Of course it is
 * up to the multithreading code to ensure that these restrictions are in
 * fact met; see the NThread class notes for further information.
 *
 * \warning If the multithreading restrictions noted above are \e not
 * adhered to, this can result in the GUI crashing within either Qt or
 * Xlib.  Again, see the NThread class notes for further information.
 *
 * \ifacespython Not present.
 */
class NPacketListener {
    private:
        std::set<NPacket*> packets;
            /**< The set of packets upon which this object is currently
                 listening. */

    public:
        /**
         * Destroys this listener.  This listener will be unregistered
         * from any packets to which it is currently listening.
         */
        virtual ~NPacketListener();

        /**
         * Unregisters this listener from any packets to which it is
         * currently listening.
         */
        void unregisterFromAllPackets();

        /**
         * Called when the contents of the packet have been changed.
         *
         * The default implementation of this routine is to do nothing.
         *
         * @param packet the packet being listened to.
         */
        virtual void packetWasChanged(NPacket* packet);
        /**
         * Called when the packet label or tags have been changed.
         *
         * The default implementation of this routine is to do nothing.
         *
         * @param packet the packet being listened to.
         */
        virtual void packetWasRenamed(NPacket* packet);
        /**
         * Called when the packet is about to be destroyed.  Note that
         * this event is called \e before the packet is destroyed, not after.
         *
         * When an entire packet subtree is to be destroyed, child packets
         * will notify their listeners of the impending destruction
         * before parent packets will.
         *
         * Note that the packet will forcibly unregister this listener
         * immediately \e before packetToBeDestroyed() is called, to avoid
         * any unpleasant consequences if this listener should also try to
         * unregister itself.  This means that, by the time this routine is
         * called, this listener will no longer be registered with the
         * packet in question (and any attempt to unregister it again
         * will be harmless).
         *
         * The default implementation of this routine is to do nothing.
         *
         * @param packet the packet being listened to.
         */
        virtual void packetToBeDestroyed(NPacket* packet);
        /**
         * Called when a child packet has been inserted directly beneath
         * the packet.
         *
         * The default implementation of this routine is to do nothing.
         *
         * @param packet the packet being listened to.
         * @param child the child packet that was added.
         */
        virtual void childWasAdded(NPacket* packet, NPacket* child);
        /**
         * Called when a child packet has been removed from directly beneath
         * the packet.  Note that the child packet may be about to be
         * destroyed (although this destruction will not have happened yet).
         *
         * Note also that this packet (the parent) may have already
         * entered its destructor (which removes and destroys all child
         * packets as a matter of course).  In this situation it may be
         * unsafe to query or update this packet, and so the third argument
         * \a inParentDestructor is provided to indicate such a situation.
         *
         * The default implementation of this routine is to do nothing.
         *
         * @param packet the packet being listened to.
         * @param child the child packet that was removed.
         * @param inParentDestructor set to \c true if the parent packet
         * is in fact being destroyed, and the child was simply removed
         * as part of the standard subtree destruction.
         */
        virtual void childWasRemoved(NPacket* packet, NPacket* child,
            bool inParentDestructor);
        /**
         * Called when the child packets directly beneath the packet
         * have been reordered.
         *
         * The default implementation of this routine is to do nothing.
         *
         * @param packet the packet being listened to.
         */
        virtual void childrenWereReordered(NPacket* packet);

    /**
     * Allow packets to automatically deregister listeners as they are
     * destroyed.
     */
    friend class NPacket;
};

/*@}*/

// Inline functions for NPacketListener

inline void NPacketListener::packetWasChanged(NPacket*) {
}

inline void NPacketListener::packetWasRenamed(NPacket*) {
}

inline void NPacketListener::packetToBeDestroyed(NPacket*) {
}

inline void NPacketListener::childWasAdded(NPacket*, NPacket*) {
}

inline void NPacketListener::childWasRemoved(NPacket*, NPacket*, bool) {
}

inline void NPacketListener::childrenWereReordered(NPacket*) {
}

} // namespace regina

#endif

