
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

#include "regina-config.h"

#include "file/nfileinfo.h"
#include "file/nglobaldirs.h"

#include "coordinatechooser.h"
#include "reginafilter.h"
#include "reginamain.h"
#include "reginapref.h"

#include <kcombobox.h>
#include <kconfig.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <klistview.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <ktexteditor/editorchooser.h>
#include <ktip.h>
#include <qcheckbox.h>
#include <qfile.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qvalidator.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/**
 * Note that QTextEdit does not seem to support word wrap in LogText
 * mode.  Word wrap configuration has therefore been commented out of
 * the preferences dialog for the time being.
 */

namespace {
    /**
     * A list view item for a single ReginaFilePref.
     */
    class ReginaFilePrefItem : public KListViewItem {
        private:
            /**
             * Pixmaps that will be loaded when we first need them.
             * Strictly speaking we have a memory leak here, since these
             * pixmaps are never destroyed.  However, there's only ever
             * two of them.
             */
            static QPixmap* activePixmap;
            static QPixmap* inactivePixmap;

            ReginaFilePref data;

        public:
            /**
             * There won't be many censuses overall so we just add the
             * item to the end of the list (which requires traversing
             * the entire list).
             */
            ReginaFilePrefItem(QListView* parent,
                    const ReginaFilePref& newData) :
                    KListViewItem(parent, parent->lastItem()), data(newData) {
            }

            const ReginaFilePref& getData() const {
                return data;
            }

            bool activateFile() {
                if (data.active)
                    return false;

                data.active = true;
                repaint();
                return true;
            }

            bool deactivateFile() {
                if (! data.active)
                    return false;

                data.active = false;
                repaint();
                return true;
            }

            QString text(int) const {
                return data.filename;
            }

            const QPixmap* pixmap(int) const {
                // if (! activePixmap)
                //     activePixmap = new QPixmap(SmallIcon("ok"));
                if (! inactivePixmap)
                    inactivePixmap = new QPixmap(SmallIcon("no"));
                return (data.active ? activePixmap : inactivePixmap);
            }
    };

    QPixmap* ReginaFilePrefItem::activePixmap = 0;
    QPixmap* ReginaFilePrefItem::inactivePixmap = 0;
}

ReginaPreferences::ReginaPreferences(ReginaMain* parent) :
        KDialogBase(IconList, i18n("Regina Preferences"),
        Help|Ok|Apply|Cancel, Ok), mainWindow(parent),
        prefSet(mainWindow->getPreferences()) {
    // Construct the individual preferences pages.
    QVBox* frame = addVBoxPage(i18n("General"), i18n("General Options"),
        BarIcon("regina", KIcon::SizeMedium));
    generalPrefs = new ReginaPrefGeneral(frame);

    frame = addVBoxPage(i18n("Triangulation"), i18n("Triangulation Options"),
        BarIcon("packet_triangulation", KIcon::SizeMedium));
    triPrefs = new ReginaPrefTri(frame);

    frame = addVBoxPage(i18n("Surfaces"), i18n("Normal Surface Options"),
        BarIcon("packet_surfaces", KIcon::SizeMedium));
    surfacePrefs = new ReginaPrefSurfaces(frame);

    frame = addVBoxPage(i18n("PDF"), i18n("PDF Options"),
        BarIcon("packet_pdf", KIcon::SizeMedium));
    pdfPrefs = new ReginaPrefPDF(frame);

    frame = addVBoxPage(i18n("Census"), i18n("Census Options"),
        BarIcon("view_text", KIcon::SizeMedium));
    censusPrefs = new ReginaPrefCensus(frame);

    frame = addVBoxPage(i18n("Python"), i18n("Python Options"),
        BarIcon("openterm", KIcon::SizeMedium));
    pythonPrefs = new ReginaPrefPython(frame);

    frame = addVBoxPage(i18n("SnapPea"), i18n("SnapPea Options"),
        BarIcon("snappea", KIcon::SizeMedium));
    snapPeaPrefs = new ReginaPrefSnapPea(frame);

    // Read the current preferences from the main window.
    generalPrefs->cbAutoDock->setChecked(prefSet.autoDock);
    generalPrefs->cbAutoFileExtension->setChecked(prefSet.autoFileExtension);
    generalPrefs->cbDisplayIcon->setChecked(prefSet.displayIcon);
    generalPrefs->cbDisplayTagsInTree->setChecked(prefSet.displayTagsInTree);
    generalPrefs->editTreeJumpSize->setText(
        QString::number(prefSet.treeJumpSize));

    generalPrefs->cbTipOfDay->setChecked(
        KConfigGroup(KGlobal::config(), "TipOfDay").
        readBoolEntry("RunOnStart", true));

    triPrefs->comboEditMode->setCurrentItem(
        prefSet.triEditMode == ReginaPrefSet::DirectEdit ? 0 : 1);

    switch (prefSet.triInitialTab) {
        case ReginaPrefSet::Skeleton:
            triPrefs->comboInitialTab->setCurrentItem(1); break;
        case ReginaPrefSet::Algebra:
            triPrefs->comboInitialTab->setCurrentItem(2); break;
        case ReginaPrefSet::Composition:
            triPrefs->comboInitialTab->setCurrentItem(3); break;
        case ReginaPrefSet::Surfaces:
            triPrefs->comboInitialTab->setCurrentItem(4); break;
        case ReginaPrefSet::SnapPea:
            triPrefs->comboInitialTab->setCurrentItem(5); break;
        default:
            triPrefs->comboInitialTab->setCurrentItem(0); break;
    }

    switch (prefSet.triInitialSkeletonTab) {
        case ReginaPrefSet::FacePairingGraph:
            triPrefs->comboInitialSkeletonTab->setCurrentItem(1); break;
        default:
            triPrefs->comboInitialSkeletonTab->setCurrentItem(0); break;
    }

    switch (prefSet.triInitialAlgebraTab) {
        case ReginaPrefSet::FundGroup:
            triPrefs->comboInitialAlgebraTab->setCurrentItem(1); break;
        case ReginaPrefSet::TuraevViro:
            triPrefs->comboInitialAlgebraTab->setCurrentItem(2); break;
        case ReginaPrefSet::CellularInfo:
            triPrefs->comboInitialAlgebraTab->setCurrentItem(3); break;
        default:
            triPrefs->comboInitialAlgebraTab->setCurrentItem(0); break;
    }

    triPrefs->editSurfacePropsThreshold->setText(
        QString::number(prefSet.triSurfacePropsThreshold));
    triPrefs->editGAPExec->setText(prefSet.triGAPExec);
    triPrefs->editGraphvizExec->setText(prefSet.triGraphvizExec);

    surfacePrefs->chooserCreationCoords->setCurrentSystem(
        prefSet.surfacesCreationCoords);

    switch (prefSet.surfacesInitialTab) {
        case ReginaPrefSet::Coordinates:
            surfacePrefs->comboInitialTab->setCurrentItem(1); break;
        case ReginaPrefSet::Matching:
            surfacePrefs->comboInitialTab->setCurrentItem(2); break;
        case ReginaPrefSet::Compatibility:
            surfacePrefs->comboInitialTab->setCurrentItem(3); break;
        default:
            surfacePrefs->comboInitialTab->setCurrentItem(0); break;
    }

    pdfPrefs->cbEmbed->setChecked(prefSet.pdfEmbed);
    pdfPrefs->editExternalViewer->setText(prefSet.pdfExternalViewer);
    pdfPrefs->cbAutoClose->setChecked(prefSet.pdfAutoClose);

    for (ReginaFilePrefList::const_iterator it = prefSet.censusFiles.begin();
            it != prefSet.censusFiles.end(); it++)
        new ReginaFilePrefItem(censusPrefs->listFiles, *it);
    censusPrefs->updateActiveCount();

    pythonPrefs->cbAutoIndent->setChecked(prefSet.pythonAutoIndent);
    pythonPrefs->editSpacesPerTab->setText(
        QString::number(prefSet.pythonSpacesPerTab));
    // pythonPrefs->cbWordWrap->setChecked(prefSet.pythonWordWrap);

    for (ReginaFilePrefList::const_iterator it =
            prefSet.pythonLibraries.begin();
            it != prefSet.pythonLibraries.end(); it++)
        new ReginaFilePrefItem(pythonPrefs->listFiles, *it);
    pythonPrefs->updateActiveCount();

    snapPeaPrefs->cbClosed->setChecked(prefSet.snapPeaClosed);

    // Finish off.
    setHelp("options", "regina");
}

