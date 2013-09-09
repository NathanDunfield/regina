
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
#include "angle/nanglestructurelist.h"
#include "progress/nprogresstracker.h"
#include "triangulation/ntriangulation.h"

// UI includes:
#include "nanglestructurecreator.h"
#include "../progressdialogs.h"
#include "reginasupport.h"

#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QWhatsThis>

NAngleStructureCreator::NAngleStructureCreator() {
    // Set up the basic layout.
    ui = new QWidget();
    QBoxLayout* layout = new QVBoxLayout(ui);

    tautOnly = new QCheckBox(ui->tr("Taut angle structures only"), ui);
    tautOnly->setChecked(false);
    tautOnly->setWhatsThis(ui->tr("If you check this box, only "
        "taut angle structures will be enumerated (that is, angle structures "
        "in which every angle is 0 or Pi).  "
        "This is typically much faster than a full enumeration of all "
        "vertex angle structures."));
    layout->addWidget(tautOnly);
}

QWidget* NAngleStructureCreator::getInterface() {
    return ui;
}

QString NAngleStructureCreator::parentPrompt() {
    return ui->tr("Triangulation:");
}

QString NAngleStructureCreator::parentWhatsThis() {
    return ui->tr("The triangulation that will contain your angle structures.");
}

regina::NPacket* NAngleStructureCreator::createPacket(
        regina::NPacket* parentPacket, QWidget* parentWidget) {
    if (parentPacket->getPacketType() != regina::NTriangulation::packetType) {
        ReginaSupport::sorry(ui,
            ui->tr("The selected parent is not a 3-manifold triangulation."),
            ui->tr("Angle structures must live within a 3-manifold "
            "triangulation.  Please select the corresponding triangulation "
            "as the location in the tree for your new angle structure list."));
        return 0;
    }

    regina::NProgressTracker tracker;
    ProgressDialogNumeric dlg(&tracker,
        ui->tr("Enumerating vertex angle structures..."), parentWidget);

    regina::NAngleStructureList* ans = regina::NAngleStructureList::enumerate(
            dynamic_cast<regina::NTriangulation*>(parentPacket),
            tautOnly->isChecked(), &tracker);

    if (dlg.run())
        return ans;
    else {
        delete ans;
        ReginaSupport::info(parentWidget,
            ui->tr("The angle structure enumeration was cancelled."));
        return 0;
    }
}

void NAngleStructureCreator::explainNoParents() {
    ReginaSupport::sorry(ui,
        ui->tr("There are no triangulations to work with."),
        ui->tr("Angle structures must live within a 3-manifold "
        "triangulation.  Please add some triangulations to your file and "
        "try again."));
}

