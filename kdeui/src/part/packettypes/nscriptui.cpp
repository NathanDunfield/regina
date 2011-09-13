
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
#include "regina-config.h"
#include "packet/nscript.h"

// UI includes:
#include "nscriptui.h"
#include "../packetchooser.h"
#include "../packeteditiface.h"
#include "../packetmanager.h"
#include "../reginapart.h"

#include <cstring>
#include <kaction.h>
#include <KActionCollection>
#include <KComponentData>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktexteditor/configinterface.h>
#include <ktexteditor/document.h>
#include <ktexteditor/view.h>
#include <ktoolbar.h>
#include <qboxlayout.h>
#include <qheaderview.h>
#include <qlineedit.h>
#include <qsplitter.h>
#include <qtablewidget.h>
#include <set>

using regina::NPacket;
using regina::NScript;

namespace {
    QRegExp rePythonIdentifier("^[A-Za-z_][A-Za-z0-9_]*$");
}

ScriptVarValueItem::ScriptVarValueItem(regina::NPacket* packet) :
        packet_(packet) {
    if (packet_)
        packet_->listen(this);

    updateData();
}

void ScriptVarValueItem::setPacket(regina::NPacket* p) {
    if (packet_)
        packet_->unlisten(this);

    packet_ = p;

    if (p)
        p->listen(this);

    updateData();
}

void ScriptVarValueItem::packetWasRenamed(regina::NPacket* p) {
    if (p == packet_)
        updateData();
}

void ScriptVarValueItem::packetToBeDestroyed(regina::NPacket* p) {
    if (p == packet_) {
        packet_->unlisten(this);
        packet_ = 0;
        updateData();
    }
}

void ScriptVarValueItem::updateData() {
    if (packet_ && ! packet_->getPacketLabel().empty()) {
        setText(packet_->getPacketLabel().c_str());
        setIcon(QPixmap(PacketManager::iconSmall(packet_, false)));
    } else {
        setText("<None>");
        setIcon(QPixmap());
    }
}

QWidget* ScriptNameDelegate::createEditor(QWidget* parent,
        const QStyleOptionViewItem&, const QModelIndex&) const {
    QLineEdit* e = new QLineEdit(parent);
    e->setValidator(new QRegExpValidator(rePythonIdentifier, e));
    return e;
}

void ScriptNameDelegate::setEditorData(QWidget* editor,
        const QModelIndex& index) const {
    QString data = index.model()->data(index, Qt::EditRole).toString();

    QLineEdit* e = static_cast<QLineEdit*>(editor);
    e->setText(data);
}

void ScriptNameDelegate::setModelData(QWidget* editor,
        QAbstractItemModel* model, const QModelIndex& index) const {
    QLineEdit* e = static_cast<QLineEdit*>(editor);
    QString data = e->text().trimmed();

    if (data.isEmpty()) {
        KMessageBox::error(e, i18n(
            "Variable names cannot be empty."));
        return;
    }
    if (! rePythonIdentifier.exactMatch(data)) {
        KMessageBox::error(e, i18n(
            "%1 is not a valid python variable name.").arg(data));

        // Construct a better variable name.
        data.replace(QRegExp("[^A-Za-z0-9_]"), "");
        if (data.isEmpty())
            return;
        if (! rePythonIdentifier.exactMatch(data))
            data.prepend('_');
    }
    if (nameUsedElsewhere(data, index.row(), model)) {
        KMessageBox::error(e, i18n(
            "Another variable is already using the name %1.").arg(data));

        // Construct a unique variable name.
        int which;
        for (which = 0; nameUsedElsewhere(data + QString::number(which),
                index.row(), model); which++)
            ;
        data.append(QString::number(which));
    }

    model->setData(index, data, Qt::EditRole);
}

void ScriptNameDelegate::updateEditorGeometry(QWidget* editor,
        const QStyleOptionViewItem& option, const QModelIndex&) const {
    editor->setGeometry(option.rect);
}

bool ScriptNameDelegate::nameUsedElsewhere(const QString& name, int currRow,
        QAbstractItemModel* model) {
    int rows = model->rowCount();
    for (int i = 0; i < rows; i++) {
        if (i == currRow)
            continue;
        if (model->data(model->index(i, 0)).toString() == name)
            return true;
    }
    return false;
}

QWidget* ScriptValueDelegate::createEditor(QWidget* parent,
        const QStyleOptionViewItem&, const QModelIndex&) const {
    PacketChooser* e = new PacketChooser(matriarch_,
        0 /* filter */, true /* allow "none" */,
        0 /* initial selection */, parent);
    e->setAutoUpdate(true);
    return e;
}