int ReginaPreferences::exec() {
    // Apply changes if OK was pressed.
    int ret = KDialogBase::exec();
    if (ret)
        slotApply();
    return ret;
}

void ReginaPreferences::slotApply() {
    // Propagate changes to the main window.
    bool ok;
    unsigned uintVal;
    QString strVal;

    prefSet.autoDock = generalPrefs->cbAutoDock->isChecked();
    prefSet.autoFileExtension = generalPrefs->cbAutoFileExtension->isChecked();
    prefSet.displayIcon = generalPrefs->cbDisplayIcon->isChecked();
    prefSet.displayTagsInTree = generalPrefs->cbDisplayTagsInTree->isChecked();
    KTipDialog::setShowOnStart(generalPrefs->cbTipOfDay->isChecked());

    uintVal = generalPrefs->editTreeJumpSize->text().toUInt(&ok);
    if (ok && uintVal > 0)
        prefSet.treeJumpSize = uintVal;
    else {
        KMessageBox::error(this, i18n("The packet tree jump size "
            "must be a positive integer.  This is the number of steps "
            "that a packet moves when Jump Up or Jump Down is selected."));
        generalPrefs->editTreeJumpSize->setText(
            QString::number(prefSet.treeJumpSize));
    }

    prefSet.triEditMode = (triPrefs->comboEditMode->currentItem() == 0 ?
        ReginaPrefSet::DirectEdit : ReginaPrefSet::Dialog);

    switch (triPrefs->comboInitialTab->currentItem()) {
        case 1:
            prefSet.triInitialTab = ReginaPrefSet::Skeleton; break;
        case 2:
            prefSet.triInitialTab = ReginaPrefSet::Algebra; break;
        case 3:
            prefSet.triInitialTab = ReginaPrefSet::Composition; break;
        case 4:
            prefSet.triInitialTab = ReginaPrefSet::Surfaces; break;
        case 5:
            prefSet.triInitialTab = ReginaPrefSet::SnapPea; break;
        default:
            prefSet.triInitialTab = ReginaPrefSet::Gluings; break;
    }

    switch (triPrefs->comboInitialSkeletonTab->currentItem()) {
        case 1:
            prefSet.triInitialSkeletonTab =
                ReginaPrefSet::FacePairingGraph; break;
        default:
            prefSet.triInitialSkeletonTab = ReginaPrefSet::SkelComp; break;
    }

    switch (triPrefs->comboInitialAlgebraTab->currentItem()) {
        case 1:
            prefSet.triInitialAlgebraTab = ReginaPrefSet::FundGroup; break;
        case 2:
            prefSet.triInitialAlgebraTab = ReginaPrefSet::TuraevViro; break;
        case 3:
            prefSet.triInitialAlgebraTab = ReginaPrefSet::CellularInfo; break;
        default:
            prefSet.triInitialAlgebraTab = ReginaPrefSet::Homology; break;
    }

    uintVal = triPrefs->editSurfacePropsThreshold->text().toUInt(&ok);
    if (ok)
        prefSet.triSurfacePropsThreshold = uintVal;
    else {
        KMessageBox::error(this, i18n("The surface calculation "
            "threshold must be a non-negative integer.  "
            "This is the maximum number of tetrahedra for which normal "
            "surface properties will be calculated automatically."));
        triPrefs->editSurfacePropsThreshold->setText(
            QString::number(prefSet.triSurfacePropsThreshold));
    }

    strVal = triPrefs->editGAPExec->text().stripWhiteSpace();
    if (strVal.isEmpty()) {
        // No no no.
        triPrefs->editGAPExec->setText(prefSet.triGAPExec);
    } else if (strVal == "gap") {
        // Don't run any checks, since this is the default.
        // GAP might not be installed.
        prefSet.triGAPExec = strVal;
    } else if (strVal.find('/') >= 0) {
        // We've specified our own executable with a full path.
        // Let's be anal about it.
        QFileInfo info(strVal);
        if (! info.exists()) {
            KMessageBox::error(this, i18n("The GAP executable \"%1\" "
                "does not exist.").arg(strVal));
            triPrefs->editGAPExec->setText(prefSet.triGAPExec);
        } else if (! (info.isFile() && info.isExecutable())) {
            KMessageBox::error(this, i18n("The GAP executable \"%1\" "
                "is not actually an executable file.").arg(strVal));
            triPrefs->editGAPExec->setText(prefSet.triGAPExec);
        } else {
            // Looking fine.  Make it absolute.
            prefSet.triGAPExec = info.absFilePath();
            triPrefs->editGAPExec->setText(prefSet.triGAPExec);
        }
    } else {
        // Search on the system path.
        // Leave their setting alone, whatever it is, since they're
        // being vague about it.  Maybe they don't have GAP installed.
        if (KStandardDirs::findExe(strVal).isNull())
            KMessageBox::informationList(this, i18n("The GAP executable \"%1\" "
                "could not be found on the default search path.  This means "
                "that you will not be able to use GAP from within Regina.\n"
                "This is not really a problem; it just means that Regina "
                "will have to do its own (far less effective) group "
                "simplifications.\n"
                "The following directories are included in the default "
                "search path:").arg(strVal), KStandardDirs::systemPaths(),
                i18n("GAP Executable Not Found"));
        prefSet.triGAPExec = strVal;
    }

    strVal = triPrefs->editGraphvizExec->text().stripWhiteSpace();
    if (strVal.isEmpty()) {
        // No no no.
        // Disallow the change.
        triPrefs->editGraphvizExec->setText(prefSet.triGraphvizExec);
    } else if (strVal == "graphviz" || strVal.endsWith("/graphviz")) {
        // The user is trying to use "graphviz" as the executable name.
        // Disallow the change.
        KMessageBox::error(this, i18n("<qt>Graphviz is the name of a "
            "software suite, not the actual executable.  Graphviz supplies "
            "several different executables for drawing graphs in several "
            "different ways.  The recommended executable for use with "
            "Regina is <i>neato</i>.<p>"
            "See <i>http://www.graphviz.org/</i> for further details.</qt>"));
        triPrefs->editGraphvizExec->setText(prefSet.triGraphvizExec);
    } else {
        // Time to check it out.
        QString gvFullExec;
        GraphvizStatus gvStatus =
            GraphvizStatus::status(strVal, gvFullExec, true);

        if (gvStatus == GraphvizStatus::version1 ||
                gvStatus == GraphvizStatus::version2) {
            // Looking fine.
            // Allow the change, and make the path absolute.
            prefSet.triGraphvizExec = gvFullExec;
            triPrefs->editGraphvizExec->setText(gvFullExec);
        } else if (strVal == ReginaPrefSet::defaultGraphvizExec &&
                gvStatus != GraphvizStatus::version1NotDot) {
            // Since we have stayed with the default, allow it with almost
            // no checks -- Graphviz might not even be installed.  However,
            // we still warn users if it's likely to give _wrong_ answers
            // (as in the case version1NotDot).
            //
            // Do not make the path absolute, since we want it to stay
            // looking like the default.
            prefSet.triGraphvizExec = strVal;
        } else {
            // We have a problem.
            // We will need to ask the user for confirmation before
            // making the change.  Set up messages that are common to
            // all casese.
            int action;
            QString title = i18n("Graphviz Not Usable");
            QString tail = i18n(
                "A misconfigured Graphviz is not really "
                "a problem.  It just means that Regina will not be "
                "able to display the face pairing graphs of "
                "triangulations.<p>"
                "Are you sure you wish to save your new Graphviz "
                "setting?");

            // Treat the individual error types as appropriate.
            if (gvStatus == GraphvizStatus::notFound)
                action = KMessageBox::warningYesNoList(this, i18n(
                    "<qt>The Graphviz executable \"%1\" could not be found "
                    "on the default search path.<p>"
                    "The directories in the default search path are "
                    "listed below.<p>%2</qt>").arg(strVal).arg(tail),
                    KStandardDirs::systemPaths(),
                    title, KStdGuiItem::save(), KStdGuiItem::dontSave());
            else if (gvStatus == GraphvizStatus::notExist)
                action = KMessageBox::warningYesNo(this, i18n(
                    "<qt>The Graphviz executable \"%1\" does not exist.<p>"
                    "%2</qt>").arg(strVal).arg(tail),
                    title, KStdGuiItem::save(), KStdGuiItem::dontSave());
            else if (gvStatus == GraphvizStatus::notExecutable)
                action = KMessageBox::warningYesNo(this, i18n(
                    "<qt>The Graphviz executable \"%1\" is not actually "
                    "an executable file.<p>%2</qt>").arg(strVal).arg(tail),
                    title, KStdGuiItem::save(), KStdGuiItem::dontSave());
            else if (gvStatus == GraphvizStatus::notStartable)
                action = KMessageBox::warningYesNo(this, i18n(
                    "<qt>The Graphviz executable \"%1\" cannot be started."
                    "<p>%2</qt>").arg(strVal).arg(tail),
                    title, KStdGuiItem::save(), KStdGuiItem::dontSave());
            else if (gvStatus == GraphvizStatus::unsupported)
                action = KMessageBox::warningYesNo(this, i18n(
                    "<qt>I cannot determine the "
                    "version of Graphviz that you are running.<p>"
                    "This is a bad sign - your Graphviz version might "
                    "be too old (version 0.x), or the program \"%1\" might "
                    "not be from Graphviz at all.<p>"
                    "It is strongly recommended that you double-check this "
                    "setting.  This should be a Graphviz graph drawing "
                    "program, such as <i>neato</i> or <i>dot</i>.<p>"
                    "See <i>http://www.graphviz.org/</i> for information "
                    "on Graphviz.  If you believe this message is in error, "
                    "please notify the Regina authors at <i>%2</i>.<p>"
                    "Are you sure you wish to save your new Graphviz "
                    "setting?</qt>").arg(strVal).arg(PACKAGE_BUGREPORT),
                    title, KStdGuiItem::save(), KStdGuiItem::dontSave());
            else if (gvStatus == GraphvizStatus::version1NotDot)
                action = KMessageBox::warningYesNo(this, i18n(
                    "<qt>You appear to be running "
                    "a very old version of Graphviz (version 1.x).<p>"
                    "Many tools in older versions of Graphviz, including "
                    "<i>neato</i> (the default setting here), cannot handle "
                    "graphs with multiple edges.<p>"
                    "It is <b>highly recommended</b> that you change this "
                    "setting to <i>dot</i>, which handles multiple edges "
                    "correctly even in this old version.<p>"
                    "Alternatively, you could upgrade to a more recent "
                    "version of Graphviz (such as 2.x).  See "
                    "<i>http://www.graphviz.org/</i> for further "
                    "information.<p>"
                    "Are you sure you wish to save your new Graphviz "
                    "setting?</qt>"),
                    title, KStdGuiItem::save(), KStdGuiItem::dontSave());
            else
                action = KMessageBox::warningYesNo(this, i18n(
                    "<qt>The status of the Graphviz installation on "
                    "this machine could not be determined.<p>"
                    "This is very unusual, and the author would be "
                    "grateful if you could file a bug report at "
                    "<i>%1</i>.<p>%2</qt>").arg(PACKAGE_BUGREPORT).arg(tail),
                    title, KStdGuiItem::save(), KStdGuiItem::dontSave());

            if (action == KMessageBox::Yes)
                prefSet.triGraphvizExec = strVal;
            else
                triPrefs->editGraphvizExec->setText(prefSet.triGraphvizExec);
        }
    }

    prefSet.surfacesCreationCoords = surfacePrefs->chooserCreationCoords->
        getCurrentSystem();

    switch (surfacePrefs->comboInitialTab->currentItem()) {
        case 1:
            prefSet.surfacesInitialTab = ReginaPrefSet::Coordinates; break;
        case 2:
            prefSet.surfacesInitialTab = ReginaPrefSet::Matching; break;
        case 3:
            prefSet.surfacesInitialTab = ReginaPrefSet::Compatibility; break;
        default:
            prefSet.surfacesInitialTab = ReginaPrefSet::Summary; break;
    }

    prefSet.pdfEmbed = pdfPrefs->cbEmbed->isChecked();

    // Don't be too fussy about what they put in this field, since the
    // PDF viewer tries hard to find a suitable executable regardless.
    strVal = pdfPrefs->editExternalViewer->text().stripWhiteSpace();
    prefSet.pdfExternalViewer = strVal;

    // pdfPrefs->editExternalViewer->setText(prefSet.pdfExternalViewer);

    prefSet.pdfAutoClose = pdfPrefs->cbAutoClose->isChecked();

    prefSet.censusFiles.clear();
    for (QListViewItem* item = censusPrefs->listFiles->firstChild();
            item; item = item->nextSibling())
        prefSet.censusFiles.push_back(
            dynamic_cast<ReginaFilePrefItem*>(item)->getData());

    prefSet.pythonAutoIndent = pythonPrefs->cbAutoIndent->isChecked();
    uintVal = pythonPrefs->editSpacesPerTab->text().toUInt(&ok);
    if (ok && uintVal > 0)
        prefSet.pythonSpacesPerTab = uintVal;
    else {
        KMessageBox::error(this, i18n("The number of spaces per tab "
            "must be a positive integer."));
        pythonPrefs->editSpacesPerTab->setText(
            QString::number(prefSet.pythonSpacesPerTab));
    }
    // prefSet.pythonWordWrap = pythonPrefs->cbWordWrap->isChecked();

    prefSet.pythonLibraries.clear();
    for (QListViewItem* item = pythonPrefs->listFiles->firstChild();
            item; item = item->nextSibling())
        prefSet.pythonLibraries.push_back(
            dynamic_cast<ReginaFilePrefItem*>(item)->getData());

    prefSet.snapPeaClosed = snapPeaPrefs->cbClosed->isChecked();

    // Save these preferences to the global configuration.
    mainWindow->setPreferences(prefSet);
    mainWindow->saveOptions();
}

