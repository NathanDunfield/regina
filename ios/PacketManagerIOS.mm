
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  iOS User Interface                                                    *
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

#import "DetailViewController.h"
#import "MasterViewController.h"
#import "NewPacketController.h"
#import "PacketManagerIOS.h"
#import "PacketTreeController.h"
#import "ReginaHelper.h"
#import "packet/ncontainer.h"
#import "packet/npacket.h"
#import "packet/ntext.h"
#import "surfaces/nsurfacefilter.h"

@implementation PacketManagerIOS

+ (UIImage*)iconFor:(regina::NPacket *)p {
    if (! p->parent())
        return [UIImage imageNamed:@"Document"];

    switch (p->type()) {
        case regina::PACKET_ANGLESTRUCTURELIST:
            return [UIImage imageNamed:@"Angles"];
        case regina::PACKET_CONTAINER:
            return [UIImage imageNamed:@"Container"];
        case regina::PACKET_DIM2TRIANGULATION:
            return [UIImage imageNamed:@"Dim2Triangulation"];
        case regina::PACKET_NORMALSURFACELIST:
            return [UIImage imageNamed:@"Surfaces"];
        case regina::PACKET_PDF:
            return [UIImage imageNamed:@"PDF"];
        case regina::PACKET_SCRIPT:
            return [UIImage imageNamed:@"Script"];
        case regina::PACKET_SNAPPEATRIANGULATION:
            return [UIImage imageNamed:@"SnapPea"];
        case regina::PACKET_SURFACEFILTER:
            return [UIImage imageNamed:@"Filter"];
        case regina::PACKET_TEXT:
            return [UIImage imageNamed:@"Text"];
        case regina::PACKET_TRIANGULATION:
            return [UIImage imageNamed:@"Triangulation"];
        default:
            return nil;
    }
}

+ (NSString *)viewerFor:(regina::NPacket *)p {
    switch (p->type()) {
        case regina::PACKET_ANGLESTRUCTURELIST: return @"viewAngles";
        case regina::PACKET_DIM2TRIANGULATION: return @"viewDim2Triangulation";
        case regina::PACKET_NORMALSURFACELIST: return @"viewSurfaces";
        case regina::PACKET_SCRIPT: return @"viewScript";
        case regina::PACKET_SNAPPEATRIANGULATION: return @"viewSnapPea";
        case regina::PACKET_SURFACEFILTER:
            switch (static_cast<regina::NSurfaceFilter*>(p)->filterType()) {
                case regina::NS_FILTER_PROPERTIES: return @"viewFilterProperties";
                case regina::NS_FILTER_COMBINATION: return @"viewFilterCombination";
                default: return @"viewDefault";
            }
        case regina::PACKET_TEXT: return @"viewText";
        case regina::PACKET_TRIANGULATION: return @"viewTriangulation";
        default: return @"viewDefault";
    }
}

+ (void)newPacket:(NewPacketSpec*)spec {
    if (! spec.parent)
        return;

    PacketTreeController* tree = [ReginaHelper tree];
    if (! tree)
        return;

    switch (spec.type) {
        case regina::PACKET_CONTAINER:
        {
            // We can do this immediately, no input required.
            regina::NContainer* c = new regina::NContainer();
            c->setPacketLabel("Container");
            spec.parent->insertChildLast(c);
            [spec created:c];
            break;
        }
        case regina::PACKET_TEXT:
        {
            // We can do this immediately, no input required.
            regina::NText* t = new regina::NText();
            t->setPacketLabel("Text");
            t->setText("Type your text here.");
            spec.parent->insertChildLast(t);
            [spec created:t];
            break;
        }
        case regina::PACKET_TRIANGULATION:
            [PacketManagerIOS newPacket:spec formSheet:@"newTriangulation"];
            break;
        case regina::PACKET_DIM2TRIANGULATION:
            [PacketManagerIOS newPacket:spec formSheet:@"newDim2Triangulation"];
            break;
        case regina::PACKET_NORMALSURFACELIST:
            [PacketManagerIOS newPacket:spec formSheet:@"newSurfaces"];
            break;
        case regina::PACKET_ANGLESTRUCTURELIST:
            [PacketManagerIOS newPacket:spec formSheet:@"newAngles"];
            break;
        case regina::PACKET_SNAPPEATRIANGULATION:
            [PacketManagerIOS newPacket:spec formSheet:@"newSnapPea"];
            break;
        case regina::PACKET_SURFACEFILTER:
            [PacketManagerIOS newPacket:spec formSheet:@"newFilter"];
            break;
        case regina::PACKET_PDF:
        case regina::PACKET_SCRIPT:
            // We don't create these in the iOS version of Regina.
            break;
    }
}

+ (void)newPacket:(NewPacketSpec*)spec formSheet:(NSString*)sheetID
{
    // Present the form sheet from the master view controller.
    // Otherwise the form sheet may appear *beneath* master (iOS 8, portrait mode).
    UIViewController* from = [ReginaHelper master];
    UIViewController* sheet = [from.storyboard instantiateViewControllerWithIdentifier:sheetID];
    static_cast<NewPacketController*>(sheet).spec = spec;
    [from presentViewController:sheet animated:YES completion:nil];
}

@end
