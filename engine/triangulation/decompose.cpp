
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

#include <list>
#include <sstream>

#include "enumerate/ntreetraversal.h"
#include "packet/ncontainer.h"
#include "subcomplex/nsnappedball.h"
#include "surfaces/nnormalsurface.h"
#include "surfaces/nnormalsurfacelist.h"
#include "triangulation/nboundarycomponent.h"
#include "triangulation/nisomorphism.h"
#include "triangulation/ntriangulation.h"

namespace regina {

unsigned long NTriangulation::splitIntoComponents(NPacket* componentParent,
        bool setLabels) {
    // Knock off the empty triangulation first.
    if (tetrahedra.empty())
        return 0;

    if (! componentParent)
        componentParent = this;

    // Create the new component triangulations.
    // Note that the following line forces a skeletal recalculation.
    unsigned long nComp = getNumberOfComponents();

    // Initialise the component triangulations.
    NTriangulation** newTris = new NTriangulation*[nComp];
    unsigned long whichComp;
    for (whichComp = 0; whichComp < nComp; ++whichComp)
        newTris[whichComp] = new NTriangulation();

    // Clone the tetrahedra, sorting them into the new components.
    unsigned long nTets = tetrahedra.size();

    NTetrahedron** newTets = new NTetrahedron*[nTets];
    NTetrahedron *tet, *adjTet;
    unsigned long tetPos, adjPos;
    NPerm4 adjPerm;
    int face;

    for (tetPos = 0; tetPos < nTets; tetPos++)
        newTets[tetPos] =
            newTris[componentIndex(tetrahedra[tetPos]->getComponent())]->
            newTetrahedron(tetrahedra[tetPos]->getDescription());

    // Clone the tetrahedron gluings also.
    for (tetPos = 0; tetPos < nTets; tetPos++) {
        tet = tetrahedra[tetPos];
        for (face = 0; face < 4; face++) {
            adjTet = tet->adjacentTetrahedron(face);
            if (adjTet) {
                adjPos = tetrahedronIndex(adjTet);
                adjPerm = tet->adjacentGluing(face);
                if (adjPos > tetPos ||
                        (adjPos == tetPos && adjPerm[face] > face))
                    newTets[tetPos]->joinTo(face, newTets[adjPos], adjPerm);
            }
        }
    }

    // Insert the component triangulations into the packet tree and clean up.
    for (whichComp = 0; whichComp < nComp; ++whichComp) {
        componentParent->insertChildLast(newTris[whichComp]);

        if (setLabels) {
            std::ostringstream label;
            if (getPacketLabel().empty())
                label << "Cmpt #" << (whichComp + 1);
            else
                label << getPacketLabel() << " - Cmpt #" << (whichComp + 1);
            newTris[whichComp]->setPacketLabel(label.str());
        }
    }

    delete[] newTets;
    delete[] newTris;

    return whichComp;
}

unsigned long NTriangulation::connectedSumDecomposition(NPacket* primeParent,
        bool setLabels) {
    // Precondition checks.
    if (! (isValid() && isClosed() && isOrientable() && isConnected()))
        return 0;

    if (! primeParent)
        primeParent = this;

    // Make a working copy, simplify and record the initial homology.
    NTriangulation* working = new NTriangulation(*this);
    working->intelligentSimplify();

    unsigned long initZ, initZ2, initZ3;
    {
        const NAbelianGroup& homology = working->getHomologyH1();
        initZ = homology.getRank();
        initZ2 = homology.getTorsionRank(2);
        initZ3 = homology.getTorsionRank(3);
    }

    // Start crushing normal spheres.
    NContainer toProcess;
    toProcess.insertChildLast(working);

    std::list<NTriangulation*> primeComponents;
    unsigned long whichComp = 0;

    NTriangulation* processing;
    NTriangulation* crushed;
    NNormalSurface* sphere;
    while ((processing = static_cast<NTriangulation*>(
            toProcess.getFirstTreeChild()))) {
        // INV: Our triangulation is the connected sum of all the
        // children of toProcess, all the elements of primeComponents
        // and possibly some copies of S2xS1, RP3 and/or L(3,1).

        // Work with the last child.
        processing->makeOrphan();

        // Find a normal 2-sphere to crush.
        sphere = processing->hasNonTrivialSphereOrDisc();
        if (sphere) {
            crushed = sphere->crush();
            delete sphere;
            delete processing;

            crushed->intelligentSimplify();

            // Insert each component of the crushed triangulation back
            // into the list to process.
            if (crushed->getNumberOfComponents() == 0)
                delete crushed;
            else if (crushed->getNumberOfComponents() == 1)
                toProcess.insertChildLast(crushed);
            else {
                crushed->splitIntoComponents(&toProcess, false);
                delete crushed;
            }
        } else {
            // We have no non-trivial normal 2-spheres!
            // The triangulation is 0-efficient (and prime).
            // Is it a 3-sphere?
            if (processing->getNumberOfVertices() > 1) {
                // Proposition 5.1 of Jaco & Rubinstein's 0-efficiency
                // paper:  If a closed orientable triangulation T is
                // 0-efficient then either T has one vertex or T is a
                // 3-sphere with precisely two vertices.
                //
                // It follows then that this is a 3-sphere.
                // Toss it away.
                delete sphere;
                delete processing;
            } else {
                // Now we have a closed orientable one-vertex 0-efficient
                // triangulation.
                // We have to look for an almost normal sphere.
                //
                // From the proof of Proposition 5.12 in Jaco & Rubinstein's
                // 0-efficiency paper, we see that we can restrict our
                // search to octagonal almost normal surfaces.
                // Furthermore, from the result in the quadrilateral-octagon
                // coordinates paper, we can restrict this search further
                // to vertex octagonal almost normal surfaces in
                // quadrilateral-octagonal space.
                sphere = processing->hasOctagonalAlmostNormalSphere();
                if (sphere) {
                    // It's a 3-sphere.  Toss this component away.
                    delete sphere;
                    delete processing;
                } else {
                    // It's a non-trivial prime component!
                    primeComponents.push_back(processing);
                }
            }
        }
    }

    // Run a final homology check and put back our missing S2xS1, RP3
    // and L(3,1) terms.
    unsigned long finalZ = 0, finalZ2 = 0, finalZ3 = 0;
    for (std::list<NTriangulation*>::iterator it = primeComponents.begin();
            it != primeComponents.end(); it++) {
        const NAbelianGroup& homology = (*it)->getHomologyH1();
        finalZ += homology.getRank();
        finalZ2 += homology.getTorsionRank(2);
        finalZ3 += homology.getTorsionRank(3);
    }

    while (finalZ++ < initZ) {
        working = new NTriangulation();
        working->insertLayeredLensSpace(0, 1);
        primeComponents.push_back(working);
        irreducible = false; // Implied by the S2xS1 summand.
        zeroEfficient = false; // Implied by the S2xS1 summand.
    }
    while (finalZ2++ < initZ2) {
        working = new NTriangulation();
        working->insertLayeredLensSpace(2, 1);
        primeComponents.push_back(working);
    }
    while (finalZ3++ < initZ3) {
        working = new NTriangulation();
        working->insertLayeredLensSpace(3, 1);
        primeComponents.push_back(working);
    }

    // All done!
    for (std::list<NTriangulation*>::iterator it = primeComponents.begin();
            it != primeComponents.end(); it++) {
        primeParent->insertChildLast(*it);

        if (setLabels) {
            std::ostringstream label;
            if (getPacketLabel().empty())
                label << "Summand #" << (whichComp + 1);
            else
                label << getPacketLabel() << " - Summand #" << (whichComp + 1);
            (*it)->setPacketLabel(label.str());
        }

        whichComp++;
    }

    // Set irreducibility while we're at it.
    if (whichComp > 1) {
        threeSphere = false;
        irreducible = false;
        zeroEfficient = false;
    } else if (whichComp == 1) {
        threeSphere = false;
        if (! irreducible.known()) {
            // If our manifold is S2xS1 then it is *not* irreducible;
            // however, in this case we will have already set irreducible
            // to false when putting back the S2xS1 summands above (and
            // therefore irreducible.known() will be true).
            irreducible = true;
        }
    } else if (whichComp == 0) {
        threeSphere = true;
        irreducible = true;
        haken = false;
    }

    return whichComp;
}

bool NTriangulation::isThreeSphere() const {
    if (threeSphere.known())
        return threeSphere.value();

    // Basic property checks.
    if (! (isValid() && isClosed() && isOrientable() && isConnected() &&
            getNumberOfTetrahedra() > 0)) {
        threeSphere = false;
        return false;
    }

    // Check homology and fundamental group.
    // Better simplify first, which means we need a clone.
    NTriangulation* working = new NTriangulation(*this);
    working->intelligentSimplify();

    // The Poincare conjecture!
    if (working->getFundamentalGroup().getNumberOfGenerators() == 0) {
        threeSphere = true;

        // Some other things that come for free:
        irreducible = true;
        haken = false;

        return true;
    }

    // We could still have a trivial group but not know it.
    // At least we can at least check homology precisely.
    if (! working->getHomologyH1().isTrivial()) {
        threeSphere = false;
        delete working;
        return false;
    }

    // Time for some more heavy machinery.  On to normal surfaces.
    NContainer toProcess;
    toProcess.insertChildLast(working);

    NTriangulation* processing;
    NTriangulation* crushed;
    NNormalSurface* sphere;
    while ((processing = static_cast<NTriangulation*>(
            toProcess.getLastTreeChild()))) {
        // INV: Our triangulation is the connected sum of all the
        // children of toProcess.  Each of these children has trivial
        // homology (and therefore we have no S2xS1 / RP3 / L(3,1)
        // summands to worry about).

        // Work with the last child.
        processing->makeOrphan();

        // Find a normal 2-sphere to crush.
        sphere = processing->hasNonTrivialSphereOrDisc();
        if (sphere) {
            crushed = sphere->crush();
            delete sphere;
            delete processing;

            crushed->intelligentSimplify();

            // Insert each component of the crushed triangulation in the
            // list to process.
            if (crushed->getNumberOfComponents() == 0)
                delete crushed;
            else if (crushed->getNumberOfComponents() == 1)
                toProcess.insertChildLast(crushed);
            else {
                crushed->splitIntoComponents(&toProcess, false);
                delete crushed;
            }
        } else {
            // We have no non-trivial normal 2-spheres!
            // The triangulation is 0-efficient.
            // We can now test directly whether we have a 3-sphere.
            if (processing->getNumberOfVertices() > 1) {
                // Proposition 5.1 of Jaco & Rubinstein's 0-efficiency
                // paper:  If a closed orientable triangulation T is
                // 0-efficient then either T has one vertex or T is a
                // 3-sphere with precisely two vertices.
                //
                // It follows then that this is a 3-sphere.
                // Toss it away.
                delete sphere;
                delete processing;
            } else {
                // Now we have a closed orientable one-vertex 0-efficient
                // triangulation.
                // We have to look for an almost normal sphere.
                //
                // From the proof of Proposition 5.12 in Jaco & Rubinstein's
                // 0-efficiency paper, we see that we can restrict our
                // search to octagonal almost normal surfaces.
                // Furthermore, from the result in the quadrilateral-octagon
                // coordinates paper, we can restrict this search further
                // to vertex octagonal almost normal surfaces in
                // quadrilateral-octagonal space.
                sphere = processing->hasOctagonalAlmostNormalSphere();
                if (sphere) {
                    // It's a 3-sphere.  Toss this component away.
                    delete sphere;
                    delete processing;
                } else {
                    // It's not a 3-sphere.  We're done!
                    threeSphere = false;
                    delete processing;
                    return false;
                }
            }
        }
    }

    // Our triangulation is the connected sum of 0 components!
    threeSphere = true;

    // Some other things that we get for free:
    irreducible = true;
    haken = false;

    return true;
}

bool NTriangulation::knowsThreeSphere() const {
    if (threeSphere.known())
        return true;

    // Run some very fast prelimiary tests before we give up and say no.
    if (! (isValid() && isClosed() && isOrientable() && isConnected())) {
        threeSphere = false;
        return true;
    }

    // More work is required.
    return false;
}

bool NTriangulation::isBall() const {
    if (threeBall.known())
        return threeBall.value();

    // Basic property checks.
    if (! (isValid() && hasBoundaryTriangles() && isOrientable() && isConnected()
            && boundaryComponents.size() == 1
            && boundaryComponents.front()->getEulerCharacteristic() == 2)) {
        threeBall = false;
        return false;
    }

    // Pass straight to isThreeSphere (which in turn will check faster things
    // like homology before pulling out the big guns).
    //
    // Cone the boundary to a point (i.e., fill it with a ball), then
    // call isThreeSphere() on the resulting closed triangulation.

    NTriangulation working(*this);
    working.intelligentSimplify();
    working.finiteToIdeal();

    // Simplify again in case our coning was inefficient.
    working.intelligentSimplify();

    threeBall = working.isThreeSphere();
    return threeBall.value();
}

bool NTriangulation::knowsBall() const {
    if (threeBall.known())
        return true;

    // Run some very fast prelimiary tests before we give up and say no.
    if (! (isValid() && hasBoundaryTriangles() && isOrientable() && isConnected()
            && boundaryComponents.size() == 1
            && boundaryComponents.front()->getEulerCharacteristic() == 2)) {
        threeBall = false;
        return true;
    }

    // More work is required.
    return false;
}

bool NTriangulation::isSolidTorus() const {
    if (solidTorus.known())
        return solidTorus.value();

    // Basic property checks.
    if (! (isValid() && isOrientable() && isConnected() &&
            boundaryComponents.size() == 1 &&
            boundaryComponents.front()->getEulerCharacteristic() == 0 &&
            boundaryComponents.front()->isOrientable()))
        return (solidTorus = false);

    // If it's ideal, make it a triangulation with real boundary.
    // If it's not ideal, clone it anyway so we can modify it.
    NTriangulation* working = new NTriangulation(*this);
    working->intelligentSimplify();
    if (working->isIdeal()) {
        working->idealToFinite();
        working->intelligentSimplify();
    }

    // Check homology.
    if (! (working->getHomologyH1().isZ())) {
        delete working;
        return (solidTorus = false);
    }

    // So:
    // We are valid, orientable, compact and connected, with H1 = Z.
    // There is exactly one boundary component, and this is a torus.

    // Note that the homology results imply that this is not a connected
    // sum of something with S2xS1 (otherwise we would have two Z terms
    // in the homology: one from the torus boundary and one from the S2xS1).
    // This observation simplifies the crushing cases later on.

    // Pull out the big guns: normal surface time.
    NNormalSurface* s;
    NTriangulation* crushed;
    NPacket* p;
    NTriangulation* comp;
    while (true) {
        // INVARIANT: working is homeomorphic to our original manifold.
        if (working->getNumberOfVertices() > 1) {
            // Try *really* hard to get to a 1-vertex triangulation,
            // since this will make hasNonTrivialSphereOrDisc() much
            // faster (it will be able to use linear programming).
            working->intelligentSimplify();
            if (working->getNumberOfVertices() > 1) {
                working->barycentricSubdivision();
                working->intelligentSimplify();
                working->intelligentSimplify();
            }
        }

        // Find a non-trivial normal disc or sphere.
        s = working->hasNonTrivialSphereOrDisc();
        if (! s) {
            // No non-trivial normal disc.  This cannot be a solid torus.
            delete working;
            return (solidTorus = false);
        }

        // Crush it and see what happens.
        // Given what we know about the manifold so far, the only things
        // that can happen during crushing are:
        // - undo connected sum decompositions;
        // - cut along properly embedded discs;
        // - gain and/or lose 3-balls and/or 3-spheres.
        crushed = s->crush();
        delete s;
        delete working;
        working = 0;

        crushed->intelligentSimplify();
        crushed->splitIntoComponents(0, false);
        for (p = crushed->getFirstTreeChild(); p; p = p->getNextTreeSibling()) {
            // Examine each connected component after crushing.
            comp = static_cast<NTriangulation*>(p);
            if (comp->isClosed()) {
                // A closed piece.
                // Must be a 3-sphere, or else we didn't have a solid torus.
                if (! comp->isThreeSphere()) {
                    delete crushed;
                    return (solidTorus = false);
                }
            } else if (comp->getNumberOfBoundaryComponents() > 1) {
                // Multiple boundaries on the same component.
                // This should never happen, since it implies there was
                // an S2xS1 summand.
                std::cerr << "ERROR: S2xS1 summand detected in "
                    "isSolidTorus() that should not exist." << std::endl;

                // At any rate, it means we did not have a solid torus.
                delete crushed;
                return (solidTorus = false);
            } else if (comp->getBoundaryComponent(0)->
                    getEulerCharacteristic() == 2) {
                // A component with sphere boundary.
                // Must be a 3-ball, or else we didn't have a solid torus.
                if (! comp->isBall()) {
                    delete crushed;
                    return (solidTorus = false);
                }
            } else {
                // The only other possibility is a component with torus
                // boundary.  We should only see at most one of these.
                //
                // Unless some other non-trivial component was split off
                // (i.e., a non-ball and/or non-sphere that will be
                // detected separately in the tests above), this
                // component must be identical to our original manifold.
                if (working) {
                    std::cerr << "ERROR: Multiple torus boundary "
                        "components detected in isSolidTorus(), which "
                        "should not be possible." << std::endl;
                }
                working = comp;
            }
        }

        if (! working) {
            // We have reduced everything down to balls and spheres.
            // The only way this can happen is if we had a solid torus
            // (and we crushed and/or cut along a compressing disc
            // during the crushing operation).
            delete crushed;
            return (solidTorus = true);
        }

        // We have the original manifold in working, but this time with
        // fewer tetrahedra.  Around we go again.
        working->makeOrphan();
        delete crushed;
    }
}

bool NTriangulation::knowsSolidTorus() const {
    if (solidTorus.known())
        return true;

    // Run some very fast prelimiary tests before we give up and say no.
    if (! (isValid() && isOrientable() && isConnected())) {
        solidTorus = false;
        return true;
    }

    if (boundaryComponents.size() != 1) {
        solidTorus = false;
        return true;
    }

    if (boundaryComponents.front()->getEulerCharacteristic() != 0 ||
            (! boundaryComponents.front()->isOrientable())) {
        solidTorus = false;
        return true;
    }

    // More work is required.
    return false;
}

NPacket* NTriangulation::makeZeroEfficient() {
    // Extract a connected sum decomposition.
    NContainer* connSum = new NContainer();
    if (getPacketLabel().empty())
        connSum->setPacketLabel("Decomposition");
    else
        connSum->setPacketLabel(getPacketLabel() + " - Decomposition");

    unsigned long ans = connectedSumDecomposition(connSum, true);
    if (ans > 1) {
        // Composite!
        return connSum;
    } else if (ans == 1) {
        // Prime.
        NTriangulation* newTri = dynamic_cast<NTriangulation*>(
            connSum->getLastTreeChild());
        if (! isIsomorphicTo(*newTri).get()) {
            removeAllTetrahedra();
            insertTriangulation(*newTri);
        }
        delete connSum;
        return 0;
    } else {
        // 3-sphere.
        if (getNumberOfTetrahedra() > 1) {
            removeAllTetrahedra();
            insertLayeredLensSpace(1,0);
        }
        delete connSum;
        return 0;
    }
}

bool NTriangulation::isIrreducible() const {
    if (irreducible.known())
        return irreducible.value();

    // Precondition checks.
    if (! (isValid() && isClosed() && isOrientable() && isConnected()))
        return 0;

    // We will essentially carry out a connected sum decomposition, but
    // instead of keeping prime summands we will just count them and
    // throw them away.
    unsigned long summands = 0;

    // Make a working copy, simplify and record the initial homology.
    NTriangulation* working = new NTriangulation(*this);
    working->intelligentSimplify();

    unsigned long Z, Z2, Z3;
    {
        const NAbelianGroup& homology = working->getHomologyH1();
        Z = homology.getRank();
        Z2 = homology.getTorsionRank(2);
        Z3 = homology.getTorsionRank(3);
    }

    // Start crushing normal spheres.
    NContainer toProcess;
    toProcess.insertChildLast(working);

    unsigned long whichComp = 0;

    NTriangulation* processing;
    NTriangulation* crushed;
    NNormalSurface* sphere;
    while ((processing = static_cast<NTriangulation*>(
            toProcess.getFirstTreeChild()))) {
        // INV: Our triangulation is the connected sum of all the
        // children of toProcess, all the prime components that we threw away,
        // and possibly some copies of S2xS1, RP3 and/or L(3,1).

        // Work with the last child.
        processing->makeOrphan();

        // Find a normal 2-sphere to crush.
        sphere = processing->hasNonTrivialSphereOrDisc();
        if (sphere) {
            crushed = sphere->crush();
            delete sphere;
            delete processing;

            crushed->intelligentSimplify();

            // Insert each component of the crushed triangulation back
            // into the list to process.
            if (crushed->getNumberOfComponents() == 0)
                delete crushed;
            else if (crushed->getNumberOfComponents() == 1)
                toProcess.insertChildLast(crushed);
            else {
                crushed->splitIntoComponents(&toProcess, false);
                delete crushed;
            }
        } else {
            // We have no non-trivial normal 2-spheres!
            // The triangulation is 0-efficient (and prime).
            // Is it a 3-sphere?
            if (processing->getNumberOfVertices() > 1) {
                // Proposition 5.1 of Jaco & Rubinstein's 0-efficiency
                // paper:  If a closed orientable triangulation T is
                // 0-efficient then either T has one vertex or T is a
                // 3-sphere with precisely two vertices.
                //
                // It follows then that this is a 3-sphere.
                // Toss it away.
                delete sphere;
                delete processing;
            } else {
                // Now we have a closed orientable one-vertex 0-efficient
                // triangulation.
                // We have to look for an almost normal sphere.
                //
                // From the proof of Proposition 5.12 in Jaco & Rubinstein's
                // 0-efficiency paper, we see that we can restrict our
                // search to octagonal almost normal surfaces.
                // Furthermore, from the result in the quadrilateral-octagon
                // coordinates paper, we can restrict this search further
                // to vertex octagonal almost normal surfaces in
                // quadrilateral-octagonal space.
                sphere = processing->hasOctagonalAlmostNormalSphere();
                if (sphere) {
                    // It's a 3-sphere.  Toss this component away.
                    delete sphere;
                    delete processing;
                } else {
                    // It's a non-trivial prime component!
                    // Note that this will never be an S2xS1 summand;
                    // those get crushed away entirely (we account for
                    // them later).
                    if (summands > 0) {
                        // We have found more than one prime component.
                        threeSphere = false; // Implied by reducibility.
                        zeroEfficient = false; // Implied by reducibility.
                        delete processing;
                        return (irreducible = false);
                    }
                    ++summands;

                    // Note which parts of our initial homology we have
                    // now accounted for.
                    const NAbelianGroup& h1 = processing->getHomologyH1();
                    Z -= h1.getRank();
                    Z2 -= h1.getTorsionRank(2);
                    Z3 -= h1.getTorsionRank(3);

                    // Toss away our prime summand and keep going.
                    delete processing;
                }
            }
        }
    }

    // Run a final homology check: were there any additional S2xS1, RP3
    // or L(3,1) terms?
    if (Z > 0) {
        // There were S2xS1 summands that were crushed away.
        // The manifold must be reducible.
        threeSphere = false; // Implied by reducibility.
        zeroEfficient = false; // Implied by reducibility.
        return (irreducible = false);
    }
    if (summands + Z2 + Z3 > 1) {
        // At least two summands were found and/or crushed away: the
        // manifold must be composite.
        threeSphere = false; // Implied by reducibility.
        zeroEfficient = false; // Implied by reducibility.
        return (irreducible = false);
    }

    // There are no S2xS1 summands, and the manifold is prime.
    return (irreducible = true);
}

bool NTriangulation::knowsIrreducible() const {
    return irreducible.known();
}

bool NTriangulation::hasCompressingDisc() const {
    if (compressingDisc.known())
        return compressingDisc.value();

    // Some sanity checks; also enforce preconditions.
    if (! hasBoundaryTriangles())
        return (compressingDisc = false);
    if ((! isValid()) || isIdeal())
        return (compressingDisc = false);

    long minBdryEuler = 2;
    for (BoundaryComponentIterator it = boundaryComponents.begin();
            it != boundaryComponents.end(); ++it) {
        if ((*it)->getEulerCharacteristic() < minBdryEuler)
            minBdryEuler = (*it)->getEulerCharacteristic();
    }
    if (minBdryEuler == 2)
        return (compressingDisc = false);

    // Off we go.
    // Work with a simplified triangulation.
    NTriangulation* use = new NTriangulation(*this);
    use->intelligentSimplify();

    // Try for a fast answer first.
    if (use->hasSimpleCompressingDisc()) {
        delete use;
        return (compressingDisc = true);
    }

    // Nope.  Decide whether we can use the fast linear programming
    // machinery or whether we need to do a full vertex surface enumeration.
    if (use->isOrientable() && use->getNumberOfBoundaryComponents() == 1) {
        NNormalSurface* ans;
        NTriangulation* crush;
        NTriangulation* comp;
        unsigned nComp;

        while (true) {
            use->intelligentSimplify();

            if (use->getNumberOfVertices() > 1) {
                // Try harder.
                use->barycentricSubdivision();
                use->intelligentSimplify();
                if (use->getNumberOfVertices() > 1) {
                    // Fall back to a full vertex enumeration.
                    // This mirrors the code for non-orientable
                    // triangulations; see that later block for details.
                    NNormalSurfaceList* q = NNormalSurfaceList::enumerate(
                        use, NS_STANDARD);

                    unsigned long nSurfaces = q->getNumberOfSurfaces();
                    for (unsigned long i = 0; i < nSurfaces; ++i) {
                        if (q->getSurface(i)->isCompressingDisc(true)) {
                            delete use;
                            return (compressingDisc = true);
                        }
                    }

                    // No compressing discs!
                    delete use;
                    return (compressingDisc = false);
                }
            }

            NTreeSingleSoln<LPConstraintEuler> search(use, NS_STANDARD);
            if (! search.find()) {
                // No compressing discs!
                delete use;
                return (compressingDisc = false);
            }

            ans = search.buildSurface();
            crush = ans->crush();
            delete ans;
            delete use;

            nComp = crush->splitIntoComponents();
            comp = static_cast<NTriangulation*>(crush->getFirstTreeChild());
            while (comp) {
                if (comp->getNumberOfBoundaryComponents() == 1 &&
                        comp->getBoundaryComponent(0)->getEulerCharacteristic()
                        == minBdryEuler) {
                    // This must be our original manifold.
                    comp->makeOrphan();
                    break;
                }

                comp = static_cast<NTriangulation*>(comp->getNextTreeSibling());
            }

            delete crush;

            if (! comp) {
                // We must have compressed.
                return (compressingDisc = true);
            }

            // Around we go again, but this time with a smaller triangulation.
            use = comp;
        }
    } else {
        // Sigh.  Enumerate all vertex normal surfaces.
        //
        // Hum, are we allowed to do this in quad space?  Jaco and Tollefson
        // use standard coordinates.  Jaco, Letscher and Rubinstein mention
        // quad space, but don't give details (which I'd prefer to see).
        // Leave it in standard coordinates for now.
        NNormalSurfaceList* q = NNormalSurfaceList::enumerate(use, NS_STANDARD);

        // Run through all vertex surfaces looking for a compressing disc.
        unsigned long nSurfaces = q->getNumberOfSurfaces();
        for (unsigned long i = 0; i < nSurfaces; ++i) {
            // Use the fact that all vertex normal surfaces are connected.
            if (q->getSurface(i)->isCompressingDisc(true)) {
                delete use;
                return (compressingDisc = true);
            }
        }

        // No compressing discs!
        delete use;
        return (compressingDisc = false);
    }
}

bool NTriangulation::knowsCompressingDisc() const {
    if (compressingDisc.known())
        return true;

    // Quickly check for non-spherical boundary components before we give up.
    for (BoundaryComponentIterator it = boundaryComponents.begin();
            it != boundaryComponents.end(); ++it) {
        if ((*it)->getEulerCharacteristic() < 2)
            return false;
    }

    // All boundary components are 2-spheres.
    compressingDisc = false;
    return true;
}

bool NTriangulation::hasSimpleCompressingDisc() const {
    // Some sanity checks; also enforce preconditions.
    if (! hasBoundaryTriangles())
        return false;
    if ((! isValid()) || isIdeal())
        return false;

    // Off we go.
    // Work with a simplified triangulation.
    NTriangulation use(*this);
    use.intelligentSimplify();

    // Check to see whether any component is a one-tetrahedron solid torus.
    for (ComponentIterator cit = use.getComponents().begin();
            cit != use.getComponents().end(); ++cit)
        if ((*cit)->getNumberOfTetrahedra() == 1 &&
                (*cit)->getNumberOfTriangles() == 3 &&
                (*cit)->getNumberOfVertices() == 1) {
            // Because we know the triangulation is valid, this rules out
            // all one-tetrahedron triangulations except for LST(1,2,3).
            return (compressingDisc = true);
        }

    // Open up as many boundary triangles as possible (to make it easier to
    // find simple compressing discs).
    TriangleIterator fit;
    bool opened = true;
    while (opened) {
        opened = false;
        for (fit = use.getTriangles().begin(); fit != use.getTriangles().end(); ++fit)
            if (use.openBook(*fit, true, true)) {
                opened = true;
                break;
            }
    }

    // How many boundary spheres do we currently have?
    // This is important because we test whether a disc is a compressing
    // disc by cutting along it and looking for any *new* boundary
    // spheres that might result.
    unsigned long origSphereCount = 0;
    BoundaryComponentIterator bit;
    for (bit = use.getBoundaryComponents().begin(); bit !=
            use.getBoundaryComponents().end(); ++bit)
        if ((*bit)->getEulerCharacteristic() == 2)
            ++origSphereCount;

    // Look for a single internal triangle surrounded by three boundary edges.
    // It doesn't matter whether the edges and/or vertices are distinct.
    NEdge *e0, *e1, *e2;
    unsigned long newSphereCount;
    for (fit = use.getTriangles().begin(); fit != use.getTriangles().end(); ++fit) {
        if ((*fit)->isBoundary())
            continue;

        e0 = (*fit)->getEdge(0);
        e1 = (*fit)->getEdge(1);
        e2 = (*fit)->getEdge(2);
        if (! (e0->isBoundary() && e1->isBoundary() && e2->isBoundary()))
            continue;

        // This could be a compressing disc.
        // Cut along the triangle to be sure.
        const NTriangleEmbedding& emb = (*fit)->getEmbedding(0);

        NTriangulation cut(use);
        cut.getTetrahedron(emb.getTetrahedron()->markedIndex())->unjoin(
            emb.getTriangle());

        // If we don't see a new boundary component, the disc boundary is
        // non-separating in the manifold boundary and is therefore a
        // non-trivial curve.
        if (cut.getNumberOfBoundaryComponents() ==
                use.getNumberOfBoundaryComponents())
            return (compressingDisc = true);

        newSphereCount = 0;
        for (bit = cut.getBoundaryComponents().begin(); bit !=
                cut.getBoundaryComponents().end(); ++bit)
            if ((*bit)->getEulerCharacteristic() == 2)
                ++newSphereCount;

        // Was the boundary of the disc non-trivial?
        if (newSphereCount == origSphereCount)
            return (compressingDisc = true);
    }

    // Look for a tetrahedron with two faces folded together, giving a
    // degree-one edge on the inside and a boundary edge on the outside.
    // The boundary edge on the outside will surround a disc that cuts
    // right through the tetrahedron.
    TetrahedronIterator tit;
    NSnappedBall* ball;
    for (tit = use.tetrahedra.begin(); tit != use.tetrahedra.end(); ++tit) {
        ball = NSnappedBall::formsSnappedBall(*tit);
        if (! ball)
            continue;

        int equator = ball->getEquatorEdge();
        if (! (*tit)->getEdge(equator)->isBoundary()) {
            delete ball;
            continue;
        }

        // This could be a compressing disc.
        // Cut through the tetrahedron to be sure.
        // We do this by removing the tetrahedron, and then plugging
        // both holes on either side of the disc with new copies of the
        // tetrahedron.
        int upper = ball->getBoundaryFace(0);
        delete ball;

        NTetrahedron* adj = (*tit)->adjacentTetrahedron(upper);
        if (! adj) {
            // The disc is trivial.
            continue;
        }

        NTriangulation cut(use);
        cut.getTetrahedron((*tit)->markedIndex())->unjoin(upper);
        NTetrahedron* tet = cut.newTetrahedron();
        tet->joinTo(NEdge::edgeVertex[equator][0], tet, NPerm4(
            NEdge::edgeVertex[equator][0], NEdge::edgeVertex[equator][1]));
        tet->joinTo(upper, cut.getTetrahedron(adj->markedIndex()),
            (*tit)->adjacentGluing(upper));

        // If we don't see a new boundary component, the disc boundary is
        // non-separating in the manifold boundary and is therefore a
        // non-trivial curve.
        if (cut.getNumberOfBoundaryComponents() ==
                use.getNumberOfBoundaryComponents())
            return (compressingDisc = true);

        newSphereCount = 0;
        for (bit = cut.getBoundaryComponents().begin(); bit !=
                cut.getBoundaryComponents().end(); ++bit)
            if ((*bit)->getEulerCharacteristic() == 2)
                ++newSphereCount;

        // Was the boundary of the disc non-trivial?
        if (newSphereCount == origSphereCount)
            return (compressingDisc = true);
    }

    // Nothing found.
    return false;
}

namespace {
    /**
     * Used to sort candidate incompressible surfaces by Euler characteristic.
     * Surfaces with smaller genus (i.e., larger Euler characteristic)
     * are to be processed first.
     */
    struct SurfaceID {
        long index;
            /**< Which surface in the list are we referring to? */
        long euler;
            /**< What is its Euler characteristic? */