ReginaPrefGeneral::ReginaPrefGeneral(QWidget* parent) : QVBox(parent) {
    cbAutoFileExtension = new QCheckBox(i18n("Automatic file extension"), this);
    QWhatsThis::add(cbAutoFileExtension, i18n("Append the default extension "
        "to filenames when saving if no extension is already given."));

    cbAutoDock = new QCheckBox(i18n("Automatic packet docking"), this);
    QWhatsThis::add(cbAutoDock, i18n("Try to dock new packet viewers into "
        "the main window instead of opening them in new windows."));

    cbDisplayIcon = new QCheckBox(i18n("Display large icon"), this);
    QWhatsThis::add(cbDisplayIcon, i18n("Display the large Regina icon "
        "beneath the packet tree."));

    cbDisplayTagsInTree = new QCheckBox(i18n("Display tags in packet tree"),
        this);
    cbDisplayTagsInTree->setEnabled(false);
    QWhatsThis::add(cbDisplayTagsInTree, i18n("Show full details of any "
        "packet tags directly within the packet tree."));

    // Set up the tree jump size.
    QHBox* box = new QHBox(this);
    box->setSpacing(5);

    QLabel* label = new QLabel(i18n("Packet tree jump size:"), box);
    editTreeJumpSize = new KLineEdit(box);
    editTreeJumpSize->setMaxLength(
         10 /* ridiculously high number of digits */);
    QIntValidator* val = new QIntValidator(box);
    val->setBottom(1);
    editTreeJumpSize->setValidator(val);
    QString msg = i18n("The number of steps that a packet moves when Jump Up "
        "or Jump Down is selected.");
    QWhatsThis::add(label, msg);
    QWhatsThis::add(editTreeJumpSize, msg);

    // More options.
    cbTipOfDay = new QCheckBox(i18n("Show tip of the day"), this);
    QWhatsThis::add(cbTipOfDay, i18n("Show a tip of the day each time "
        "Regina is started."));

    // Add some space at the end.
    setStretchFactor(new QWidget(this), 1);
}

