
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
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

#include "dim4/dim4edge.h"
#include "dim4/dim4triangle.h"

namespace regina {

const int Dim4Triangle::triangleNumber[5][5][5] = {
    { { -1,-1,-1,-1,-1 },
      { -1,-1, 9, 8, 7 },
      { -1, 9,-1, 6, 5 },
      { -1, 8, 6,-1, 4 },
      { -1, 7, 5, 4,-1 } },

    { { -1,-1, 9, 8, 7 },
      { -1,-1,-1,-1,-1 },
      {  9,-1,-1, 3, 2 },
      {  8,-1, 3,-1, 1 },
      {  7,-1, 2, 1,-1 } },

    { { -1, 9,-1, 6, 5 },
      {  9,-1,-1, 3, 2 },
      { -1,-1,-1,-1,-1 },
      {  6, 3,-1,-1, 0 },
      {  5, 2,-1, 0,-1 } },

    { { -1, 8, 6,-1, 4 },
      {  8,-1, 3,-1, 1 },
      {  6, 3,-1,-1, 0 },
      { -1,-1,-1,-1,-1 },
      {  4, 1, 0,-1,-1 } },

    { { -1, 7, 5, 4,-1 },
      {  7,-1, 2, 1,-1 },
      {  5, 2,-1, 0,-1 },
      {  4, 1, 0,-1,-1 },
      { -1,-1,-1,-1,-1 } } };

const int Dim4Triangle::triangleVertex[10][3] = {
    { 2, 3, 4 },
    { 1, 3, 4 },
    { 1, 2, 4 },
    { 1, 2, 3 },
    { 0, 3, 4 },
    { 0, 2, 4 },
    { 0, 2, 3 },
    { 0, 1, 4 },
    { 0, 1, 3 },
    { 0, 1, 2 } };

const NPerm5 Dim4Triangle::ordering[10] = {
    NPerm5(2, 3, 4, 0, 1),
    NPerm5(1, 3, 4, 2, 0),
    NPerm5(1, 2, 4, 0, 3),
    NPerm5(1, 2, 3, 4, 0),
    NPerm5(0, 3, 4, 1, 2),
    NPerm5(0, 2, 4, 3, 1),
    NPerm5(0, 2, 3, 1, 4),
    NPerm5(0, 1, 4, 2, 3),
    NPerm5(0, 1, 3, 4, 2),
    NPerm5(0, 1, 2, 3, 4)
};

Dim4Edge* Dim4Triangle::getEdge(int edge) const {
    const Dim4TriangleEmbedding& e(emb_.front());
    NPerm5 p = e.getVertices();
    return e.getPentachoron()->getEdge(
        Dim4Edge::edgeNumber[p[(edge + 1) % 3]][p[(edge + 2) % 3]]);
}

NPerm5 Dim4Triangle::getEdgeMapping(int edge) const {
    const Dim4TriangleEmbedding& e(emb_.front());

    NPerm5 trianglePerm = e.getVertices();
    NPerm5 edgePerm = e.getPentachoron()->getEdgeMapping(
        Dim4Edge::edgeNumber[trianglePerm[(edge + 1) % 3]]
                            [trianglePerm[(edge + 2) % 3]]);

    return NPerm5(
        trianglePerm.preImageOf(edgePerm[0]),
        trianglePerm.preImageOf(edgePerm[1]),
        edge, 3, 4);
}

void Dim4Triangle::writeTextLong(std::ostream& out) const {
    writeTextShort(out);
    out << std::endl;

    out << "Appears as:" << std::endl;
    std::deque<Dim4TriangleEmbedding>::const_iterator it;
    for (it = emb_.begin(); it != emb_.end(); ++it)
        out << "  " << it->getPentachoron()->markedIndex()
            << " (" << it->getVertices().trunc3() << ')' << std::endl;
}

} // namespace regina