void ScriptValueDelegate::setEditorData(QWidget* editor,
        const QModelIndex& index) const {
    PacketChooser* e = static_cast<PacketChooser*>(editor);
    e->selectPacket(static_cast<ScriptVarValueItem*>(
        table_->item(index.row(), index.column()))->getPacket());
}

void ScriptValueDelegate::setModelData(QWidget* editor,
        QAbstractItemModel*, const QModelIndex& index) const {
    PacketChooser* e = static_cast<PacketChooser*>(editor);
    NPacket* p = e->selectedPacket();
    ScriptVarValueItem* item = static_cast<ScriptVarValueItem*>(table_->item(
        index.row(), index.column()));

    if (item->getPacket() != p)
        item->setPacket(p);
}

void ScriptValueDelegate::updateEditorGeometry(QWidget* editor,
        const QStyleOptionViewItem& option, const QModelIndex&) const {
    editor->setGeometry(option.rect);
}

NScriptUI::NScriptUI(NScript* packet, PacketPane* enclosingPane,
        KTextEditor::Document* doc) :
        PacketUI(enclosingPane), script(packet), document(doc) {
    bool readWrite = enclosingPane->isReadWrite();

    ui = new QWidget(enclosingPane);
    QVBoxLayout* layout = new QVBoxLayout(ui);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // --- Action Toolbar ---

    KToolBar* actionBar = new KToolBar(ui, false, true);
    actionBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    layout->addWidget(actionBar);

    // --- Variable Table ---

    // Prepare a splitter for the remaining components.
    QSplitter* splitter = new QSplitter(Qt::Vertical);
    layout->addWidget(splitter, 1);

    varTable = new ScriptVarTable(0, 2);
    varTable->setSelectionMode(QAbstractItemView::ContiguousSelection);

    if (readWrite )
        varTable->setEditTriggers(QAbstractItemView::AllEditTriggers);
    else
        varTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    varTable->setWhatsThis(i18n("<qt>A list of variables that will be "
        "set before the script is run.  Each variable may refer to a "
        "single packet.<p>"
        "This allows your script to easily access the other packets in "
        "this data file.</qt>"));

    varTable->verticalHeader()->hide();

    varTable->setHorizontalHeaderLabels(
        QStringList() << i18n("Variable") << i18n("Value"));
    varTable->horizontalHeader()->setStretchLastSection(true);

    nameDelegate = new ScriptNameDelegate();
    valueDelegate = new ScriptValueDelegate(varTable,
        packet->getTreeMatriarch());
    varTable->setItemDelegateForColumn(0, nameDelegate);
    varTable->setItemDelegateForColumn(1, valueDelegate);

    splitter->addWidget(varTable);
   
    // --- Text Editor ---

    // Create a view (which must be parented) before we do anything else.
    // Otherwise the Vim component crashes.
    view = document->createView(splitter);

    // Prepare the components.
    document->setReadWrite(readWrite);

    KTextEditor::ConfigInterface *iface =
        qobject_cast<KTextEditor::ConfigInterface*>(view);
    if (iface)
        iface->setConfigValue("dynamic-word-wrap", false);

    setPythonMode();

    view->setFocus();
    view->setWhatsThis(i18n("Type the Python script into this "
        "area.  Any variables listed in the table above will be "
        "set before the script is run."));

    editIface = new PacketEditTextEditor(view);

    splitter->addWidget(view);

    splitter->setTabOrder(view, varTable);
    ui->setFocusProxy(view);

    // --- Script Actions ---

    scriptActions = new KActionCollection((QObject*)0);
    
    actAdd = scriptActions->addAction("script_add_var");
    actAdd->setText(i18n("&Add Var"));
    actAdd->setIcon(KIcon("edit-table-insert-row-below"));
    actAdd->setToolTip(i18n("Add a new script variable"));
    actAdd->setEnabled(readWrite);
    actAdd->setWhatsThis(i18n("Add a new variable to this script.<p>"
        "A script may come with any number of variables, each of which "
        "refers to a single packet.  "
        "This allows your script to easily access the other packets in "
        "this data file."));
    connect(actAdd, SIGNAL(triggered()), this, SLOT(addVariable()));
    actionBar->addAction(actAdd);
    scriptActionList.append(actAdd);

    actRemove = scriptActions->addAction("script_remove_var");
    actRemove->setText(i18n("Re&move Var"));
    actRemove->setIcon(KIcon("edit-table-delete-row"));
    actRemove->setToolTip(i18n(
        "Remove the currently selected script variable(s)"));
    actRemove->setEnabled(false);
    actRemove->setWhatsThis(i18n("Remove the selected variable(s) from "
        "this script.<p>"
        "A script may come with any number of variables, each of which "
        "refers to a single packet.  "
        "This allows your script to easily access the other packets in "
        "this data file."));
    connect(actRemove, SIGNAL(triggered()), this, 
        SLOT(removeSelectedVariables()));
    connect(varTable, SIGNAL(itemSelectionChanged()), this,
        SLOT(updateRemoveState()));
    actionBar->addAction(actRemove);
    scriptActionList.append(actRemove);

    KAction* actSep = scriptActions->addAction("script_separator");
    actSep->setSeparator(true);
    actionBar->addAction(actSep);
    scriptActionList.append(actSep);

    KAction* actCompile = scriptActions->addAction("script_compile");;
    actCompile->setText(i18n("&Compile"));
    actCompile->setIcon(KIcon("run-build-file"));
    actCompile->setToolTip(i18n("Compile the Python script"));
    actCompile->setWhatsThis(i18n("Test whether this Python script "
        "actually compiles.  Any errors will be shown in a separate "
        "Python console."));
    connect(actCompile, SIGNAL(triggered()), this, SLOT(compile()));
    actionBar->addAction(actCompile);
    scriptActionList.append(actCompile);

    KAction* actRun = scriptActions->addAction("script_run");;
    actRun->setText(i18n("&Run"));
    actRun->setIcon(KIcon("system-run"));
    actRun->setToolTip(i18n("Execute the Python script"));
    actRun->setWhatsThis(i18n("Execute this Python script.  The "
        "script will be run in a separate Python console."));
    connect(actRun, SIGNAL(triggered()), this, SLOT(execute()));
    actionBar->addAction(actRun);
    scriptActionList.append(actRun);

    // --- Finalising ---

    // Make the editor get most of the space.
    splitter->setStretchFactor(0 /* index */, 0 /* weight */);
    splitter->setStretchFactor(1 /* index */, 1 /* weight */);

    // Fill the components with data.
    refresh();
    // varTable->horizontalHeader()->resizeSections(
    //     QHeaderView::ResizeToContents);

    // Notify us of any changes.
    connect(varTable, SIGNAL(itemChanged(QTableWidgetItem*)),
        this, SLOT(notifyScriptChanged()));
    connect(document, SIGNAL(textChanged(KTextEditor::Document*)),
        this, SLOT(notifyScriptChanged()));
}

