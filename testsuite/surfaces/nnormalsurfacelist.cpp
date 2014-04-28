
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Test Suite                                                            *
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

#include "regina-config.h" // For EXCLUDE_NORMALIZ

#include <algorithm>
#include <cppunit/extensions/HelperMacros.h>
#include <memory>
#include "packet/ncontainer.h"
#include "split/nsignature.h"
#include "surfaces/nnormalsurfacelist.h"
#include "triangulation/nexampletriangulation.h"
#include "triangulation/ntriangulation.h"

#include "testsuite/exhaustive.h"
#include "testsuite/surfaces/testsurfaces.h"

using regina::NAbelianGroup;
using regina::NBoolSet;
using regina::NBoundaryComponent;
using regina::NContainer;
using regina::NEdge;
using regina::NExampleTriangulation;
using regina::NNormalSurface;
using regina::NNormalSurfaceList;
using regina::NNormalSurfaceVector;
using regina::NPacket;
using regina::NPerm4;
using regina::NSignature;
using regina::NTetrahedron;
using regina::NTriangulation;

using regina::NS_STANDARD;
using regina::NS_QUAD;
using regina::NS_AN_STANDARD;
using regina::NS_AN_QUAD_OCT;

using regina::NS_VERTEX;
using regina::NS_FUNDAMENTAL;

using regina::NS_VERTEX_DD;
using regina::NS_VERTEX_TREE;
using regina::NS_VERTEX_STD_DIRECT;
using regina::NS_VERTEX_VIA_REDUCED;

using regina::NS_HILBERT_DUAL;
using regina::NS_HILBERT_PRIMAL;

class NNormalSurfaceListTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(NNormalSurfaceListTest);

    CPPUNIT_TEST(standardEmpty);
    CPPUNIT_TEST(standardOneTet);
    CPPUNIT_TEST(standardGieseking);
    CPPUNIT_TEST(standardFigure8);
    CPPUNIT_TEST(standardS3);
    CPPUNIT_TEST(standardLoopC2);
    CPPUNIT_TEST(standardLoopCtw3);
    CPPUNIT_TEST(standardLargeS3);
    CPPUNIT_TEST(standardLargeRP3);
    CPPUNIT_TEST(standardTwistedKxI);
    CPPUNIT_TEST(standardNorSFS);
    CPPUNIT_TEST(quadEmpty);
    CPPUNIT_TEST(quadOneTet);
    CPPUNIT_TEST(quadGieseking);
    CPPUNIT_TEST(quadFigure8);
    CPPUNIT_TEST(quadS3);
    CPPUNIT_TEST(quadLoopC2);
    CPPUNIT_TEST(quadLoopCtw3);
    CPPUNIT_TEST(quadLargeS3);
    CPPUNIT_TEST(quadLargeRP3);
    CPPUNIT_TEST(quadTwistedKxI);
    CPPUNIT_TEST(quadNorSFS);
    CPPUNIT_TEST(almostNormalEmpty);
    CPPUNIT_TEST(almostNormalOneTet);
    CPPUNIT_TEST(almostNormalGieseking);
    CPPUNIT_TEST(almostNormalFigure8);
    CPPUNIT_TEST(almostNormalS3);
    CPPUNIT_TEST(almostNormalLoopC2);
    CPPUNIT_TEST(almostNormalLoopCtw3);
    CPPUNIT_TEST(almostNormalLargeS3);
    CPPUNIT_TEST(almostNormalLargeRP3);
    CPPUNIT_TEST(almostNormalTwistedKxI);
    CPPUNIT_TEST(largeDimensionsStandard);
    CPPUNIT_TEST(largeDimensionsQuad);
    CPPUNIT_TEST(largeDimensionsAlmostNormal);
    CPPUNIT_TEST(standardQuadConversionsConstructed);
    CPPUNIT_TEST(standardQuadConversionsCensus);
    CPPUNIT_TEST(standardANQuadOctConversionsConstructed);
    CPPUNIT_TEST(standardANQuadOctConversionsCensus);
    CPPUNIT_TEST(treeVsDDCensus<NS_QUAD>);
    CPPUNIT_TEST(treeVsDDCensus<NS_STANDARD>);
    CPPUNIT_TEST(treeVsDDCensus<NS_AN_QUAD_OCT>);
    CPPUNIT_TEST(treeVsDDCensus<NS_AN_STANDARD>);
    CPPUNIT_TEST(fundPrimalVsDual<NS_QUAD>);
    CPPUNIT_TEST(fundPrimalVsDual<NS_STANDARD>);
    CPPUNIT_TEST(fundPrimalVsDual<NS_AN_QUAD_OCT>);
    CPPUNIT_TEST(fundPrimalVsDual<NS_AN_STANDARD>);
    CPPUNIT_TEST(disjointConstructed);
    CPPUNIT_TEST(disjointCensus);
    CPPUNIT_TEST(cutAlongConstructed);
    CPPUNIT_TEST(cutAlongCensus);

    CPPUNIT_TEST_SUITE_END();

    private:
        NTriangulation empty;
            /**< An empty triangulation. */
        NTriangulation oneTet;
            /**< A one-tetrahedron ball. */
        NTriangulation figure8;
            /**< The figure eight knot complement. */
        NTriangulation gieseking;
            /**< The Gieseking manifold. */
        NTriangulation S3;
            /**< A one-tetrahedron two-vertex 3-sphere. */
        NTriangulation loopC2;
            /**< An untwisted layered loop of length 2. */
        NTriangulation loopCtw3;
            /**< A twisted layered loop of length 3. */
        NTriangulation largeS3;
            /**< A 3-vertex 5-tetrahedron triangulation of the 3-sphere. */
        NTriangulation largeRP3;
            /**< A 2-vertex 5-tetrahedron triangulation of real
                 projective space. */
        NTriangulation twistedKxI;
            /**< A 3-tetrahedron non-orientable twisted I-bundle over the
                 Klein bottle. */
        NTriangulation norSFS;
            /**< A 9-tetrahedron triangulation of the space
                 SFS [RP2: (2,1) (2,1) (2,1)].  Specifically, this is
                 triangulation #5 of this space from the non-orientable
                 census as it was shipped with Regina 4.5. */


    public:
        void copyAndDelete(NTriangulation& dest, NTriangulation* source) {
            dest.insertTriangulation(*source);
            delete source;
        }

        void generateFromSig(NTriangulation& tri, const std::string& sigStr) {
            NSignature* sig = NSignature::parse(sigStr);
            if (sig == 0)
                return;

            NTriangulation* triNew = sig->triangulate();
            delete sig;
            if (triNew == 0)
                return;

            tri.insertTriangulation(*triNew);
            delete triNew;
        }

        void setUp() {
            NTetrahedron* r;
            NTetrahedron* s;
            NTetrahedron* t;

            // Some triangulations have no face identifications at all.
            empty.setPacketLabel("Empty");

            oneTet.newTetrahedron();
            oneTet.setPacketLabel("Lone tetrahedron");

            // Use pre-coded triangulations where we can.
            copyAndDelete(figure8,
                NExampleTriangulation::figureEightKnotComplement());
            figure8.setPacketLabel("Figure eight knot complement");

            copyAndDelete(gieseking, NExampleTriangulation::gieseking());
            gieseking.setPacketLabel("Gieseking manifold");

            // Layered loops can be constructed automatically.
            S3.insertLayeredLoop(1, false);
            S3.setPacketLabel("S3");

            loopC2.insertLayeredLoop(2, false);
            loopC2.setPacketLabel("C(2)");

            loopCtw3.insertLayeredLoop(3, true);
            loopCtw3.setPacketLabel("C~(3)");

            // Some non-minimal triangulations can be generated from
            // splitting surfaces.
            generateFromSig(largeS3, "abcd.abe.c.d.e");
            largeS3.setPacketLabel("Large S3");

            generateFromSig(largeRP3, "aabcd.be.c.d.e");
            largeRP3.setPacketLabel("Large RP3");

            // A 3-tetrahedron non-orientable twisted I-bundle over the
            // Klein bottle is described in Chapter 3 of Burton's PhD thesis.
            r = twistedKxI.newTetrahedron();
            s = twistedKxI.newTetrahedron();
            t = twistedKxI.newTetrahedron();
            r->joinTo(0, s, NPerm4(0, 1, 2, 3));
            r->joinTo(1, t, NPerm4(2, 1, 0, 3));
            r->joinTo(2, t, NPerm4(1, 3, 2, 0));
            s->joinTo(1, t, NPerm4(0, 3, 2, 1));
            s->joinTo(2, t, NPerm4(3, 1, 0, 2));
            twistedKxI.setPacketLabel("Twisted KxI");

            // Build the 9-tetrahedron SFS from its dehydration string;
            // obscure but painless at least.
            norSFS.insertRehydration("jnnafaabcfighhihimgbpqpepbr");
            norSFS.setPacketLabel("SFS [RP2: (2,1) (2,1) (2,1)]");
        }

        void tearDown() {
        }

        void testSize(NNormalSurfaceList* list,
                const char* listType, unsigned long expectedSize) {
            std::ostringstream msg;
            msg << "Number of " << listType << " for "
                << list->getTriangulation()->getPacketLabel()
                << " should be " << expectedSize << ", not "
                << list->getNumberOfSurfaces() << '.';

            CPPUNIT_ASSERT_MESSAGE(msg.str(),
                list->getNumberOfSurfaces() == expectedSize);
        }

        void testSurface(const NNormalSurface* surface, const char* triName,
                const char* surfaceName, int euler, bool connected,
                bool orient, bool twoSided, bool compact, bool realBdry,
                bool vertexLink, unsigned edgeLink,
                unsigned long central, bool splitting) {
            // Begin with the compactness test so we know which other
            // tests may be performed.
            {
                std::ostringstream msg;
                msg << "Surface [" << surfaceName << "] for " << triName
                    << " should be "
                    << (compact ? "compact." : "non-compact.");

                CPPUNIT_ASSERT_MESSAGE(msg.str(),
                    surface->isCompact() == compact);
            }
            if (compact) {
                std::ostringstream msg;
                msg << "Surface [" << surfaceName << "] for " << triName
                    << " should have Euler char. " << euler << ", not "
                    << surface->getEulerChar() << '.';

                CPPUNIT_ASSERT_MESSAGE(msg.str(),
                    surface->getEulerChar() == euler);
            }
            if (compact) {
                std::ostringstream msg;
                msg << "Surface [" << surfaceName << "] for " << triName
                    << " should be ";
                if (connected)
                    msg << "connected.";
                else
                    msg << "disconnected.";

                CPPUNIT_ASSERT_MESSAGE(msg.str(),
                    surface->isConnected() == connected);
            }
            if (compact) {
                std::ostringstream msg;
                msg << "Surface [" << surfaceName << "] for " << triName
                    << " should be ";
                if (orient)
                    msg << "orientable.";
                else
                    msg << "non-orientable.";

                CPPUNIT_ASSERT_MESSAGE(msg.str(),
                    surface->isOrientable() == orient);
            }
            if (compact) {
                std::ostringstream msg;
                msg << "Surface [" << surfaceName << "] for " << triName
                    << " should be ";
                if (twoSided)
                    msg << "2-sided.";
                else
                    msg << "1-sided.";

                CPPUNIT_ASSERT_MESSAGE(msg.str(),
                    surface->isTwoSided() == twoSided);
            }
            {
                std::ostringstream msg;
                msg << "Surface [" << surfaceName << "] for " << triName
                    << " should have "
                    << (realBdry ? "real boundary." : "no real boundary.");

                CPPUNIT_ASSERT_MESSAGE(msg.str(),
                    surface->hasRealBoundary() == realBdry);
            }
            {
                std::ostringstream msg;
                msg << "Surface [" << surfaceName << "] for " << triName
                    << " should ";
                if (! vertexLink)
                    msg << "not ";
                msg << "be vertex linking.";

                CPPUNIT_ASSERT_MESSAGE(msg.str(),
                    surface->isVertexLinking() == vertexLink);
            }
            {
                std::ostringstream msg;
                msg << "Surface [" << surfaceName << "] for " << triName
                    << " should ";
                if (edgeLink == 0)
                    msg << "not be thin edge linking.";
                else if (edgeLink == 1)
                    msg << "be the (thin) link of one edge.";
                else
                    msg << "be the (thin) link of two edges.";

                std::pair<const regina::NEdge*, const regina::NEdge*> links
                    = surface->isThinEdgeLink();
                unsigned ans;
                if (links.first == 0)
                    ans = 0;
                else if (links.second == 0)
                    ans = 1;
                else
                    ans = 2;

                CPPUNIT_ASSERT_MESSAGE(msg.str(), ans == edgeLink);
            }
            {
                std::ostringstream msg;
                msg << "Surface [" << surfaceName << "] for " << triName
                    << " should ";
                if (central == 0)
                    msg << "not be a central surface.";
                else
                    msg << "be a central surface with " << central
                        << " disc(s).";

                CPPUNIT_ASSERT_MESSAGE(msg.str(),
                    surface->isCentral() == central);
            }
            {
                std::ostringstream msg;
                msg << "Surface [" << surfaceName << "] for " << triName
                    << " should ";
                if (! splitting)
                    msg << "not ";
                msg << "be a splitting surface.";

                CPPUNIT_ASSERT_MESSAGE(msg.str(),
                    surface->isSplitting() == splitting);
            }
        }

        void countCompactSurfaces(const NNormalSurfaceList* list,
                const char* surfaceType,
                unsigned long expectedCount, int euler,
                bool connected, bool orient, bool twoSided,
                bool realBdry, bool vertexLink, unsigned edgeLink,
                unsigned long central, bool splitting) {
            unsigned long tot = 0;
            unsigned long size = list->getNumberOfSurfaces();

            const NNormalSurface* s;
            for (unsigned long i = 0; i < size; i++) {
                s = list->getSurface(i);

                if (s->getEulerChar() == euler &&
                        s->isConnected() == connected &&
                        s->isOrientable() == orient &&
                        s->isTwoSided() == twoSided &&
                        s->hasRealBoundary() == realBdry &&
                        s->isVertexLinking() == vertexLink &&
                        s->isCentral() == central &&
                        s->isSplitting() == splitting) {
                    std::pair<const regina::NEdge*, const regina::NEdge*> links
                        = s->isThinEdgeLink();
                    unsigned linkCount;
                    if (links.first == 0)
                        linkCount = 0;
                    else if (links.second == 0)
                        linkCount = 1;
                    else
                        linkCount = 2;

                    if (linkCount == edgeLink)
                        tot++;
                }
            }

            std::ostringstream msg;
            msg << "Number of " << surfaceType << " in "
                    << list->getTriangulation()->getPacketLabel()
                    << " should be " << expectedCount << ", not "
                    << tot << '.';
            CPPUNIT_ASSERT_MESSAGE(msg.str(), expectedCount == tot);
        }

        static bool lexLess(const NNormalSurfaceVector* a,
                const NNormalSurfaceVector* b) {
            for (unsigned i = 0; i < a->size(); ++i) {
                if ((*a)[i] < (*b)[i])
                    return true;
                if ((*a)[i] > (*b)[i])
                    return false;
            }
            return false;
        }

        static bool identical(const NNormalSurfaceList* lhs,
                const NNormalSurfaceList* rhs) {
            if (lhs->getNumberOfSurfaces() != rhs->getNumberOfSurfaces())
                return false;

            unsigned long n = lhs->getNumberOfSurfaces();
            if (n == 0)
                return true;

            typedef const NNormalSurfaceVector* VecPtr;
            VecPtr* lhsRaw = new VecPtr[n];
            VecPtr* rhsRaw = new VecPtr[n];

            unsigned long i;
            for (i = 0; i < n; ++i) {
                lhsRaw[i] = lhs->getSurface(i)->rawVector();
                rhsRaw[i] = rhs->getSurface(i)->rawVector();
            }

            std::sort(lhsRaw, lhsRaw + n, lexLess);
            std::sort(rhsRaw, rhsRaw + n, lexLess);

            bool ok = true;
            for (i = 0; i < n; ++i)
                if (! (*(lhsRaw[i]) == *(rhsRaw[i]))) {
                    ok = false;
                    break;
                }

            delete[] lhsRaw;
            delete[] rhsRaw;
            return ok;
        }

        void standardEmpty() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &empty, NS_STANDARD);

            testSize(list, "standard normal surfaces", 0);

            delete list;
        }

        void quadEmpty() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &empty, NS_QUAD);

            testSize(list, "quad normal surfaces", 0);

            delete list;
        }

        void almostNormalEmpty() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &empty, NS_AN_STANDARD);

            testSize(list, "standard almost normal surfaces", 0);

            delete list;
        }

        void standardOneTet() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &oneTet, NS_STANDARD);

            testSize(list, "standard normal surfaces", 7);
            countCompactSurfaces(list, "triangular discs", 4,
                1 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                true /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                1 /* central */, false /* splitting */);
            countCompactSurfaces(list, "quadrilateral discs", 3,
                1 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 2 /* edge link */,
                1 /* central */, true /* splitting */);

            delete list;
        }

        void quadOneTet() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &oneTet, NS_QUAD);

            testSize(list, "quad normal surfaces", 3);
            countCompactSurfaces(list, "quadrilateral discs", 3,
                1 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 2 /* edge link */,
                1 /* central */, true /* splitting */);

            delete list;
        }

        void almostNormalOneTet() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &oneTet, NS_AN_STANDARD);

            testSize(list, "standard almost normal surfaces", 10);
            countCompactSurfaces(list, "triangular discs", 4,
                1 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                true /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                1 /* central */, false /* splitting */);
            countCompactSurfaces(list, "quadrilateral discs", 3,
                1 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 2 /* edge link */,
                1 /* central */, true /* splitting */);
            countCompactSurfaces(list, "octagonal discs", 3,
                1 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                1 /* central */, false /* splitting */);

            delete list;
        }

        void standardGieseking() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &gieseking, NS_STANDARD);

            testSize(list, "standard normal surfaces", 1);
            testSurface(list->getSurface(0), "the Gieseking manifold",
                "vertex link",
                0 /* euler */, true /* connected */,
                false /* orient */, true /* two-sided */,
                true /* compact */, false /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);

            delete list;
        }

        void quadGieseking() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &gieseking, NS_QUAD);

            testSize(list, "quad normal surfaces", 0);

            delete list;
        }

        void almostNormalGieseking() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &gieseking, NS_AN_STANDARD);

            testSize(list, "standard almost normal surfaces", 1);
            testSurface(list->getSurface(0), "the Gieseking manifold",
                "vertex link",
                0 /* euler */, true /* connected */,
                false /* orient */, true /* two-sided */,
                true /* compact */, false /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);

            delete list;
        }

        void standardFigure8() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &figure8, NS_STANDARD);

            testSize(list, "standard normal surfaces", 1);
            testSurface(list->getSurface(0), "the figure eight knot complement",
                "vertex link",
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                true /* compact */, false /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);

            delete list;
        }

        void quadFigure8() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &figure8, NS_QUAD);

            testSize(list, "quad normal surfaces", 4);
            for (unsigned long i = 0; i < list->getNumberOfSurfaces(); i++)
                testSurface(list->getSurface(i),
                    "the figure eight knot complement", "spun surface",
                    0 /* euler, N/A */, 0 /* connected, N/A */,
                    0 /* orient, N/A */, 0 /* two-sided, N/A */,
                    false /* compact */, false /* realBdry */,
                    false /* vertex link */, 0 /* edge link */,
                    0 /* central */, false /* splitting */);

            delete list;
        }

        void almostNormalFigure8() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &figure8, NS_AN_STANDARD);

            testSize(list, "standard almost normal surfaces", 1);
            testSurface(list->getSurface(0), "the figure eight knot complement",
                "vertex link",
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                true /* compact */, false /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);

            delete list;
        }

        void standardS3() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &S3, NS_STANDARD);

            testSize(list, "standard normal surfaces", 3);
            countCompactSurfaces(list,
                "standard normal vertex linking spheres", 2,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal double-edge linking tori", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 2 /* edge link */,
                1 /* central */, true /* splitting */);

            delete list;
        }

        void quadS3() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &S3, NS_QUAD);

            testSize(list, "quad normal surfaces", 1);
            countCompactSurfaces(list,
                "quad normal double-edge linking tori", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 2 /* edge link */,
                1 /* central */, true /* splitting */);

            delete list;
        }

        void almostNormalS3() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &S3, NS_AN_STANDARD);

            testSize(list, "standard almost normal surfaces", 4);
            countCompactSurfaces(list,
                "standard normal vertex linking spheres", 2,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal double-edge linking tori", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 2 /* edge link */,
                1 /* central */, true /* splitting */);
            countCompactSurfaces(list,
                "standard almost normal central 2-spheres", 1,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                1 /* central */, false /* splitting */);

            delete list;
        }

        void standardLoopC2() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &loopC2, NS_STANDARD);

            testSize(list, "standard normal surfaces", 5);
            countCompactSurfaces(list,
                "standard normal vertex linking spheres", 2,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal double-edge linking tori", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 2 /* edge link */,
                2 /* central */, true /* splitting */);
            countCompactSurfaces(list,
                "standard normal splitting projective planes", 2,
                1 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                2 /* central */, true /* splitting */);

            delete list;
        }

        void quadLoopC2() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &loopC2, NS_QUAD);

            testSize(list, "quad normal surfaces", 3);
            countCompactSurfaces(list,
                "quad normal double-edge linking tori", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 2 /* edge link */,
                2 /* central */, true /* splitting */);
            countCompactSurfaces(list,
                "quad normal splitting projective planes", 2,
                1 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                2 /* central */, true /* splitting */);

            delete list;
        }

        void almostNormalLoopC2() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &loopC2, NS_AN_STANDARD);

            testSize(list, "standard almost normal surfaces", 5);
            countCompactSurfaces(list,
                "standard normal vertex linking spheres", 2,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal double-edge linking tori", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 2 /* edge link */,
                2 /* central */, true /* splitting */);
            countCompactSurfaces(list,
                "standard normal splitting projective planes", 2,
                1 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                2 /* central */, true /* splitting */);

            delete list;
        }

        void standardLoopCtw3() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &loopCtw3, NS_STANDARD);

            testSize(list, "standard normal surfaces", 5);
            countCompactSurfaces(list,
                "standard normal vertex linking spheres", 1,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal edge linking tori", 3,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal splitting Klein bottles", 1,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                3 /* central */, true /* splitting */);

            delete list;
        }

        void quadLoopCtw3() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &loopCtw3, NS_QUAD);

            testSize(list, "quad normal surfaces", 4);
            countCompactSurfaces(list,
                "quad normal edge linking tori", 3,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal splitting Klein bottles", 1,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                3 /* central */, true /* splitting */);

            delete list;
        }

        void almostNormalLoopCtw3() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &loopCtw3, NS_AN_STANDARD);

            testSize(list, "standard almost normal surfaces", 5);
            countCompactSurfaces(list,
                "standard normal vertex linking spheres", 1,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal edge linking tori", 3,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal splitting Klein bottles", 1,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                3 /* central */, true /* splitting */);

            delete list;
        }

        void standardLargeS3() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &largeS3, NS_STANDARD);

            testSize(list, "standard normal surfaces", 15);
            countCompactSurfaces(list,
                "standard normal vertex linking non-central spheres", 2,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal vertex linking non-central spheres", 1,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                2 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal edge linking non-central spheres", 2,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal edge linking non-central tori", 2,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal edge linking central tori", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                5 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal miscellaneous spheres", 3,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal miscellaneous tori", 3,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal splitting genus two tori", 1,
                -2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                5 /* central */, true /* splitting */);

            delete list;
        }

        void quadLargeS3() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &largeS3, NS_QUAD);

            testSize(list, "quad normal surfaces", 4);
            countCompactSurfaces(list,
                "quad normal edge linking non-central spheres", 2,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal edge linking non-central tori", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal miscellaneous spheres", 1,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);

            delete list;
        }

        void almostNormalLargeS3() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &largeS3, NS_AN_STANDARD);

            // Bleh.  Too messy.  Just count them.
            testSize(list, "standard normal surfaces", 27);

            delete list;
        }

        void standardLargeRP3() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &largeRP3, NS_STANDARD);

            // Bleh.  Too messy.  Just count them.
            testSize(list, "standard normal surfaces", 29);

            delete list;
        }

        void quadLargeRP3() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &largeRP3, NS_QUAD);

            testSize(list, "quad normal surfaces", 5);
            countCompactSurfaces(list,
                "quad normal edge linking non-central spheres", 2,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal edge linking non-central tori", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal miscellaneous spheres", 1,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal miscellaneous projective planes", 1,
                1 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);

            delete list;
        }

        void almostNormalLargeRP3() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &largeRP3, NS_AN_STANDARD);

            // Bleh.  Too messy.  Just count them.
            testSize(list, "standard normal surfaces", 59);

            delete list;
        }

        void standardTwistedKxI() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &twistedKxI, NS_STANDARD);

            testSize(list, "standard normal surfaces", 8);
            countCompactSurfaces(list,
                "standard normal vertex linking discs", 1,
                1 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                true /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal thin edge-linking annuli", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal splitting punctured tori", 1,
                -1 /* euler */, true /* connected */,
                true /* orient */, false /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                3 /* central */, true /* splitting */);
            countCompactSurfaces(list,
                "standard normal central 1-sided Klein bottles", 1,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                3 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal central 2-sided Mobius bands", 1,
                0 /* euler */, true /* connected */,
                false /* orient */, true /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                3 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal generic 1-sided Mobius bands", 2,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal generic 1-sided annuli", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, false /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);

            delete list;
        }

        void quadTwistedKxI() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &twistedKxI, NS_QUAD);

            testSize(list, "quad normal surfaces", 6);
            countCompactSurfaces(list,
                "quad normal thin edge-linking annuli", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal central 1-sided Klein bottles", 1,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                3 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal central 2-sided Mobius bands", 1,
                0 /* euler */, true /* connected */,
                false /* orient */, true /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                3 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal generic 1-sided Mobius bands", 2,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal generic 1-sided annuli", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, false /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);

            delete list;
        }

        void almostNormalTwistedKxI() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &twistedKxI, NS_AN_STANDARD);

            testSize(list, "standard almost normal surfaces", 13);
            countCompactSurfaces(list,
                "standard normal vertex linking discs", 1,
                1 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                true /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal thin edge-linking annuli", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal splitting punctured tori", 1,
                -1 /* euler */, true /* connected */,
                true /* orient */, false /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                3 /* central */, true /* splitting */);
            countCompactSurfaces(list,
                "standard normal central 1-sided Klein bottles", 1,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                3 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal central 2-sided Mobius bands", 1,
                0 /* euler */, true /* connected */,
                false /* orient */, true /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                3 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal generic 1-sided Mobius bands", 2,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal generic 1-sided annuli", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, false /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard almost normal surfaces "
                    "(chi=-1, 1-sided, non-orbl)", 2,
                -1 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard almost normal surfaces "
                    "(chi=-1, 1-sided, orbl)", 1,
                -1 /* euler */, true /* connected */,
                true /* orient */, false /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard almost normal surfaces "
                    "(chi=-2, 1-sided, non-orbl)", 2,
                -2 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                true /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);

            delete list;
        }

        void standardNorSFS() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &norSFS, NS_STANDARD);

            testSize(list, "standard normal surfaces", 25);

            countCompactSurfaces(list,
                "standard normal vertex linking spheres", 1,
                2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                true /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal thin edge-linking Klein bottles", 6,
                0 /* euler */, true /* connected */,
                false /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal central(8) one-sided Klein bottles", 4,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                8 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal central(6) one-sided Klein bottles", 1,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                6 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal miscellaneous one-sided Klein bottles", 4,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal central(9) one-sided tori", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                9 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal miscellaneous one-sided tori", 3,
                0 /* euler */, true /* connected */,
                true /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal two-sided genus two tori", 1,
                -2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal two-sided genus two Klein bottles", 1,
                -2 /* euler */, true /* connected */,
                false /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal one-sided genus two Klein bottles", 2,
                -2 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "standard normal central one-sided genus two Klein bottles", 1,
                -2 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                9 /* central */, false /* splitting */);

            delete list;
        }

        void quadNorSFS() {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &norSFS, NS_QUAD);

            testSize(list, "quad normal surfaces", 21);

            countCompactSurfaces(list,
                "quad normal thin edge-linking Klein bottles", 6,
                0 /* euler */, true /* connected */,
                false /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal central(8) one-sided Klein bottles", 4,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                8 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal central(6) one-sided Klein bottles", 1,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                6 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal miscellaneous one-sided Klein bottles", 4,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal central(9) one-sided tori", 1,
                0 /* euler */, true /* connected */,
                true /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                9 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal miscellaneous one-sided tori", 3,
                0 /* euler */, true /* connected */,
                true /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal two-sided genus two tori", 1,
                -2 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list,
                "quad normal one-sided genus two Klein bottles", 1,
                -2 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 0 /* edge link */,
                0 /* central */, false /* splitting */);

            delete list;
        }

        void testStandardLoopCtwGeneric(unsigned len) {
            NTriangulation loop;
            loop.insertLayeredLoop(len, true);
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &loop, NS_STANDARD);

            std::ostringstream name;
            name << "the twisted layered loop C~(" << len << ")";
            loop.setPacketLabel(name.str());

            // For standard normal and almost normal coordinates we just
            // count the surfaces (as opposed to in quad space, where we can
            // describe the surfaces precisely, with proof).

            // The following pattern has been observed experimentally.
            // For the purposes of the test suite I'm happy to assume it
            // holds in general; certainly it has been verified for all
            // the cases that we actually test here.
            unsigned long curr, prev, tmp;
            if (len == 1)
                curr = 2;
            else if (len == 2)
                curr = 4;
            else {
                curr = 4;
                prev = 2;
                for (unsigned counted = 2; counted < len; ++counted) {
                    tmp = curr + prev - 1;
                    prev = curr;
                    curr = tmp;
                }
            }

            testSize(list, "standard normal surfaces", curr);
        }

        void testQuadLoopCtwGeneric(unsigned len) {
            NTriangulation loop;
            loop.insertLayeredLoop(len, true);
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &loop, NS_QUAD);

            std::ostringstream name;
            name << "the twisted layered loop C~(" << len << ")";
            loop.setPacketLabel(name.str());

            // It is easy to prove in general that C~(len) has precisely
            // (len + 1) vertex surfaces, as described by the following tests.
            testSize(list, "quad normal surfaces", len + 1);
            countCompactSurfaces(list, "quad normal edge linking tori", len,
                0 /* euler */, true /* connected */,
                true /* orient */, true /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                0 /* central */, false /* splitting */);
            countCompactSurfaces(list, "quad normal splitting Klein bottles", 1,
                0 /* euler */, true /* connected */,
                false /* orient */, false /* two-sided */,
                false /* realBdry */,
                false /* vertex link */, 1 /* edge link */,
                len /* central */, true /* splitting */);

            delete list;
        }

        void testAlmostNormalLoopCtwGeneric(unsigned len) {
            NTriangulation loop;
            loop.insertLayeredLoop(len, true);
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                &loop, NS_AN_STANDARD);

            std::ostringstream name;
            name << "the twisted layered loop C~(" << len << ")";
            loop.setPacketLabel(name.str());

            // For standard normal and almost normal coordinates we just
            // count the surfaces (as opposed to in quad space, where we can
            // describe the surfaces precisely, with proof).

            // The following pattern has been observed experimentally.
            // For the purposes of the test suite I'm happy to assume it
            // holds in general; certainly it has been verified for all
            // the cases that we actually test here.
            unsigned long curr, prev, tmp, currgap, prevgap;
            if (len < 7) {
                switch (len) {
                    case 1: curr = 3; break;
                    case 2: curr = 4; break;
                    case 3: curr = 5; break;
                    case 4: curr = 12; break;
                    case 5: curr = 12; break;
                    case 6: curr = 25; break;
                }
            } else {
                prev = 12; prevgap = 2;
                curr = 25; currgap = 1;

                for (unsigned counted = 6; counted < len; ++counted) {
                    tmp = curr + prev + currgap - 1;
                    prev = curr;
                    curr = tmp;

                    tmp = currgap + prevgap;
                    prevgap = currgap;
                    currgap = tmp;
                }
            }

            testSize(list, "standard almost normal surfaces", curr);
        }

        void largeDimensionsStandard() {
            testStandardLoopCtwGeneric(4);
            testStandardLoopCtwGeneric(8);
            testStandardLoopCtwGeneric(12);
        }

        void largeDimensionsQuad() {
            testQuadLoopCtwGeneric(5);
            testQuadLoopCtwGeneric(10);
            testQuadLoopCtwGeneric(20);
            testQuadLoopCtwGeneric(30);
            testQuadLoopCtwGeneric(40);
            testQuadLoopCtwGeneric(50);
        }

        void largeDimensionsAlmostNormal() {
            testAlmostNormalLoopCtwGeneric(3);
            testAlmostNormalLoopCtwGeneric(6);
            testAlmostNormalLoopCtwGeneric(9);
            testAlmostNormalLoopCtwGeneric(12);
            testAlmostNormalLoopCtwGeneric(15);
        }

        static void verifyConversions(NTriangulation* tri) {
            std::auto_ptr<NNormalSurfaceList> stdDirect(
                NNormalSurfaceList::enumerate(
                tri, NS_STANDARD, NS_VERTEX, NS_VERTEX_STD_DIRECT));
            std::auto_ptr<NNormalSurfaceList> stdConv(
                NNormalSurfaceList::enumerate(
                tri, NS_STANDARD, NS_VERTEX, NS_VERTEX_VIA_REDUCED));
            if (tri->getNumberOfTetrahedra() > 0 &&
                    (stdDirect->algorithm().has(NS_VERTEX_VIA_REDUCED) ||
                    ! stdDirect->algorithm().has(NS_VERTEX_STD_DIRECT))) {
                std::ostringstream msg;
                msg << "Direct enumeration in standard coordinates gives "
                    "incorrect algorithm flags for "
                    << tri->getPacketLabel() << '.';
                CPPUNIT_FAIL(msg.str());
            }
            if (tri->isValid() && ! tri->isIdeal()) {
                if (tri->getNumberOfTetrahedra() > 0 &&
                        (stdConv->algorithm().has(NS_VERTEX_STD_DIRECT) ||
                        ! stdConv->algorithm().has(NS_VERTEX_VIA_REDUCED))) {
                    std::ostringstream msg;
                    msg << "Quad-to-standard conversion gives "
                        "incorrect algorithm flags for "
                        << tri->getPacketLabel() << '.';
                    CPPUNIT_FAIL(msg.str());
                }
            } else {
                // Ideal or invalid triangluations should use the standard
                // enumeration process regardless of what the user requested.
                if (tri->getNumberOfTetrahedra() > 0 &&
                        (stdConv->algorithm().has(NS_VERTEX_VIA_REDUCED) ||
                        ! stdConv->algorithm().has(NS_VERTEX_STD_DIRECT))) {
                    std::ostringstream msg;
                    msg << "Quad-to-standard conversion request was "
                        "incorrectly granted for "
                        << tri->getPacketLabel() << '.';
                    CPPUNIT_FAIL(msg.str());
                }
            }
            if (! identical(stdDirect.get(), stdConv.get())) {
                std::ostringstream msg;
                msg << "Direct enumeration vs conversion gives different "
                    "surfaces in standard coordinates for "
                        << tri->getPacketLabel() << '.';
                CPPUNIT_FAIL(msg.str());
            }

            // Only test standard-to-quad if the preconditions for
            // standardToQuad() hold.
            if (tri->isValid() && ! tri->isIdeal()) {
                std::auto_ptr<NNormalSurfaceList> quadDirect(
                    NNormalSurfaceList::enumerate(tri, NS_QUAD));
                std::auto_ptr<NNormalSurfaceList> quadConv(
                    stdDirect->standardToQuad());
                if (! identical(quadDirect.get(), quadConv.get())) {
                    std::ostringstream msg;
                    msg << "Direct enumeration vs conversion gives different "
                        "surfaces in quadrilateral coordinates for "
                            << tri->getPacketLabel() << '.';
                    CPPUNIT_FAIL(msg.str());
                }
            }
        }

        static void verifyConversionsAN(NTriangulation* tri) {
            std::auto_ptr<NNormalSurfaceList> stdDirect(
                NNormalSurfaceList::enumerate(
                tri, NS_AN_STANDARD, NS_VERTEX, NS_VERTEX_STD_DIRECT));
            std::auto_ptr<NNormalSurfaceList> stdConv(
                NNormalSurfaceList::enumerate(
                tri, NS_AN_STANDARD, NS_VERTEX, NS_VERTEX_VIA_REDUCED));
            if (tri->getNumberOfTetrahedra() > 0 &&
                    (stdDirect->algorithm().has(NS_VERTEX_VIA_REDUCED) ||
                    ! stdDirect->algorithm().has(NS_VERTEX_STD_DIRECT))) {
                std::ostringstream msg;
                msg << "Direct enumeration in standard AN coordinates gives "
                    "incorrect algorithm flags for "
                    << tri->getPacketLabel() << '.';
                CPPUNIT_FAIL(msg.str());
            }
            if (tri->isValid() && ! tri->isIdeal()) {
                if (tri->getNumberOfTetrahedra() > 0 &&
                        (stdConv->algorithm().has(NS_VERTEX_STD_DIRECT) ||
                        ! stdConv->algorithm().has(NS_VERTEX_VIA_REDUCED))) {
                    std::ostringstream msg;
                    msg << "Quad-oct-to-standard-AN conversion gives "
                        "incorrect algorithm flags for "
                        << tri->getPacketLabel() << '.';
                    CPPUNIT_FAIL(msg.str());
                }
            } else {
                // Ideal or invalid triangluations should use the standard
                // enumeration process regardless of what the user requested.
                if (tri->getNumberOfTetrahedra() > 0 &&
                        (stdConv->algorithm().has(NS_VERTEX_VIA_REDUCED) ||
                        ! stdConv->algorithm().has(NS_VERTEX_STD_DIRECT))) {
                    std::ostringstream msg;
                    msg << "Quad-oct-to-standard-AN conversion request was "
                        "incorrectly granted for "
                        << tri->getPacketLabel() << '.';
                    CPPUNIT_FAIL(msg.str());
                }
            }
            if (! identical(stdDirect.get(), stdConv.get())) {
                std::ostringstream msg;
                msg << "Direct enumeration vs conversion gives different "
                    "surfaces in standard almost normal coordinates for "
                        << tri->getPacketLabel() << '.';
                CPPUNIT_FAIL(msg.str());
            }

            // Only test standard-to-quad if the preconditions for
            // standardToQuad() hold.
            if (tri->isValid() && ! tri->isIdeal()) {
                std::auto_ptr<NNormalSurfaceList> quadDirect(
                    NNormalSurfaceList::enumerate(tri, NS_AN_QUAD_OCT));
                std::auto_ptr<NNormalSurfaceList> quadConv(
                    stdDirect->standardANToQuadOct());
                if (! identical(quadDirect.get(), quadConv.get())) {
                    std::ostringstream msg;
                    msg << "Direct enumeration vs conversion gives different "
                        "surfaces in quadrilateral-octagon coordinates for "
                            << tri->getPacketLabel() << '.';
                    CPPUNIT_FAIL(msg.str());
                }
            }
        }

        void standardQuadConversionsConstructed() {
            verifyConversions(&empty);
            verifyConversions(&oneTet);
            verifyConversions(&S3);
            verifyConversions(&loopC2);
            verifyConversions(&loopCtw3);
            verifyConversions(&largeS3);
            verifyConversions(&largeRP3);
            verifyConversions(&twistedKxI);
            verifyConversions(&norSFS);
        }

        void standardQuadConversionsCensus() {
            runCensusMinClosed(&verifyConversions);
            runCensusAllClosed(&verifyConversions);
            runCensusAllBounded(&verifyConversions);
            runCensusAllIdeal(&verifyConversions);
        }

        void standardANQuadOctConversionsConstructed() {
            verifyConversionsAN(&empty);
            verifyConversionsAN(&oneTet);
            verifyConversionsAN(&S3);
            verifyConversionsAN(&loopC2);
            verifyConversionsAN(&loopCtw3);
            verifyConversionsAN(&largeS3);
            verifyConversionsAN(&largeRP3);
            verifyConversionsAN(&twistedKxI);
            verifyConversionsAN(&norSFS);
        }

        void standardANQuadOctConversionsCensus() {
            runCensusMinClosed(verifyConversionsAN);
            runCensusAllClosed(verifyConversionsAN);
            runCensusAllBounded(verifyConversionsAN);
            runCensusAllIdeal(verifyConversionsAN);
        }

        template <regina::NormalCoords coords>
        static void verifyTreeVsDD(NTriangulation* tri) {
            NNormalSurfaceList* dd = NNormalSurfaceList::enumerate(
                tri, coords, NS_VERTEX, NS_VERTEX_DD | NS_VERTEX_STD_DIRECT);
            NNormalSurfaceList* tree = NNormalSurfaceList::enumerate(
                tri, coords, NS_VERTEX, NS_VERTEX_TREE | NS_VERTEX_STD_DIRECT);
            if (tri->getNumberOfTetrahedra() > 0 &&
                    (dd->algorithm().has(NS_VERTEX_TREE) ||
                    ! dd->algorithm().has(NS_VERTEX_DD))) {
                std::ostringstream msg;
                msg << "Double description enumeration in coordinate system "
                    << coords << " gives incorrect algorithm flags for "
                    << tri->getPacketLabel() << '.';
                CPPUNIT_FAIL(msg.str());
            }
            if (tri->getNumberOfTetrahedra() > 0 &&
                    (tree->algorithm().has(NS_VERTEX_DD) ||
                    ! tree->algorithm().has(NS_VERTEX_TREE))) {
                std::ostringstream msg;
                msg << "Tree traversal enumeration in coordinate system "
                    << coords << " gives incorrect algorithm flags for "
                    << tri->getPacketLabel() << '.';
                CPPUNIT_FAIL(msg.str());
            }
            if (! identical(dd, tree)) {
                std::ostringstream msg;
                msg << "Double description vs tree enumeration in "
                    "coordinate system " << coords << " gives different "
                    "surfaces for " << tri->getPacketLabel() << '.';
                CPPUNIT_FAIL(msg.str());
            }
            delete dd;
            delete tree;
        }

        template <regina::NormalCoords coords>
        void treeVsDDCensus() {
            runCensusMinClosed(verifyTreeVsDD<coords>);
            runCensusAllClosed(verifyTreeVsDD<coords>);
            runCensusAllBounded(verifyTreeVsDD<coords>);
            runCensusAllIdeal(verifyTreeVsDD<coords>);
        }

        template <regina::NormalCoords coords>
        static void verifyFundPrimalVsDual(NTriangulation* tri) {
            NNormalSurfaceList* primal = NNormalSurfaceList::enumerate(
                tri, coords, NS_FUNDAMENTAL, NS_HILBERT_PRIMAL);
            NNormalSurfaceList* dual = NNormalSurfaceList::enumerate(
                tri, coords, NS_FUNDAMENTAL, NS_HILBERT_DUAL);
#ifdef EXCLUDE_NORMALIZ
            // If we build without Normaliz, the primal algorithm falls
            // through to the dual algorithm.
            if (tri->getNumberOfTetrahedra() > 0 &&
                    (primal->algorithm().has(NS_HILBERT_PRIMAL) ||
                    ! primal->algorithm().has(NS_HILBERT_DUAL))) {
                std::ostringstream msg;
                msg << "Primal Hilbert basis enumeration in coordinate system "
                    << coords << " does not fall through to dual Hilbert "
                    "basis enumeration when Normaliz is excluded for "
                    << tri->getPacketLabel() << '.';
                CPPUNIT_FAIL(msg.str());
            }
#else
            if (tri->getNumberOfTetrahedra() > 0 &&
                    (primal->algorithm().has(NS_HILBERT_DUAL) ||
                    ! primal->algorithm().has(NS_HILBERT_PRIMAL))) {
                std::ostringstream msg;
                msg << "Primal Hilbert basis enumeration in coordinate system "
                    << coords << " gives incorrect algorithm flags for "
                    << tri->getPacketLabel() << '.';
                CPPUNIT_FAIL(msg.str());
            }
#endif
            if (tri->getNumberOfTetrahedra() > 0 &&
                    (dual->algorithm().has(NS_HILBERT_PRIMAL) ||
                    ! dual->algorithm().has(NS_HILBERT_DUAL))) {
                std::ostringstream msg;
                msg << "Dual Hilbert basis enumeration in coordinate system "
                    << coords << " gives incorrect algorithm flags for "
                    << tri->getPacketLabel() << '.';
                CPPUNIT_FAIL(msg.str());
            }
            if (! identical(primal, dual)) {
                std::ostringstream msg;
                msg << "Primal vs dual Hilbert basis enumeration in "
                    "coordinate system " << coords << " gives different "
                    "surfaces for " << tri->getPacketLabel() << '.';
                CPPUNIT_FAIL(msg.str());
            }
            delete primal;
            delete dual;
        }

        template <regina::NormalCoords coords>
        void fundPrimalVsDual() {
            runCensusMinClosed(verifyFundPrimalVsDual<coords>, true);
            runCensusAllClosed(verifyFundPrimalVsDual<coords>, true);
            runCensusAllBounded(verifyFundPrimalVsDual<coords>, true);
            runCensusAllIdeal(verifyFundPrimalVsDual<coords>, true);
        }

        static void testDisjoint(NTriangulation* tri) {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                tri, NS_AN_STANDARD);
            unsigned long n = list->getNumberOfSurfaces();

            unsigned long i, j;
            const NNormalSurface *s, *t;
            std::pair<const NEdge*, const NEdge*> edges;
            unsigned long edge;

            for (i = 0; i < n; ++i) {
                s = list->getSurface(i);

                // For some types of surfaces we know exactly what it
                // should be disjoint from.
                if (s->isVertexLinking()) {
                    // Vertex links are disjoint from everything.
                    for (j = 0; j < n; ++j) {
                        t = list->getSurface(j);
                        if (! s->disjoint(*t)) {
                            std::ostringstream msg;
                            msg << "Surface #" << i << " for "
                                << tri->getPacketLabel()
                                << " is a vertex link "
                                "and therefore should be disjoint from "
                                "surface #" << j << ".";
                            CPPUNIT_FAIL(msg.str());
                        }
                    }
                } else if ((edges = s->isThinEdgeLink()).first) {
                    // A thin edge link is disjoint from (i) all vertex
                    // links, and (ii) all surfaces that do not meet the
                    // relevant edge (except the edge link itself, if it
                    // is 1-sided).
                    edge = tri->edgeIndex(edges.first);

                    for (j = 0; j < n; ++j) {
                        // Deal with (s, s) later.
                        if (j == i)
                            continue;

                        t = list->getSurface(j);
                        if (t->isVertexLinking()) {
                            if (! s->disjoint(*t)) {
                                std::ostringstream msg;
                                msg << "Surface #" << i << " for "
                                    << tri->getPacketLabel()
                                    << " is a thin edge link and therefore "
                                    "should be disjoint from surface #" << j
                                    << ", which is a vertex link.";
                                CPPUNIT_FAIL(msg.str());
                            }
                        } else if (t->getEdgeWeight(edge) == 0) {
                            if (! s->disjoint(*t)) {
                                std::ostringstream msg;
                                msg << "Surface #" << i << " for "
                                    << tri->getPacketLabel()
                                    << " is a thin edge link and therefore "
                                    "should be disjoint from surface #" << j
                                    << ", which does not meet the "
                                    "corresponding edge.";
                                CPPUNIT_FAIL(msg.str());
                            }
                        } else {
                            if (s->disjoint(*t)) {
                                std::ostringstream msg;
                                msg << "Surface #" << i <<
                                    " is a thin edge link and therefore "
                                    "should not be disjoint from surface #"
                                    << j << ", which meets the "
                                    "corresponding edge.";
                                CPPUNIT_FAIL(msg.str());
                            }
                        }
                    }
                }

                // Ensure that the surface is disjoint from itself
                // iff it is two-sided.
                if (s->isTwoSided() && ! s->disjoint(*s)) {
                    std::ostringstream msg;
                    msg << "Surface #" << i << " for "
                        << tri->getPacketLabel()
                        << " is two-sided and therefore should be "
                        "disjoint from itself.";
                    CPPUNIT_FAIL(msg.str());
                } else if ((! s->isTwoSided()) && s->disjoint(*s)) {
                    std::ostringstream msg;
                    msg << "Surface #" << i << " for "
                        << tri->getPacketLabel()
                        << " is one-sided and therefore should not be "
                        "disjoint from itself.";
                    CPPUNIT_FAIL(msg.str());
                }
            }

            delete list;
        }

        void disjointConstructed() {
            testDisjoint(&oneTet);
            testDisjoint(&figure8);
            testDisjoint(&gieseking);
            testDisjoint(&S3);
            testDisjoint(&loopC2);
            testDisjoint(&loopCtw3);
            testDisjoint(&largeS3);
            testDisjoint(&largeRP3);
            testDisjoint(&twistedKxI);
            testDisjoint(&norSFS);
        }

        void disjointCensus() {
            runCensusAllClosed(&testDisjoint);
            runCensusAllBounded(&testDisjoint);
            runCensusAllIdeal(&testDisjoint);
        }

        static NNormalSurface* doubleSurface(const NNormalSurface* s) {
            NNormalSurfaceVector* v =
                static_cast<NNormalSurfaceVector*>(s->rawVector()->clone());
            (*v) *= 2;
            return new NNormalSurface(s->getTriangulation(), v);
        }

        /**
         * PRE: tri is valid with only one component, and all vertex
         * links are spheres or discs.
         */
        static bool mightBeTwistedProduct(const NTriangulation* tri) {
            if (tri->getNumberOfBoundaryComponents() != 1)
                return false;

            // Check the relationship between H1 and H1Bdry.
            // We must have one of:
            //  -  H1 = (2g)Z, H1Bdry = (4g-2)Z;
            //  -  H1 = Z_2 + (g-1)Z, H1Bdry = Z_2 + (2g-3)Z;
            //  -  H1 = Z_2 + (g-1)Z, H1Bdry = (2g-2)Z;
            const NAbelianGroup& h1 = tri->getHomologyH1();
            const NAbelianGroup& bdry = tri->getHomologyH1Bdry();

            if (h1.getNumberOfInvariantFactors() == 0) {
                // Must have H1 = (2g)Z.
                if (bdry.getNumberOfInvariantFactors() != 0)
                    return false;
                if (bdry.getRank() != 2 * h1.getRank() - 2)
                    return false;
            } else if (h1.getNumberOfInvariantFactors() == 1) {
                // Must have H1 = Z_2 + (g-1)Z.
                if (h1.getInvariantFactor(0) != 2)
                    return false;

                if (bdry.getNumberOfInvariantFactors() == 0) {
                    if (bdry.getRank() != 2 * h1.getRank())
                        return false;
                } else {
                    if (bdry.getNumberOfInvariantFactors() != 1)
                        return false;
                    if (bdry.getInvariantFactor(0) != 2)
                        return false;
                    if (bdry.getRank() != 2 * h1.getRank() - 1)
                        return false;
                }
            } else
                return false;

            // Check that H1Rel is just Z_2.
            if (! tri->getHomologyH1Rel().isZn(2))
                return false;

            return true;
        }

        /**
         * PRE: tri is valid with only one component, and all vertex
         * links are spheres or discs.
         */
        static bool mightBeUntwistedProduct(const NTriangulation* tri) {
            if (tri->getNumberOfBoundaryComponents() != 2)
                return false;

            // Check that both boundary components are homeomorphic.
            NBoundaryComponent* b0 = tri->getBoundaryComponent(0);
            NBoundaryComponent* b1 = tri->getBoundaryComponent(1);

            if (b0->getEulerChar() != b1->getEulerChar())
                return false;
            if (b0->isOrientable() && ! b1->isOrientable())
                return false;
            if (b1->isOrientable() && ! b0->isOrientable())
                return false;

            // Check that H1 is of the form (k)Z or Z_2 + (k)Z, and that
            // H1Bdry = 2 H1.
            const NAbelianGroup& h1 = tri->getHomologyH1();
            const NAbelianGroup& bdry = tri->getHomologyH1Bdry();

            if (h1.getNumberOfInvariantFactors() == 0) {
                // Must have H1 = (k)Z.
                if (bdry.getRank() != 2 * h1.getRank())
                    return false;
                if (bdry.getNumberOfInvariantFactors() != 0)
                    return false;
            } else if (h1.getNumberOfInvariantFactors() == 1) {
                // Must have H1 = Z_2 + (k)Z.
                if (h1.getInvariantFactor(0) != 2)
                    return false;
                if (bdry.getRank() != 2 * h1.getRank())
                    return false;
                if (bdry.getNumberOfInvariantFactors() != 2)
                    return false;
                if (bdry.getInvariantFactor(0) != 2)
                    return false;
                if (bdry.getInvariantFactor(1) != 2)
                    return false;
            } else
                return false;

            // Check that H1Rel is just Z.
            if (! tri->getHomologyH1Rel().isZ())
                return false;

            return true;
        }

        // Check whether the boundary of the given triangulation *might*
        // be equal to (i) the surface s, (ii) two copies of the surface s,
        // or (iii) a double cover of the surface s.
        // Increment the relevant counters accordingly.
        static void checkBoundaryType(const NNormalSurface* s,
                const NTriangulation* tri, unsigned& foundS,
                unsigned& foundTwoCopies, unsigned& foundDoubleCover) {
            if (tri->getNumberOfBoundaryComponents() == 1) {
                const NBoundaryComponent* b = tri->getBoundaryComponent(0);

                if (s->getEulerChar() == b->getEulerChar()
                        && s->isOrientable() == b->isOrientable())
                    ++foundS;
                if (s->getEulerChar() * 2 == b->getEulerChar() &&
                        (b->isOrientable() || ! s->isOrientable()))
                    ++foundDoubleCover;
            } else if (tri->getNumberOfBoundaryComponents() == 2) {
                const NBoundaryComponent* b0 = tri->getBoundaryComponent(0);
                const NBoundaryComponent* b1 = tri->getBoundaryComponent(1);

                if (
                        s->getEulerChar() == b0->getEulerChar() &&
                        s->getEulerChar() == b1->getEulerChar() &&
                        s->isOrientable() == b0->isOrientable() &&
                        s->isOrientable() == b1->isOrientable())
                    ++foundTwoCopies;
            }
        }

        /**
         * PRE: tri is valid and has only one component.
         */
        static void testCutAlong(NTriangulation* tri) {
            NNormalSurfaceList* list = NNormalSurfaceList::enumerate(
                tri, NS_STANDARD);
            unsigned long n = list->getNumberOfSurfaces();

            const NNormalSurface *s;
            std::auto_ptr<NTriangulation> t;
            std::auto_ptr<NContainer> comp;
            unsigned long nComp;

            std::auto_ptr<NNormalSurface> sDouble;
            std::auto_ptr<NTriangulation> tDouble;
            std::auto_ptr<NContainer> compDouble;
            unsigned long nCompDouble;

            bool separating;

            unsigned long expected;
            NPacket* p;

            // We use the fact that each normal surface is connected.
            for (unsigned long i = 0; i < n; ++i) {
                s = list->getSurface(i);
                t.reset(s->cutAlong());
                t->intelligentSimplify();
                comp.reset(new NContainer());
                nComp = t->splitIntoComponents(comp.get(), false);

                sDouble.reset(doubleSurface(s));
                tDouble.reset(sDouble->cutAlong());
                tDouble->intelligentSimplify();
                compDouble.reset(new NContainer());
                nCompDouble = tDouble->splitIntoComponents(compDouble.get(),
                    false);

                separating = (s->isTwoSided() && nComp > 1);

                expected = (separating ? 2 : 1);
                if (nComp != expected) {
                    std::ostringstream msg;
                    msg << "Cutting along surface #" << i << " for "
                        << tri->getPacketLabel()
                        << " gives " << nComp << " component(s), not "
                        << expected << " as expected.";
                    CPPUNIT_FAIL(msg.str());
                }

                expected = (separating ? 3 : 2);
                if (nCompDouble != expected) {
                    std::ostringstream msg;
                    msg << "Cutting along double surface #" << i
                        << " for " << tri->getPacketLabel()
                        << " gives " << nCompDouble << " component(s), not "
                        << expected << " as expected.";
                    CPPUNIT_FAIL(msg.str());
                }

                if (! t->isValid()) {
                    std::ostringstream msg;
                    msg << "Cutting along surface #" << i << " for "
                        << tri->getPacketLabel()
                        << " gives an invalid triangulation.";
                    CPPUNIT_FAIL(msg.str());
                }
                if (! tDouble->isValid()) {
                    std::ostringstream msg;
                    msg << "Cutting along double surface #" << i
                        << " for " << tri->getPacketLabel()
                        << " gives an invalid triangulation.";
                    CPPUNIT_FAIL(msg.str());
                }

                if (tri->isIdeal() && ! t->isIdeal()) {
                    std::ostringstream msg;
                    msg << "Cutting along surface #" << i
                        << " for "
                        << tri->getPacketLabel() << " (which is ideal)"
                        << " gives a non-ideal triangulation.";
                    CPPUNIT_FAIL(msg.str());
                }
                if (tri->isIdeal() && ! tDouble->isIdeal()) {
                    std::ostringstream msg;
                    msg << "Cutting along double surface #" << i
                        << " for "
                        << tri->getPacketLabel() << " (which is ideal)"
                        << " gives a non-ideal triangulation.";
                    CPPUNIT_FAIL(msg.str());
                }
                if ((! tri->isIdeal()) && t->isIdeal()) {
                    std::ostringstream msg;
                    msg << "Cutting along surface #" << i
                        << " for "
                        << tri->getPacketLabel() << " (which is not ideal)"
                        << " gives an ideal triangulation.";
                    CPPUNIT_FAIL(msg.str());
                }
                if ((! tri->isIdeal()) && tDouble->isIdeal()) {
                    std::ostringstream msg;
                    msg << "Cutting along double surface #" << i
                        << " for "
                        << tri->getPacketLabel() << " (which is not ideal)"
                        << " gives an ideal triangulation.";
                    CPPUNIT_FAIL(msg.str());
                }

                if (tri->isOrientable() && ! t->isOrientable()) {
                    std::ostringstream msg;
                    msg << "Cutting along surface #" << i
                        << " for "
                        << tri->getPacketLabel() << " (which is orientable)"
                        << " gives a non-orientable triangulation.";
                    CPPUNIT_FAIL(msg.str());
                }
                if (tri->isOrientable() && ! tDouble->isOrientable()) {
                    std::ostringstream msg;
                    msg << "Cutting along double surface #" << i
                        << " for "
                        << tri->getPacketLabel() << " (which is orientable)"
                        << " gives a non-orientable triangulation.";
                    CPPUNIT_FAIL(msg.str());
                }

                for (p = comp->getFirstTreeChild(); p;
                        p = p->getNextTreeSibling())
                    if (! static_cast<NTriangulation*>(p)->
                            hasBoundaryTriangles()) {
                        std::ostringstream msg;
                        msg << "Cutting along surface #" << i
                            << " for " << tri->getPacketLabel()
                            << " gives a component with no boundary triangles.";
                        CPPUNIT_FAIL(msg.str());
                    }
                for (p = compDouble->getFirstTreeChild(); p;
                        p = p->getNextTreeSibling())
                    if (! static_cast<NTriangulation*>(p)->
                            hasBoundaryTriangles()) {
                        std::ostringstream msg;
                        msg << "Cutting along double surface #" << i
                            << " for " << tri->getPacketLabel()
                            << " gives a component with no boundary triangles.";
                        CPPUNIT_FAIL(msg.str());
                    }

                // The remaining tests only work for closed triangulations.
                if (! tri->isClosed())
                    continue;

                // Check the boundaries of components of t.
                unsigned expectS, expectTwoCopies, expectDoubleCover;
                unsigned foundS, foundTwoCopies, foundDoubleCover;
                if (separating) {
                    expectS = 2;
                    expectTwoCopies = 0;
                    expectDoubleCover = 0;
                } else if (s->isTwoSided()) {
                    expectS = 0;
                    expectTwoCopies = 1;
                    expectDoubleCover = 0;
                } else {
                    expectS = 0;
                    expectTwoCopies = 0;
                    expectDoubleCover = 1;
                }
                if (t->getNumberOfBoundaryComponents() !=
                        expectS + 2 * expectTwoCopies + expectDoubleCover) {
                    std::ostringstream msg;
                    msg << "Cutting along surface #" << i << " for "
                        << tri->getPacketLabel()
                        << " gives the wrong number of boundary components.";
                    CPPUNIT_FAIL(msg.str());
                }
                foundS = foundTwoCopies = foundDoubleCover = 0;
                for (p = comp->getFirstTreeChild(); p;
                        p = p->getNextTreeSibling())
                    checkBoundaryType(s, static_cast<NTriangulation*>(p),
                        foundS, foundTwoCopies, foundDoubleCover);
                if (foundS < expectS || foundTwoCopies < expectTwoCopies ||
                        foundDoubleCover < expectDoubleCover) {
                    std::ostringstream msg;
                    msg << "Cutting along surface #" << i << " for "
                        << tri->getPacketLabel()
                        << " gives boundary components of the wrong type.";
                    CPPUNIT_FAIL(msg.str());
                }

                // Check the boundaries of components of tDouble.
                if (separating) {
                    expectS = 2;
                    expectTwoCopies = 1;
                    expectDoubleCover = 0;
                } else if (s->isTwoSided()) {
                    expectS = 0;
                    expectTwoCopies = 2;
                    expectDoubleCover = 0;
                } else {
                    expectS = 0;
                    expectTwoCopies = 0;
                    expectDoubleCover = 2;
                }
                if (tDouble->getNumberOfBoundaryComponents() !=
                        expectS + 2 * expectTwoCopies + expectDoubleCover) {
                    std::ostringstream msg;
                    msg << "Cutting along double surface #" << i
                        << " for " << tri->getPacketLabel()
                        << " gives the wrong number of boundary components.";
                    CPPUNIT_FAIL(msg.str());
                }
                foundS = foundTwoCopies = foundDoubleCover = 0;
                for (p = compDouble->getFirstTreeChild(); p;
                        p = p->getNextTreeSibling())
                    checkBoundaryType(s, static_cast<NTriangulation*>(p),
                        foundS, foundTwoCopies, foundDoubleCover);
                if (foundS < expectS || foundTwoCopies < expectTwoCopies ||
                        foundDoubleCover < expectDoubleCover) {
                    std::ostringstream msg;
                    msg << "Cutting along double surface #" << i
                        << " for " << tri->getPacketLabel()
                        << " gives boundary components of the wrong type.";
                    CPPUNIT_FAIL(msg.str());
                }

                // Look for the product piece when cutting along the
                // double surface.
                for (p = compDouble->getFirstTreeChild(); p;
                        p = p->getNextTreeSibling()) {
                    if (s->isTwoSided()) {
                        if (mightBeUntwistedProduct(
                                static_cast<NTriangulation*>(p)))
                            break;
                    } else {
                        if (mightBeTwistedProduct(
                                static_cast<NTriangulation*>(p)))
                            break;
                    }
                }
                if (! p) {
                    std::ostringstream msg;
                    msg << "Cutting along double surface #" << i
                        << " for " << tri->getPacketLabel()
                        << " does not yield a product piece as expected.";
                    CPPUNIT_FAIL(msg.str());
                }
            }

            delete list;
        }

        void cutAlongConstructed() {
            testCutAlong(&oneTet);
            testCutAlong(&figure8);
            testCutAlong(&gieseking);
            testCutAlong(&S3);
            testCutAlong(&loopC2);
            testCutAlong(&loopCtw3);
            testCutAlong(&largeS3);
            testCutAlong(&largeRP3);
            testCutAlong(&twistedKxI);
            testCutAlong(&norSFS);
        }

        void cutAlongCensus() {
            runCensusAllClosed(&testCutAlong, true);
            runCensusAllBounded(&testCutAlong, true);
            runCensusAllIdeal(&testCutAlong, true);
        }
};

void addNNormalSurfaceList(CppUnit::TextUi::TestRunner& runner) {
    runner.addTest(NNormalSurfaceListTest::suite());
}