ReginaPrefTri::ReginaPrefTri(QWidget* parent) : QVBox(parent) {
    setSpacing(5);

    // WARNING: Note that any change of order in the combo boxes must be
    // reflected in the ReginaPreferences methods as well.

    // Set up the edit mode.
    QHBox* box = new QHBox(this);
    box->setSpacing(5);

    QLabel* label = new QLabel(i18n("Edit mode:"), box);
    comboEditMode = new KComboBox(box);
    comboEditMode->insertItem(SmallIcon("editclear"), i18n("Direct edit"));
    comboEditMode->insertItem(SmallIcon("view_text"), i18n("Pop-up dialog"));
    QString msg = i18n("Specifies the way in which face gluings are edited.");
    QWhatsThis::add(label, msg);
    QWhatsThis::add(comboEditMode, msg);

    // Set up the initial tab.
    box = new QHBox(this);
    box->setSpacing(5);

    label = new QLabel(i18n("Default top-level tab:"), box);
    comboInitialTab = new KComboBox(box);
    comboInitialTab->insertItem(i18n("Gluings"));
    comboInitialTab->insertItem(i18n("Skeleton"));
    comboInitialTab->insertItem(i18n("Algebra"));
    comboInitialTab->insertItem(i18n("Composition"));
    comboInitialTab->insertItem(i18n("Surfaces"));
    comboInitialTab->insertItem(i18n("SnapPea"));
    msg = i18n("Specifies which tab should be initially visible "
        "when a new triangulation viewer/editor is opened.");
    QWhatsThis::add(label, msg);
    QWhatsThis::add(comboInitialTab, msg);

    // Set up the initial skeleton tab.
    box = new QHBox(this);
    box->setSpacing(5);

    label = new QLabel(i18n("Default skeleton tab:"), box);
    comboInitialSkeletonTab = new KComboBox(box);
    comboInitialSkeletonTab->insertItem(i18n("Skeletal Components"));
    comboInitialSkeletonTab->insertItem(i18n("Face Pairing Graph"));
    msg = i18n("Specifies which tab should be initially visible "
        "when a new triangulation skeleton viewer is opened.");
    QWhatsThis::add(label, msg);
    QWhatsThis::add(comboInitialSkeletonTab, msg);

    // Set up the initial algebra tab.
    box = new QHBox(this);
    box->setSpacing(5);

    label = new QLabel(i18n("Default algebra tab:"), box);
    comboInitialAlgebraTab = new KComboBox(box);
    comboInitialAlgebraTab->insertItem(i18n("Homology"));
    comboInitialAlgebraTab->insertItem(i18n("Fundamental Group"));
    comboInitialAlgebraTab->insertItem(i18n("Turaev-Viro"));
    comboInitialAlgebraTab->insertItem(i18n("Cellular Info"));
    msg = i18n("Specifies which tab should be initially visible "
        "when a new triangulation algebra viewer is opened.");
    QWhatsThis::add(label, msg);
    QWhatsThis::add(comboInitialAlgebraTab, msg);

    // Set up the surface properties threshold.
    box = new QHBox(this);
    box->setSpacing(5);

    label = new QLabel(i18n("Surface calculation threshold:"), box);
    editSurfacePropsThreshold = new KLineEdit(box);
    editSurfacePropsThreshold->setMaxLength(
         3 /* ridiculously high number of digits */);
    editSurfacePropsThreshold->setValidator(new QIntValidator(0,
         999 /* ridiculously high */, box));
    msg = i18n("The maximum number of tetrahedra for which normal "
        "surface properties will be calculated automatically.");
    QWhatsThis::add(label, msg);
    QWhatsThis::add(editSurfacePropsThreshold, msg);

    // Set up the GAP executable.
    box = new QHBox(this);
    box->setSpacing(5);

    label = new QLabel(i18n("GAP executable:"), box);
    editGAPExec = new KLineEdit(box);
    msg = i18n("<qt>The command used to run GAP (Groups, Algorithms and "
        "Programming).  GAP can be used to help simplify presentations "
        "of fundamental groups.<p>"
        "This should be a single executable name (e.g., <i>gap</i>).  You "
        "may specify the full path to the executable if you wish "
        "(e.g., <i>/usr/bin/gap</i>); otherwise the default search path "
        "will be used.<p>"
        "There is no trouble if GAP is not installed; this just means that "
        "Regina will have to do its own (much less effective) group "
        "simplifications.</qt>");
    QWhatsThis::add(label, msg);
    QWhatsThis::add(editGAPExec, msg);

    // Set up the Graphviz executable.
    box = new QHBox(this);
    box->setSpacing(5);

    label = new QLabel(i18n("Graphviz executable:"), box);
    editGraphvizExec = new KLineEdit(box);
    msg = i18n("<qt>The command used to run Graphviz for drawing "
        "undirected graphs.  The recommended Graphviz command for this "
        "job is <i>neato</i>, though you are of course welcome to use "
        "others.<p>"
        "This should be a single executable name (e.g., <i>neato</i>).  You "
        "may specify the full path to the executable if you wish "
        "(e.g., <i>/usr/bin/neato</i>); otherwise the default search path "
        "will be used.<p>"
        "There is no trouble if Graphviz is not installed; this just means "
        "that Regina will not be able to display the face pairing graphs "
        "of triangulations.<p>"
        "For more information on Graphviz, see "
        "<i>http://www.graphviz.org/</i>.</qt>");
    QWhatsThis::add(label, msg);
    QWhatsThis::add(editGraphvizExec, msg);

    // Add some space at the end.
    setStretchFactor(new QWidget(this), 1);
}

