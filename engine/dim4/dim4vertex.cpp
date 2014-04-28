
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

#include "dim4/dim4vertex.h"
#include "maths/permconv.h"
#include "triangulation/ntriangulation.h"
#include <sstream>

namespace regina {

Dim4Vertex::~Dim4Vertex() {
    // Deleting null is always safe.
    delete link_;
}

void Dim4Vertex::writeTextShort(std::ostream& out) const {
    if (! link_->isClosed())
        out << "Boundary ";
    else if (link_->isThreeSphere())
        out << "Internal ";
    else
        out << "Ideal ";

    out << "vertex of degree " << emb_.size();
}

void Dim4Vertex::writeTextLong(std::ostream& out) const {
    writeTextShort(out);
    out << std::endl;

    out << "Appears as:" << std::endl;
    std::vector<Dim4VertexEmbedding>::const_iterator it;
    for (it = emb_.begin(); it != emb_.end(); ++it)
        out << "  " << it->getPentachoron()->markedIndex()
            << " (" << it->getVertex() << ')' << std::endl;
}

NTriangulation* Dim4Vertex::buildLinkDetail(bool labels,
        Dim4Isomorphism** inclusion) const {
    // Build the triangulation.
    NTriangulation* ans = new NTriangulation();
    NPacket::ChangeEventSpan span(ans);

    if (inclusion)
        *inclusion = new Dim4Isomorphism(emb_.size());

    std::vector<Dim4VertexEmbedding>::const_iterator it, adjIt;
    NTetrahedron* tTet;
    int i;
    for (it = emb_.begin(), i = 0; it != emb_.end(); ++it, ++i) {
        tTet = ans->newTetrahedron();
        if (labels) {
            std::stringstream s;
            s << it->getPentachoron()->markedIndex() <<
                " (" << it->getVertex() << ')';
            tTet->setDescription(s.str());
        }
        if (inclusion) {
            (*inclusion)->pentImage(i) = it->getPentachoron()->markedIndex();
            (*inclusion)->facetPerm(i) = it->getPentachoron()->
                getTetrahedronMapping(it->getVertex());
        }
    }

    Dim4Pentachoron *pent, *adj;
    int exitTet, v;
    int faceInLink;
    int adjIndex;
    int adjVertex;
    for (it = emb_.begin(), i = 0; it != emb_.end(); ++it, ++i) {
        pent = it->getPentachoron();
        v = it->getVertex();

        for (exitTet = 0; exitTet < 5; ++exitTet) {
            if (exitTet == v)
                continue;

            adj = pent->adjacentPentachoron(exitTet);
            if (! adj)
                continue;

            faceInLink = pent->getTetrahedronMapping(v).preImageOf(exitTet);
            if (ans->getTetrahedron(i)->adjacentTetrahedron(faceInLink)) {
                // We've already made this gluing in the vertex link
                // from the other side.
                continue;
            }

            adjVertex = pent->adjacentGluing(exitTet)[v];

            // TODO: We need to find which *embedding* corresponds to
            // the adjacent pentachoron/vertex pair.
            // Currently we do a simple linear scan, which makes the
            // overall link construction quadratic.  This can surely be
            // made linear(ish) with the right data structure and/or algorithm.
            for (adjIt = emb_.begin(), adjIndex = 0;
                    adjIt != emb_.end(); ++adjIt, ++adjIndex)
                if (adjIt->getPentachoron() == adj &&
                        adjIt->getVertex() == adjVertex)
                    break; // Sets adjIndex to the right value.

            ans->getTetrahedron(i)->joinTo(faceInLink,
                ans->getTetrahedron(adjIndex),
                perm5to4(adj->getTetrahedronMapping(adjVertex).inverse() *
                    pent->adjacentGluing(exitTet) *
                    pent->getTetrahedronMapping(v)));
        }
    }

    return ans;
}

} // namespace regina
