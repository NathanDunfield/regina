
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

#include "generic/ngenerictriangulation.h"

namespace regina {

namespace {
    /**
     * For internal use by makeCanonical().  This routines assumes that
     * the preimage of simplex 0 has been fixed (along with the
     * corresponding vertex permutation), and tries to extend
     * this to a "possibly canonical" isomorphism.
     *
     * If it becomes clear that the isomorphism cannot be made canonical
     * and/or cannot be made better (i.e., lexicographically smaller) than
     * the best isomorphism found so far, this routine returns \c false
     * (possibly before the full isomorphism has been constructed).
     * Otherwise it returns \c true (and it is guaranteed that the
     * isomorphism is both fully constructed and moreover a strict
     * improvement upon the best found so far).
     *
     * This routine currently only works for connected triangulations.
     */
    template <int dim>
    bool extendIsomorphism(
            const typename DimTraits<dim>::Triangulation* tri,
            typename DimTraits<dim>::Isomorphism& current,
            typename DimTraits<dim>::Isomorphism& currentInv,
            const typename DimTraits<dim>::Isomorphism& best,
            const typename DimTraits<dim>::Isomorphism& bestInv) {
        bool better = false;

        unsigned nSimp = tri->getNumberOfSimplices();
        unsigned simplex;

        for (simplex = 0; simplex < nSimp; ++simplex)
            if (simplex != currentInv.simpImage(0))
                current.simpImage(simplex) = -1;

        int facet;

        unsigned origTri, origTriBest;
        int origFacet, origFacetBest;

        typename DimTraits<dim>::Simplex *adjTri, *adjTriBest;
        unsigned adjTriIndex, adjTriIndexBest;
        unsigned finalImage, finalImageBest;

        typename DimTraits<dim>::Perm gluingPerm, gluingPermBest;
        typename DimTraits<dim>::Perm finalGluing, finalGluingBest;
        int comp;

        bool justAssigned;
        unsigned lastAssigned = 0;
        for (simplex = 0; simplex < nSimp; ++simplex) {
            // INV: We have already selected the preimage of simplex and
            // the corresponding facet permutation by the time we reach
            // this point.
            origTri = currentInv.simpImage(simplex);
            origTriBest = bestInv.simpImage(simplex);

            for (facet = 0; facet <= dim; ++facet) {
                origFacet = current.facetPerm(origTri).preImageOf(facet);
                origFacetBest = best.facetPerm(origTriBest).preImageOf(facet);

                // Check out the adjacency along simplex/facet.
                adjTri = tri->getSimplex(origTri)->adjacentSimplex(origFacet);
                adjTriIndex = (adjTri ? tri->simplexIndex(adjTri) : nSimp);
                adjTriBest = tri->getSimplex(origTriBest)->
                    adjacentSimplex(origFacetBest);
                adjTriIndexBest = (adjTriBest ?
                    tri->simplexIndex(adjTriBest) : nSimp);

                justAssigned = false;
                if (adjTri && current.simpImage(adjTriIndex) < 0) {
                    // We have a new simplex that needs assignment.
                    ++lastAssigned;
                    current.simpImage(adjTriIndex) = lastAssigned;
                    currentInv.simpImage(lastAssigned) = adjTriIndex;
                    justAssigned = true;
                }

                finalImage = (adjTri ? current.simpImage(adjTriIndex) : nSimp);
                finalImageBest = (adjTriBest ?
                    best.simpImage(adjTriIndexBest) : nSimp);

                // We now have a gluing (but possibly not a gluing
                // permutation).  Compare adjacent simplex indices.
                if ((! better) && finalImage > finalImageBest)
                    return false; // Worse than best-so-far.
                if (finalImage < finalImageBest)
                    better = true;

                // Time now to look at the gluing permutation.
                if (! adjTri)
                    continue;

                gluingPerm = tri->getSimplex(origTri)->adjacentGluing(
                    origFacet);
                gluingPermBest = tri->getSimplex(origTriBest)->
                    adjacentGluing(origFacetBest);

                if (justAssigned) {
                    // We can choose the permutation ourselves.
                    // Make it so that the final gluing (computed later
                    // below) becomes the identity.
                    current.facetPerm(adjTriIndex) =
                        current.facetPerm(origTri) * gluingPerm.inverse();
                    currentInv.facetPerm(lastAssigned) =
                        current.facetPerm(adjTriIndex).inverse();
                }

                // Although adjTri is guaranteed to exist, adjTriBest is
                // not.  However, if adjTriBest does not exist then our
                // isomorphism-under-construction must already be an
                // improvement over best.
                if (better)
                    continue;

                // Now we are guaranteed that adjTriBest exists.
                finalGluing = current.facetPerm(adjTriIndex) *
                    gluingPerm * current.facetPerm(origTri).inverse();
                finalGluingBest = best.facetPerm(adjTriIndexBest) *
                    gluingPermBest * best.facetPerm(origTriBest).inverse();

                comp = finalGluing.compareWith(finalGluingBest);
                if ((! better) && comp > 0)
                    return false; // Worse than best-so-far.
                if (comp < 0)
                    better = true;
            }
        }

        return better;
    }
}

template <int dim>
bool NGenericTriangulation<dim>::makeCanonical() {
    typename DimTraits<dim>::Triangulation* me =
        static_cast<typename DimTraits<dim>::Triangulation*>(this);

    typedef typename DimTraits<dim>::Perm Perm;
    unsigned nSimp = me->getNumberOfSimplices();

    // Get the empty triangulation out of the way.
    if (nSimp == 0)
        return false;

    // Prepare to search for isomorphisms.
    typename DimTraits<dim>::Isomorphism current(nSimp), currentInv(nSimp);
    typename DimTraits<dim>::Isomorphism best(nSimp), bestInv(nSimp);

    // The thing to best is the identity isomorphism.
    unsigned simp, inner;
    for (simp = 0; simp < nSimp; ++simp) {
        best.simpImage(simp) = bestInv.simpImage(simp) = simp;
        best.facetPerm(simp) = bestInv.facetPerm(simp) = Perm();
    }

    // Run through potential preimages of simplex 0.
    int perm;
    for (simp = 0; simp < nSimp; ++simp) {
        for (perm = 0; perm < Perm::nPerms; ++perm) {
            // Build a "perhaps canonical" isomorphism based on this
            // preimage of simplex 0.
            current.simpImage(simp) = 0;
            currentInv.simpImage(0) = simp;

            current.facetPerm(simp) = Perm::Sn[Perm::invSn[perm]];
            currentInv.facetPerm(0) = Perm::Sn[perm];

            if (extendIsomorphism<dim>(me, current, currentInv,
                    best, bestInv)) {
                // This is better than anything we've seen before.
                for (inner = 0; inner < nSimp; ++inner) {
                    best.simpImage(inner) = current.simpImage(inner);
                    best.facetPerm(inner) = current.facetPerm(inner);
                    bestInv.simpImage(inner) = currentInv.simpImage(inner);
                    bestInv.facetPerm(inner) = currentInv.facetPerm(inner);
                }
            }
        }
    }

    // Is there anything to do?
    if (best.isIdentity())
        return false;

    // Do it.
    best.applyInPlace(me);
    return true;
}

} // namespace regina

