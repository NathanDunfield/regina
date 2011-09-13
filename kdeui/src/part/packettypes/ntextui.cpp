
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
#include "packet/ntext.h"

// UI includes:
#include "../packeteditiface.h"
#include "ntextui.h"

#include <cstring>
#include <sstream>
#include <klocale.h>
#include <ktexteditor/configinterface.h>
#include <ktexteditor/document.h>
#include <ktexteditor/view.h>

using regina::NPacket;
using regina::NText;

NTextUI::NTextUI(NText* packet, PacketPane* enclosingPane,
        KTextEditor::Document* doc) :
        PacketUI(enclosingPane), text(packet), document(doc) {
    // Create a view (which must be parented) before we do anything else.
    // Otherwise the Vim component crashes.
    view = document->createView(enclosingPane);
    document->setReadWrite(enclosingPane->isReadWrite());

    KTextEditor::ConfigInterface *iface =
        qobject_cast<KTextEditor::ConfigInterface*>(view);
    if (iface)
        iface->setConfigValue("dynamic-word-wrap",true);

    editIface = new PacketEditTextEditor(view);

    refresh();

    connect(document, SIGNAL(textChanged(KTextEditor::Document*)),
        this, SLOT(notifyTextChanged()));
}

NTextUI::~NTextUI() {
    delete editIface;
    delete document;
}

NPacket* NTextUI::getPacket() {
    return text;
}

QWidget* NTextUI::getInterface() {
    return view;
}

QString NTextUI::getPacketMenuText() const {
    return i18n("Te&xt");
}

void NTextUI::commit() {
    text->setText(document->text().toAscii().constData());
    setDirty(false);
}

void NTextUI::refresh() {
    // A kate part needs to be in read-write mode before we can alter its
    // contents.
    bool wasReadWrite = document->isReadWrite();
    if (! wasReadWrite)
        document->setReadWrite(true);

    document->clear();

    // Back to all-at-once insertion instead of line-by-line insertion.
    // Grrr vimpart.
    if (! text->getText().empty()) {
        QString data = text->getText().c_str();

        // We are guaranteed that data.length() >= 1.
        if (data[data.length() - 1] == '\n')
            data.truncate(data.length() - 1);

        document->setText(data);
        document->views().front()->setCursorPosition(KTextEditor::Cursor(0, 0));
    }

    if (! wasReadWrite)
        document->setReadWrite(false);

    setDirty(false);
}

void NTextUI::setReadWrite(bool readWrite) {
    document->setReadWrite(readWrite);
}

void NTextUI::notifyTextChanged() {
    setDirty(true);
}

