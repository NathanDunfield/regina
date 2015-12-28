
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

#import "ReginaHelper.h"
#import "SnapPeaViewController.h"
#import "TriangulationViewController.h"
#import "TriSkeleton.h"
#import "TextHelper.h"
#import "dim2/dim2triangulation.h"
#import "triangulation/ntriangulation.h"

#define KEY_LAST_TRI_SKELETON_TYPE @"ViewTriSkeletonWhich"

@interface TriSkeleton () <UITableViewDataSource, UITableViewDelegate> {
    CGFloat headerHeight, fatHeaderHeight;
}
@property (weak, nonatomic) IBOutlet UILabel *header;
@property (weak, nonatomic) IBOutlet UILabel *volume;
@property (weak, nonatomic) IBOutlet UILabel *solnType;
@property (weak, nonatomic) IBOutlet UIButton *lockIcon;

@property (weak, nonatomic) IBOutlet UILabel *fVector;
@property (weak, nonatomic) IBOutlet UISegmentedControl *viewWhich;

@property (weak, nonatomic) IBOutlet UIButton *vtxLinksButton;
@property (weak, nonatomic) IBOutlet UIButton *vtxLinksIcon;
@property (weak, nonatomic) IBOutlet UIButton *drillEdgeButton;
@property (weak, nonatomic) IBOutlet UIButton *drillEdgeIcon;

@property (assign, nonatomic) regina::NTriangulation* packet;
@end

@implementation TriSkeleton

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    self.packet = static_cast<regina::NTriangulation*>(static_cast<id<PacketViewer> >(self.parentViewController).packet);

    self.details.delegate = self;
    self.details.dataSource = self;

    [self reloadPacket];
}

- (void)reloadPacket
{
    if ([self.parentViewController isKindOfClass:[SnapPeaViewController class]])
        [static_cast<SnapPeaViewController*>(self.parentViewController) updateHeader:self.header volume:self.volume solnType:self.solnType];
    else
        [static_cast<TriangulationViewController*>(self.parentViewController) updateHeader:self.header lockIcon:self.lockIcon];

    self.fVector.text = [NSString stringWithFormat:@"f-vector: (%ld, %ld, %ld, %ld)",
                         self.packet->getNumberOfFaces<0>(),
                         self.packet->getNumberOfFaces<1>(),
                         self.packet->getNumberOfFaces<2>(),
                         self.packet->getNumberOfFaces<3>()];

    [self.viewWhich setTitle:[TextHelper countString:self.packet->getNumberOfFaces<0>() singular:"vertex" plural:"vertices"] forSegmentAtIndex:0];
    [self.viewWhich setTitle:[TextHelper countString:self.packet->getNumberOfFaces<1>() singular:"edge" plural:"edges"] forSegmentAtIndex:1];
    [self.viewWhich setTitle:[TextHelper countString:self.packet->getNumberOfFaces<2>() singular:"triangle" plural:"triangles"] forSegmentAtIndex:2];
    [self.viewWhich setTitle:[TextHelper countString:self.packet->getNumberOfFaces<3>() singular:"tetrahedron" plural:"tetrahedra"] forSegmentAtIndex:3];
    [self.viewWhich setTitle:[TextHelper countString:self.packet->getNumberOfComponents() singular:"component" plural:"components"] forSegmentAtIndex:4];
    [self.viewWhich setTitle:[TextHelper countString:self.packet->getNumberOfBoundaryComponents() singular:"boundary" plural:"boundaries"] forSegmentAtIndex:5];

    self.viewWhich.selectedSegmentIndex = [[NSUserDefaults standardUserDefaults] integerForKey:KEY_LAST_TRI_SKELETON_TYPE];

    [self.details reloadData];
    [self updateActions];
}

- (IBAction)whichChanged:(id)sender {
    [self.details reloadData];
    [self updateActions];
    [[NSUserDefaults standardUserDefaults] setInteger:self.viewWhich.selectedSegmentIndex forKey:KEY_LAST_TRI_SKELETON_TYPE];
}

