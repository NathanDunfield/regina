
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
#include "maths/numbertheory.h"
#include "split/nsignature.h"
#include "triangulation/ntriangulation.h"

// UI includes:
#include "ntriangulationcreator.h"

#include <kcombobox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <qcheckbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qregexp.h>
#include <qvalidator.h>
#include <qwhatsthis.h>
#include <qwidgetstack.h>

using regina::NTriangulation;

namespace {
    /**
     * Triangulation type IDs that correspond to indices in the
     * triangulation type combo box.
     */
    const int TRI_EMPTY = 0;
    const int TRI_LAYERED_LENS_SPACE = 1;
    const int TRI_LAYERED_LOOP = 2;
    const int TRI_LAYERED_SOLID_TORUS = 3;
    const int TRI_DEHYDRATION = 4;
    const int TRI_SPLITTING_SURFACE = 5;

    /**
     * Regular expressions describing different sets of parameters.
     */
    QRegExp reLensParams("^[^0-9\\-]*(\\d+)[^0-9\\-]+(\\d+)[^0-9\\-]*$");
    QRegExp reLSTParams(
        "^[^0-9\\-]*(\\d+)[^0-9\\-]+(\\d+)[^0-9\\-]+(\\d+)[^0-9\\-]*$");
    QRegExp reDehydration("^([A-Za-z]+)$");
    QRegExp reSignature("^([\\(\\)\\.,;:\\|\\-A-Za-z]+)$");
}

NTriangulationCreator::NTriangulationCreator() {
    // Set up the basic layout.
    ui = new QWidget();
    QBoxLayout* layout = new QVBoxLayout(ui);

    QBoxLayout* typeArea = new QHBoxLayout(layout, 5);
    QString expln = i18n("Specifies what type of triangulation to create.");
    QLabel* label = new QLabel(i18n("Type of triangulation:"), ui);
    QWhatsThis::add(label, expln);
    typeArea->addWidget(label);
    type = new KComboBox(ui);
    QWhatsThis::add(type, expln);
    typeArea->addWidget(type, 1);

    layout->addSpacing(5);

    details = new QWidgetStack(ui);
    layout->addWidget(details, 1);

    // Set up the individual types of triangulation.
    // Note that the order in which these options are added to the combo
    // box must correspond precisely to the type IDs defined at the head
    // of this file.
    QHBox* hArea;

    type->insertItem(i18n("Empty"));
    details->addWidget(new QWidget(), TRI_EMPTY);

    type->insertItem(i18n("Layered lens space"));
    hArea = new QHBox();
    hArea->setSpacing(5);
    expln = i18n("<qt>The (p,q) parameters of the new "
        "lens space.  These integers must be relatively prime.  Example "
        "parameters are <i>8,3</i>.</qt>");
    QWhatsThis::add(new QLabel(i18n("Parameters (p,q):"), hArea), expln);
    lensParams = new KLineEdit(hArea);
    lensParams->setValidator(new QRegExpValidator(reLensParams, hArea));
    QWhatsThis::add(lensParams, expln);
    hArea->setStretchFactor(lensParams, 1);
    details->addWidget(hArea, TRI_LAYERED_LENS_SPACE);

    type->insertItem(i18n("Layered loop"));
    hArea = new QHBox();
    hArea->setSpacing(5);
    expln = i18n("The number of tetrahedra in the new layered loop.");
    QWhatsThis::add(new QLabel(i18n("Length:"), hArea), expln);
    loopLen = new KLineEdit(hArea);
    QIntValidator* val = new QIntValidator(hArea);
    val->setBottom(1);
    loopLen->setValidator(val);
    QWhatsThis::add(loopLen, expln);
    hArea->setStretchFactor(loopLen, 1);
    loopTwisted = new QCheckBox(i18n("Twisted"), hArea);
    loopTwisted->setChecked(true);
    QWhatsThis::add(loopTwisted, i18n("Specifies whether or not the "
        "new layered loop is twisted."));
    details->addWidget(hArea, TRI_LAYERED_LOOP);

    type->insertItem(i18n("Layered solid torus"));
    hArea = new QHBox();
    hArea->setSpacing(5);
    expln = i18n("<qt>The three parameters of the new "
        "layered solid torus.  These must be relatively prime non-negative "
        "integers, and two of them must add to give the third.  Example "
        "parameters are <i>3,4,7</i>.</qt>");
    QWhatsThis::add(new QLabel(i18n("Parameters (a,b,c):"), hArea), expln);
    lstParams = new KLineEdit(hArea);
    lstParams->setValidator(new QRegExpValidator(reLSTParams, hArea));
    QWhatsThis::add(lstParams, expln);
    hArea->setStretchFactor(lstParams, 1);
    details->addWidget(hArea, TRI_LAYERED_SOLID_TORUS);

    type->insertItem(i18n("From dehydration"));
    hArea = new QHBox();
    hArea->setSpacing(5);
    expln = i18n("<qt>The dehydration string "
        "from which the new triangulation will be created.  An example "
        "dehydration string is <i>baaaade</i>.<p>"
        "Dehydration strings are described in detail in "
        "<i>A census of cusped hyperbolic 3-manifolds</i>, "
        "Callahan, Hildebrand and Weeks, published in "
        "<i>Mathematics of Computation</i> <b>68</b>, 1999.</qt>");
    QWhatsThis::add(new QLabel(i18n("Dehydration string:"), hArea), expln);
    dehydrationString = new KLineEdit(hArea);
    dehydrationString->setValidator(new QRegExpValidator(reDehydration, hArea));
    QWhatsThis::add(dehydrationString, expln);
    hArea->setStretchFactor(dehydrationString, 1);
    details->addWidget(hArea, TRI_DEHYDRATION);

    type->insertItem(i18n("From splitting surface"));
    hArea = new QHBox();
    hArea->setSpacing(5);
    expln = i18n("<qt>The signature of the "
        "splitting surface from which the new triangulation will be "
        "created.  An example signature is <i>(abb)(ac)(c)</i>.<p>"
        "Splitting surface signatures are described in detail in "
        "<i>Minimal triangulations and normal surfaces</i>, "
        "Burton, PhD thesis, available from the Regina website.</qt>");
    QWhatsThis::add(new QLabel(i18n("Signature:"), hArea), expln);
    splittingSignature = new KLineEdit(hArea);
    splittingSignature->setValidator(new QRegExpValidator(reSignature, hArea));
    QWhatsThis::add(splittingSignature, expln);
    hArea->setStretchFactor(splittingSignature, 1);
    details->addWidget(hArea, TRI_SPLITTING_SURFACE);

    // Tidy up.
    type->setCurrentItem(0);
    details->raiseWidget((int)0);

    QObject::connect(type, SIGNAL(activated(int)), details,
        SLOT(raiseWidget(int)));
}