ReginaPrefSurfaces::ReginaPrefSurfaces(QWidget* parent) : QVBox(parent) {
    setSpacing(5);

    // WARNING: Note that any change of order in the combo boxes must be
    // reflected in the ReginaPreferences methods as well.

    // Set up the default creation coordinate system.
    QHBox* box = new QHBox(this);
    box->setSpacing(5);

    QLabel* label = new QLabel(i18n("Default coordinate system:"), box);
    chooserCreationCoords = new CoordinateChooser(box);
    chooserCreationCoords->insertAllCreators();
    QString msg = i18n("The default coordinate system for creating new normal "
        "surface lists.");
    QWhatsThis::add(label, msg);
    QWhatsThis::add(chooserCreationCoords, msg);

    // Set up the initial tab.
    box = new QHBox(this);
    box->setSpacing(5);

    label = new QLabel(i18n("Default top-level tab:"), box);
    comboInitialTab = new KComboBox(box);
    comboInitialTab->insertItem(i18n("Summary"));
    comboInitialTab->insertItem(i18n("Surface Coordinates"));
    comboInitialTab->insertItem(i18n("Matching Equations"));
    comboInitialTab->insertItem(i18n("Compatibility"));
    msg = i18n("Specifies which tab should be initially visible "
        "when a new normal surface list viewer is opened.");
    QWhatsThis::add(label, msg);
    QWhatsThis::add(comboInitialTab, msg);

    // Add some space at the end.
    setStretchFactor(new QWidget(this), 1);
}

