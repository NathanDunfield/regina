
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
#include "file/nxmlfile.h"
#include "dim2/dim2edge.h"
#include "dim2/dim2triangulation.h"

// UI includes:
#include "dim2trigluings.h"
#include "reginamain.h"
#include "reginasupport.h"

#include <memory>
#include <QAction>
#include <QCoreApplication>
#include <QFileInfo>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QProgressDialog>
#include <QRegExp>
#include <QTableView>
#include <QTextDocument>
#include <QToolBar>
#include <set>

using regina::NPacket;
using regina::Dim2Triangulation;

namespace {
    /**
     * Represents a destination for a single edge gluing.
     */
    QRegExp reEdgeGluing(
        "^\\s*"
        "(\\d+)"
        "(?:\\s*\\(\\s*|\\s+)"
        "([0-2][0-2])"
        "\\s*\\)?\\s*$");

    /**
     * Represents a single triangle edge.
     */
    QRegExp reEdge("^[0-2][0-2]$");
}

Dim2GluingsModel::Dim2GluingsModel(bool readWrite) :
        nTri(0), name(0), adjTri(0), adjPerm(0), isReadWrite_(readWrite) {
}

void Dim2GluingsModel::refreshData(regina::Dim2Triangulation* triangulation) {
    beginResetModel();

    delete[] name;
    delete[] adjTri;
    delete[] adjPerm;

    nTri = triangulation->getNumberOfTriangles();
    if (nTri == 0) {
        name = 0;
        adjTri = 0;
        adjPerm = 0;

        endResetModel();
        return;
    }

    name = new QString[nTri];
    adjTri = new int[3 * nTri];
    adjPerm = new regina::NPerm3[3 * nTri];

    int triNum, edge;
    regina::Dim2Triangle* tri;
    regina::Dim2Triangle* adj;
    for (triNum = 0; triNum < nTri; triNum++) {
        tri = triangulation->getTriangle(triNum);
        name[triNum] = tri->getDescription().c_str();
        for (edge = 0; edge < 3; ++edge) {
            adj = tri->adjacentTriangle(edge);
            if (adj) {
                adjTri[triNum * 3 + edge] = triangulation->triangleIndex(adj);
                adjPerm[triNum * 3 + edge] = tri->adjacentGluing(edge);
            } else
                adjTri[triNum * 3 + edge] = -1;
        }
    }

    endResetModel();
}

void Dim2GluingsModel::addTri() {
    beginInsertRows(QModelIndex(), nTri, nTri);

    QString* newName = new QString[nTri + 1];
    int* newTri = new int[3 * (nTri + 1)];
    regina::NPerm3* newPerm = new regina::NPerm3[3 * (nTri + 1)];

    std::copy(name, name + nTri, newName);
    std::copy(adjTri, adjTri + 3 * nTri, newTri);
    std::copy(adjPerm, adjPerm + 3 * nTri, newPerm);

    for (int edge = 0; edge < 3; ++edge)
        newTri[3 * nTri + edge] = -1;

    delete[] name;
    delete[] adjTri;
    delete[] adjPerm;

    name = newName;
    adjTri = newTri;
    adjPerm = newPerm;

    ++nTri;

    endInsertRows();
}

void Dim2GluingsModel::removeTri(int first, int last) {
    beginResetModel();

    if (first == 0 && last == nTri - 1) {
        delete[] name;
        delete[] adjTri;
        delete[] adjPerm;

        name = 0;
        adjTri = 0;
        adjPerm = 0;

        nTri = 0;

        endResetModel();
        return;
    }

    // Adjust other triangle numbers.
    int nCut = last - first + 1;

    QString* newName = new QString[nTri - nCut];
    int* newTri = new int[3 * (nTri - nCut)];
    regina::NPerm3* newPerm = new regina::NPerm3[3 * (nTri - nCut)];

    int row, edge, i;
    for (row = 0; row < first; ++row) {
        newName[row] = name[row];
        for (edge = 0; edge < 3; ++edge) {
            newTri[3 * row + edge] = adjTri[3 * row + edge];
            newPerm[3 * row + edge] = adjPerm[3 * row + edge];
        }
    }

    for (row = first; row < nTri - nCut; ++row) {
        newName[row] = name[row + nCut];
        for (edge = 0; edge < 3; ++edge) {
            newTri[3 * row + edge] = adjTri[3 * (row + nCut) + edge];
            newPerm[3 * row + edge] = adjPerm[3 * (row + nCut) + edge];
        }
    }

    for (i = 0; i < 3 * (nTri - nCut); ++i)
        if (newTri[i] >= first && newTri[i] <= last)
            newTri[i] = -1;
        else if (newTri[i] > last)
            newTri[i] -= nCut;

    delete[] name;
    delete[] adjTri;
    delete[] adjPerm;

    name = newName;
    adjTri = newTri;
    adjPerm = newPerm;

    nTri -= nCut;

    // Done!
    endResetModel();
}

