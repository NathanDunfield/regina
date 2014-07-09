
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

#include <set>
#include <vector>

#include "triangulation/ntriangulation.h"
#include "utilities/memutils.h"
#include "utilities/stlutils.h"

namespace regina {

void NTriangulation::barycentricSubdivision() {
    // Rewritten for Regina 4.94 to use a more sensible labelling scheme.

    // IMPORTANT: If this code is ever rewritten (and in particular, if
    // the labelling of new tetrahedra ever changes), then the
    // drillEdge() code must be rewritten as well (since it relies on
    // the specific labelling scheme that we use here).

    unsigned long nOldTet = tetrahedra_.size();
    if (nOldTet == 0)
        return;

    NTriangulation staging;
    ChangeEventSpan span1(&staging);

    NTetrahedron** newTet = new NTetrahedron*[nOldTet * 24];
    NTetrahedron* oldTet;

    // A tetrahedron in the subdivision is uniquely defined by the
    // permutation (face, edge, vtx, corner) of (0, 1, 2, 3).
    // This is the tetrahedron that:
    // - meets the boundary in the face opposite vertex "face";
    // - meets that face in the edge opposite vertex "edge";
    // - meets that edge in the vertex opposite vertex "vtx";
    // - directly touches vertex "corner".

    unsigned long tet;
    for (tet = 0; tet < 24 * nOldTet; ++tet)
        newTet[tet] = staging.newTetrahedron();

    // Do all of the internal gluings
    int permIdx;
    NPerm4 perm, glue;
    for (tet=0; tet < nOldTet; ++tet)
        for (permIdx = 0; permIdx < 24; ++permIdx) {
            perm = NPerm4::S4[permIdx];
            // (0, 1, 2, 3) -> (face, edge, vtx, corner)

            // Internal gluings within the old tetrahedron:
            newTet[24 * tet + permIdx]->joinTo(perm[3],
                newTet[24 * tet + (perm * NPerm4(3, 2)).S4Index()],
                NPerm4(perm[3], perm[2]));

            newTet[24 * tet + permIdx]->joinTo(perm[2],
                newTet[24 * tet + (perm * NPerm4(2, 1)).S4Index()],
                NPerm4(perm[2], perm[1]));

            newTet[24 * tet + permIdx]->joinTo(perm[1],
                newTet[24 * tet + (perm * NPerm4(1, 0)).S4Index()],
                NPerm4(perm[1], perm[0]));

            // Adjacent gluings to the adjacent tetrahedron:
            oldTet = getTetrahedron(tet);
            if (! oldTet->adjacentTetrahedron(perm[0]))
                continue; // This hits a boundary triangle.
            if (newTet[24 * tet + permIdx]->adjacentTetrahedron(perm[0]))
                continue; // We've already done this gluing from the other side.

            glue = oldTet->adjacentGluing(perm[0]);
            newTet[24 * tet + permIdx]->joinTo(perm[0],
                newTet[24 * tetrahedronIndex(
                    oldTet->adjacentTetrahedron(perm[0])) +
                    (glue * perm).S4Index()],
                glue);
        }


    // Delete the existing tetrahedra and put in the new ones.
    ChangeEventSpan span2(this);
    removeAllTetrahedra();
    swapContents(staging);
    delete[] newTet;
}

void NTriangulation::drillEdge(NEdge* e) {
    // Recall from the barycentric subdivision code above that
    // a tetrahedron in the subdivision is uniquely defined by the
    // permutation (face, edge, vtx, corner) of (0, 1, 2, 3).
    //
    // For an edge (i,j) opposite vertices (k,l), the tetrahedra that
    // meet it are:
    //
    // (k,l,i,j) and (l,k,i,j), both containing the half-edge touching vertex j;
    // (k,l,j,i) and (l,k,j,i), both containing the half-edge touching vertex i.
    //
    // In each case the corresponding edge number in the new tetrahedron
    // equals the edge number from the original tetrahedron.

    int edgeNum = e->getEmbedding(0).getEdge();
    long tetNum = tetrahedronIndex(e->getEmbedding(0).getTetrahedron());

    int oldToNew[2]; // Identifies two of the 24 tetrahedra in a subdivision
                     // that contain the two corresponding half-edges.
    oldToNew[0] = NPerm4(
        NEdge::edgeVertex[5 - edgeNum][0], NEdge::edgeVertex[5 - edgeNum][1],
        NEdge::edgeVertex[edgeNum][0], NEdge::edgeVertex[edgeNum][1]).
        S4Index();
    oldToNew[1] = NPerm4(
        NEdge::edgeVertex[5 - edgeNum][0], NEdge::edgeVertex[5 - edgeNum][1],
        NEdge::edgeVertex[edgeNum][1], NEdge::edgeVertex[edgeNum][0]).
        S4Index();

    ChangeEventSpan span(this);
    barycentricSubdivision();
    barycentricSubdivision();

    std::set<unsigned long> toRemove;

    int i, j, k;
    unsigned long finalTet;
    NVertex* finalVertex;
    std::vector<NVertexEmbedding>::const_iterator it;
    for (i = 0; i < 2; ++i)
        for (j = 0; j < 2; ++j) {
            finalTet = 24 * (24 * tetNum + oldToNew[i]) + oldToNew[j];

            // Remove all tetrahedra that touch each endpoint of the
            // resulting edge in the second barycentric subdivision.
            for (k = 0; k < 2; ++k) {
                finalVertex = tetrahedra_[finalTet]->getEdge(edgeNum)->
                    getVertex(k);
                for (it = finalVertex->getEmbeddings().begin();
                        it != finalVertex->getEmbeddings().end(); ++it)
                    toRemove.insert(tetrahedronIndex(it->getTetrahedron()));
            }
        }

    // Make sure we remove tetrahedra in reverse order, so the numbering
    // doesn't change.
    for (std::set<unsigned long>::reverse_iterator rit = toRemove.rbegin();
            rit != toRemove.rend(); ++rit)
        removeTetrahedronAt(*rit);

    // We have lots of tetrahedra now.  Simplify.
    intelligentSimplify();
}

bool NTriangulation::idealToFinite(bool forceDivision) {
    // The call to isValid() ensures the skeleton has been calculated.
    if (isValid() && ! isIdeal())
        if (! forceDivision)
            return false;

    int i,j,k,l;
    long numOldTet = tetrahedra_.size();
    if (! numOldTet)
        return false;

    NTriangulation staging;
    ChangeEventSpan span1(&staging);

    NTetrahedron **newTet = new NTetrahedron*[32*numOldTet];
    for (i=0; i<32*numOldTet; i++)
        newTet[i] = staging.newTetrahedron();

    int tip[4];
    int interior[4];
    int edge[4][4];
    int vertex[4][4];

    int nDiv = 0;
    for (j=0; j<4; j++) {
        tip[j] = nDiv++;
        interior[j] = nDiv++;

        for (k=0; k<4; k++)
            if (j != k) {
                edge[j][k] = nDiv++;
                vertex[j][k] = nDiv++;
            }
    }

    // First glue all of the tetrahedra inside the same
    // old tetrahedron together.
    for (i=0; i<numOldTet; i++) {
        // Glue the tip tetrahedra to the others.
        for (j=0; j<4; j++)
            newTet[tip[j] + i * nDiv]->joinTo(j,
                newTet[interior[j] + i * nDiv], NPerm4());

        // Glue the interior tetrahedra to the others.
        for (j=0; j<4; j++) {
            for (k=0; k<4; k++)
                if (j != k) {
                    newTet[interior[j] + i * nDiv]->joinTo(k,
                        newTet[vertex[k][j] + i * nDiv], NPerm4());
                }
        }

        // Glue the edge tetrahedra to the others.
        for (j=0; j<4; j++)
            for (k=0; k<4; k++)
                if (j != k) {
                    newTet[edge[j][k] + i * nDiv]->joinTo(j,
                        newTet[edge[k][j] + i * nDiv], NPerm4(j,k));

                    for (l=0; l<4; l++)
                        if ( (l != j) && (l != k) )
                            newTet[edge[j][k] + i * nDiv]->joinTo(l,
                                newTet[vertex[j][l] + i * nDiv], NPerm4(k,l));
                }
    }

    // Now deal with the gluings between the pieces inside adjacent tetrahedra.
    NTetrahedron *ot;
    long oppTet;
    NPerm4 p;
    for (i=0; i<numOldTet; i++) {
        ot = getTetrahedron(i);
        for (j=0; j<4; j++)
            if (ot->adjacentTetrahedron(j)) {
                 oppTet = tetrahedronIndex(ot->adjacentTetrahedron(j));
                 p = ot->adjacentGluing(j);

                 // First deal with the tip tetrahedra.
                 for (k=0; k<4; k++)
                     if (j != k)
                          newTet[tip[k] + i * nDiv]->joinTo(j,
                              newTet[tip[p[k]] + oppTet * nDiv], p);

                 // Next the edge tetrahedra.
                 for (k=0; k<4; k++)
                     if (j != k)
                         newTet[edge[j][k] + i * nDiv]->joinTo(k,
                             newTet[edge[p[j]][p[k]] + oppTet * nDiv], p);

                 // Finally, the vertex tetrahedra.
                 for (k=0; k<4; k++)
                     if (j != k)
                         newTet[vertex[j][k] + i * nDiv]->joinTo(k,
                             newTet[vertex[p[j]][p[k]] + oppTet * nDiv], p);

            }
    }

    ChangeEventSpan span2(this);

    removeAllTetrahedra();
    swapContents(staging);
    calculateSkeleton();

    // Remove the tetrahedra that meet any of the non-standard or
    // ideal vertices.
    // First we make a list of the tetrahedra.
    std::vector<NTetrahedron*> tetList;
    std::vector<NVertexEmbedding>::const_iterator vembit;
    for (VertexIterator vIter = vertices_.begin();
            vIter != vertices_.end(); vIter++)
        if ((*vIter)->isIdeal() || ! (*vIter)->isStandard())
            for (vembit = (*vIter)->getEmbeddings().begin();
                    vembit != (*vIter)->getEmbeddings().end(); vembit++)
                tetList.push_back((*vembit).getTetrahedron());

    // Now remove the tetrahedra.
    // For each tetrahedron, remove it and delete it.
    for_each(tetList.begin(), tetList.end(),
        std::bind1st(std::mem_fun(&NTriangulation::removeTetrahedron), this));

    delete[] newTet;
    return true;
}

bool NTriangulation::finiteToIdeal() {
    if (! hasBoundaryTriangles())
        return false;

    // Make a list of all boundary triangles, indexed by triangle number,
    // and create the corresponding new tetrahedra.
    unsigned long nTriangles = getNumberOfTriangles();

    NTriangulation staging;
    NTetrahedron** bdry = new NTetrahedron*[nTriangles];
    NPerm4* bdryPerm = new NPerm4[nTriangles];
    NTetrahedron** newTet = new NTetrahedron*[nTriangles];

    ChangeEventSpan span1(&staging);

    TriangleIterator fit;
    unsigned i;
    for (i = 0, fit = triangles_.begin(); fit != triangles_.end(); ++i, ++fit) {
        if (! (*fit)->isBoundary()) {
            bdry[i] = newTet[i] = 0;
            continue;
        }

        bdry[i] = (*fit)->getEmbedding(0).getTetrahedron();
        bdryPerm[i] = (*fit)->getEmbedding(0).getVertices();
    }

    // Add the new tetrahedra one boundary component at a time, so that
    // the tetrahedron labels are compatible with previous versions of
    // regina (<= 4.6).
    BoundaryComponentIterator bit;
    for (bit = boundaryComponents_.begin();
            bit != boundaryComponents_.end(); bit++)
        for (i = 0; i < (*bit)->getNumberOfTriangles(); ++i)
            newTet[(*bit)->getTriangle(i)->markedIndex()] =
                staging.newTetrahedron();

    // Glue the new tetrahedra to each other.
    NEdge* edge;
    unsigned long tetTriangle1, tetTriangle2;
    NPerm4 t1Perm, t2Perm;
    for (bit = boundaryComponents_.begin();
            bit != boundaryComponents_.end(); bit++)
        for (i = 0; i < (*bit)->getNumberOfEdges(); i++) {
            edge = (*bit)->getEdge(i);

            // This must be a valid boundary edge.
            // Find the boundary triangles at either end.
            NEdgeEmbedding e1 = edge->getEmbeddings().front();
            NEdgeEmbedding e2 = edge->getEmbeddings().back();

            tetTriangle1 = e1.getTetrahedron()->getTriangle(
                e1.getVertices()[3])->markedIndex();
            tetTriangle2 = e2.getTetrahedron()->getTriangle(
                e2.getVertices()[2])->markedIndex();

            t1Perm = bdryPerm[tetTriangle1].inverse() * e1.getVertices();
            t2Perm = bdryPerm[tetTriangle2].inverse() * e2.getVertices() *
                NPerm4(2, 3);

            newTet[tetTriangle1]->joinTo(t1Perm[2], newTet[tetTriangle2],
                t2Perm * t1Perm.inverse());
        }

    // Now join the new tetrahedra to the boundary triangles of the original
    // triangulation.

    // Set up a change block, since here we start changing the original
    // triangulation.
    ChangeEventSpan span2(this);

    staging.moveContentsTo(*this);

    for (i = 0; i < nTriangles; ++i)
        if (newTet[i])
            newTet[i]->joinTo(3, bdry[i], bdryPerm[i]);

    // Clean up and return.
    delete[] newTet;
    delete[] bdryPerm;
    delete[] bdry;
    return true;
}

void NTriangulation::puncture(NTetrahedron* tet) {
    if (! tet) {
        // Preconditions disallow empty triangulations, but anyway:
        if (tetrahedra_.empty())
            return;

        tet = tetrahedra_.front();
    }

    ChangeEventSpan span(this);

    // We will attach a pair of triangular prisms to face 123 of tet.
    // We will join the rectangular walls of the prisms together, and
    // one triangular end from each will join to form the new S^2 boundary.
    NTetrahedron* prism[2][3];

    // Create the new tetrahedra in an order that ensures that the new
    // S^2 boundary will appear in the final two tetrahedra.
    int i, j;
    for (j = 0; j < 3; ++j)
        for (i = 0; i < 2; ++i)
            prism[i][j] = newTetrahedron();

    prism[0][0]->joinTo(0, prism[0][1], NPerm4(3,0,1,2));
    prism[0][1]->joinTo(0, prism[0][2], NPerm4(3,0,1,2));

    prism[1][0]->joinTo(1, prism[1][1], NPerm4(3,0,1,2));
    prism[1][1]->joinTo(1, prism[1][2], NPerm4(3,2,0,1));

    prism[0][0]->joinTo(1, prism[1][0], NPerm4(1,2,3,0));
    prism[0][0]->joinTo(2, prism[1][0], NPerm4(1,2,3,0));
    prism[0][1]->joinTo(1, prism[1][1], NPerm4(1,2,3,0));
    prism[0][1]->joinTo(2, prism[1][1], NPerm4(1,2,3,0));
    prism[0][2]->joinTo(1, prism[1][2], NPerm4(0,1,3,2));
    prism[0][2]->joinTo(2, prism[1][2], NPerm4(0,1,3,2));

    NTetrahedron* adj = tet->adjacentTetrahedron(0);
    if (adj) {
        NPerm4 gluing = tet->adjacentGluing(0);
        tet->unjoin(0);
        prism[1][0]->joinTo(0, adj, gluing);
    }

    tet->joinTo(0, prism[0][0], NPerm4(3,0,1,2));
}

void NTriangulation::connectedSumWith(const NTriangulation& other) {
    // Precondition check.
    if (tetrahedra_.empty() || ! isConnected())
        return;

    ChangeEventSpan span(this);

    NTetrahedron* bdry[2][2];
    unsigned long n;

    puncture();

    n = tetrahedra_.size();
    bdry[0][0] = tetrahedra_[n - 2];
    bdry[0][1] = tetrahedra_[n - 1];

    insertTriangulation(other);
    puncture(tetrahedra_[n]);

    n = tetrahedra_.size();
    bdry[1][0] = tetrahedra_[n - 2];
    bdry[1][1] = tetrahedra_[n - 1];

    bdry[0][0]->joinTo(0, bdry[1][0], NPerm4(2, 3));
    bdry[0][1]->joinTo(0, bdry[1][1], NPerm4(2, 3));
}

} // namespace regina
