
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  KDE User Interface                                                    *
 *                                                                        *
 *  Copyright (c) 1999-2003, Ben Burton                                   *
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
 *  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,        *
 *  MA 02111-1307, USA.                                                   *
 *                                                                        *
 **************************************************************************/

/* end stub */

// Regina core includes:
#include "algebra/ngrouppresentation.h"
#include "triangulation/ntriangulation.h"

// UI includes:
#include "ntrialgebra.h"

#include <klistview.h>
#include <klocale.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>

using regina::NPacket;
using regina::NTriangulation;

NTriAlgebraUI::NTriAlgebraUI(regina::NTriangulation* packet,
        PacketTabbedUI* useParentUI) : PacketViewerTab(useParentUI),
        tri(packet) {
    ui = new QWidget();
    QBoxLayout* layout = new QHBoxLayout(ui);

    // Set up the homology area.
    QBoxLayout* homologyArea = new QVBoxLayout(layout, 5);
    layout->setStretchFactor(homologyArea, 1);

    homologyArea->addStretch(2);

    QLabel* label = new QLabel(i18n("Homology"), ui);
    label->setAlignment(Qt::AlignCenter);
    homologyArea->addWidget(label);

    homologyArea->addStretch(1);

    QGridLayout* homologyGrid = new QGridLayout(homologyArea, 5, 4);
    homologyGrid->setColStretch(0, 1);
    homologyGrid->setColStretch(3, 1);

    label = new QLabel(i18n("H1(M)"), ui);
    homologyGrid->addWidget(label, 0, 1);
    label = new QLabel(i18n("H1(M, Bdry M)"), ui);
    homologyGrid->addWidget(label, 1, 1);
    label = new QLabel(i18n("H1(Bdry M)"), ui);
    homologyGrid->addWidget(label, 2, 1);
    label = new QLabel(i18n("H2(M)"), ui);
    homologyGrid->addWidget(label, 3, 1);
    label = new QLabel(i18n("H2(M ; Z_2)"), ui);
    homologyGrid->addWidget(label, 4, 1);

    H1 = new QLabel(ui);
    homologyGrid->addWidget(H1, 0, 2);
    H1Rel = new QLabel(ui);
    homologyGrid->addWidget(H1Rel, 1, 2);
    H1Bdry = new QLabel(ui);
    homologyGrid->addWidget(H1Bdry, 2, 2);
    H2 = new QLabel(ui);
    homologyGrid->addWidget(H2, 3, 2);
    H2Z2 = new QLabel(ui);
    homologyGrid->addWidget(H2Z2, 4, 2);

    homologyArea->addStretch(2);

    // Add a central divider.
    QFrame* divider = new QFrame(ui);
    divider->setFrameStyle(QFrame::VLine | QFrame::Sunken);
    layout->addWidget(divider);

    // Set up the fundamental group area.

    QBoxLayout* fundArea = new QVBoxLayout(layout, 5);
    layout->setStretchFactor(fundArea, 1);

    fundArea->addStretch(2);

    label = new QLabel(i18n("Fundamental Group"), ui);
    label->setAlignment(Qt::AlignCenter);
    fundArea->addWidget(label);

    fundArea->addStretch(1);

    fundName = new QLabel(ui);
    fundName->setAlignment(Qt::AlignCenter);
    fundArea->addWidget(fundName);

    fundArea->addStretch(1);

    QBoxLayout* wideFundPresArea = new QHBoxLayout(fundArea);
    wideFundPresArea->addStretch(1);

    QBoxLayout* fundPresArea = new QVBoxLayout(wideFundPresArea);
    fundGens = new QLabel(ui);
    fundPresArea->addWidget(fundGens);
    fundRelCount = new QLabel(ui);
    fundPresArea->addWidget(fundRelCount);
    fundRels = new KListView(ui);
    fundRels->header()->hide();
    fundRels->addColumn(QString::null);
    fundRels->setSorting(-1);
    fundRels->setSelectionMode(QListView::NoSelection);
    fundPresArea->addWidget(fundRels, 1);

    wideFundPresArea->addStretch(1);

    fundArea->addStretch(2);
}

regina::NPacket* NTriAlgebraUI::getPacket() {
    return tri;
}

QWidget* NTriAlgebraUI::getInterface() {
    return ui;
}

void NTriAlgebraUI::refresh() {
    H1->setText(tri->getHomologyH1().toString().c_str());
    if (tri->isValid()) {
        H1Rel->setText(tri->getHomologyH1Rel().toString().c_str());
        H1Bdry->setText(tri->getHomologyH1Bdry().toString().c_str());
        H2->setText(tri->getHomologyH2().toString().c_str());
        unsigned long coeffZ2 = tri->getHomologyH2Z2();
        if (coeffZ2 == 0)
            H2Z2->setText("0");
        else if (coeffZ2 == 1)
            H2Z2->setText("Z_2");
        else
            H2Z2->setText(QString::number(coeffZ2) + " Z_2");
    } else {
        QString msg(i18n("Invalid Triangulation"));
        H1Rel->setText(msg);
        H1Bdry->setText(msg);
        H2->setText(msg);
        H2Z2->setText(msg);
    }

    if (tri->getNumberOfComponents() <= 1) {
        const regina::NGroupPresentation& pres = tri->getFundamentalGroup();

        std::string name = pres.recogniseGroup();
        if (name.length())
            fundName->setText(name.c_str());
        else
            fundName->setText(i18n("Not recognised"));

        unsigned long nGens = pres.getNumberOfGenerators();
        if (nGens == 0)
            fundGens->setText(i18n("No generators"));
        else if (nGens == 1)
            fundGens->setText(i18n("1 generator: g0"));
        else if (nGens == 2)
            fundGens->setText(i18n("2 generators: g0, g1"));
        else
            fundGens->setText(i18n("%1 generators: g0 ... g%2").
                arg(nGens).arg(nGens - 1));

        unsigned long nRels = pres.getNumberOfRelations();
        if (nRels == 0)
            fundRelCount->setText(i18n("No relations"));
        else if (nRels == 1)
            fundRelCount->setText(i18n("1 relation:"));
        else
            fundRelCount->setText(i18n("%1 relations:").arg(nRels));

        fundRels->clear();
        for (unsigned long i = 0; i < nRels; i++)
            new KListViewItem(fundRels,
                QString("1 = ") + pres.getRelation(i).toString().c_str());
    } else {
        fundName->setText(i18n("Cannot calculate\n(disconnected triang.)"));
        fundGens->setText(i18n("Generators"));
        fundRelCount->setText(i18n("Relations"));
        fundRels->clear();
    }
}

void NTriAlgebraUI::editingElsewhere() {
    QString msg(i18n("Editing..."));

    H1->setText(msg);
    H1Rel->setText(msg);
    H1Bdry->setText(msg);
    H2->setText(msg);
    H2Z2->setText(msg);

    fundName->setText(msg);
    fundGens->setText(i18n("Generators"));
    fundRelCount->setText(i18n("Relations"));
    fundRels->clear();
}