ReginaPrefPDF::ReginaPrefPDF(QWidget* parent) : QVBox(parent) {
    // Set up the embedded checkbox.
    cbEmbed = new QCheckBox(i18n("Use embedded viewer if possible"), this);
    QWhatsThis::add(cbEmbed, i18n("If possible, view PDF packets using "
        "a viewer that can embed directly into Regina's main window, "
        "such as KPDF or KGhostView."));

    // Set up the external viewer.
    QHBox* box = new QHBox(this);
    box->setSpacing(5);

    QLabel* label = new QLabel(i18n("External PDF viewer:"), box);
    editExternalViewer = new KLineEdit(box);
    QString msg = i18n("<qt>The command used to view PDF packets if we are "
        "forced to use an external application.  Examples might include "
        "<tt>kpdf</tt>, <tt>evince</tt> or <tt>xpdf</tt>.<p>"
        "You may include optional command-line arguments here.  The PDF "
        "filename will be added to the end of the argument list, and the "
        "entire command will be passed to a shell for execution.<p>"
        "You are welcome to leave this option empty, in which case Regina "
        "will try to find a suitable application.<p>"
        "This option only relates to external viewers.  If you have "
        "requested an <i>embedded</i> viewer in the checkbox above (and if an "
        "embedded viewer is available), then this option will not be "
        "used.</qt>");
    QWhatsThis::add(label, msg);
    QWhatsThis::add(editExternalViewer, msg);

    cbAutoClose = new QCheckBox(
        i18n("Automatically close external viewers"), this);
    QWhatsThis::add(cbAutoClose, i18n("When using an external PDF viewer "
        "(such as <tt>kpdf</tt> or <tt>xpdf</tt>), "
        "close it automatically when Regina's packet viewer is closed.  "
        "Likewise, close and reopen the external viewer whenever Regina's "
        "packet viewer is refreshed.<p>"
        "If you do not select this option, Regina will never close any "
        "external PDF viewers on its own; instead this task will be left "
        "up to the user.</qt>"));

    // Add some space at the end.
    setStretchFactor(new QWidget(this), 1);
}

ReginaPrefCensus::ReginaPrefCensus(QWidget* parent) : QVBox(parent) {
    // Set up the active file count.
    activeCount = new QLabel(this);

    // Prepare the main area.
    QHBox* box = new QHBox(this);
    box->setSpacing(5);
    setStretchFactor(box, 1);

    // Set up the list view.
    listFiles = new KListView(box);
    box->setStretchFactor(listFiles, 1);
    listFiles->header()->hide();
    listFiles->addColumn(QString::null);
    listFiles->setSorting(-1);
    listFiles->setSelectionModeExt(KListView::Extended);
    listFiles->setItemsMovable(true);
    QString msg = i18n("The list of census files to be searched "
        "when asked to locate an arbitrary triangulation in all available "
        "censuses.  Note that census files in this list may be deactivated, "
        "which means that they will not be searched during a census lookup.");
    QWhatsThis::add(listFiles, msg);
    QWhatsThis::add(activeCount, msg);
    connect(listFiles, SIGNAL(selectionChanged()), this, SLOT(updateButtons()));

    // Set up the button panel.
    QVBox* vBox = new QVBox(box);
    vBox->setSpacing(5);

    QPushButton* btnAdd = new QPushButton(SmallIconSet("insert_table_row"),
        i18n("Add..."), vBox);
    // btnAdd->setFlat(true);
    connect(btnAdd, SIGNAL(clicked()), this, SLOT(add()));
    QToolTip::add(btnAdd, i18n("Add a new census file"));
    QWhatsThis::add(btnAdd, i18n("Add a new census file.  "
        "This list contains the census files that are searched when asked "
        "to locate an arbitrary triangulation in all available censuses."));

    btnRemove = new QPushButton(SmallIconSet("delete_table_row"),
        i18n("Remove"), vBox);
    // btnRemove->setFlat(true);
    connect(btnRemove, SIGNAL(clicked()), this, SLOT(remove()));
    QToolTip::add(btnRemove, i18n("Remove selected census file(s)"));
    QWhatsThis::add(btnRemove, i18n("Remove the selected census file(s).  "
        "This list contains the census files that are searched when asked "
        "to locate an arbitrary triangulation in all available censuses."));

    btnActivate = new QPushButton(SmallIconSet("ok"),
        i18n("Activate"), vBox);
    // btnActivate->setFlat(true);
    connect(btnActivate, SIGNAL(clicked()), this, SLOT(activate()));
    QToolTip::add(btnActivate, i18n("Activate selected census file(s)"));
    QWhatsThis::add(btnActivate, i18n("Activate the selected census "
        "file(s).  When asked to locate an arbitrary triangulation in all "
        "available censuses, only the activated census files in this list "
        "are searched."));

    btnDeactivate = new QPushButton(SmallIconSet("no"),
        i18n("Deactivate"), vBox);
    // btnDeactivate->setFlat(true);
    connect(btnDeactivate, SIGNAL(clicked()), this, SLOT(deactivate()));
    QToolTip::add(btnDeactivate, i18n("Deactivate selected census file(s)"));
    QWhatsThis::add(btnDeactivate, i18n("Deactivate the selected census "
        "file(s).  When asked to locate an arbitrary triangulation in all "
        "available censuses, only the activated census files in this list "
        "are searched."));

    setStretchFactor(new QWidget(vBox), 1);

    QPushButton* btnDefaults = new QPushButton(i18n("Defaults"), vBox);
    // btnDefaults->setFlat(true);
    connect(btnDefaults, SIGNAL(clicked()), this, SLOT(restoreDefaults()));
    QToolTip::add(btnDefaults, i18n("Restore default list of census files"));
    QWhatsThis::add(btnDefaults, i18n("Restore the default list of "
        "census files."));

    updateButtons();
}

void ReginaPrefCensus::updateActiveCount() {
    long count = 0;
    for (QListViewItem* item = listFiles->firstChild(); item;
            item = item->nextSibling())
        if (dynamic_cast<ReginaFilePrefItem*>(item)->getData().active)
            count++;

    if (count == 0)
        activeCount->setText(i18n("No active census data files"));
    else if (count == 1)
        activeCount->setText(i18n("1 active census data file"));
    else
        activeCount->setText(i18n("%1 active census data files").arg(count));
}