void Dim2GluingsModel::commitData(regina::Dim2Triangulation* tri) {
    tri->removeAllTriangles();

    if (nTri == 0)
        return;

    regina::NPacket::ChangeEventSpan span(tri);

    regina::Dim2Triangle** tris = new regina::Dim2Triangle*[nTri];
    int triNum, adjTriNum;
    int edge, adjEdge;

    // Create the triangles.
    for (triNum = 0; triNum < nTri; triNum++)
        tris[triNum] = tri->newTriangle(name[triNum].toAscii().constData());

    // Glue the triangles together.
    for (triNum = 0; triNum < nTri; triNum++)
        for (edge = 0; edge < 3; ++edge) {
            adjTriNum = adjTri[3 * triNum + edge];
            if (adjTriNum < triNum) // includes adjTriNum == -1
                continue;
            adjEdge = adjPerm[3 * triNum + edge][edge];
            if (adjTriNum == triNum && adjEdge < edge)
                continue;

            // It's a forward gluing.
            tris[triNum]->joinTo(edge, tris[adjTriNum],
                adjPerm[3 * triNum + edge]);
        }

    // Tidy up.
    delete[] tris;
}

QModelIndex Dim2GluingsModel::index(int row, int column,
        const QModelIndex& /* unused parent*/) const {
    return createIndex(row, column, quint32(4 * row + column));
}

int Dim2GluingsModel::rowCount(const QModelIndex& /* unused parent*/) const {
    return nTri;
}

int Dim2GluingsModel::columnCount(const QModelIndex& /* unused parent*/) const {
    return 4;
}

QVariant Dim2GluingsModel::data(const QModelIndex& index, int role) const {
    int tri = index.row();
    if (role == Qt::DisplayRole) {
        // Triangle name?
        if (index.column() == 0)
            return (name[tri].isEmpty() ? QString::number(tri) :
                (QString::number(tri) + " (" + name[tri] + ')'));

        // Edge gluing?
        int edge = 3 - index.column();
        if (edge >= 0)
            return destString(edge, adjTri[3 * tri + edge],
                adjPerm[3 * tri + edge]);
        return QVariant();
    } else if (role == Qt::EditRole) {
        // Triangle name?
        if (index.column() == 0)
            return name[tri];

        // Edge gluing?
        int edge = 3 - index.column();
        if (edge >= 0)
            return destString(edge, adjTri[3 * tri + edge],
                adjPerm[3 * tri + edge]);
        return QVariant();
    } else
        return QVariant();
}

QVariant Dim2GluingsModel::headerData(int section, Qt::Orientation orientation,
        int role) const {
    if (orientation != Qt::Horizontal)
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
        case 0: return tr("Triangle");
        case 1: return tr("Edge 01");
        case 2: return tr("Edge 02");
        case 3: return tr("Edge 12");
    }
    return QVariant();
}

