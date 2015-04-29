
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
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

#include <sstream>
#include "census/ngluingpermsearcher.h"
#include "triangulation/nedge.h"
#include "triangulation/nfacepair.h"
#include "triangulation/ntriangulation.h"
#include "utilities/memutils.h"

namespace regina {

OneStepSearcher::OneStepSearcher(const NFacePairing* pairing,
        const NFacePairing::IsoList* autos, CensusDB* db, bool orientableOnly,
        int whichPurge, UseGluingPerms use, void* useArgs) :
        NGluingPermSearcher(pairing, autos, orientableOnly,
            true /* finiteOnly */, whichPurge, use, useArgs), db_(db),
        usedDB(false) {

    // First check the database to see if results are available.

    DBStatus status = db_->request(pairing); // TODO string rep?
    while ( status == CensusDB::PairingRunning ) {
        sleep(5);
        status = db_->request(pairing);
    }
    if ( status == CensusDB::PairingDone ) {
        useDB = true;
        // TODO use each result from DB
        // use(0, useArgs);
        return;
    }

    if (pairing->size() > 1) {
      NFacePairing childPairing(*pairing);
      childPairing.removeSimplex(childPairing.size() - 1); // Remove last simplex
      child = new OneStepSearcher(childPairing, 0, db, orientableOnly, whichPurge,
          use, useArgs);
    } else {
      child = NULL;
    }

    // Initialise the internal arrays to accurately reflect the underlying
    // face pairing.

    unsigned nTets = size();

    // ---------- Tracking of vertex / edge equivalence classes ----------

    unsigned i;

    nVertexClasses = nTets * 4;
    vertexState = new TetVertexState[nTets * 4];
    vertexStateChanged = new int[nTets * 8];
    std::fill(vertexStateChanged, vertexStateChanged + nTets * 8, -1);
    for (i = 0; i < nTets * 4; i++) {
        vertexState[i].bdryEdges = 3;
        vertexState[i].bdryNext[0] = vertexState[i].bdryNext[1] = i;
        vertexState[i].bdryTwist[0] = vertexState[i].bdryTwist[1] = 0;
        // Initialise the backup members also so we're not writing
        // uninitialised data via dumpData().
        vertexState[i].bdryNextOld[0] = vertexState[i].bdryNextOld[1] = -1;
        vertexState[i].bdryTwistOld[0] = vertexState[i].bdryTwistOld[1] = 0;
    }

    nEdgeClasses = nTets * 6;
    edgeState = new TetEdgeState[nTets * 6];
    edgeStateChanged = new int[nTets * 8];
    std::fill(edgeStateChanged, edgeStateChanged + nTets * 8, -1);

    // Since NQitmaskLen64 only supports 64 faces, only work with
    // the first 16 tetrahedra.  If n > 16, this just weakens the
    // optimisation; however, this is no great loss since for n > 16 the
    // census code is at present infeasibly slow anyway.
    for (i = 0; i < nTets && i < 16; ++i) {
        /* 01 on +012, +013             */
        edgeState[6 * i    ].facesPos.set(4 * i + 3, 1);
        edgeState[6 * i    ].facesPos.set(4 * i + 2, 1);
        /* 02 on -012        +023       */
        edgeState[6 * i + 1].facesNeg.set(4 * i + 3, 1);
        edgeState[6 * i + 1].facesPos.set(4 * i + 1, 1);
        /* 03 on       -013, -023       */
        edgeState[6 * i + 2].facesNeg.set(4 * i + 2, 1);
        edgeState[6 * i + 2].facesNeg.set(4 * i + 1, 1);
        /* 12 on +012,             +123 */
        edgeState[6 * i + 3].facesPos.set(4 * i + 3, 1);
        edgeState[6 * i + 3].facesPos.set(4 * i + 0, 1);
        /* 13 on       +013        -123 */
        edgeState[6 * i + 4].facesPos.set(4 * i + 2, 1);
        edgeState[6 * i + 4].facesNeg.set(4 * i + 0, 1);
        /* 23 on             +023, +123 */
        edgeState[6 * i + 5].facesPos.set(4 * i + 1, 1);
        edgeState[6 * i + 5].facesPos.set(4 * i + 0, 1);
    }
}

// TODO (net): See what was removed when we brought in vertex link checking.
void OneStepSearcher::runSearch(long maxDepth) {
    if (useDB) {
        // TODO use each result from DB
        use(0, useArgs);
        return;
    }
    if (child)
        child->runSearch( & (this->buildUp)) ; // TODO Check syntax
    else
        buildUp(NULL);
}

void buildUp(OneStepSearcher *child_) { // Note that child_ and child should
                                        // always be identical.
    if (child) {
        std::memcpy(permIndex, child->permIndex, sizeof(child->permIndex));
        std::memcpy(TetVertexState, child->TetVertexState,
            sizeof(child->TetVertexState));
        std::memcpy(TetVertexStateChanged, child->TetVertexStateChanged,
            sizeof(child->TetVertexStateChanged));
        std::memcpy(TetEdgeState, child->TetEdgeState,
            sizeof(child->TetEdgeState));
        std::memcpy(TetEdgeStateChanged, child->TetEdgeStateChanged,
            sizeof(child->TetEdgeStateChanged));
        if (orientableOnly_)
          std::memcpy(orientation, child->orientation, sizeof(child->orientation));
        // TODO Set up edge + vertex states for the last tetrahedron
    }

    permIndex(NFacetSpec(size()-1,0)) = 0; // First permutation in first gluing.

    glue(0); // Attempt first gluing
}

void glue(int f, int step) {
    NFacetSpec face(size()-1,f);
    if (face.isPastEnd(size(),true)) { // Either this facet is not to be
      // matched, or we have completed a triangulation. Either way, use the
      // result.
        if (isCanonical()) {
            use_(this, useArgs);
            db->sendResult(pairing_, this);
        }
        return;
    }

    // Note that, unlike other GluingPermSearcher classes, here the "adjacent"
    // facet is an earlier facet that has already been determined. We only have
    // to iterate over four faces of the last tetrahedron, so we call those
    // "face"
    NFacetSpec adj = (*pairing_)[face];

    // Is this the last gluing before a closed triangulation is complete?
    bool lastGluing = false;
    if ((f == 3) || (( adj.simp == (size()-1) && adj.facet == 3)))
        lastGluing = true;

    for ( ; permIndex(facet) < 6; permIndex(face) += step ) {
        // The first permutation to this new tetrahedron must be the identity
        // permutation to be canonical.
        if ( (f == 1) && (permIndex(facet) > 0) )
            return;

        // Merge edge links and run corresponding tests.
        if (mergeEdgeClasses(face)) {
            // We created a structure that should not appear in a final
            // census triangulation (e.g., a low-degree or invalid edge,
            // or a face whose edges are identified in certain ways).
            splitEdgeClasses(face);
            continue;
        }

        // Merge vertex links and run corresponding tests.
        mergeResult = mergeVertexClasses(face);
        if (mergeResult & VLINK_CLOSED) {
            // We closed off a vertex link, which means we will end up
            // with more than one vertex (unless this was our very last
            // gluing).
            if (!lastGluing) {
                splitVertexClasses(face);
                splitEdgeClasses(face);
                continue;
            }
        }
        if (mergeResult & VLINK_NON_SPHERE) {
            // Our vertex link will never be a 2-sphere.  Stop now.
            splitVertexClasses(face);
            splitEdgeClasses(face);
            continue;
        }

        // We are sitting on a new permutation to try.
        permIndex(adj) = NPerm4::invS3[permIndex(face)];
        // Fix the orientation if appropriate.
        if ( (f == 0) && (orientableOnly_) {
            // It's the first time we've hit this tetrahedron.
            if ((permIndex(face) + (face.facet == 3 ? 0 : 1) +
                    (adj.facet == 3 ? 0 : 1)) % 2 == 0)
                orientation[face.simp] = -orientation[adj.simp];
            else
                orientation[face.simp] = orientation[adj.simp];
        }
        int nextFacet = f+1;
        if (adj.simp = face.simp)
            nextFacet++;
        permIndex(NFacetSpec(size()-1,nextFacet)) = 0; // First permutation in next gluing.
        glue(nextFacet);

        splitVertexClasses(face);
        splitEdgeClasses(face);

    }




void OneStepSearcher::dumpData(std::ostream& out) const {
    NGluingPermSearcher::dumpData(out);

    unsigned nTets = getNumberOfTetrahedra();
    unsigned i;

    out << nVertexClasses << std::endl;
    for (i = 0; i < 4 * nTets; i++) {
        vertexState[i].dumpData(out);
        out << std::endl;
    }
    for (i = 0; i < 8 * nTets; i++) {
        if (i)
            out << ' ';
        out << vertexStateChanged[i];
    }
    out << std::endl;

    out << nEdgeClasses << std::endl;
    for (i = 0; i < 6 * nTets; i++) {
        edgeState[i].dumpData(out, nTets);
        out << std::endl;
    }
    for (i = 0; i < 8 * nTets; i++) {
        if (i)
            out << ' ';
        out << edgeStateChanged[i];
    }
    out << std::endl;
}

//NCompactSearcher::NCompactSearcher(std::istream& in,
//        UseGluingPerms use, void* useArgs) :
//        NGluingPermSearcher(in, use, useArgs),
//        nVertexClasses(0), vertexState(0), vertexStateChanged(0),
//        nEdgeClasses(0), edgeState(0), edgeStateChanged(0) {
//    if (inputError_)
//        return;
//
//    unsigned nTets = getNumberOfTetrahedra();
//    unsigned i;
//
//    in >> nVertexClasses;
//    if (nVertexClasses > 4 * nTets) {
//        inputError_ = true; return;
//    }
//
//    vertexState = new TetVertexState[4 * nTets];
//    for (i = 0; i < 4 * nTets; i++)
//        if (! vertexState[i].readData(in, 4 * nTets)) {
//            inputError_ = true; return;
//        }
//
//    vertexStateChanged = new int[8 * nTets];
//    for (i = 0; i < 8 * nTets; i++) {
//        in >> vertexStateChanged[i];
//        if (vertexStateChanged[i] < -1 ||
//                 vertexStateChanged[i] >= 4 * static_cast<int>(nTets)) {
//            inputError_ = true; return;
//        }
//    }
//
//    in >> nEdgeClasses;
//    if (nEdgeClasses > 6 * nTets) {
//        inputError_ = true; return;
//    }
//
//    edgeState = new TetEdgeState[6 * nTets];
//    for (i = 0; i < 6 * nTets; i++)
//        if (! edgeState[i].readData(in, nTets)) {
//            inputError_ = true; return;
//        }
//
//    edgeStateChanged = new int[8 * nTets];
//    for (i = 0; i < 8 * nTets; i++) {
//        in >> edgeStateChanged[i];
//        if (edgeStateChanged[i] < -1 ||
//                 edgeStateChanged[i] >= 6 * static_cast<int>(nTets)) {
//            inputError_ = true; return;
//        }
//    }
//
//    // Did we hit an unexpected EOF?
//    if (in.eof())
//        inputError_ = true;
//}

int OneStepSearcher::mergeEdgeClasses(NTetFace face) {
    NTetFace adj = (*pairing_)[face];

    int retVal = 0;

    NPerm4 p = gluingPerm(face);
    int v1, w1, v2, w2;
    int e, f;
    int orderIdx;
    int eRep, fRep;
    int middleTet;

    v1 = face.facet;
    w1 = p[v1];

    char parentTwists, hasTwist;
    for (v2 = 0; v2 < 4; v2++) {
        if (v2 == v1)
            continue;

        w2 = p[v2];

        // Look at the edge opposite v1-v2.
        e = 5 - NEdge::edgeNumber[v1][v2];
        f = 5 - NEdge::edgeNumber[w1][w2];

        orderIdx = v2 + 4 * orderElt;

        // We declare the natural orientation of an edge to be smaller
        // vertex to larger vertex.
        hasTwist = (p[NEdge::edgeVertex[e][0]] > p[NEdge::edgeVertex[e][1]] ?
            1 : 0);

        parentTwists = 0;
        eRep = findEdgeClass(e + 6 * face.simp, parentTwists);
        fRep = findEdgeClass(f + 6 * adj.simp, parentTwists);

        if (eRep == fRep) {
            edgeState[eRep].bounded = false;

            if (edgeState[eRep].size <= 2)
                retVal |= ECLASS_LOWDEG;
            else if (edgeState[eRep].size == 3) {
                // Flag as LOWDEG only if three distinct tetrahedra are used.
                middleTet = pairing_->dest(face.simp, v2).simp;
                if (face.simp != adj.simp && adj.simp != middleTet &&
                        middleTet != face.simp)
                    retVal |= ECLASS_LOWDEG;
            }
            if (hasTwist ^ parentTwists)
                retVal |= ECLASS_TWISTED;

            edgeStateChanged[orderIdx] = -1;
        } else {
#if PRUNE_HIGH_DEG_EDGE_SET
            if (edgeState[eRep].size >= highDegLimit) {
                if (edgeState[fRep].size >= highDegLimit)
                    highDegSum += highDegLimit;
                else
                    highDegSum += edgeState[fRep].size;
            } else if (edgeState[fRep].size >= highDegLimit)
                highDegSum += edgeState[eRep].size;
            else if (edgeState[eRep].size + edgeState[fRep].size >
                    highDegLimit)
                highDegSum += (edgeState[eRep].size + edgeState[fRep].size -
                    highDegLimit);
#endif

            if (edgeState[eRep].rank < edgeState[fRep].rank) {
                // Join eRep beneath fRep.
                edgeState[eRep].parent = fRep;
                edgeState[eRep].twistUp = hasTwist ^ parentTwists;

                edgeState[fRep].size += edgeState[eRep].size;
#if PRUNE_HIGH_DEG_EDGE_SET
#else
                if (edgeState[fRep].size > 3 * getNumberOfTetrahedra())
                    retVal |= ECLASS_HIGHDEG;
#endif

                if (edgeState[eRep].twistUp) {
                    edgeState[fRep].facesPos += edgeState[eRep].facesNeg;
                    edgeState[fRep].facesNeg += edgeState[eRep].facesPos;
                } else {
                    edgeState[fRep].facesPos += edgeState[eRep].facesPos;
                    edgeState[fRep].facesNeg += edgeState[eRep].facesNeg;
                }
                if (edgeState[fRep].facesPos.hasNonZeroMatch(
                        edgeState[fRep].facesNeg))
                    retVal |= ECLASS_CONE;
                if (edgeState[fRep].facesPos.has3() ||
                        edgeState[fRep].facesNeg.has3())
                    retVal |= ECLASS_L31;

                edgeStateChanged[orderIdx] = eRep;
            } else {
                // Join fRep beneath eRep.
                edgeState[fRep].parent = eRep;
                edgeState[fRep].twistUp = hasTwist ^ parentTwists;
                if (edgeState[eRep].rank == edgeState[fRep].rank) {
                    edgeState[eRep].rank++;
                    edgeState[fRep].hadEqualRank = true;
                }

                edgeState[eRep].size += edgeState[fRep].size;
#if PRUNE_HIGH_DEG_EDGE_SET
#else
                if (edgeState[eRep].size > 3 * getNumberOfTetrahedra())
                    retVal |= ECLASS_HIGHDEG;
#endif

                if (edgeState[fRep].twistUp) {
                    edgeState[eRep].facesPos += edgeState[fRep].facesNeg;
                    edgeState[eRep].facesNeg += edgeState[fRep].facesPos;
                } else {
                    edgeState[eRep].facesPos += edgeState[fRep].facesPos;
                    edgeState[eRep].facesNeg += edgeState[fRep].facesNeg;
                }
                if (edgeState[eRep].facesPos.hasNonZeroMatch(
                        edgeState[eRep].facesNeg))
                    retVal |= ECLASS_CONE;
                if (edgeState[eRep].facesPos.has3() ||
                        edgeState[eRep].facesNeg.has3())
                    retVal |= ECLASS_L31;

                edgeStateChanged[orderIdx] = fRep;
            }

#if PRUNE_HIGH_DEG_EDGE_SET
            if (highDegSum > highDegBound)
                retVal |= ECLASS_HIGHDEG;
#endif

            nEdgeClasses--;
        }
    }

    return retVal;
}

void OneStepSearcher::splitEdgeClasses(NTetFace face) {

    int v1, v2;
    int e;
    int eIdx, orderIdx;
    int rep, subRep;

    v1 = face.facet;

    for (v2 = 3; v2 >= 0; v2--) {
        if (v2 == v1)
            continue;

        // Look at the edge opposite v1-v2.
        e = 5 - NEdge::edgeNumber[v1][v2];

        eIdx = e + 6 * face.simp;
        orderIdx = v2 + 4 * orderElt;

        if (edgeStateChanged[orderIdx] < 0)
            edgeState[findEdgeClass(eIdx)].bounded = true;
        else {
            subRep = edgeStateChanged[orderIdx];
            rep = edgeState[subRep].parent;

            edgeState[subRep].parent = -1;
            if (edgeState[subRep].hadEqualRank) {
                edgeState[subRep].hadEqualRank = false;
                edgeState[rep].rank--;
            }

            edgeState[rep].size -= edgeState[subRep].size;
#if PRUNE_HIGH_DEG_EDGE_SET
            if (edgeState[rep].size >= highDegLimit) {
                if (edgeState[subRep].size >= highDegLimit)
                    highDegSum -= highDegLimit;
                else
                    highDegSum -= edgeState[subRep].size;
            } else if (edgeState[subRep].size >= highDegLimit)
                highDegSum -= edgeState[rep].size;
            else if (edgeState[rep].size + edgeState[subRep].size >
                    highDegLimit)
                highDegSum -= (edgeState[rep].size + edgeState[subRep].size
                    - highDegLimit);
#endif

            if (edgeState[subRep].twistUp) {
                edgeState[rep].facesPos -= edgeState[subRep].facesNeg;
                edgeState[rep].facesNeg -= edgeState[subRep].facesPos;
            } else {
                edgeState[rep].facesPos -= edgeState[subRep].facesPos;
                edgeState[rep].facesNeg -= edgeState[subRep].facesNeg;
            }

            edgeStateChanged[orderIdx] = -1;
            nEdgeClasses++;
        }
    }
}

int OneStepSearcher::mergeVertexClasses(NTetFace face) {
    // Merge all three vertex pairs for the current face.
    NTetFace adj = (*pairing_)[face];

    int retVal = 0;

    int v, w;
    int vIdx, wIdx, tmpIdx, nextIdx;
    unsigned orderIdx;
    int vRep, wRep;
    int vNext[2], wNext[2];
    char vTwist[2], wTwist[2];
    NPerm4 p = gluingPerm(face);
    char parentTwists, hasTwist, tmpTwist;
    for (v = 0; v < 4; v++) {
        if (v == face.facet)
            continue;

        w = p[v];
        vIdx = v + 4 * face.simp;
        wIdx = w + 4 * adj.simp;
        orderIdx = v + 4 * orderElt;

        // Are the natural 012 representations of the two faces joined
        // with reversed orientations?
        // Here we combine the sign of permutation p with the mappings
        // from 012 to the native tetrahedron vertices, i.e., v <-> 3 and
        // w <-> 3.
        hasTwist = (p.sign() < 0 ? 0 : 1);
        if ((v == 3 && w != 3) || (v != 3 && w == 3))
            hasTwist ^= 1;

        parentTwists = 0;
        for (vRep = vIdx; vertexState[vRep].parent >= 0;
                vRep = vertexState[vRep].parent)
            parentTwists ^= vertexState[vRep].twistUp;
        for (wRep = wIdx; vertexState[wRep].parent >= 0;
                wRep = vertexState[wRep].parent)
            parentTwists ^= vertexState[wRep].twistUp;

        if (vRep == wRep) {
            vertexState[vRep].bdry -= 2;
            if (vertexState[vRep].bdry == 0)
                retVal |= VLINK_CLOSED;

            // Have we made the vertex link non-orientable?
            if (hasTwist ^ parentTwists)
                retVal |= VLINK_NON_SPHERE;

            vertexStateChanged[orderIdx] = -1;

            // Examine the cycles of boundary components.
            if (vIdx == wIdx) {
                // Either we are folding together two adjacent edges of the
                // vertex link, or we are making the vertex link
                // non-orientable.

                // The possible cases are:
                //
                // 1) hasTwist is true.  The vertex becomes
                // non-orientable, but we should already have flagged
                // this above.  Don't touch anything.
                //
                // 2) hasTwist is false, and vertexState[vIdx].bdryEdges is 3.
                // Here we are taking a stand-alone triangle and folding
                // two of its edges together.  Nothing needs to change.
                //
                // 3) hasTwist is false, and vertexState[vIdx].bdryEdges is 2.
                // This means we are folding together two edges of a
                // triangle whose third edge is already joined elsewhere.
                // We deal with this as follows:
                //
                if ((! hasTwist) && vertexState[vIdx].bdryEdges < 3) {
                    // Although bdryEdges is 2, we don't bother keeping
                    // a backup in bdryTwistOld[].  This is because
                    // bdryEdges jumps straight from 2 to 0, and the
                    // neighbours in bdryNext[] / bdryTwist[] never get
                    // overwritten.
                    if (vertexState[vIdx].bdryNext[0] == vIdx) {
                        // We are closing off a single boundary of length
                        // two.  All good.
                    } else {
                        // Adjust each neighbour to point to the other.
                        vtxBdryJoin(vertexState[vIdx].bdryNext[0],
                            1 ^ vertexState[vIdx].bdryTwist[0],
                            vertexState[vIdx].bdryNext[1],
                            vertexState[vIdx].bdryTwist[1] ^
                                vertexState[vIdx].bdryTwist[0]);
                    }
                }

                vertexState[vIdx].bdryEdges -= 2;
            } else {
                // We are joining two distinct tetrahedron vertices that
                // already contribute to the same vertex link.
                if (vertexState[vIdx].bdryEdges == 2)
                    vtxBdryBackup(vIdx);
                if (vertexState[wIdx].bdryEdges == 2)
                    vtxBdryBackup(wIdx);

                if (vtxBdryLength1(vIdx) && vtxBdryLength1(wIdx)) {
                    // We are joining together two boundaries of length one.
                    // Do nothing and mark the non-trivial genus.
                    // std::cerr << "NON-SPHERE: 1 >-< 1" << std::endl;
                    retVal |= VLINK_NON_SPHERE;
                } else if (vtxBdryLength2(vIdx, wIdx)) {
                    // We are closing off a single boundary of length two.
                    // All good.
                } else {
                    vtxBdryNext(vIdx, face.simp, v, face.facet, vNext, vTwist);
                    vtxBdryNext(wIdx, adj.simp, w, adj.facet, wNext, wTwist);

                    if (vNext[0] == wIdx && wNext[1 ^ vTwist[0]] == vIdx) {
                        // We are joining two adjacent edges of the vertex link.
                        // Simply eliminate them.
                        vtxBdryJoin(vNext[1], 0 ^ vTwist[1],
                            wNext[0 ^ vTwist[0]],
                            (vTwist[0] ^ wTwist[0 ^ vTwist[0]]) ^ vTwist[1]);
                    } else if (vNext[1] == wIdx &&
                            wNext[0 ^ vTwist[1]] == vIdx) {
                        // Again, joining two adjacent edges of the vertex link.
                        vtxBdryJoin(vNext[0], 1 ^ vTwist[0],
                            wNext[1 ^ vTwist[1]],
                            (vTwist[1] ^ wTwist[1 ^ vTwist[1]]) ^ vTwist[0]);
                    } else {
                        // See if we are joining two different boundary cycles
                        // together; if so, we have created non-trivial genus in
                        // the vertex link.
                        tmpIdx = vertexState[vIdx].bdryNext[0];
                        tmpTwist = vertexState[vIdx].bdryTwist[0];
                        while (tmpIdx != vIdx && tmpIdx != wIdx) {
                            nextIdx = vertexState[tmpIdx].
                                bdryNext[0 ^ tmpTwist];
                            tmpTwist ^= vertexState[tmpIdx].
                                bdryTwist[0 ^ tmpTwist];
                            tmpIdx = nextIdx;
                        }

                        if (tmpIdx == vIdx) {
                            // Different boundary cycles.
                            // Don't touch anything; just flag a
                            // high genus error.
                            // std::cerr << "NON-SPHERE: (X)" << std::endl;
                            retVal |= VLINK_NON_SPHERE;
                        } else {
                            // Same boundary cycle.
                            vtxBdryJoin(vNext[0], 1 ^ vTwist[0],
                                wNext[1 ^ hasTwist],
                                vTwist[0] ^ (hasTwist ^ wTwist[1 ^ hasTwist]));
                            vtxBdryJoin(vNext[1], 0 ^ vTwist[1],
                                wNext[0 ^ hasTwist],
                                vTwist[1] ^ (hasTwist ^ wTwist[0 ^ hasTwist]));
                        }
                    }
                }

                vertexState[vIdx].bdryEdges--;
                vertexState[wIdx].bdryEdges--;
            }
        } else {
            // We are joining two distinct vertices together and merging
            // their vertex links.
            if (vertexState[vRep].rank < vertexState[wRep].rank) {
                // Join vRep beneath wRep.
                vertexState[vRep].parent = wRep;
                vertexState[vRep].twistUp = hasTwist ^ parentTwists;

                vertexState[wRep].bdry = vertexState[wRep].bdry +
                    vertexState[vRep].bdry - 2;
                if (vertexState[wRep].bdry == 0)
                    retVal |= VLINK_CLOSED;

                vertexStateChanged[orderIdx] = vRep;
            } else {
                // Join wRep beneath vRep.
                vertexState[wRep].parent = vRep;
                vertexState[wRep].twistUp = hasTwist ^ parentTwists;
                if (vertexState[vRep].rank == vertexState[wRep].rank) {
                    vertexState[vRep].rank++;
                    vertexState[wRep].hadEqualRank = true;
                }

                vertexState[vRep].bdry = vertexState[vRep].bdry +
                    vertexState[wRep].bdry - 2;
                if (vertexState[vRep].bdry == 0)
                    retVal |= VLINK_CLOSED;

                vertexStateChanged[orderIdx] = wRep;
            }

            nVertexClasses--;

            // Adjust the cycles of boundary components.
            if (vertexState[vIdx].bdryEdges == 2)
                vtxBdryBackup(vIdx);
            if (vertexState[wIdx].bdryEdges == 2)
                vtxBdryBackup(wIdx);

            if (vtxBdryLength1(vIdx)) {
                if (vtxBdryLength1(wIdx)) {
                    // Both vIdx and wIdx form entire boundary components of
                    // length one; these are joined together and the vertex
                    // link is closed off.
                    // No changes to make for the boundary cycles.
                } else {
                    // Here vIdx forms a boundary component of length one,
                    // and wIdx does not.  Ignore vIdx, and simply excise the
                    // relevant edge from wIdx.
                    // There is nothing to do here unless wIdx only has one
                    // boundary edge remaining (in which case we know it
                    // joins to some different tetrahedron vertex).
                    if (vertexState[wIdx].bdryEdges == 1) {
                        wNext[0] = vertexState[wIdx].bdryNext[0];
                        wNext[1] = vertexState[wIdx].bdryNext[1];
                        wTwist[0] = vertexState[wIdx].bdryTwist[0];
                        wTwist[1] = vertexState[wIdx].bdryTwist[1];

                        vtxBdryJoin(wNext[0], 1 ^ wTwist[0], wNext[1],
                            wTwist[0] ^ wTwist[1]);
                    }
                }
            } else if (vtxBdryLength1(wIdx)) {
                // As above, but with the two vertices the other way around.
                if (vertexState[vIdx].bdryEdges == 1) {
                    vNext[0] = vertexState[vIdx].bdryNext[0];
                    vNext[1] = vertexState[vIdx].bdryNext[1];
                    vTwist[0] = vertexState[vIdx].bdryTwist[0];
                    vTwist[1] = vertexState[vIdx].bdryTwist[1];

                    vtxBdryJoin(vNext[0], 1 ^ vTwist[0], vNext[1],
                        vTwist[0] ^ vTwist[1]);
                }
            } else {
                // Each vertex belongs to a boundary component of length
                // at least two.  Merge the components together.
                vtxBdryNext(vIdx, face.simp, v, face.facet, vNext, vTwist);
                vtxBdryNext(wIdx, adj.simp, w, adj.facet, wNext, wTwist);

                vtxBdryJoin(vNext[0], 1 ^ vTwist[0], wNext[1 ^ hasTwist],
                    vTwist[0] ^ (hasTwist ^ wTwist[1 ^ hasTwist]));
                vtxBdryJoin(vNext[1], 0 ^ vTwist[1], wNext[0 ^ hasTwist],
                    vTwist[1] ^ (hasTwist ^ wTwist[0 ^ hasTwist]));
            }

            vertexState[vIdx].bdryEdges--;
            vertexState[wIdx].bdryEdges--;
        }
    }

    return retVal;
}

void OneStepSearcher::splitVertexClasses(NTetFace face) {
    // Split all three vertex pairs for the current face.
    NTetFace adj = (*pairing_)[face];

    int v, w;
    int vIdx, wIdx;
    unsigned orderIdx;
    int rep, subRep;
    NPerm4 p = gluingPerm(face);
    // Do everything in reverse.  This includes the loop over vertices.
    for (v = 3; v >= 0; v--) {
        if (v == face.facet)
            continue;

        w = p[v];
        vIdx = v + 4 * face.simp;
        wIdx = w + 4 * adj.simp;
        orderIdx = v + 4 * orderElt;

        if (vertexStateChanged[orderIdx] < 0) {
            for (rep = vIdx; vertexState[rep].parent >= 0;
                    rep = vertexState[rep].parent)
                ;
            vertexState[rep].bdry += 2;
        } else {
            subRep = vertexStateChanged[orderIdx];
            rep = vertexState[subRep].parent;

            vertexState[subRep].parent = -1;
            if (vertexState[subRep].hadEqualRank) {
                vertexState[subRep].hadEqualRank = false;
                vertexState[rep].rank--;
            }

            vertexState[rep].bdry = vertexState[rep].bdry + 2 -
                vertexState[subRep].bdry;

            vertexStateChanged[orderIdx] = -1;
            nVertexClasses++;
        }

        // Restore cycles of boundary components.
        if (vIdx == wIdx) {
            vertexState[vIdx].bdryEdges += 2;

            // Adjust neighbours to point back to vIdx if required.
            if (vertexState[vIdx].bdryEdges == 2)
                vtxBdryFixAdj(vIdx);
        } else {
            vertexState[wIdx].bdryEdges++;
            vertexState[vIdx].bdryEdges++;

            switch (vertexState[wIdx].bdryEdges) {
                case 3: vertexState[wIdx].bdryNext[0] =
                            vertexState[wIdx].bdryNext[1] = wIdx;
                        vertexState[wIdx].bdryTwist[0] =
                            vertexState[wIdx].bdryTwist[1] = 0;
                        break;

                case 2: vtxBdryRestore(wIdx);
                        // Fall through to the next case, so we can
                        // adjust the neighbours.

                case 1: // Nothing was changed for wIdx during the merge,
                        // so there is nothing there to restore.

                        // Adjust neighbours to point back to wIdx.
                        vtxBdryFixAdj(wIdx);
            }

            switch (vertexState[vIdx].bdryEdges) {
                case 3: vertexState[vIdx].bdryNext[0] =
                            vertexState[vIdx].bdryNext[1] = vIdx;
                        vertexState[vIdx].bdryTwist[0] =
                            vertexState[vIdx].bdryTwist[1] = 0;
                        break;

                case 2: vtxBdryRestore(vIdx);
                        // Fall through to the next case, so we can
                        // adjust the neighbours.

                case 1: // Nothing was changed for vIdx during the merge,
                        // so there is nothing there to restore.

                        // Adjust neighbours to point back to vIdx.
                        vtxBdryFixAdj(vIdx);
            }
        }
    }
}

} // namespace regina
