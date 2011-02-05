
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  KDE User Interface                                                    *
 *                                                                        *
 *  Copyright (c) 1999-2009, Ben Burton                                   *
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

#include "packettreeview.h"
#include "reginapart.h"
#include "foreign/dehydrationhandler.h"
#include "foreign/importdialog.h"
#include "foreign/isosighandler.h"
#include "foreign/orbhandler.h"
#include "foreign/pdfhandler.h"
#include "foreign/pythonhandler.h"
#include "foreign/reginahandler.h"
#include "foreign/snappeahandler.h"
#include "reginafilter.h"

#include <kencodingfiledialog.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <qtextcodec.h>

void ReginaPart::importDehydration() {
    importFile(DehydrationHandler::instance, 0, i18n(FILTER_ALL),
        i18n("Import Dehydrated Triangulation List"));
}

void ReginaPart::importIsoSig3() {
    importFile(IsoSigHandler::instance3, 0, i18n(FILTER_ALL),
        i18n("Import Isomorphism Signature List (3-D)"));
}

void ReginaPart::importIsoSig4() {
    importFile(IsoSigHandler::instance4, 0, i18n(FILTER_ALL),
        i18n("Import Isomorphism Signature List (4-D)"));
}

void ReginaPart::importPDF() {
    importFile(PDFHandler::instance, 0, i18n(FILTER_PDF),
        i18n("Import PDF Document"));
}

void ReginaPart::importPython() {
    importFile(PythonHandler::instance, 0, i18n(FILTER_PYTHON_SCRIPTS),
        i18n("Import Python Script"));
}

void ReginaPart::importRegina() {
    importFile(ReginaHandler(), 0, i18n(FILTER_REGINA),
        i18n("Import Regina Data File"));
}

void ReginaPart::importSnapPea() {
    importFile(SnapPeaHandler::instance, 0, i18n(FILTER_SNAPPEA),
        i18n("Import SnapPea Triangulation"));
}

void ReginaPart::importOrb() {
    importFile(OrbHandler::instance, 0, i18n(FILTER_ORB),
        i18n("Import Orb or Casson Triangulation"));
}

void ReginaPart::importFile(const PacketImporter& importer,
        PacketFilter* parentFilter, const QString& fileFilter,
        const QString& dialogTitle) {
    if (! checkReadWrite())
        return;

    regina::NPacket* newTree = 0;

    if (importer.offerImportEncoding()) {
        KEncodingFileDialog::Result result =
            KEncodingFileDialog::getOpenFileNameAndEncoding(
            QString::null /* default encoding */,
            QString::null, fileFilter, widget(), dialogTitle);
        if (result.fileNames.empty() || result.fileNames.front().isEmpty())
            return;
        newTree = importer.import(result.fileNames.front(),
            QTextCodec::codecForName(result.encoding), widget());
    } else {
        QString file = KFileDialog::getOpenFileName(QString::null,
            fileFilter, widget(), dialogTitle);
        if (file.isEmpty())
            return;
        newTree = importer.import(file, widget());
    }

    if (newTree) {
        ImportDialog dlg(widget(), newTree, packetTree,
            treeView->selectedPacket(), parentFilter, dialogTitle);
        if (dlg.validate() && dlg.exec() == QDialog::Accepted)
            packetView(newTree, true);
        else
            delete newTree;
    }
}