Qt::ItemFlags Dim2GluingsModel::flags(const QModelIndex& /* unused index*/) const {
    if (isReadWrite_)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool Dim2GluingsModel::setData(const QModelIndex& index, const QVariant& value,
        int /* unused role*/) {
    int tri = index.row();
    if (index.column() == 0) {
        QString newName = value.toString().trimmed();
        if (newName == name[tri])
            return false;

        name[tri] = newName;
        emit dataChanged(index, index);
        return true;
    }

    int edge = 3 - index.column();
    if (edge < 0)
        return false;

    int newAdjTri;
    regina::NPerm3 newAdjPerm;

    // Find the proposed new gluing.
    QString text = value.toString().trimmed();

    if (text.isEmpty()) {
        // Boundary edge.
        newAdjTri = -1;
    } else if (! reEdgeGluing.exactMatch(text)) {
        // Bad string.
        showError(tr("<qt>The edge gluing should be of the "
            "form: <i>triangle (edge)</i>.  An example is <i>5 (02)</i>, "
            "which represents edge 02 of triangle 5.</qt>"));
        return false;
    } else {
        // Real edge.
        newAdjTri = reEdgeGluing.cap(1).toInt();
        QString triEdge = reEdgeGluing.cap(2);

        // Check explicitly for a negative triangle number
        // since isEdgeStringValid() takes an unsigned integer.
        if (newAdjTri < 0 || newAdjTri >= nTri) {
            showError(tr("There is no triangle number %1.").arg(newAdjTri));
            return false;
        }

        // Do we have a valid gluing?
        QString err = isEdgeStringValid(tri, edge, newAdjTri, triEdge,
            &newAdjPerm);
        if (! err.isNull()) {
            showError(err);
            return false;
        }
    }

    // Yes, looks valid.
    int oldAdjTri = adjTri[3 * tri + edge];
    regina::NPerm3 oldAdjPerm = adjPerm[3 * tri + edge];
    int oldAdjEdge = oldAdjPerm[edge];

    // Have we even made a change?
    if (oldAdjTri < 0 && newAdjTri < 0)
        return false;
    if (oldAdjTri == newAdjTri && oldAdjPerm == newAdjPerm)
        return false;

    // Yes!  Go ahead and make the change.

    // First unglue from the old partner if it exists.
    if (oldAdjTri >= 0) {
        adjTri[3 * oldAdjTri + oldAdjEdge] = -1;

        QModelIndex oldAdjIndex = this->index(oldAdjTri, 3 - oldAdjEdge,
            QModelIndex());
        emit dataChanged(oldAdjIndex, oldAdjIndex);
    }

    // Are we making the edge boundary?
    if (newAdjTri < 0) {
        adjTri[3 * tri + edge] = -1;

        emit dataChanged(index, index);
        return true;
    }

    // We are gluing the edge to a new partner.
    int newAdjEdge = newAdjPerm[edge];

    // Does this new partner already have its own partner?
    if (adjTri[3 * newAdjTri + newAdjEdge] >= 0) {
        // Yes.. better unglue it.
        int extraTri = adjTri[3 * newAdjTri + newAdjEdge];
        int extraEdge = adjPerm[3 * newAdjTri + newAdjEdge][newAdjEdge];

        adjTri[3 * extraTri + extraEdge] = -1;

        QModelIndex extraIndex = this->index(extraTri, 3 - extraEdge,
            QModelIndex());
        emit dataChanged(extraIndex, extraIndex);
    }

    // Glue the two edges together.
    adjTri[3 * tri + edge] = newAdjTri;
    adjTri[3 * newAdjTri + newAdjEdge] = tri;

    adjPerm[3 * tri + edge] = newAdjPerm;
    adjPerm[3 * newAdjTri + newAdjEdge] = newAdjPerm.inverse();

    emit dataChanged(index, index);

    QModelIndex newAdjIndex = this->index(newAdjTri, 3 - newAdjEdge,
        QModelIndex());
    emit dataChanged(newAdjIndex, newAdjIndex);

    return true;
}

QString Dim2GluingsModel::isEdgeStringValid(unsigned long srcTri, int srcEdge,
        unsigned long destTri, const QString& destEdge,
        regina::NPerm3* gluing) {
    if (destTri >= nTri)
        return tr("There is no triangle number %1.").arg(destTri);

    if (! reEdge.exactMatch(destEdge))
        return tr("<qt>%1 is not a valid triangle edge.  A triangle "
            "edge must be described by a sequence of two vertices, each "
            "between 0 and 2 inclusive.  An example is <i>02</i>.</qt>").
            arg(destEdge);

    if (destEdge[0] == destEdge[1])
        return tr("%1 is not a valid triangle edge.  The two vertices "
            "forming the edge must be distinct.").arg(destEdge);

    regina::NPerm3 foundGluing = edgeStringToPerm(srcEdge, destEdge);
    if (srcTri == destTri && foundGluing[srcEdge] == srcEdge)
        return tr("An edge cannot be glued to itself.");

    // It's valid!
    if (gluing)
        *gluing = foundGluing;

    return QString::null;
}

void Dim2GluingsModel::showError(const QString& message) {
    // We should actually pass the view to the message box, not 0, but we
    // don't have access to any widget from here...
    ReginaSupport::info(0 /* should be the view? */,
        tr("This is not a valid gluing."), message);
}

QString Dim2GluingsModel::destString(int srcEdge, int destTri,
        const regina::NPerm3& gluing) {
    if (destTri < 0)
        return "";
    else
        return QString::number(destTri) + " (" + (gluing *
            regina::Dim2Edge::ordering[srcEdge]).trunc2().c_str() + ')';
}

regina::NPerm3 Dim2GluingsModel::edgeStringToPerm(int srcEdge,
        const QString& str) {
    int destVertex[3];

    destVertex[2] = 3; // This will be adjusted in a moment.
    for (int i = 0; i < 2; i++) {
        // Use toLatin1() here because we are converting characters,
        // not strings.
        destVertex[i] = str[i].toLatin1() - '0';
        destVertex[2] -= destVertex[i];
    }

    return regina::NPerm3(destVertex[0], destVertex[1], destVertex[2]) *
        regina::Dim2Edge::ordering[srcEdge].inverse();
}

Dim2TriGluingsUI::Dim2TriGluingsUI(regina::Dim2Triangulation* packet,
        PacketTabbedUI* useParentUI, bool readWrite) :
        PacketEditorTab(useParentUI), triangulation(packet) {
    // Set up the table of edge gluings.
    model = new Dim2GluingsModel(readWrite);
    edgeTable = new QTableView();
    edgeTable->setSelectionMode(QAbstractItemView::ContiguousSelection);
    edgeTable->setModel(model);
    
    if (readWrite) {
        QAbstractItemView::EditTriggers flags(
            QAbstractItemView::AllEditTriggers);
        flags ^= QAbstractItemView::CurrentChanged;
        edgeTable->setEditTriggers(flags);
    } else
        edgeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);


    edgeTable->setWhatsThis(tr("<qt>A table specifying which triangle "
        "edges are identified with which others.<p>"
        "Triangles are numbered upwards from 0, and the three vertices of "
        "each triangle are numbered 0, 1 and 2.  Each row of the table "
        "represents a single triangle, and shows the identifications "
        "for each of its three edges.<p>"
        "As an example, if we are looking at the table cell for edge 01 of "
        "triangle 7, a gluing of <i>5 (20)</i> shows that "
        "that this edge is identified with edge 20 of triangle 5, in "
        "such a way that vertices 0 and 1 of triangle "
        "7 are mapped to vertices 2 and 0 respectively of triangle 5.<p>"
        "To change these identifications, simply type your own gluings into "
        "the table.</qt>"));

    edgeTable->verticalHeader()->hide();

    //edgeTable->setColumnStretchable(0, true);
    //edgeTable->setColumnStretchable(1, true);
    //edgeTable->setColumnStretchable(2, true);
    //edgeTable->setColumnStretchable(3, true);

    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SLOT(notifyDataChanged()));

    ui = edgeTable;

    // Set up the triangulation actions.
    QAction* sep;

    actAddTri = new QAction(this);
    actAddTri->setText(tr("&Add Triangle"));
    actAddTri->setIcon(ReginaSupport::themeIcon("list-add"));
    actAddTri->setToolTip(tr("Add a new triangle"));
    actAddTri->setEnabled(readWrite);
    actAddTri->setWhatsThis(tr("Add a new triangle to this triangulation."));
    enableWhenWritable.append(actAddTri);
    triActionList.append(actAddTri);
    connect(actAddTri, SIGNAL(triggered()), this, SLOT(addTri()));

    actRemoveTri = new QAction(this);
    actRemoveTri->setText(tr("&Remove Triangle"));
    actRemoveTri->setIcon(ReginaSupport::themeIcon("list-remove"));
    actRemoveTri->setToolTip(tr("Remove the currently selected triangles"));
    actRemoveTri->setEnabled(false);
    actRemoveTri->setWhatsThis(tr("Remove the currently selected "
        "triangles from this triangulation."));
    connect(actRemoveTri, SIGNAL(triggered()), this, SLOT(removeSelectedTris()));
    connect(edgeTable->selectionModel(),
        SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this, SLOT(updateRemoveState()));
    triActionList.append(actRemoveTri);

    //sep = new QAction(this);
    //sep->setSeparator(true);
    //triActionList.append(sep);

    // Tidy up.

    refresh();
}

