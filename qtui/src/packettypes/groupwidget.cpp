
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
#include "algebra/ngrouppresentation.h"

// UI includes:
#include "gaprunner.h"
#include "groupwidget.h"
#include "reginaprefset.h"
#include "reginasupport.h"

#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QTextDocument> // For Qt::escape()

GroupWidget::GroupWidget(bool allowSimplify, bool paddingStretch) :
        QWidget(), group_(0), simplified_(0) {
    QBoxLayout* layout = new QVBoxLayout(this);
    simpDepth_ = 1;

    if (paddingStretch)
        layout->addStretch(1);

    fundName_ = new QLabel();
    // fundName_->setAlignment(Qt::AlignCenter);
    layout->addWidget(fundName_);

    fundGens_ = new QLabel();
    layout->addWidget(fundGens_);
    fundRelCount_ = new QLabel();
    layout->addWidget(fundRelCount_);
    fundRels_ = new QListWidget();
    fundRels_->setSelectionMode(QListWidget::NoSelection);
    layout->addWidget(fundRels_, 3);

    // The simplification buttons:
    if (allowSimplify) {
        layout->addStretch(1);

        QLabel* label = new QLabel(tr("Try to simplify:"));
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);

        QBoxLayout* buttonBox = new QVBoxLayout();
        buttonBox->setSpacing(0);

        QBoxLayout* hLayout;
        QPushButton* btn;

        btn = new QPushButton(tr("By proliferation"));
        btn->setToolTip(tr("Simplify the group presentation by "
            "recursively exploring the consequences of the relations."));
        btn->setWhatsThis(tr("<qt>Simplify the group presentation by "
            "recursively exploring the consequences of the relations.  "
            "Doing this more than once may produce better results.</qt>"));
        connect(btn, SIGNAL(clicked()), this, SLOT(simplifyPi1()));
        hLayout = new QHBoxLayout();
        hLayout->addStretch(1);
        hLayout->addWidget(btn);
        hLayout->addStretch(1);
        buttonBox->addLayout(hLayout);

        btn = new QPushButton(tr("Using GAP"));
        btn->setToolTip(tr("Simplify the group presentation using "
            "GAP (Groups, Algorithms and Programming)"));
        btn->setWhatsThis(tr("<qt>Simplify the presentation of the "
            "fundamental group using the program GAP (Groups, Algorithms and "
            "Programming).<p>Note that GAP will need to be installed "
            "separately on your system.</qt>"));
        connect(btn, SIGNAL(clicked()), this, SLOT(simplifyGAP()));
        hLayout = new QHBoxLayout();
        hLayout->addStretch(1);
        hLayout->addWidget(btn);
        hLayout->addStretch(1);
        buttonBox->addLayout(hLayout);

        layout->addLayout(buttonBox);
    } else if (paddingStretch)
        layout->addStretch(1);
}

GroupWidget::~GroupWidget() {
    delete simplified_;
}

void GroupWidget::refresh(const regina::NGroupPresentation* group) {
    group_ = group;

    std::string name = group_->recogniseGroup();
    if (name.length()) {
        fundName_->setText(tr("Name: %1").arg(name.c_str()));
        fundName_->show();
    } else
        fundName_->hide();

    unsigned long nGens = group_->getNumberOfGenerators();
    bool alphabetic = (nGens <= 26);
    if (nGens == 0)
        fundGens_->setText(tr("No generators"));
    else if (nGens == 1)
        fundGens_->setText(tr("1 generator: a"));
    else if (nGens == 2)
        fundGens_->setText(tr("2 generators: a, b"));
    else if (alphabetic)
        fundGens_->setText(tr("%1 generators: a ... %2").
            arg(nGens).arg(char('a' + nGens - 1)));
    else
        fundGens_->setText(tr("%1 generators: g0 ... g%2").
            arg(nGens).arg(nGens - 1));

    unsigned long nRels = group_->getNumberOfRelations();
    if (nRels == 0) {
        fundRelCount_->setText(tr("No relations"));
        fundRels_->hide();
    } else if (nRels == 1) {
        fundRelCount_->setText(tr("1 relation:"));
        fundRels_->show();
    } else {
        fundRelCount_->setText(tr("%1 relations:").arg(nRels));
        fundRels_->show();
    }

    fundRels_->clear();
    if (alphabetic) {
        // Generators are a, b, ...
        for (long i = 0; i < nRels; ++i) {
            QString rel;
            const std::list<regina::NGroupExpressionTerm>& terms(
                group_->getRelation(i).getTerms());
            if (terms.empty())
                rel = "1";
            else {
                std::list<regina::NGroupExpressionTerm>::const_iterator it;
                for (it = terms.begin(); it != terms.end(); ++it) {
                    if (it != terms.begin())
                        rel += ' ';
                    if (it->exponent == 0)
                        rel += '1';
                    else {
                        rel += char('a' + it->generator);
                        if (it->exponent != 1)
                            rel += QString("^%1").arg(it->exponent);
                    }
                }
            }
            new QListWidgetItem(rel, fundRels_);
        }
    } else {
        // Generators are g0, g1, ...
        // This is the default text that comes from the calculation engine.
        for (long i = 0; i < nRels; ++i)
            new QListWidgetItem(QString(group_->getRelation(i).str().c_str()),
                fundRels_);
    }

    simpDepth_ = 1;
}