- (void)updateActions {
    // Vertex links:
    BOOL enable = NO;
    if (self.viewWhich.selectedSegmentIndex == 0) {
        NSIndexPath* seln = self.details.indexPathForSelectedRow;
        if (seln && seln.row > 0 && seln.row <= self.packet->getNumberOfVertices())
            enable = YES;
    }
    self.vtxLinksButton.enabled = self.vtxLinksIcon.enabled = enable;
    
    // Drill edge:
    enable = NO;
    if (self.viewWhich.selectedSegmentIndex == 1 && self.packet->getPacketType() == regina::PACKET_TRIANGULATION) {
        NSIndexPath* seln = self.details.indexPathForSelectedRow;
        if (seln && seln.row > 0 && seln.row <= self.packet->getNumberOfEdges())
            enable = YES;
    }
    self.drillEdgeButton.enabled = self.drillEdgeIcon.enabled = enable;
}

- (IBAction)vertexLinks:(id)sender {
    if (self.viewWhich.selectedSegmentIndex != 0)
        return;
    
    NSIndexPath* seln = self.details.indexPathForSelectedRow;
    if ((! seln) || seln.row <= 0 || seln.row > self.packet->getNumberOfVertices()) {
        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Please Select a Vertex"
                                                        message:nil
                                                       delegate:nil
                                              cancelButtonTitle:@"Close"
                                              otherButtonTitles:nil];
        [alert show];
        return;
    }
    
    regina::Dim2Triangulation* ans = new regina::Dim2Triangulation(*self.packet->getVertex(seln.row - 1)->buildLink());
    ans->setPacketLabel([NSString stringWithFormat:@"Link of vertex %zd", seln.row - 1].UTF8String);
    self.packet->insertChildLast(ans);
    [ReginaHelper viewPacket:ans];
}

- (IBAction)drillEdge:(id)sender {
    if (self.viewWhich.selectedSegmentIndex != 1)
        return;
    
    NSIndexPath* seln = self.details.indexPathForSelectedRow;
    if ((! seln) || seln.row <= 0 || seln.row > self.packet->getNumberOfEdges()) {
        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Please Select an Edge"
                                                        message:nil
                                                       delegate:nil
                                              cancelButtonTitle:@"Close"
                                              otherButtonTitles:nil];
        [alert show];
        return;
    }
    
    regina::NTriangulation* ans = new regina::NTriangulation(*self.packet);
    [ReginaHelper runWithHUD:@"Drilling…"
                        code:^{
                            ans->drillEdge(ans->getEdge(seln.row - 1));
                            ans->setPacketLabel(self.packet->adornedLabel("Drilled"));
                        }
                     cleanup:^{
                         self.packet->insertChildLast(ans);
                         [ReginaHelper viewPacket:ans];
                     }];
}