Dim2TriGluingsUI::~Dim2TriGluingsUI() {
    // Make sure the actions, including separators, are all deleted.

    delete model;
}

const QLinkedList<QAction*>& Dim2TriGluingsUI::getPacketTypeActions() {
    return triActionList;
}

void Dim2TriGluingsUI::fillToolBar(QToolBar* bar) {
    bar->addAction(actAddTri);
    bar->addAction(actRemoveTri);
}

regina::NPacket* Dim2TriGluingsUI::getPacket() {
    return triangulation;
}

QWidget* Dim2TriGluingsUI::getInterface() {
    return ui;
}

void Dim2TriGluingsUI::commit() {
    model->commitData(triangulation);
    setDirty(false);
}

void Dim2TriGluingsUI::refresh() {
    model->refreshData(triangulation);
    setDirty(false);
}

void Dim2TriGluingsUI::setReadWrite(bool readWrite) {
    model->setReadWrite(readWrite);

    if (readWrite) {
        QAbstractItemView::EditTriggers flags(
            QAbstractItemView::AllEditTriggers);
        flags ^= QAbstractItemView::CurrentChanged;
        edgeTable->setEditTriggers(flags);
    } else
        edgeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QLinkedListIterator<QAction*> it(enableWhenWritable);
    while (it.hasNext())
        (it.next())->setEnabled(readWrite);

    updateRemoveState();
}