void GroupWidget::simplifyPi1() {
    if (! group_)
        return;

    // Note: We might have group_ == simplified_, so we cannot delete
    // simplified_ just yet.
    regina::NGroupPresentation* ans = new regina::NGroupPresentation(*group_);
    ans->proliferateRelators(simpDepth_);
    ans->intelligentSimplify();

    delete simplified_;
    simplified_ = ans;

    unsigned oldDepth = simpDepth_;
    refresh(simplified_); // This will reset simpDepth_ by default...
    simpDepth_ = oldDepth;

    // Let's not let the end-user go beyond too many iterates for now.
    if (simpDepth_ < 4)
        simpDepth_++;

    emit simplified();
}

void GroupWidget::simplifyGAP() {
    if (! group_)
        return;

    // Can we actually run GAP?
    QString useExec = verifyGAPExec();
    if (useExec.isNull())
        return;

    // Note: We might have group_ == simplified_, so we cannot delete
    // simplified_ just yet.
    GAPRunner dlg(this, useExec, *group_);
    if (dlg.exec() == GAPRunner::Accepted) {
        regina::NGroupPresentation* ans = dlg.simplifiedGroup().release();
        if (ans) {
            delete simplified_;
            simplified_ = ans;

            refresh(simplified_);
            emit simplified();
        } else {
            ReginaSupport::sorry(this,
                tr("An unexpected error occurred whilst "
                "attempting to simplify the group presentation using GAP."),
                tr("<qt>Please verify that GAP "
                "(Groups, Algorithms and Programming) "
                "is correctly installed on your system, and that Regina "
                "has been correctly configured to use it (see the "
                "<i>Tools</i> section in Regina's settings).</qt>"));
        }
    }
}

QString GroupWidget::verifyGAPExec() {
    QString useExec = ReginaPrefSet::global().triGAPExec;

    if (useExec.indexOf('/') < 0) {
        // Hunt on the search path.
        // Search through $PATH to find the executable
        QString paths = QProcessEnvironment::systemEnvironment().value("PATH");
        // Windows uses a different separator in $PATH
#if defined _WIN32 || defined _WIN64 || defined __CYGWIN
        QString pathSeparator = ";";
#else
        QString pathSeparator = ":";
#endif
        QStringList pathList = paths.split(pathSeparator);

        bool found = false;
        for( QStringList::iterator it = pathList.begin(); it != pathList.end();
            ++it) {
            QDir dir(*it);
            if ( dir.exists(useExec) ) {
                found = true;
                break;
            }
        }
        if (! found) {
            ReginaSupport::sorry(this,
                tr("<qt>I could not find the GAP executable <i>%1</i> "
                "on the default search path.</qt>").arg(Qt::escape(useExec)),
                tr("<qt>If you have GAP (Groups, Algorithms and Programming) "
                "installed on your system, please go into Regina's "
                "settings (<i>Tools</i> section) and tell Regina "
                "where it can find GAP.</qt>"));
            return QString::null;
        }
    }

    // We have a full path to the GAP executable.
    QFileInfo info(useExec);
    if (! info.exists()) {
        ReginaSupport::sorry(this,
            tr("<qt>The GAP executable <i>%1</i> does not exist.</qt>").
                arg(Qt::escape(useExec)),
            tr("<qt>If you have GAP (Groups, Algorithms and Programming) "
            "installed on your system, please go into Regina's "
            "settings (<i>Tools</i> section) and tell Regina "
            "where it can find GAP.</qt>"));
        return QString::null;
    } else if (! (info.isFile() && info.isExecutable())) {
        ReginaSupport::sorry(this,
            tr("<qt>The GAP executable <i>%1</i> does not appear to be "
            "an executable program.</qt>").arg(Qt::escape(useExec)),
            tr("<qt>If you have GAP (Groups, Algorithms and Programming) "
            "installed on your system, please go into Regina's "
            "settings (<i>Tools</i> section) and tell Regina "
            "where it can find GAP.</qt>"));
        return QString::null;
    }

    // All good.
    return useExec;
}