NScriptUI::~NScriptUI() {
    // Make sure the actions, including separators, are all deleted.
    delete scriptActions;

    // Clean up.
    delete nameDelegate;
    delete valueDelegate;
    delete editIface;
    delete document;
}

NPacket* NScriptUI::getPacket() {
    return script;
}

QWidget* NScriptUI::getInterface() {
    return ui;
}

const QLinkedList<KAction*>& NScriptUI::getPacketTypeActions() {
    return scriptActionList;
}

QString NScriptUI::getPacketMenuText() const {
    return i18n("S&cript");
}

void NScriptUI::commit() {
    // Finish whatever edit was going on in the table.
    varTable->endEdit();

    // Update the lines.
    script->removeAllLines();
    unsigned nLines = document->lines();
    for (unsigned i = 0; i < nLines; i++) {
        QString s = document->line(i);
        script->addLast(s.isNull() ? "" : s.toAscii().constData());
    }

    // Update the variables.
    script->removeAllVariables();
    unsigned nVars = varTable->rowCount();
    regina::NPacket* value;
    for (unsigned i = 0; i < nVars; i++) {
        value = dynamic_cast<ScriptVarValueItem*>(varTable->item(i, 1))->
            getPacket();
        script->addVariable(
            varTable->item(i, 0)->text().toAscii().constData(),
            value ? value->getPacketLabel() : std::string());
    }

    setDirty(false);
}

void NScriptUI::refresh() {
    // Refresh the variables.
    unsigned long nVars = script->getNumberOfVariables();
    varTable->setRowCount(nVars);

    regina::NPacket* matriarch = script->getTreeMatriarch();
    for (unsigned long i = 0; i < nVars; i++) {
        varTable->setItem(i, 0, new QTableWidgetItem(
            script->getVariableName(i).c_str()));
        varTable->setItem(i, 1, new ScriptVarValueItem(
            matriarch->findPacketLabel(script->getVariableValue(i).c_str())));
    }

    // A kate part needs to be in read-write mode before we can alter its
    // contents.
    bool wasReadWrite = document->isReadWrite();
    if (! wasReadWrite)
        document->setReadWrite(true);

    // Refresh the lines.
    // The first line is handled separately to avoid an additional blank
    // line from being appended.
    unsigned long nLines = script->getNumberOfLines();
    if (nLines == 0)
        document->clear();
    else {
        // Bloody hell.
        // Trying to support both kate and vimpart with line-by-line
        // insertion is just too much drama, especially with vimpart's
        // continually changing behaviour.
        // Just use setText() and be done with it.
        QString allLines;
        for (unsigned long i = 0; i < nLines; i++) {
            allLines += script->getLine(i).c_str();
            if (i + 1 < nLines)
                allLines += '\n';
        }
        document->setText(allLines);
        view->setCursorPosition(KTextEditor::Cursor(0, 0));
    }

    if (! wasReadWrite)
        document->setReadWrite(false);

    setDirty(false);
}