void ReginaPrefCensus::updateButtons() {
    bool hasSelection = ! (listFiles->selectedItems().isEmpty());
    btnRemove->setEnabled(hasSelection);
    btnActivate->setEnabled(hasSelection);
    btnDeactivate->setEnabled(hasSelection);
}

void ReginaPrefCensus::add() {
    QStringList files = KFileDialog::getOpenFileNames(QString::null,
        FILTER_REGINA, this, i18n("Add Census File(s)"));
    if (! files.isEmpty()) {
        for (QStringList::const_iterator it = files.begin();
                it != files.end(); it++) {
            // Run a basic check over the file.
            bool active = true;
            regina::NFileInfo* info = regina::NFileInfo::identify(
                static_cast<const char*>(QFile::encodeName(*it)));
            if (! info) {
                if (KMessageBox::warningContinueCancel(this,
                        i18n("The file %1 does not appear "
                        "to be a Regina data file.  Only Regina data files "
                        "can be used for census data.  Are you sure you wish "
                        "to add it?").
                        arg(QFileInfo(*it).fileName())) !=
                        KMessageBox::Continue)
                    continue;
                active = false;
            } else if (info->isInvalid()) {
                KMessageBox::information(this, i18n("The file %1 might be a "
                    "Regina data file, but it appears to contain unusual "
                    "header information.  It is being deactivated for now; "
                    "you may wish to examine it more closely.").
                    arg(QFileInfo(*it).fileName()));
                active = false;
            }

            // Add the new item.
            new ReginaFilePrefItem(listFiles, ReginaFilePref(*it, active));
        }
        updateActiveCount();
    }
}

void ReginaPrefCensus::remove() {
    QPtrList<QListViewItem> selection = listFiles->selectedItems();
    if (selection.isEmpty())
        KMessageBox::error(this,
            i18n("No files have been selected to remove."));
    else {
        for (QListViewItem* item = selection.first(); item;
                item = selection.next())
            delete item;
        updateActiveCount();
    }
}

void ReginaPrefCensus::activate() {
    QPtrList<QListViewItem> selection = listFiles->selectedItems();
    if (selection.isEmpty())
        KMessageBox::error(this,
            i18n("No files have been selected to activate."));
    else {
        bool done = false;
        for (QListViewItem* item = selection.first(); item;
                item = selection.next())
            done |= dynamic_cast<ReginaFilePrefItem*>(item)->activateFile();
        if (done)
            updateActiveCount();
        else
            KMessageBox::sorry(this,
                i18n("All of the selected files are already active."));
    }
}

void ReginaPrefCensus::deactivate() {
    QPtrList<QListViewItem> selection = listFiles->selectedItems();
    if (selection.isEmpty())
        KMessageBox::error(this,
            i18n("No files have been selected to deactivate."));
    else {
        bool done = false;
        for (QListViewItem* item = selection.first(); item;
                item = selection.next())
            done |= dynamic_cast<ReginaFilePrefItem*>(item)->deactivateFile();
        if (done)
            updateActiveCount();
        else
            KMessageBox::sorry(this,
                i18n("All of the selected files have already been "
                    "deactivated."));
    }
}

void ReginaPrefCensus::restoreDefaults() {
    ReginaFilePrefList defaults = ReginaPrefSet::defaultCensusFiles();

    listFiles->clear();
    for (ReginaFilePrefList::const_iterator it = defaults.begin();
            it != defaults.end(); it++)
        new ReginaFilePrefItem(listFiles, *it);
    updateActiveCount();
}

ReginaPrefPython::ReginaPrefPython(QWidget* parent) : QVBox(parent) {
    // Set up the checkboxes.
    cbAutoIndent = new QCheckBox(i18n("Auto-indent"), this);
    QWhatsThis::add(cbAutoIndent, i18n("Should command lines in a Python "
        "console be automatically indented?"));

    // cbWordWrap = new QCheckBox(i18n("Word wrap"), this);
    // QWhatsThis::add(cbWordWrap, i18n("Should Python consoles be word "
    //     "wrapped?"));

    // Add some space.
    (new QWidget(this))->setMinimumHeight(5);

    // Set up the number of spaces per tab.
    QHBox* box = new QHBox(this);
    box->setSpacing(5);

    QLabel* label = new QLabel(i18n("Spaces per tab:"), box);
    editSpacesPerTab = new KLineEdit(box);
    editSpacesPerTab->setMaxLength(
         10 /* ridiculously high number of digits */);
    QIntValidator* val = new QIntValidator(box);
    val->setBottom(1);
    editSpacesPerTab->setValidator(val);
    QString msg = i18n("The number of spaces to insert into the "
        "command line when TAB is pressed.");
    QWhatsThis::add(label, msg);
    QWhatsThis::add(editSpacesPerTab, msg);

    // Add some more space.
    (new QWidget(this))->setMinimumHeight(5);

    // Set up the active file count.
    activeCount = new QLabel(this);

    // Prepare the main area.
    box = new QHBox(this);
    box->setSpacing(5);
    setStretchFactor(box, 1);

    // Set up the list view.
    listFiles = new KListView(box);
    box->setStretchFactor(listFiles, 1);
    listFiles->header()->hide();
    listFiles->addColumn(QString::null);
    listFiles->setSorting(-1);
    listFiles->setSelectionModeExt(KListView::Extended);
    listFiles->setItemsMovable(true);
    msg = i18n("The list of Python libraries to be "
        "loaded at the beginning of each new Python session.  Note that "
        "libraries in this list may be deactivated, "
        "which means that they will not be loaded.");
    QWhatsThis::add(listFiles, msg);
    QWhatsThis::add(activeCount, msg);
    connect(listFiles, SIGNAL(selectionChanged()), this, SLOT(updateButtons()));

    // Set up the button panel.
    QVBox* vBox = new QVBox(box);
    vBox->setSpacing(5);

    QPushButton* btnAdd = new QPushButton(SmallIconSet("insert_table_row"),
        i18n("Add..."), vBox);
    // btnAdd->setFlat(true);
    connect(btnAdd, SIGNAL(clicked()), this, SLOT(add()));
    QToolTip::add(btnAdd, i18n("Add a new Python library"));
    QWhatsThis::add(btnAdd, i18n("Add a new Python library.  "
        "This list contains the Python libraries to be loaded at "
        "the beginning of each new Python session."));

    btnRemove = new QPushButton(SmallIconSet("delete_table_row"),
        i18n("Remove"), vBox);
    // btnRemove->setFlat(true);
    connect(btnRemove, SIGNAL(clicked()), this, SLOT(remove()));
    QToolTip::add(btnRemove, i18n("Remove selected Python libraries"));
    QWhatsThis::add(btnRemove, i18n("Remove the selected Python libraries.  "
        "This list contains the Python libraries to be loaded at "
        "the beginning of each new Python session."));

    btnActivate = new QPushButton(SmallIconSet("ok"),
        i18n("Activate"), vBox);
    // btnActivate->setFlat(true);
    connect(btnActivate, SIGNAL(clicked()), this, SLOT(activate()));
    QToolTip::add(btnActivate, i18n("Activate selected Python libraries"));
    QWhatsThis::add(btnActivate, i18n("Activate the selected Python "
        "libraries.  When a new Python session is started, only the active "
        "libraries in this list will be loaded."));

    btnDeactivate = new QPushButton(SmallIconSet("no"),
        i18n("Deactivate"), vBox);
    // btnDeactivate->setFlat(true);
    connect(btnDeactivate, SIGNAL(clicked()), this, SLOT(deactivate()));
    QToolTip::add(btnDeactivate, i18n("Deactivate selected Python libraries"));
    QWhatsThis::add(btnDeactivate, i18n("Deactivate the selected Python "
        "libraries.  When a new Python session is started, only the active "
        "libraries in this list will be loaded."));

    setStretchFactor(new QWidget(vBox), 1);

    updateButtons();
}

