
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Qt User Interface                                                     *
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

/*! \file cuspchooser.h
 *  \brief Provides a widget for selecting a single cusp
 *  of a SnapPea triangulation.
 */

#ifndef __CUSPCHOOSER_H
#define __CUSPCHOOSER_H

#include "packet/npacketlistener.h"

#include <QDialog>
#include <QComboBox>
#include <vector>

namespace regina {
    class NCusp;
    class NSnapPeaTriangulation;
};

/**
 * A filter function, used to determine whether a given cusp
 * should appear in the list.
 */
typedef bool (*CuspFilterFunc)(const regina::NCusp*);

/**
 * A widget through which a single cusp of some SnapPea triangulation
 * can be selected.  An optional filter may be applied to restrict the
 * available selections.
 *
 * The contents of this chooser will be updated in real time if the
 * triangulation is externally modified.
 */
class CuspChooser : public QComboBox, public regina::NPacketListener {
    Q_OBJECT

    public:
        enum {
            CUSP_ALL = -1,
            CUSP_NO_SELECTION = -2
        };

    private:
        regina::NSnapPeaTriangulation* tri_;
            /**< The triangulation whose cusps we are choosing from. */
        CuspFilterFunc filter_;
            /**< A filter to restrict the available selections, or
                 0 if no filter is necessary. */
        std::vector<int> options_;
            /**< A list of the available options to choose from. */

    public:
        /**
         * Constructors that fills the chooser with available selections.
         *
         * If \a autoUpdate is \c true (the default), then this chooser
         * will be updated when the triangulation changes.
         *
         * If \a autoUpdate is \c false, then contents of this chooser will
         * only be updated when refresh() is manually called.  Be careful
         * when using this setting, since if the triangulation changes
         * but the chooser is \e not refreshed, then selected() may end
         * up returning an invalid pointer.
         *
         * The given filter may be 0, in which case every cusp
         * will be offered.
         */
        CuspChooser(regina::NSnapPeaTriangulation* tri,
                CuspFilterFunc filter, QWidget* parent,
                bool autoUpdate = true);

        /**
         * Returns an integer that identifies the currently selected cusp.
         *
         * This will either be a non-negative cusp index, or one of the
         * special values \a CUSP_ALL or \a CUSP_NO_SELECTION.
         */
        int selected();

        /**
         * Changes the selection to the given cusp.
         *
         * The given cusp must be identified by either a non-negative
         * cusp index, or the special value \a CUSP_ALL.
         *
         * If the given cusp is not one of the options in this
         * chooser, then the first entry in the chooser will be selected.
         *
         * The activated() signal will \e not be emitted.
         */
        void select(int option);

        /**
         * Forces a manual refresh of the contents of this chooser.
         * Returns \c true if and only if the chooser is non-empty
         * (i.e., at least one option is present) after the refresh.
         */
        bool refresh();

        /**
         * NPacketListener overrides.
         */
        void packetToBeChanged(regina::NPacket*);
        void packetWasChanged(regina::NPacket*);
        void packetToBeDestroyed(regina::NPacket*);

        /**
         * Some ready-made cusp filters.
         */
        static bool filterFilled(const regina::NCusp*);
        static bool filterComplete(const regina::NCusp*);

    private:
        /**
         * The text to be displayed for a given option.
         */
        QString description(int option);

        /**
         * Fills the chooser with the set of allowable options.
         */
        void fill();
};

/**
 * A dialog used to select a single cusp of a given triangulation.
 */
class CuspDialog : public QDialog {
    Q_OBJECT

    private:
        /**
         * Internal components:
         */
        CuspChooser* chooser;

    public:
        /**
         * Constructor and destructor.
         */
        CuspDialog(QWidget* parent,
            regina::NSnapPeaTriangulation* tri,
            CuspFilterFunc filter,
            const QString& title,
            const QString& message,
            const QString& whatsThis);

        static int choose(QWidget* parent,
            regina::NSnapPeaTriangulation* tri,
            CuspFilterFunc filter,
            const QString& title,
            const QString& message,
            const QString& whatsThis);
};

inline bool CuspChooser::refresh() {
    clear();
    options_.clear();
    fill();
    return (count() > 0);
}

inline void CuspChooser::packetToBeChanged(regina::NPacket*) {
    clear();
    options_.clear();
}

inline void CuspChooser::packetWasChanged(regina::NPacket*) {
    fill();
}

inline void CuspChooser::packetToBeDestroyed(regina::NPacket*) {
    clear();
    options_.clear();
}

#endif
