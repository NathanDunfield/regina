
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  KDE User Interface                                                    *
 *                                                                        *
 *  Copyright (c) 1999-2014, Ben Burton                                   *
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
#include "packet/packetregistry.h"
#include "surfaces/filterregistry.h"

// UI includes:
#include "iconcache.h"
#include "packetmanager.h"
#include "reginamain.h"
#include "reginasupport.h"
#include "packettypes/dim2triui.h"
#include "packettypes/nanglestructureui.h"
#include "packettypes/ncontainerui.h"
#include "packettypes/nnormalsurfaceui.h"
#include "packettypes/npdfui.h"
#include "packettypes/nscriptui.h"
#include "packettypes/nsnappeaui.h"
#include "packettypes/nsurfacefiltercomb.h"
#include "packettypes/nsurfacefilterprop.h"
#include "packettypes/ntextui.h"
#include "packettypes/ntriangulationui.h"

#include <QPlainTextEdit>

using namespace regina;

QIcon PacketManager::icon(NPacket* packet, bool allowLock) {
    IconCache::IconID id;

    if (packet->type() == PACKET_ANGLESTRUCTURELIST)
        id = IconCache::packet_angles;
    else if (packet->type() == PACKET_CONTAINER)
        id = (packet->parent() ? IconCache::packet_container :
            IconCache::regina);
    else if (packet->type() == PACKET_DIM2TRIANGULATION)
        id = IconCache::packet_dim2triangulation;
    else if (packet->type() == PACKET_PDF)
        id = IconCache::packet_pdf;
    else if (packet->type() == PACKET_SURFACEFILTER) {
        if (((NSurfaceFilter*)packet)->filterType() == NS_FILTER_COMBINATION)
            id = IconCache::filter_comb;
        else if (((NSurfaceFilter*)packet)->filterType() ==
                NS_FILTER_PROPERTIES)
            id = IconCache::filter_prop;
        else
            id = IconCache::packet_filter;
    }
    else if (packet->type() == PACKET_SCRIPT)
        id = IconCache::packet_script;
    else if (packet->type() == PACKET_SNAPPEATRIANGULATION)
        id = IconCache::packet_snappea;
    else if (packet->type() == PACKET_NORMALSURFACELIST)
        id = IconCache::packet_surfaces;
    else if (packet->type() == PACKET_TEXT)
        id = IconCache::packet_text;
    else if (packet->type() == PACKET_TRIANGULATION)
        id = IconCache::packet_triangulation;
    else {
        // Unknown packet type.
        return QIcon();
    }

    if (allowLock && ! packet->isPacketEditable())
        return IconCache::lockedIcon(id);
    else
        return IconCache::icon(id);
}

PacketUI* PacketManager::createUI(regina::NPacket* packet,
        PacketPane* enclosingPane) {
    if (packet->type() == PACKET_ANGLESTRUCTURELIST)
        return new NAngleStructureUI(dynamic_cast<NAngleStructureList*>(packet),
            enclosingPane);
    if (packet->type() == PACKET_CONTAINER)
        return new NContainerUI(dynamic_cast<NContainer*>(packet),
            enclosingPane);
    if (packet->type() == PACKET_DIM2TRIANGULATION)
        return new Dim2TriangulationUI(dynamic_cast<Dim2Triangulation*>(packet),
            enclosingPane);
    if (packet->type() == PACKET_NORMALSURFACELIST)
        return new NNormalSurfaceUI(dynamic_cast<NNormalSurfaceList*>(packet),
            enclosingPane);
    if (packet->type() == PACKET_SCRIPT) {
        return new NScriptUI(dynamic_cast<NScript*>(packet), enclosingPane);
    }
    if (packet->type() == PACKET_SNAPPEATRIANGULATION) {
        return new NSnapPeaUI(dynamic_cast<NSnapPeaTriangulation*>(packet),
            enclosingPane);
    }
    if (packet->type() == PACKET_SURFACEFILTER) {
        if (((NSurfaceFilter*)packet)->filterType() == NS_FILTER_COMBINATION)
            return new NSurfaceFilterCombUI(
                dynamic_cast<NSurfaceFilterCombination*>(packet),
                enclosingPane);
        if (((NSurfaceFilter*)packet)->filterType() == NS_FILTER_PROPERTIES)
            return new NSurfaceFilterPropUI(
                dynamic_cast<NSurfaceFilterProperties*>(packet),
                enclosingPane);
        return new DefaultPacketUI(packet, enclosingPane);
    }
    if (packet->type() == PACKET_TEXT) {
        return new NTextUI(dynamic_cast<NText*>(packet), enclosingPane);
    }
    if (packet->type() == PACKET_TRIANGULATION)
        return new NTriangulationUI(dynamic_cast<NTriangulation*>(packet),
            enclosingPane);
    return new DefaultPacketUI(packet, enclosingPane);
}

PacketExternalViewer PacketManager::externalViewer(regina::NPacket* packet) {
    if (packet->type() == PACKET_PDF)
        return &NPDFExternalViewer::view;
    return 0;
}

