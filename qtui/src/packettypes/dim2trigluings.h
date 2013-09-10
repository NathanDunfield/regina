
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

/*! \file dim2trigluings.h
 *  \brief Provides an edge gluing editor for 2-manifold triangulations.
 */

#ifndef __DIM2TRIGLUINGS_H
#define __DIM2TRIGLUINGS_H

#include "../packettabui.h"
#include "reginaprefset.h"
#include "maths/nperm3.h"

#include <QAbstractItemModel>

class QTableView;
class QToolBar;

namespace regina {
    class Dim2Triangulation;
    class NPacket;
};

class Dim2GluingsModel : public QAbstractItemModel {
    protected:
        /**
         * Details of the working (non-committed) triangulation
         */
        int nTri;
        QString* name;
        int* adjTri;
        regina::NPerm3* adjPerm;

        /**
         * Internal status
         */
        bool isReadWrite_;

    public:
        /**
         * Constructor and destructor.
         */
        Dim2GluingsModel(bool readWrite);
        ~Dim2GluingsModel();

        /**
         * Read-write state.
         */
        bool isReadWrite() const;
        void setReadWrite(bool readWrite);

        /**
         * Loading and saving local data from/to the packet.
         */
        void refreshData(regina::Dim2Triangulation* tri);
        void commitData(regina::Dim2Triangulation* tri);

        /**
         * Overrides for describing and editing data in the model.
         */
        QModelIndex index(int row, int column,
                const QModelIndex& parent) const;
        QModelIndex parent(const QModelIndex& index) const;
        int rowCount(const QModelIndex& parent) const;
        int columnCount(const QModelIndex& parent) const;
        QVariant data(const QModelIndex& index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation,
            int role) const;
        Qt::ItemFlags flags(const QModelIndex& index) const;
        bool setData(const QModelIndex& index, const QVariant& value, int role);

        /**
         * Gluing edit actions.
         */
        void addTri();
        void removeTri(int first, int last);

    private:
        /**
         * Determine whether the given destination triangle and edge
         * string are valid.  If so, a null string is returned; if not,
         * an appropriate error message is returned.
         *
         * If the given permutation pointer is not null, the resulting
         * gluing permutation will be returned in this variable.
         */
        QString isEdgeStringValid(unsigned long srcTri, int srcEdge,
            unsigned long destTri, const QString& destEdge,
            regina::NPerm3* gluing);

        /**
         * Return a short string describing the destination of an
         * edge gluing.  This routine handles both boundary and
         * non-boundary edges.
         */
        static QString destString(int srcEdge, int destTri,
            const regina::NPerm3& gluing);

        /**
         * Convert an edge string (e.g., "20") to an edge permutation.
         *
         * The given edge string must be valid; otherwise the results
         * could be unpredictable (and indeed a crash could result).
         */
        static regina::NPerm3 edgeStringToPerm(int srcEdge, const QString& str);

        /**
         * Display the given error to the user.
         */
        void showError(const QString& message);
};

/**
 * A 2-manifold triangulation page for editing edge gluings.
 */
class Dim2TriGluingsUI : public QObject, public PacketEditorTab {
    Q_OBJECT

    private:
        /**
         * Packet details
         */
        regina::Dim2Triangulation* triangulation;

        /**
         * Internal components
         */
        QWidget* ui;
        QTableView* edgeTable;
        Dim2GluingsModel* model;

        /**
         * Gluing actions
         */
        QAction* actAddTri;
        QAction* actRemoveTri;
        QLinkedList<QAction*> triActionList;
        QLinkedList<QAction*> enableWhenWritable;

    public:
        /**
         * Constructor and destructor.
         */
        Dim2TriGluingsUI(regina::Dim2Triangulation* packet,
                PacketTabbedUI* useParentUI, bool readWrite);
        ~Dim2TriGluingsUI();

        /**
         * Fill the given toolbar with triangulation actions.
         *
         * This is necessary since the toolbar will not be a part of
         * this page, but this page (as the editor) keeps track of the
         * available actions.
         */
        void fillToolBar(QToolBar* bar);

        /**
         * PacketEditorTab overrides.
         */
        regina::NPacket* getPacket();
        QWidget* getInterface();
        const QLinkedList<QAction*>& getPacketTypeActions();
        void commit();
        void refresh();
        void setReadWrite(bool readWrite);

    public slots:
        /**
         * Gluing edit actions.
         */
        void addTri();
        void removeSelectedTris();

        /**
         * Triangulation actions.
         */
        // None for now, but more may come.

        /**
         * Update the states of internal components.
         */
        void updateRemoveState();

        /**
         * Notify us of the fact that an edit has been made.
         */
        void notifyDataChanged();
};

inline Dim2GluingsModel::~Dim2GluingsModel() {
    delete[] name;
    delete[] adjTri;
    delete[] adjPerm;
}

inline bool Dim2GluingsModel::isReadWrite() const {
    return isReadWrite_;
}

inline void Dim2GluingsModel::setReadWrite(bool readWrite) {
    if (isReadWrite_ != readWrite) {
        // Edit flags will all change.
        // A full model reset is probably too severe, but.. *shrug*
        beginResetModel();
        isReadWrite_ = readWrite;
        endResetModel();
    }
}

inline QModelIndex Dim2GluingsModel::parent(const QModelIndex&) const {
    // All items are top-level.
    return QModelIndex();
}

#endif