void Dim2TriGluingsUI::addTri() {
    model->addTri();
    setDirty(true);
}

void Dim2TriGluingsUI::removeSelectedTris() {
    // Gather together all the triangles to be deleted.
    QModelIndexList sel = edgeTable->selectionModel()->selectedIndexes();
    if (sel.empty()) {
        ReginaSupport::warn(ui, tr("No triangles are selected to remove."));
        return;
    }

    // Selections are contiguous.
    int first, last;
    first = last = sel.front().row();

    int row, i;
    for (i = 1; i < sel.count(); ++i) {
        row = sel[i].row();
        if (row < first)
            first = row;
        if (row > last)
            last = row;
    }

    // Notify the user that triangles will be removed.
    QMessageBox msgBox(ui);
    msgBox.setWindowTitle(tr("Question"));
    msgBox.setIcon(QMessageBox::Question);
    if (first == last) {
        msgBox.setText(tr("Triangle number %1 will be removed.").arg(first));
        msgBox.setInformativeText(tr("Are you sure?"));
    } else {
        msgBox.setText(
            tr("<qt>%1 triangles (numbers %2&ndash;%3) will be removed.</qt>")
            .arg(last - first + 1).arg(first).arg(last));
        msgBox.setInformativeText(tr("Are you sure?"));
    }
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if (msgBox.exec() != QMessageBox::Yes)
        return;

    // Off we go!
    model->removeTri(first, last);
    setDirty(true);
}

void Dim2TriGluingsUI::updateRemoveState() {
    if (model->isReadWrite())
        actRemoveTri->setEnabled(
            ! edgeTable->selectionModel()->selectedIndexes().empty());
    else
        actRemoveTri->setEnabled(false);
}

void Dim2TriGluingsUI::notifyDataChanged() {
    setDirty(true);
}

