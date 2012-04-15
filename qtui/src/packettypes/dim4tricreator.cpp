
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
#include "dim4/dim4exampletriangulation.h"
#include "dim4/dim4triangulation.h"

// UI includes:
#include "dim4tricreator.h"
#include "reginasupport.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QRegExp>
#include <QValidator>
#include <QStackedWidget>

using regina::Dim4ExampleTriangulation;
using regina::Dim4Triangulation;

namespace {
    /**
     * Triangulation type IDs that correspond to indices in the
     * triangulation type combo box.
     */
    enum {
        TRI_EMPTY,
        TRI_EXAMPLE
    };

    /**
     * Example IDs that correspond to indices in the example
     * triangulation combo box.
     */
    enum {
        EXAMPLE_S4,
        EXAMPLE_RP4,
        EXAMPLE_S3xS1,
        EXAMPLE_S3xS1TWISTED,
        EXAMPLE_CAPPELL_SHANESON
    };
}

Dim4TriangulationCreator::Dim4TriangulationCreator() {
    // Set up the basic layout.
    ui = new QWidget();
    QBoxLayout* layout = new QVBoxLayout(ui);

    QBoxLayout* typeArea = new QHBoxLayout();
    layout->addLayout(typeArea);
    QString expln =
        QObject::tr("Specifies what type of triangulation to create.");
    QLabel* label = new QLabel(QObject::tr("Type of triangulation:"), ui);
    label->setWhatsThis(expln);
    typeArea->addWidget(label);
    type = new QComboBox(ui);
    type->setWhatsThis(expln);
    typeArea->addWidget(type, 1);

    layout->addSpacing(5);

    details = new QStackedWidget(ui);
    layout->addWidget(details, 1);

    // Set up the individual types of triangulation.
    // Note that the order in which these options are added to the combo
    // box must correspond precisely to the type IDs defined at the head
    // of this file.
    QWidget* hArea;
    QBoxLayout* hLayout;

    type->insertItem(type->count(), QObject::tr("Empty"));
    details->addWidget(new QWidget());

    type->insertItem(type->count(), QObject::tr("Example triangulation"));
    hArea = new QWidget();
    hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);
    hArea->setLayout(hLayout);
    expln = QObject::tr(
        "<qt>Specifies which particular example triangulation to "
        "create.<p>"
        "A selection of ready-made 4-manifold triangulations is offered "
        "here to help you experiment and see how Regina works.</qt>");
    label = new QLabel(QObject::tr("Example:"));
    label->setWhatsThis(expln);
    hLayout->addWidget(label);
    exampleWhich = new QComboBox(hArea);
    exampleWhich->insertItem(0, QObject::tr("4-sphere"));
    exampleWhich->insertItem(1, QObject::tr("RP4"));
    exampleWhich->insertItem(2, QObject::tr("Product S3 x S1"));
    exampleWhich->insertItem(3, QObject::tr("Twisted product S3 x S1"));
    exampleWhich->insertItem(4, QObject::tr("Cappell-Shaneson knot complement"));
    exampleWhich->setCurrentIndex(0);
    exampleWhich->setWhatsThis(expln);
    hLayout->addWidget(exampleWhich, 1);
    details->addWidget(hArea);

    // Tidy up.
    type->setCurrentIndex(0);
    details->setCurrentIndex((int)0);

    QObject::connect(type, SIGNAL(activated(int)), details,
        SLOT(setCurrentIndex(int)));
}

QWidget* Dim4TriangulationCreator::getInterface() {
    return ui;
}

regina::NPacket* Dim4TriangulationCreator::createPacket(regina::NPacket*,
        QWidget* parentWidget) {
    int typeId = type->currentIndex();
    if (typeId == TRI_EMPTY)
        return new Dim4Triangulation();
    else if (typeId == TRI_EXAMPLE) {
        switch (exampleWhich->currentIndex()) {
            case EXAMPLE_S4:
                return Dim4ExampleTriangulation::fourSphere();
            case EXAMPLE_RP4:
                return Dim4ExampleTriangulation::rp4();
            case EXAMPLE_S3xS1:
                return Dim4ExampleTriangulation::s3xs1();
            case EXAMPLE_S3xS1TWISTED:
                return Dim4ExampleTriangulation::s3xs1Twisted();
            case EXAMPLE_CAPPELL_SHANESON:
                return Dim4ExampleTriangulation::cappellShaneson();
        }

        ReginaSupport::info(parentWidget,
            QObject::tr("Please select an example triangulation."));
        return 0;
    }

    ReginaSupport::info(parentWidget,
        QObject::tr("Please select a triangulation type."));
    return 0;
}

