
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

/*! \file ntrialgebra.h
 *  \brief Provides an algebra viewer for triangulations.
 */

#ifndef __NTRIALGEBRA_H
#define __NTRIALGEBRA_H

#include "../packettabui.h"

class NTriFundGroupUI;
class QLabel;
class QLineEdit;
class QListWidget;
class QTreeWidget;
class QPushButton;

namespace regina {
    class NPacket;
    class NTriangulation;
};

/**
 * A triangulation page for viewing algebraic properties.
 */
class NTriAlgebraUI : public PacketTabbedViewerTab {
    private:
        /**
         * Internal components
         */
        NTriFundGroupUI* fundGroup;

    public:
        /**
         * Constructor.
         */
        NTriAlgebraUI(regina::NTriangulation* packet,
                PacketTabbedUI* useParentUI);
};

/**
 * A triangulation page for viewing homology groups.
 */
class NTriHomologyUI : public PacketViewerTab {
    private:
        /**
         * Packet details
         */
        regina::NTriangulation* tri;

        /**
         * Internal components
         */
        QWidget* ui;
        QLabel* H1;
        QLabel* H1Rel;
        QLabel* H1Bdry;
        QLabel* H2;
        QLabel* H2Z2;

    public:
        /**
         * Constructor.
         */
        NTriHomologyUI(regina::NTriangulation* packet,
                PacketTabbedViewerTab* useParentUI);

        /**
         * PacketViewerTab overrides.
         */
        regina::NPacket* getPacket();
        QWidget* getInterface();
        void refresh();
        void editingElsewhere();
};

/**
 * A triangulation page for viewing the fundamental group.
 */
class NTriFundGroupUI : public QObject, public PacketViewerTab {
    Q_OBJECT

    private:
        /**
         * Packet details
         */
        regina::NTriangulation* tri;

        /**
         * Internal components
         */
        QWidget* ui;
        QLabel* fundName;
        QLabel* fundGens;
        QLabel* fundRelCount;
        QListWidget* fundRels;
        QPushButton* btnGAP;
        QPushButton* btnSimp;
        unsigned simpDepth;

    public:
        /**
         * Constructor.
         */
        NTriFundGroupUI(regina::NTriangulation* packet,
                PacketTabbedViewerTab* useParentUI);

        /**
         * PacketViewerTab overrides.
         */
        regina::NPacket* getPacket();
        QWidget* getInterface();
        void refresh();
        void editingElsewhere();

    public slots:
        /**
         * Group simplification actions.
         */
        void simplifyGAP();
        /**
         * Our internal pi1 simplification code.
         */
        void simplifyPi1();

    private:
        /**
         * Returns the full path to the GAP executable, or QString::null
         * if the GAP executable does not appear to be valid.
         */
        QString verifyGAPExec();
};

/**
 * A triangulation page for viewing Turaev-Viro invariants.
 */
class NTriTuraevViroUI : public QObject, public PacketViewerTab {
    Q_OBJECT

    private:
        /**
         * Packet details
         */
        regina::NTriangulation* tri;

        /**
         * Internal components
         */
        QWidget* ui;
        QLabel* paramsLabel;
        QLineEdit* params;
        QPushButton* calculate;
        QTreeWidget* invariants;

    public:
        /**
         * Constructor.
         */
        NTriTuraevViroUI(regina::NTriangulation* packet,
                PacketTabbedViewerTab* useParentUI);

        /**
         * PacketViewerTab overrides.
         */
        regina::NPacket* getPacket();
        QWidget* getInterface();
        void refresh();
        void editingElsewhere();

    public slots:
        /**
         * Calculate the requested Turaev-Viro invariant.
         */
        void calculateInvariant();
};

/**
 * A page for viewing detailed cellular information about the manifold.
 *
 * \author Ryan Budney
 */
class NTriCellularInfoUI: public PacketViewerTab {
    private:
        /**
         * Packet details
         */
        regina::NTriangulation* tri;

        /**
         * Internal components
         */
        QWidget* ui;
        QLabel* Cells;
        QLabel* DualCells;
        QLabel* EulerChar;
        QLabel* H0H1H2H3;
        QLabel* HBdry;
        QLabel* BdryMap;
        QLabel* TorForOrders; // torsion subgroup prime power decomp
        QLabel* TorForSigma;  // the Kawauchi-Kojima 2-torsion sigma vector
        QLabel* TorForLegendre; // the odd p-torsion Legendre symbol vector
        QLabel* AlexInv; // Alexander module invariants (if exists)
        QLabel* EmbeddingComments; // comments on what the manifold may
                                   // or may not embed in.

    public:
        /**
         * Constructor.
         */
        NTriCellularInfoUI(regina::NTriangulation* packet,
                PacketTabbedViewerTab* useParentUI);

        /**
         * PacketViewerTab overrides.
         */
        regina::NPacket* getPacket();
        QWidget* getInterface();
        void refresh();
        void editingElsewhere();
};

#endif