#pragma mark - Table view

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    switch (self.viewWhich.selectedSegmentIndex) {
        case 0: /* vertices */
            return 1 + MAX(self.packet->getNumberOfVertices(), 1);
        case 1: /* edges */
            return 1 + MAX(self.packet->getNumberOfEdges(), 1);
        case 2: /* triangles */
            return 1 + MAX(self.packet->getNumberOfTriangles(), 1);;
        case 3: /* tetrahedra */
            return 1 + MAX(self.packet->getNumberOfTetrahedra(), 1);;
        case 4: /* components */
            return 1 + MAX(self.packet->getNumberOfComponents(), 1);
        case 5: /* boundary components */
            return 1 + MAX(self.packet->getNumberOfBoundaryComponents(), 1);
        default:
            return 0;
    }
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.row == 0) {
        switch (self.viewWhich.selectedSegmentIndex) {
            case 0: /* vertices */
                return [tableView dequeueReusableCellWithIdentifier:@"VertexHeader"];
            case 1: /* edges */
                return [tableView dequeueReusableCellWithIdentifier:@"EdgeHeader"];
            case 2: /* triangles */
                return [tableView dequeueReusableCellWithIdentifier:@"TriangleHeader"];
            case 3: /* tetrahedra */
                return [tableView dequeueReusableCellWithIdentifier:@"TetrahedronHeader"];
            case 4: /* components */
                return [tableView dequeueReusableCellWithIdentifier:@"ComponentHeader"];
            case 5: /* boundary components */
                return [tableView dequeueReusableCellWithIdentifier:@"BdryHeader"];
            default:
                return nil;
        }
    }

    SkeletonCell *cell;
    switch (self.viewWhich.selectedSegmentIndex) {
        case 0: /* vertices */
            if (self.packet->getNumberOfVertices() == 0) {
                cell = [tableView dequeueReusableCellWithIdentifier:@"Empty" forIndexPath:indexPath];
                cell.index.text = @"No vertices";
                cell.data0.text = cell.data1.text = cell.data2.text = @"";
            } else {
                regina::NVertex* v = self.packet->getVertex(indexPath.row - 1);
                cell = [tableView dequeueReusableCellWithIdentifier:@"Vertex" forIndexPath:indexPath];
                cell.index.text = [NSString stringWithFormat:@"%zd.", indexPath.row - 1];
                cell.data1.text = [NSString stringWithFormat:@"%ld", v->getDegree()];

                switch (v->getLink()) {
                    case regina::NVertex::SPHERE:
                        cell.data0.text = @"Internal";
                        break;
                    case regina::NVertex::DISC:
                        cell.data0.text = @"Bdry";
                        break;
                    case regina::NVertex::TORUS:
                        cell.data0.text = @"Ideal: Torus";
                        break;
                    case regina::NVertex::KLEIN_BOTTLE:
                        cell.data0.text = @"Ideal: Klein bottle";
                        break;
                    case regina::NVertex::NON_STANDARD_CUSP:
                        if (v->isLinkOrientable())
                            cell.data0.text = [NSString stringWithFormat:@"Ideal: Genus %ld orbl", (1 - (v->getLinkEulerChar() / 2))];
                        else
                            cell.data0.text = [NSString stringWithFormat:@"Ideal: Genus %ld non-orbl", (2 - v->getLinkEulerChar())];
                        break;
                    case regina::NVertex::NON_STANDARD_BDRY:
                        cell.data0.attributedText = [TextHelper badString:@"Invalid"];
                        break;
                    default:
                        cell.data0.attributedText = [TextHelper badString:@"Unknown"];
                }

                NSMutableString* pieces = [NSMutableString string];
                std::vector<regina::NVertexEmbedding>::const_iterator it;
                for (it = v->getEmbeddings().begin(); it != v->getEmbeddings().end(); ++it)
                    [TextHelper appendToList:pieces
                                        item:[NSString stringWithFormat:@"%ld (%d)",
                                              self.packet->tetrahedronIndex(it->getTetrahedron()),
                                              it->getVertex()]];
                cell.data2.text = pieces;
            }
            break;
        case 1: /* edges */
            if (self.packet->getNumberOfEdges() == 0) {
                cell = [tableView dequeueReusableCellWithIdentifier:@"Empty" forIndexPath:indexPath];
                cell.index.text = @"No edges";
                cell.data0.text = cell.data1.text = cell.data2.text = @"";
            } else {
                regina::NEdge* e = self.packet->getEdge(indexPath.row - 1);
                cell = [tableView dequeueReusableCellWithIdentifier:@"Edge" forIndexPath:indexPath];
                cell.index.text = [NSString stringWithFormat:@"%zd.", indexPath.row - 1];
                cell.data1.text = [NSString stringWithFormat:@"%ld", e->getNumberOfEmbeddings()];

                if (! e->isValid())
                    cell.data0.attributedText = [TextHelper badString:@"Invalid"];
                else if (e->isBoundary())
                    cell.data0.text = @"Bdry";
                else
                    cell.data0.text = @"Internal";

                NSMutableString* pieces = [NSMutableString string];
                std::deque<regina::NEdgeEmbedding>::const_iterator it;
                for (it = e->getEmbeddings().begin(); it != e->getEmbeddings().end(); ++it)
                    [TextHelper appendToList:pieces
                                        item:[NSString stringWithFormat:@"%ld (%s)",
                                              self.packet->tetrahedronIndex(it->getTetrahedron()),
                                              it->getVertices().trunc2().c_str()]];
                cell.data2.text = pieces;
            }
            break;
        case 2: /* triangles */
            if (self.packet->getNumberOfTriangles() == 0) {
                cell = [tableView dequeueReusableCellWithIdentifier:@"Empty" forIndexPath:indexPath];
                cell.index.text = @"No triangles";
                cell.data0.text = cell.data1.text = cell.data2.text = @"";
            } else {
                regina::NTriangle* t = self.packet->getTriangle(indexPath.row - 1);
                cell = [tableView dequeueReusableCellWithIdentifier:@"Triangle" forIndexPath:indexPath];
                cell.index.text = [NSString stringWithFormat:@"%zd.", indexPath.row - 1];

                cell.data0.text = (t->isBoundary() ? @"Bdry" : @"Internal");

                switch (t->getType()) {
                    case regina::NTriangle::TRIANGLE:
                        cell.data1.text = @"Triangle";
                        break;
                    case regina::NTriangle::SCARF:
                        cell.data1.text = @"Scarf";
                        break;
                    case regina::NTriangle::PARACHUTE:
                        cell.data1.text = @"Parachute";
                        break;
                    case regina::NTriangle::MOBIUS:
                        cell.data1.text = @"Möbius band";
                        break;
                    case regina::NTriangle::CONE:
                        cell.data1.text = @"Cone";
                        break;
                    case regina::NTriangle::HORN:
                        cell.data1.text = @"Horn";
                        break;
                    case regina::NTriangle::DUNCEHAT:
                        cell.data1.text = @"Dunce hat";
                        break;
                    case regina::NTriangle::L31:
                        cell.data1.text = @"L(3,1)";
                        break;
                    case regina::NTriangle::UNKNOWN_TYPE:
                        cell.data1.attributedText = [TextHelper badString:@"Unknown"];
                        break;
                }

                NSMutableString* pieces = [NSMutableString string];
                for (unsigned i = 0; i < t->getNumberOfEmbeddings(); i++)
                    [TextHelper appendToList:pieces
                                        item:[NSString stringWithFormat:@"%ld (%s)",
                                              self.packet->tetrahedronIndex(t->getEmbedding(i).getTetrahedron()),
                                              t->getEmbedding(i).getVertices().trunc3().c_str()]];
                cell.data2.text = pieces;
            }
            break;
        case 3: /* tetrahedra */
            if (self.packet->getNumberOfTetrahedra() == 0) {
                cell = [tableView dequeueReusableCellWithIdentifier:@"Empty" forIndexPath:indexPath];
                cell.index.text = @"No tetrahedra";
                cell.data0.text = cell.data1.text = cell.data2.text = @"";
            } else {
                regina::NTetrahedron *t = self.packet->getTetrahedron(indexPath.row - 1);
                cell = [tableView dequeueReusableCellWithIdentifier:@"Tetrahedron" forIndexPath:indexPath];
                cell.index.text = [NSString stringWithFormat:@"%zd.", indexPath.row - 1];

                cell.data0.text = [NSString stringWithFormat:@"%ld, %ld, %ld, %ld",
                                  t->getVertex(0)->markedIndex(),
                                  t->getVertex(1)->markedIndex(),
                                  t->getVertex(2)->markedIndex(),
                                  t->getVertex(3)->markedIndex()];

                cell.data1.text = [NSString stringWithFormat:@"%ld, %ld, %ld, %ld, %ld, %ld",
                                  t->getEdge(0)->markedIndex(),
                                  t->getEdge(1)->markedIndex(),
                                  t->getEdge(2)->markedIndex(),
                                  t->getEdge(3)->markedIndex(),
                                  t->getEdge(4)->markedIndex(),
                                  t->getEdge(5)->markedIndex()];

                cell.data2.text = [NSString stringWithFormat:@"%ld, %ld, %ld, %ld",
                                    t->getTriangle(3)->markedIndex(),
                                    t->getTriangle(2)->markedIndex(),
                                    t->getTriangle(1)->markedIndex(),
                                    t->getTriangle(0)->markedIndex()];
            }
            break;
        case 4: /* components */
            if (self.packet->getNumberOfComponents() == 0) {
                cell = [tableView dequeueReusableCellWithIdentifier:@"Empty" forIndexPath:indexPath];
                cell.index.text = @"No components";
                cell.data0.text = cell.data1.text = cell.data2.text = @"";
            } else {
                regina::NComponent* c = self.packet->getComponent(indexPath.row - 1);
                cell = [tableView dequeueReusableCellWithIdentifier:@"Component" forIndexPath:indexPath];
                cell.index.text = [NSString stringWithFormat:@"%zd.", indexPath.row - 1];
                cell.data1.text = [TextHelper countString:c->getNumberOfSimplices() singular:"tetrahedron" plural:"tetrahedra"];

                cell.data0.text = [NSString stringWithFormat:@"%s %s",
                                  (c->isIdeal() ? "Ideal, " : "Real, "),
                                  (c->isOrientable() ? "orbl" : "non-orbl")];

                if (self.packet->getNumberOfComponents() == 1) {
                    cell.data2.text = @"All tetrahedra";
                } else {
                    NSMutableString* pieces = [NSMutableString string];
                    for (unsigned long i = 0; i < c->getNumberOfTriangles(); ++i)
                        [TextHelper appendToList:pieces
                                            item:[NSString stringWithFormat:@"%ld",
                                                  self.packet->tetrahedronIndex(c->getTetrahedron(i))]];
                    cell.data2.text = pieces;
                }
            }
            break;
        case 5: /* boundary components */
            if (self.packet->getNumberOfBoundaryComponents() == 0) {
                cell = [tableView dequeueReusableCellWithIdentifier:@"Empty" forIndexPath:indexPath];
                cell.index.text = @"No boundary components";
                cell.data0.text = cell.data1.text = cell.data2.text = @"";
            } else {
                regina::NBoundaryComponent* b = self.packet->getBoundaryComponent(indexPath.row - 1);
                cell = [tableView dequeueReusableCellWithIdentifier:@"Bdry" forIndexPath:indexPath];
                cell.index.text = [NSString stringWithFormat:@"%zd.", indexPath.row - 1];
                cell.data0.text = (b->isIdeal() ? @"Ideal" : @"Real");
                // Note: by parity, #triangles must be >= 2 and so we can safely use the plural form.
                cell.data1.text = (b->isIdeal() ?
                                  [NSString stringWithFormat:@"Degree %ld", b->getVertex(0)->getDegree()] :
                                  [NSString stringWithFormat:@"%ld triangles", b->getNumberOfTriangles()]);

                NSMutableString* pieces = [NSMutableString string];
                if (b->isIdeal()) {
                    regina::NVertex* v = b->getVertex(0);
                    std::vector<regina::NVertexEmbedding>::const_iterator it;
                    for (it = v->getEmbeddings().begin(); it != v->getEmbeddings().end(); ++it)
                        [TextHelper appendToList:pieces
                                            item:[NSString stringWithFormat:@"%ld (%d)",
                                                  self.packet->tetrahedronIndex(it->getTetrahedron()),
                                                  it->getVertex()]];
                    cell.data2.text = [NSString stringWithFormat:@"Vertices %@", pieces];
                    // Parity says #vertices >= 2, so always use plural form.
                } else {
                    for (unsigned long i = 0; i < b->getNumberOfTriangles(); ++i) {
                        const regina::NTriangleEmbedding& emb = b->getTriangle(i)->getEmbedding(0);
                        [TextHelper appendToList:pieces
                                            item:[NSString stringWithFormat:@"%ld (%s)",
                                                  self.packet->tetrahedronIndex(emb.getTetrahedron()),
                                                  emb.getVertices().trunc3().c_str()]];
                    }
                    cell.data2.text = [NSString stringWithFormat:@"Faces %@", pieces];
                    // Parity says #faces >= 2, so always use plural form.
                }
            }
            break;
    }
    return cell;
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    return NO;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (indexPath.row > 0)
        return self.details.rowHeight;

    if (self.viewWhich.selectedSegmentIndex != 3) {
        // The header row is smaller.  Calculate it based on the cell contents, which include
        // auto-layout constraints that pin the labels to the upper and lower boundaries.
        if (headerHeight == 0) {
            UITableViewCell* cell = [self.details dequeueReusableCellWithIdentifier:@"EdgeHeader"];
            [cell layoutIfNeeded];
            CGSize size = [cell.contentView systemLayoutSizeFittingSize:UILayoutFittingCompressedSize];
            headerHeight = size.height;
        }
        return headerHeight;
    } else {
        // Same deal for a two-line header.
        if (fatHeaderHeight == 0) {
            UITableViewCell* cell = [self.details dequeueReusableCellWithIdentifier:@"TetrahedronHeader"];
            [cell layoutIfNeeded];
            CGSize size = [cell.contentView systemLayoutSizeFittingSize:UILayoutFittingCompressedSize];
            fatHeaderHeight = size.height;
        }
        return fatHeaderHeight;
    }
}

- (void)tableView:(UITableView *)tableView didDeselectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [self updateActions];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [self updateActions];
}

@end