QWidget* NTriangulationCreator::getInterface() {
    return ui;
}

regina::NPacket* NTriangulationCreator::createPacket(regina::NPacket*,
        QWidget* parentWidget) {
    int typeId = type->currentItem();
    if (typeId == TRI_EMPTY)
        return new NTriangulation();
    else if (typeId == TRI_LAYERED_LENS_SPACE) {
        if (! reLensParams.exactMatch(lensParams->text())) {
            KMessageBox::error(parentWidget, i18n("<qt>The lens space "
                "parameters (p,q) must be two non-negative integers.  "
                "Example parameters are <i>8,3</i>.</qt>"));
            return 0;
        }

        unsigned long p = reLensParams.cap(1).toULong();
        unsigned long q = reLensParams.cap(2).toULong();

        if (p == 0 && q == 0) {
            KMessageBox::error(parentWidget, i18n("At least one of the "
                "two lens space parameters must be strictly positive."));
            return 0;
        }
        if (p < q && ! (p == 0 && q == 1)) {
            KMessageBox::error(parentWidget, i18n("<qt>The second lens space "
                "parameter must be smaller than the first.  For instance, "
                "the parameters <i>8,3</i> are valid whereas <i>3,8</i> "
                "are not."));
            return 0;
        }
        if (regina::gcd(p, q) != 1) {
            KMessageBox::error(parentWidget, i18n("The two lens space "
                "parameters must be relatively prime."));
            return 0;
        }

        NTriangulation* ans = new NTriangulation();
        ans->insertLayeredLensSpace(p, q);
        return ans;
    } else if (typeId == TRI_LAYERED_LOOP) {
        unsigned long len = loopLen->text().toULong();
        if (len == 0) {
            KMessageBox::error(parentWidget, i18n("The layered loop length "
                "must be a strictly positive integer."));
            return 0;
        }

        NTriangulation* ans = new NTriangulation();
        ans->insertLayeredLoop(len, loopTwisted->isChecked());
        return ans;
    } else if (typeId == TRI_LAYERED_SOLID_TORUS) {
        if (! reLSTParams.exactMatch(lstParams->text())) {
            KMessageBox::error(parentWidget, i18n("<qt>The layered solid "
                "torus parameters (a,b,c) must be three non-negative "
                "integers.  Example parameters are <i>3,4,7</i>.</qt>"));
            return 0;
        }

        unsigned long a = reLSTParams.cap(1).toULong();
        unsigned long b = reLSTParams.cap(2).toULong();
        unsigned long c = reLSTParams.cap(3).toULong();

        if (a == 0 && b == 0 && c == 0) {
            KMessageBox::error(parentWidget, i18n("At least one of the "
                "three layered solid torus parameters must be strictly "
                "positive."));
            return 0;
        }
        if (regina::gcd(a, b) != 1) {
            KMessageBox::error(parentWidget, i18n("The three layered "
                "solid torus parameters must be relatively prime."));
            return 0;
        }

        if (a + b == c) {
            NTriangulation* ans = new NTriangulation();
            if (a <= b)
                ans->insertLayeredSolidTorus(a, b);
            else
                ans->insertLayeredSolidTorus(b, a);
            return ans;
        } else if (a + c == b) {
            NTriangulation* ans = new NTriangulation();
            if (a <= c)
                ans->insertLayeredSolidTorus(a, c);
            else
                ans->insertLayeredSolidTorus(c, a);
            return ans;
        } else if (b + c == a) {
            NTriangulation* ans = new NTriangulation();
            if (b <= c)
                ans->insertLayeredSolidTorus(b, c);
            else
                ans->insertLayeredSolidTorus(c, b);
            return ans;
        } else {
            KMessageBox::error(parentWidget, i18n("<qt>Two of the layered "
                "solid torus parameters must add to give the third.  "
                "For instance, the parameters <i>3,4,7</i> are valid "
                "whereas the parameters <i>3,4,5</i> are not.</qt>"));
            return 0;
        }
    } else if (typeId == TRI_DEHYDRATION) {
        if (! reDehydration.exactMatch(dehydrationString->text())) {
            KMessageBox::error(parentWidget, i18n("<qt>The dehydration "
                "string must be a sequence of letters of the alphabet.  "
                "An example dehydration string is <i>baaaade</i>.<p>"
                "Dehydration strings are described in detail in "
                "<i>A census of cusped hyperbolic 3-manifolds</i>, "
                "Callahan, Hildebrand and Weeks, published in "
                "<i>Mathematics of Computation</i> <b>68</b>, 1999.</qt>"));
            return 0;
        }

        NTriangulation* ans = new NTriangulation();
        if (! ans->insertRehydration(reDehydration.cap(1).ascii())) {
            delete ans;
            KMessageBox::error(parentWidget, i18n("<qt>The given "
                "dehydration string was not valid.<p>"
                "Dehydration strings are described in detail in "
                "<i>A census of cusped hyperbolic 3-manifolds</i>, "
                "Callahan, Hildebrand and Weeks, published in "
                "<i>Mathematics of Computation</i> <b>68</b>, 1999.</qt>"));
            return 0;
        }
        return ans;
    } else if (typeId == TRI_SPLITTING_SURFACE) {
        if (! reSignature.exactMatch(splittingSignature->text())) {
            KMessageBox::error(parentWidget, i18n("<qt>The splitting "
                "surface signature must be a sequence of cycles.  "
                "Cycles should consist of letters of the alphabet and "
                "should be separated by brackets, periods or commas.  "
                "An example splitting surface signature is "
                "<i>(abb)(ac)(c)</i>.<p>"
                "Splitting surface signatures are described in detail in "
                "<i>Minimal triangulations and normal surfaces</i>, "
                "Burton, PhD thesis, available from the Regina website.</qt>"));
            return 0;
        }

        regina::NSignature* sig = regina::NSignature::parse(
            reSignature.cap(1).ascii());
        if (! sig) {
            KMessageBox::error(parentWidget, i18n("<qt>The given "
                "splitting surface signature was not valid.<p>"
                "Splitting surface signatures are described in detail in "
                "<i>Minimal triangulations and normal surfaces</i>, "
                "Burton, PhD thesis, available from the Regina website.</qt>"));
            return 0;
        }
        NTriangulation* ans = sig->triangulate();
        delete sig;
        return ans;
    }

    KMessageBox::error(parentWidget, i18n("No triangulation type has "
        "been selected."));
    return 0;
}

