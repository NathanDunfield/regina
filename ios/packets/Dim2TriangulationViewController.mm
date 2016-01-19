
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

#import "Dim2TriangulationViewController.h"
#import "dim2/dim2triangulation.h"

@implementation Dim2TriangulationViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self setSelectedImages:@[@"Tab-Gluings-Bold",
                              @"Tab-Skeleton-Bold"]];
    [self registerDefaultKey:@"ViewDim2Tab"];
}

- (void)updateHeader:(UILabel *)header
{
    if (self.packet->isEmpty()) {
        header.text = @"Empty";
        return;
    }

    NSMutableString* msg;
    if (! self.packet->isConnected()) {
        msg = [NSMutableString stringWithString:@"Disconnected, "];

        if (self.packet->isClosed())
            [msg appendString:@"closed, "];
        else
            [msg appendString:@"with boundary, "];
        
        if (self.packet->isOrientable())
            [msg appendString:@"orientable"];
        else
            [msg appendString:@"non-orientable"];
    } else {
        // It's connected.  Report the exact manifold.
        if (self.packet->isOrientable()) {
            long punctures = self.packet->countBoundaryComponents();
            long genus = (2 - self.packet->eulerChar() - punctures) / 2;

            // Special names for surfaces with boundary:
            if (genus == 0 && punctures == 1)
                msg = [NSMutableString stringWithString:@"Disc"];
            else if (genus == 0 && punctures == 2)
                msg = [NSMutableString stringWithString:@"Annulus"];
            else {
                if (genus == 0)
                    msg = [NSMutableString stringWithString:@"Sphere"];
                else if (genus == 1)
                    msg = [NSMutableString stringWithString:@"Torus"];
                else
                    msg = [NSMutableString stringWithFormat:@"Genus %ld torus", genus];

                if (punctures == 1)
                    [msg appendString:@", 1 puncture"];
                else if (punctures > 1)
                    [msg appendFormat:@", %ld punctures", punctures];
            }
        } else {
            long punctures = self.packet->countBoundaryComponents();
            long genus = (2 - self.packet->eulerChar() - punctures);

            // Special names for surfaces with boundary:
            if (genus == 1 && punctures == 1)
                msg = [NSMutableString stringWithString:@"Möbius band"];
            else {
                if (genus == 1)
                    msg = [NSMutableString stringWithString:@"Projective plane"];
                else if (genus == 2)
                    msg = [NSMutableString stringWithString:@"Klein bottle"];
                else
                    msg = [NSMutableString stringWithFormat:@"Non-orientable genus %ld surface", genus];

                if (punctures == 1)
                    [msg appendString:@", 1 puncture"];
                else if (punctures > 1)
                    [msg appendFormat:@", %ld punctures", punctures];
            }
        }
    }

    [msg appendFormat:@" (χ = %ld)", self.packet->eulerChar()];
    header.text = msg;
}

@end
