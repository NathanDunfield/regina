
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

#include "file/nglobaldirs.h"
#include "surfaces/nnormalsurfacelist.h"
#include "utilities/stringutils.h"

#include "reginaprefset.h"
#include "shortrunner.h"

#include <fstream>
#include <klocale.h>
#include <kmessagebox.h>
#include <krun.h>
#include <kstandarddirs.h>
#include <ktoolinvocation.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextcodec.h>
#include <qtextstream.h>

namespace {
    QString INACTIVE("## INACTIVE ##");
}

const GraphvizStatus GraphvizStatus::unknown(0);
const GraphvizStatus GraphvizStatus::notFound(-1);
const GraphvizStatus GraphvizStatus::notExist(-2);
const GraphvizStatus GraphvizStatus::notExecutable(-3);
const GraphvizStatus GraphvizStatus::notStartable(-4);
const GraphvizStatus GraphvizStatus::unsupported(-5);
const GraphvizStatus GraphvizStatus::version1(1);
const GraphvizStatus GraphvizStatus::version1NotDot(2);
const GraphvizStatus GraphvizStatus::version2(3);

const char* ReginaPrefSet::defaultGAPExec = "gap";
const char* ReginaPrefSet::defaultGraphvizExec = "neato";

// No need to initialise these, since the cache is only used when the
// given executable matches cacheGraphvizExec (which begins life as null).
QMutex GraphvizStatus::cacheGraphvizMutex;
QString GraphvizStatus::cacheGraphvizExec;
QString GraphvizStatus::cacheGraphvizExecFull;
GraphvizStatus GraphvizStatus::cacheGraphvizStatus;

QByteArray ReginaFilePref::encodeFilename() const {
    return static_cast<const char*>(QFile::encodeName(filename));
}

GraphvizStatus GraphvizStatus::status(const QString& userExec,
        QString& fullExec, bool forceRecheck) {
    QMutexLocker lock(&cacheGraphvizMutex);

    if ((! forceRecheck) && cacheGraphvizStatus != unknown &&
            userExec == cacheGraphvizExec) {
        fullExec = cacheGraphvizExecFull;
        return cacheGraphvizStatus;
    }

    // We need a full requery.
    if (! userExec.contains("/")) {
        // Hunt on the search path.
        fullExec = KStandardDirs::findExe(userExec);
        if (fullExec.isNull())
            return notFound;
    } else
        fullExec = QFileInfo(userExec).absoluteFilePath();

    // We have a full path to the Graphviz executable.
    QFileInfo info(fullExec);
    if (! info.exists())
        return notExist;
    if (! (info.isFile() && info.isExecutable()))
        return notExecutable;

    // Run to extract a version string.
    ShortRunner graphviz;
    graphviz << fullExec << "-V";
    QString output = graphviz.run(true);
    if (output.isNull()) {
        if (graphviz.timedOut())
            return unsupported;
        else
            return notStartable;
    }

    if (output.contains("version 1.")) {
        // Only test for "dot", not "/dot".  I'd rather not get tripped
        // up with alternate path separators, and this still
        // distinguishes between the different 1.x graph drawing tools.
        if (userExec.endsWith("dot", Qt::CaseInsensitive))
            return version1;
        return version1NotDot;
    } else if (output.contains("version 0."))
        return unsupported;
    else if (output.contains("version")) {
        // Assume any other version is >= 2.x.
        return version2;
    } else {
        // Could not find a version string at all.
        return unsupported;
    }
}

ReginaPrefSet::ReginaPrefSet() :
        autoDock(true),
        autoFileExtension(true),
        censusFiles(defaultCensusFiles()),
        dim4GraphvizLabels(false),
        dim4InitialTab(Dim4Gluings),
        displayTagsInTree(false),
        handbookInKHelpCenter(true),
        pdfAutoClose(true),
        pdfEmbed(true),
        pythonAutoIndent(true),
        pythonSpacesPerTab(4),
        pythonWordWrap(false),
        snapPeaClosed(false),
        surfacesCompatThreshold(100),
        surfacesCreationCoords(regina::NNormalSurfaceList::STANDARD),
        surfacesInitialCompat(LocalCompat),
        surfacesInitialTab(Summary),
        treeJumpSize(10),
        triGAPExec(defaultGAPExec),
        triGraphvizExec(defaultGraphvizExec),
        triGraphvizLabels(false),
        triInitialTab(Gluings),
        triInitialSkeletonTab(SkelComp),
        triInitialAlgebraTab(Homology),
        triSurfacePropsThreshold(6) {
}