void ReginaPrefPython::updateActiveCount() {
    long count = 0;
    for (QListViewItem* item = listFiles->firstChild(); item;
            item = item->nextSibling())
        if (dynamic_cast<ReginaFilePrefItem*>(item)->getData().active)
            count++;

    if (count == 0)
        activeCount->setText(i18n("No active Python libraries"));
    else if (count == 1)
        activeCount->setText(i18n("1 active Python library"));
    else
        activeCount->setText(i18n("%1 active Python libraries").arg(count));
}

void ReginaPrefPython::updateButtons() {
    bool hasSelection = ! (listFiles->selectedItems().isEmpty());
    btnRemove->setEnabled(hasSelection);
    btnActivate->setEnabled(hasSelection);
    btnDeactivate->setEnabled(hasSelection);
}

void ReginaPrefPython::add() {
    QStringList files = KFileDialog::getOpenFileNames(
        QFile::decodeName(regina::NGlobalDirs::pythonLibs().c_str()),
        FILTER_PYTHON_LIBRARIES, this, i18n("Add Python Libraries"));
    if (! files.isEmpty()) {
        for (QStringList::const_iterator it = files.begin();
                it != files.end(); it++)
            new ReginaFilePrefItem(listFiles, ReginaFilePref(*it));
        updateActiveCount();
    }
}

void ReginaPrefPython::remove() {
    QPtrList<QListViewItem> selection = listFiles->selectedItems();
    if (selection.isEmpty())
        KMessageBox::error(this,
            i18n("No libraries have been selected to remove."));
    else {
        for (QListViewItem* item = selection.first(); item;
                item = selection.next())
            delete item;
        updateActiveCount();
    }
}

void ReginaPrefPython::activate() {
    QPtrList<QListViewItem> selection = listFiles->selectedItems();
    if (selection.isEmpty())
        KMessageBox::error(this,
            i18n("No libraries have been selected to activate."));
    else {
        bool done = false;
        for (QListViewItem* item = selection.first(); item;
                item = selection.next())
            done |= dynamic_cast<ReginaFilePrefItem*>(item)->activateFile();
        if (done)
            updateActiveCount();
        else
            KMessageBox::sorry(this,
                i18n("All of the selected libraries are already active."));
    }
}

void ReginaPrefPython::deactivate() {
    QPtrList<QListViewItem> selection = listFiles->selectedItems();
    if (selection.isEmpty())
        KMessageBox::error(this,
            i18n("No libraries have been selected to deactivate."));
    else {
        bool done = false;
        for (QListViewItem* item = selection.first(); item;
                item = selection.next())
            done |= dynamic_cast<ReginaFilePrefItem*>(item)->deactivateFile();
        if (done)
            updateActiveCount();
        else
            KMessageBox::sorry(this,
                i18n("All of the selected libraries have already been "
                    "deactivated."));
    }
}

ReginaPrefSnapPea::ReginaPrefSnapPea(QWidget* parent) : QVBox(parent) {
    cbClosed = new QCheckBox(i18n("Allow closed triangulations"), this);
    QWhatsThis::add(cbClosed, i18n("<qt>Allow the SnapPea kernel to work with "
        "closed triangulations.  By default it is only allowed to work with "
        "ideal triangulations.<p>"
        "<b>Warning:</b> SnapPea is primarily designed to work with ideal "
        "triangulations only.  Allowing closed triangulations may "
        "occasionally cause the SnapPea kernel to raise a fatal error "
        "and crash Regina completely.  You might lose unsaved work "
        "as a result.</qt>"));

    QHBox* box = new QHBox(this);
    box->setSpacing(5);
    QLabel* label;

    box->setStretchFactor(label = new QLabel(i18n(
        "<qt><b>Warning:</b></qt>"), box), 0);
    label->setAlignment(Qt::AlignAuto | AlignTop);

    box->setStretchFactor(new QLabel(i18n(
        "<qt>SnapPea is primarily designed "
        "to work with ideal triangulations only!  Allowing it to work "
        "with closed triangulations may occasionally cause the "
        "SnapPea kernel to raise a fatal error, and you may lose "
        "unsaved work as a result.</qt>"), box), 1);

    // Add some space at the end.
    setStretchFactor(new QWidget(this), 1);
}

ReginaEditorChooser::ReginaEditorChooser(QWidget* /* ignored */) :
        KDialogBase(KDialogBase::Plain, i18n("Choose Text Editor Component"),
        Ok | Cancel, Cancel) {
    QBoxLayout* layout = new QVBoxLayout(plainPage());
    layout->setAutoAdd(true);

    chooser = new KTextEditor::EditorChooser(plainPage());
    setMainWidget(chooser);
    chooser->readAppSetting();
}

void ReginaEditorChooser::slotOk() {
    chooser->writeAppSetting();
    KDialogBase::slotOk();
}

#include "reginapref.moc"