void NScriptUI::setReadWrite(bool readWrite) {
    if (readWrite)
        varTable->setEditTriggers(QAbstractItemView::AllEditTriggers);
    else
        varTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    document->setReadWrite(readWrite);
    actAdd->setEnabled(readWrite);
    updateRemoveState();
}

void NScriptUI::addVariable() {
    // Find a suitable variable name.
    QString varName;

    unsigned rows = varTable->rowCount();
    unsigned which = 0;
    unsigned i;

    while (true) {
        varName = QString("var") + QString::number(which);
        for (i = 0; i < rows; i++)
            if (varTable->item(i, 0)->text() == varName)
                break;
        if (i == rows)
            break;
        which++;
    }

    // Add the new variable.
    varTable->insertRow(rows);
    QTableWidgetItem* nameItem = new QTableWidgetItem(varName);
    varTable->setItem(rows, 0, nameItem);
    varTable->setItem(rows, 1, new ScriptVarValueItem(0));
    varTable->scrollToItem(nameItem);

    // Done!
    setDirty(true);
}

void NScriptUI::removeSelectedVariables() {
    // Gather together all the rows to be deleted.
    std::set<int> rows;

    // Note that selections are contiguous.
    if (varTable->selectedRanges().empty()) {
        KMessageBox::error(ui, i18n(
            "No variables are currently selected for removal."));
        return;
    }
    QTableWidgetSelectionRange range = varTable->selectedRanges().front();
    std::cerr << "REMOVING: " << range.topRow() << "--" <<
        range.bottomRow() << std::endl;

    // Notify the user that variables will be removed.
    QString message;
    if (range.bottomRow() == range.topRow())
        message = i18n("The variable %1 will be removed.  Are you sure?").
            arg(varTable->item(range.topRow(), 0)->text());
    else if (range.bottomRow() == range.topRow() + 1)
        message = i18n("The variables %1 and %2 will be removed.  "
            "Are you sure?").arg(varTable->item(range.topRow(), 0)->text()).
            arg(varTable->item(range.bottomRow(), 0)->text());
    else
        message = i18n("%1 variables from %2 to %3 will be removed.  "
            "Are you sure?").arg(range.bottomRow() - range.topRow() + 1).
            arg(varTable->item(range.topRow(), 0)->text()).
            arg(varTable->item(range.bottomRow(), 0)->text());

    if (KMessageBox::warningContinueCancel(ui, message) ==
            KMessageBox::Cancel)
        return;

    // Remove the variables!
    for (int i = range.bottomRow(); i >= range.topRow(); --i)
        varTable->removeRow(i);

    setDirty(true);
}

void NScriptUI::updateRemoveState() {
    // Are we read-write?
    if (actAdd->isEnabled())
        actRemove->setEnabled(varTable->selectedItems().count() > 0);
    else
        actRemove->setEnabled(false);
}

void NScriptUI::compile() {
    ReginaPart* part = enclosingPane->getPart();
    if (part->getPythonManager().compileScript(ui, &part->getPreferences(),
            document->text() + "\n\n") == 0) {
        #ifdef BOOST_PYTHON_FOUND
        KMessageBox::information(ui,
            i18n("The script compiles successfully."), i18n("Success"));
        #endif
    } else
        KMessageBox::error(ui, i18n("The script does not compile.\n"
            "See the Python console for details.  You may interact with "
            "this console to further investigate the problem."),
            i18n("Compile Failure"));
}

void NScriptUI::execute() {
    // Finish whatever edit was going on in the table.
    varTable->endEdit();

    // Set up the variable list.
    PythonVariableList vars;

    unsigned nVars = varTable->rowCount();
    for (unsigned i = 0; i < nVars; i++)
        vars.push_back(PythonVariable(varTable->item(i, 0)->text(),
            dynamic_cast<ScriptVarValueItem*>(varTable->item(i, 1))->
                getPacket()));

    // Run the script.
    ReginaPart* part = enclosingPane->getPart();
    part->getPythonManager().launchPythonConsole(ui, &part->getPreferences(),
            document->text() + "\n\n", vars);
}

void NScriptUI::notifyScriptChanged() {
    setDirty(true);
}

void NScriptUI::setPythonMode() {
    // KDE4 lets us do document->setHighlightingMode("python") with graceful
    // failure, possibly making code smaller.
    QStringList nModes = document->highlightingModes();
    for ( int i = 0; i < nModes.count(); i++)
        if (nModes[i].toLower() == "python") {
            document->setHighlightingMode(nModes[i]);
            break;
        } 
}

