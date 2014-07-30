
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

#include "triangulation/ntriangulation.h"

namespace regina {

void NTriangulation::maximalForestInBoundary(std::set<NEdge*>& edgeSet,
        std::set<NVertex*>& vertexSet) const {
    if (! calculatedSkeleton_)
        calculateSkeleton();

    vertexSet.clear();
    edgeSet.clear();
    for (BoundaryComponentIterator bit = boundaryComponents_.begin();
            bit != boundaryComponents_.end(); bit++)
        stretchBoundaryForestFromVertex((*bit)->getVertex(0),
            edgeSet, vertexSet);
}

void NTriangulation::stretchBoundaryForestFromVertex(NVertex* from,
        std::set<NEdge*>& edgeSet,
        std::set<NVertex*>& vertexSet) const {
    vertexSet.insert(from);

    std::vector<NVertexEmbedding>::const_iterator it =
        from->getEmbeddings().begin();
    NTetrahedron* tet;
    NVertex* otherVertex;
    NEdge* edge;
    int vertex, yourVertex;
    while (it != from->getEmbeddings().end()) {
        const NVertexEmbedding& emb = *it;
        tet = emb.getTetrahedron();
        vertex = emb.getVertex();
        for (yourVertex = 0; yourVertex < 4; yourVertex++) {
            if (vertex == yourVertex)
                continue;
            edge = tet->getEdge(NEdge::edgeNumber[vertex][yourVertex]);
            if (! (edge->isBoundary()))
                continue;
            otherVertex = tet->getVertex(yourVertex);
            if (! vertexSet.count(otherVertex)) {
                edgeSet.insert(edge);
                stretchBoundaryForestFromVertex(otherVertex, edgeSet,
                    vertexSet);
            }
        }
        it++;
    }
}

void NTriangulation::maximalForestInSkeleton(std::set<NEdge*>& edgeSet,
        bool canJoinBoundaries) const {
    if (! calculatedSkeleton_)
        calculateSkeleton();

    std::set<NVertex*> vertexSet;
    std::set<NVertex*> thisBranch;

    if (canJoinBoundaries)
        edgeSet.clear();
    else
        maximalForestInBoundary(edgeSet, vertexSet);

    for (VertexIterator vit = vertices_.begin(); vit != vertices_.end(); vit++)
        if (! (vertexSet.count(*vit))) {
            stretchForestFromVertex(*vit, edgeSet, vertexSet, thisBranch);
            thisBranch.clear();
        }
}

bool NTriangulation::stretchForestFromVertex(NVertex* from,
        std::set<NEdge*>& edgeSet,
        std::set<NVertex*>& vertexSet,
        std::set<NVertex*>& thisStretch) const {
    // Moves out from the vertex until we hit a vertex that has already
    //     been visited; then stops.
    // Returns true if we make such a link.
    // PRE: Such a link has not already been made.
    vertexSet.insert(from);
    thisStretch.insert(from);

    std::vector<NVertexEmbedding>::const_iterator it =
        from->getEmbeddings().begin();
    NTetrahedron* tet;
    NVertex* otherVertex;
    int vertex, yourVertex;
    bool madeLink = false;
    while (it != from->getEmbeddings().end()) {
        const NVertexEmbedding& emb = *it;
        tet = emb.getTetrahedron();
        vertex = emb.getVertex();
        for (yourVertex = 0; yourVertex < 4; yourVertex++) {
            if (vertex == yourVertex)
                continue;
            otherVertex = tet->getVertex(yourVertex);
            if (thisStretch.count(otherVertex))
                continue;
            madeLink = vertexSet.count(otherVertex);
            edgeSet.insert(tet->getEdge(NEdge::edgeNumber[vertex][yourVertex]));
            if (! madeLink)
                madeLink =
                    stretchForestFromVertex(otherVertex, edgeSet, vertexSet,
                    thisStretch);
            if (madeLink)
                return true;
        }
        it++;
    }
    return false;
}

void NTriangulation::maximalForestInDualSkeleton(std::set<NTriangle*>& triSet)
        const {
    if (! calculatedSkeleton_)
        calculateSkeleton();

    triSet.clear();
    std::set<NTetrahedron*> visited;
    for (TetrahedronIterator it = tetrahedra_.begin(); it != tetrahedra_.end();
            it++)
        if (! (visited.count(*it)))
            stretchDualForestFromTet(*it, triSet, visited);
}

void NTriangulation::stretchDualForestFromTet(NTetrahedron* tet,
        std::set<NTriangle*>& triSet, std::set<NTetrahedron*>& visited) const {
    visited.insert(tet);

    NTetrahedron* adjTet;
    for (int face = 0; face < 4; face++) {
        adjTet = tet->adjacentTetrahedron(face);
        if (adjTet)
            if (! (visited.count(adjTet))) {
                triSet.insert(tet->getTriangle(face));
                stretchDualForestFromTet(adjTet, triSet, visited);
            }
    }
}

} // namespace regina

