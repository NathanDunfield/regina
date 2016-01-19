
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

// Regina core includes:
#include "snappea/nsnappeatriangulation.h"

// UI includes:
#include "nsnappeaui.h"
#include "nsnappeaalgebra.h"
#include "nsnappeafile.h"
#include "nsnappeagluings.h"
#include "nsnappeashapes.h"
#include "ntricomposition.h"
#include "ntriskeleton.h"
#include "ntrisurfaces.h"
#include "packeteditiface.h"
#include "reginamain.h"

#include <QLabel>
#include <QToolBar>
#include <QVBoxLayout>

using regina::NPacket;
using regina::NSnapPeaTriangulation;

NSnapPeaUI::NSnapPeaUI(regina::NSnapPeaTriangulation* packet,
        PacketPane* newEnclosingPane) :
        PacketTabbedUI(newEnclosingPane,
            ReginaPrefSet::global().tabSnapPeaTri) {
    NSnapPeaHeaderUI* header = new NSnapPeaHeaderUI(packet, this);
    shapes = new NSnapPeaShapesUI(packet, this,
        newEnclosingPane->isReadWrite());
    gluings = new NSnapPeaGluingsUI(packet, this);
    skeleton = new NTriSkeletonUI(packet, this);
    algebra = new NSnapPeaAlgebraUI(packet, this);

    shapes->fillToolBar(header->getToolBar());

    addHeader(header);
    addTab(shapes, QObject::tr("S&hapes && Cusps"));
    addTab(gluings, QObject::tr("&Gluings"));
    addTab(skeleton, QObject::tr("&Skeleton"));
    addTab(algebra, QObject::tr("&Algebra"));
    addTab(new NTriCompositionUI(packet, this), QObject::tr("&Composition"));
    addTab(new NTriSurfacesUI(packet, this), QObject::tr("&Recognition"));
    addTab(new NSnapPeaFileUI(packet, this), QObject::tr("&File"));

    editIface = new PacketEditTabbedUI(this);
}

NSnapPeaUI::~NSnapPeaUI() {
    delete editIface;
}

const QLinkedList<QAction*>& NSnapPeaUI::getPacketTypeActions() {
    return shapes->getPacketTypeActions();
}

QString NSnapPeaUI::getPacketMenuText() const {
    return QObject::tr("&SnapPea Triangulation");
}

NSnapPeaHeaderUI::NSnapPeaHeaderUI(regina::NSnapPeaTriangulation* packet,
        PacketTabbedUI* useParentUI) : PacketViewerTab(useParentUI),
        tri(packet) {
    ui = new QWidget();
    QBoxLayout* uiLayout = new QVBoxLayout(ui);
    uiLayout->setContentsMargins(0, 0, 0, 0);

    bar = new QToolBar(ui);
    bar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    uiLayout->addWidget(bar);

    header = new QLabel();
    header->setAlignment(Qt::AlignCenter);
    header->setMargin(10);
    header->setWhatsThis(QObject::tr("Displays a few basic properties of the "
        "triangulation, such as orientability and solution type."));
    uiLayout->addWidget(header);
}

regina::NPacket* NSnapPeaHeaderUI::getPacket() {
    return tri;
}

QWidget* NSnapPeaHeaderUI::getInterface() {
    return ui;
}

void NSnapPeaHeaderUI::refresh() {
    header->setText(summaryInfo(tri));
}

QString NSnapPeaHeaderUI::summaryInfo(regina::NSnapPeaTriangulation* tri) {
    if (tri->isNull())
        return QObject::tr("Null triangulation (no SnapPea data)");
    if (tri->isEmpty())
        return QObject::tr("Empty");

    if (! tri->isValid())
        return QObject::tr("INVALID TRIANGULATION!");

    QString msg;

    if (tri->isOrientable())
        msg += QObject::tr("Orientable, ");
    else
        msg += QObject::tr("Non-orientable, ");

    unsigned nFilled = tri->countFilledCusps();
    unsigned nComplete = tri->countCompleteCusps();

    if (nFilled + nComplete == 1) {
        msg += QObject::tr("1 cusp, ");
        if (nFilled)
            msg += QObject::tr("filled\n");
        else
            msg += QObject::tr("complete\n");
    } else if (nFilled == 0) {
        msg += QObject::tr("%1 cusps, all complete\n").arg(nComplete);
    } else if (nComplete == 0) {
        msg += QObject::tr("%1 cusps, all filled\n").arg(nFilled);
    } else {
        msg += QObject::tr("%1 cusps, %2 filled\n")
            .arg(nFilled + nComplete).arg(nFilled);
    }

    switch (tri->solutionType()) {
        case NSnapPeaTriangulation::not_attempted:
            msg += QObject::tr("Solution not attempted");
            break;
        case NSnapPeaTriangulation::geometric_solution:
            if (tri->volumeZero())
                msg += QObject::tr("Volume approximately zero\n");
            else
                msg += QObject::tr("Volume: %1\n").arg(tri->volume());
            msg += QObject::tr("Tetrahedra positively oriented");
            break;
        case NSnapPeaTriangulation::nongeometric_solution:
            if (tri->volumeZero())
                msg += QObject::tr("Volume approximately zero\n");
            else
                msg += QObject::tr("Volume: %1\n").arg(tri->volume());
            msg += QObject::tr("Contains negatively oriented tetrahedra");
            break;
        case NSnapPeaTriangulation::flat_solution:
            if (tri->volumeZero())
                msg += QObject::tr("Volume approximately zero\n");
            else
                msg += QObject::tr("Volume: %1\n").arg(tri->volume());
            msg += QObject::tr("All tetrahedra flat");
            break;
        case NSnapPeaTriangulation::degenerate_solution:
            if (tri->volumeZero())
                msg += QObject::tr("Volume approximately zero\n");
            else
                msg += QObject::tr("Volume: %1\n").arg(tri->volume());
            msg += QObject::tr("Contains degenerate tetrahedra");
            break;
        case NSnapPeaTriangulation::other_solution:
            msg += QObject::tr("Unrecognised solution type");
            break;
        case NSnapPeaTriangulation::no_solution:
            msg += QObject::tr("No solution found");
            break;
        default:
            msg += QObject::tr("INVALID SOLUTION TYPE");
    }

    return msg;
}

