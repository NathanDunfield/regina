
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

#include "packet/npacket.h"

#include "importdialog.h"
#include "../packetchooser.h"
#include "../packetfilter.h"

#include <klocale.h>
#include <kmessagebox.h>
#include <qframe.h>
#include <QHBoxLayout>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qwhatsthis.h>

ImportDialog::ImportDialog(QWidget* parent, regina::NPacket* importedData,
        regina::NPacket* packetTree, regina::NPacket* defaultParent,
        PacketFilter* useFilter, const QString& dialogTitle) :
        KDialog(parent), // dialogTitle, Ok|Cancel, Ok, parent),
        tree(packetTree), newTree(importedData) {
    setCaption(dialogTitle);
    setButtons(KDialog::Ok | KDialog::Cancel);

    QWidget* page = new QWidget(this);
    setMainWidget(page);
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0); // Margins come from the dialog.

    QHBoxLayout* parentStrip = new QHBoxLayout();
    layout->addLayout(parentStrip);
    QString expln = i18n("Select where in the packet tree "
        "the new data should be imported.  The imported data will be "
        "made a new child of the selected packet.");
    QLabel* l = new QLabel(i18n("Import beneath:"));
    l->setWhatsThis(expln);
    parentStrip->addWidget(l);
    chooser = new PacketChooser(tree, useFilter, false, defaultParent);
    chooser->setWhatsThis(expln);
    parentStrip->addWidget(chooser, 1);

    QHBoxLayout* labelStrip = new QHBoxLayout();
    layout->addLayout(labelStrip);
    expln = i18n("Select a packet label for the new "
        "imported data.  This will become the label of the first packet that "
        "is imported.");
    l = new QLabel(i18n("Label:"));
    l->setWhatsThis(expln);
    labelStrip->addWidget(l);
    label = new QLineEdit(
        tree->makeUniqueLabel(newTree->getPacketLabel()).c_str());
    label->setWhatsThis(expln);
    labelStrip->addWidget(label, 1);

    layout->addStretch(1);

    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
}

bool ImportDialog::validate() {
    if (chooser->hasPackets())
        return true;
    KMessageBox::sorry(this, i18n(
        "No suitable parent packets could be found for the imported data.\n"
        "Some packets have particular requirements of their parents.  "
        "For instance, a list of normal surfaces or angle structures must "
        "be imported beneath the triangulation in which they live.\n"
        "See the users' handbook for further information."));
    return false;
}

void ImportDialog::slotOk() {
    // Get the parent packet.
    regina::NPacket* parentPacket = chooser->selectedPacket();
    if (! parentPacket) {
        KMessageBox::error(this, i18n(
            "No parent packet has been selected."));
        return;
    }
    PacketFilter* filter = chooser->getFilter();
    if (filter && ! filter->accept(parentPacket)) {
        KMessageBox::error(this, i18n(
            "The packet %1 is not capable of acting as a parent for "
            "the imported data.").arg(parentPacket->getPacketLabel().c_str()));
        return;
    }

    // Check the label.
    QString useLabel = label->text().trimmed();
    const char* useLabelStr = useLabel.toAscii().constData();
    if (useLabel.isEmpty()) {
        KMessageBox::error(this, i18n("The packet label cannot be empty."));
        return;
    }
    if (tree->findPacketLabel(useLabelStr)) {
        KMessageBox::error(this, i18n(
            "There is already a packet labelled %1.").arg(useLabel));
        label->setText(tree->makeUniqueLabel(useLabelStr).c_str());
        return;
    }

    // Insert the imported data into the packet tree.
    newTree->setPacketLabel(useLabelStr);
    newTree->makeUniqueLabels(tree);
    parentPacket->insertChildLast(newTree);
}