ReginaFilePrefList ReginaPrefSet::defaultCensusFiles() {
    ReginaFilePrefList ans;

    QString exDir = QFile::decodeName(regina::NGlobalDirs::examples().c_str());
    ans.push_back(ReginaFilePref(exDir + "/closed-or-census.rga"));
    ans.push_back(ReginaFilePref(exDir + "/closed-nor-census.rga"));
    ans.push_back(ReginaFilePref(exDir + "/knot-link-census.rga"));
    ans.push_back(ReginaFilePref(exDir + "/snappea-census.rga"));
    ans.push_back(ReginaFilePref(exDir + "/closed-hyp-census.rga"));

    return ans;
}

QString ReginaPrefSet::pdfDefaultViewer() {
    QString app;

    // If we're on a mac, try the default Mac PDF viewer.
    #ifdef __APPLE__
    if (QFile::exists("/Applications"))
        if (! (app = KStandardDirs::findExe("open")).isNull())
            return app;
    #endif

    // Try KDE applications (okular, kpdf).
    if (! (app = KStandardDirs::findExe("okular")).isNull())
        return app;
    if (! (app = KStandardDirs::findExe("kpdf")).isNull())
        return app;

    // Try GNOME applications (evince).
    if (! (app = KStandardDirs::findExe("evince")).isNull())
        return app;

    // Fall back to xpdf if we can.
    if (! (app = KStandardDirs::findExe("xpdf")).isNull())
        return app;

    // Bapow.  We'll try going through KRun if/when somebody actually
    // tries to open a PDF packet.
    return QString();
}

QString ReginaPrefSet::pythonLibrariesConfig() {
    return QDir::homePath() + "/.regina-libs";
}

bool ReginaPrefSet::readPythonLibraries() {
    pythonLibraries.clear();

    QFile f(pythonLibrariesConfig());
    if (! f.open(QIODevice::ReadOnly))
        return false;

    QTextStream in(&f);
    in.setCodec(QTextCodec::codecForName("UTF-8"));

    bool active;
    QString line = in.readLine();
    while (! line.isNull()) {
        // Is the file inactive?
        active = true;
        if (line.startsWith(INACTIVE)) {
            active = false;
            line = line.mid(INACTIVE.length());
        }

        line = line.trimmed();

        // Is it a file at all?  If so, add it.
        if ((! line.isEmpty()) && line[0] != '#')
            pythonLibraries.push_back(ReginaFilePref(line, active));

        // Next!
        line = in.readLine();
    }

    return true;
}

bool ReginaPrefSet::writePythonLibraries() const {
    QFile f(pythonLibrariesConfig());
    if (! f.open(QIODevice::WriteOnly))
        return false;

    QTextStream out(&f);
    out.setCodec(QTextCodec::codecForName("UTF-8"));

    out << "# Python libraries configuration file\n#\n";
    out << "# Automatically generated by the KDE user interface.\n\n";

    for (ReginaFilePrefList::const_iterator it = pythonLibraries.begin();
            it != pythonLibraries.end(); it++)
        if ((*it).active)
            out << (*it).filename << '\n';
        else
            out << INACTIVE << ' ' << (*it).filename << '\n';

    return true;
}

void ReginaPrefSet::openHandbook(const char* section, const char* handbook,
        QWidget* parentWidget) {
    if (handbookInKHelpCenter) {
        KToolInvocation::invokeHelp(section, handbook);
        return;
    }

    QString handbookName = (handbook ? handbook : "regina");
    QString index = KStandardDirs::locate("html",
        QString("en/%1/index.html").arg(handbookName));
    QString page = KStandardDirs::locate("html",
        QString("en/%1/%2.html").arg(handbookName).arg(section));
    if (QFileInfo(page).exists()) {
        // If we're on a mac, just use the default Mac browser.
#ifdef __APPLE__
        // Hmm.  Assume this command executes successfully, since on my
        // fink it returns false even when it *does* execute successfully..!
        KRun::runCommand(QString("open \"%1\"").arg(page), parentWidget);
#else
        if (! KRun::runUrl("file:" + page, "text/html", parentWidget,
                false /* temp file */, false /* run executables */)) {
            if (handbook)
                KMessageBox::sorry(parentWidget, i18n(
                    "<qt>The requested handbook could not be opened.  "
                    "Please try pointing your web browser to "
                    "<tt>%1</tt>.</qt>").arg(page));
            else
                KMessageBox::sorry(parentWidget, i18n(
                    "<qt>The Regina handbook could not be opened.  "
                    "Please try pointing your web browser to "
                    "<tt>%1</tt>.</qt>").arg(page));
        }
#endif
    } else {
        if (handbook)
            KMessageBox::sorry(parentWidget, i18n(
                "<qt>The requested handbook could "
                "not be found.  Perhaps it is not installed?<p>"
                "The handbook should be accessible as "
                "<tt>%1/</tt>.</qt>").arg(index));
        else
            KMessageBox::sorry(parentWidget, i18n(
                "<qt>The Regina handbook could "
                "not be found.  Perhaps it is not installed?<p>"
                "The handbook should be accessible as "
                "<tt>%1/</tt>.</qt>").arg(index));
    }
}