        inline bool operator < (const SurfaceID& rhs) const {
            return (euler > rhs.euler ||
                (euler == rhs.euler && index < rhs.index));
        }
    };
}

bool NTriangulation::isHaken() const {
    if (haken.known())
        return haken.value();

    // Check basic preconditions.
    if (! (isValid() && isOrientable() && isClosed() && isConnected()))
        return false;

    // Irreducibility is not a precondition, but we promise to return
    // false immediately if the triangulation is not irreducible.
    // Do not set the property in this situation.
    if (! isIrreducible())
        return false;

    // Okay: we are closed, connected, orientable and irreducible.
    // Move to a copy of this triangulation, which we can mess with.
    NTriangulation t(*this);

    // First check for an easy answer via homology:
    if (t.getHomologyH1().getRank() > 0) {
        threeSphere = false; // Implied by Hakenness.
        return (haken = true);
    }

    // Enumerate vertex normal surfaces in quad coordinates.
    // std::cout << "Enumerating surfaces..." << std::endl;
    NNormalSurfaceList* list = NNormalSurfaceList::enumerate(&t, NS_QUAD);

    // Run through each surface, one at a time.
    // Sort them first however, so we process the (easier) smaller genus
    // surfaces first.
    SurfaceID* id = new SurfaceID[list->getNumberOfSurfaces()];
    unsigned i;
    for (i = 0; i < list->getNumberOfSurfaces(); ++i) {
        id[i].index = i;
        id[i].euler = list->getSurface(i)->getEulerCharacteristic().longValue();
    }
    std::sort(id, id + list->getNumberOfSurfaces());

    const NNormalSurface* f;
    unsigned long discs;
    for (unsigned i = 0; i < list->getNumberOfSurfaces(); ++i) {
        // std::cout << "Testing surface " << i << "..." << std::endl;
        if (list->getSurface(id[i].index)->isIncompressible()) {
            delete[] id;
            threeSphere = false; // Implied by Hakenness.
            return (haken = true);
        }
    }

    delete[] id;
    return (haken = false);
}

bool NTriangulation::knowsHaken() const {
    return haken.known();
}

} // namespace regina

