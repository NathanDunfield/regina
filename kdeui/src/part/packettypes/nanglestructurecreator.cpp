
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  KDE User Interface                                                    *
 *                                                                        *
 *  Copyright (c) 1999-2011, Ben Burton                                   *
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

// Regina core includes:
#include "angle/nanglestructurelist.h"
#include "progress/nprogressmanager.h"
#include "progress/nprogressmanager.h"
#include "triangulation/ntriangulation.h"

// UI includes:
#include "nanglestructurecreator.h"
#include "../progressdialogs.h"

#include <klocale.h>
#include <kmessagebox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qwhatsthis.h>

NAngleStructureCreator::NAngleStructureCreator() {
    // Set up the basic layout.
    ui = new QWidget();
    QBoxLayout* layout = new QVBoxLayout(ui);

    tautOnly = new QCheckBox(i18n("Taut structures only"), ui);
    tautOnly->setChecked(false);
    QWhatsThis::add(tautOnly, i18n("If you check this box, only "
        "taut structures will be enumerated (that is, angle structures "
        "in which every angle is 0 or Pi).  "
        "This is typically much faster than a full enumeration of all "
        "vertex angle structures."));
    layout->addWidget(tautOnly);
}

QWidget* NAngleStructureCreator::getInterface() {
    return ui;
}

regina::NPacket* NAngleStructureCreator::createPacket(
        regina::NPacket* parentPacket, QWidget* parentWidget) {
    if (parentPacket->getPacketType() != regina::NTriangulation::packetType) {
        KMessageBox::error(parentWidget, i18n(
            "Angle structure lists can only be created directly beneath "
            "triangulations."));
        return 0;
    }

    regina::NProgressManager manager;
    ProgressDialogNumeric dlg(&manager, i18n("Angle Structure Enumeration"),
        i18n("Enumerating vertex angle structures..."), parentWidget);

    regina::NAngleStructureList* ans = regina::NAngleStructureList::enumerate(
            dynamic_cast<regina::NTriangulation*>(parentPacket),
            tautOnly->isChecked(), &manager);

    if (dlg.run())
        return ans;
    else {
        delete ans;
        KMessageBox::information(parentWidget,
            i18n("The angle structure enumeration was cancelled."));
        return 0;
    }
}

