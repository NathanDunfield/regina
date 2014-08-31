
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Test Suite                                                            *
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

#include <cppunit/extensions/HelperMacros.h>
#include "dim2/dim2exampletriangulation.h"
#include "dim2/dim2triangulation.h"

#include "testsuite/exhaustive.h"
#include "testsuite/generic/generictriangulation.h"
#include "testsuite/dim2/testdim2.h"

using regina::Dim2Triangulation;
using regina::Dim2ExampleTriangulation;

class Dim2TriangulationTest : public TriangulationTest<2> {
    CPPUNIT_TEST_SUITE(Dim2TriangulationTest);

    // Generic tests:
    CPPUNIT_TEST(makeCanonical);
    CPPUNIT_TEST(isomorphismSignature);

    // Dimension-specific tests:
    CPPUNIT_TEST(eltMove13);

    CPPUNIT_TEST_SUITE_END();

    private:
        // Trivial:
        Dim2Triangulation empty;
            /**< An empty triangulation. */

        // Closed orientable:
        Dim2Triangulation s2;
            /**< A 2-sphere with two triangles. */
        Dim2Triangulation s2Tet;
            /**< A 2-sphere with four triangles. */
        Dim2Triangulation s2Oct;
            /**< A 2-sphere with eight triangles. */
        Dim2Triangulation torus;
            /**< A torus with two triangles. */
        Dim2Triangulation torus2;
            /**< A genus two torus. */

        // Closed non-orientable:
        Dim2Triangulation rp2;
            /**< A projective plane with two triangles. */
        Dim2Triangulation kb;
            /**< A Klein bottle with two triangles. */

        // Bounded:
        Dim2Triangulation disc;
            /**< A disc with one triangle. */
        Dim2Triangulation annulus;
            /**< An annulus with two triangles. */
        Dim2Triangulation mobius;
            /**< A Mobius band with one triangle. */

        // Disconnected triangulations:
        Dim2Triangulation disjoint2;
            /**< A disjoint union of two triangulations. */
        Dim2Triangulation disjoint3;
            /**< A disjoint union of three triangulations. */

    public:
        void copyAndDelete(Dim2Triangulation& dest, Dim2Triangulation* source) {
            dest.insertTriangulation(*source);
            dest.setPacketLabel(source->getPacketLabel());
            delete source;
        }

        void setUp() {
            empty.setPacketLabel("Empty triangulation");

            copyAndDelete(s2, Dim2ExampleTriangulation::sphere());
            s2.setPacketLabel("S^2");

            copyAndDelete(s2Tet, Dim2ExampleTriangulation::sphereTetrahedron());
            s2Tet.setPacketLabel("Tetrahedron boundary");

            copyAndDelete(s2Oct, Dim2ExampleTriangulation::sphereOctahedron());
            s2Oct.setPacketLabel("Octahedron boundary");

            copyAndDelete(torus, Dim2ExampleTriangulation::torus());
            torus.setPacketLabel("Torus");

            copyAndDelete(torus2, Dim2ExampleTriangulation::orientable(2, 0));
            torus2.setPacketLabel("Genus 2 torus");

            copyAndDelete(rp2, Dim2ExampleTriangulation::rp2());
            rp2.setPacketLabel("RP^2");

            copyAndDelete(kb, Dim2ExampleTriangulation::kb());
            kb.setPacketLabel("KB");

            copyAndDelete(disc, Dim2ExampleTriangulation::disc());
            disc.setPacketLabel("Disc");

            copyAndDelete(annulus, Dim2ExampleTriangulation::annulus());
            annulus.setPacketLabel("Annulus");

            copyAndDelete(mobius, Dim2ExampleTriangulation::mobius());
            mobius.setPacketLabel("Mobius band");

            disjoint2.insertTriangulation(torus);
            disjoint2.insertTriangulation(mobius);
            disjoint2.setPacketLabel("Torus U Mobius");

            disjoint3.insertTriangulation(kb);
            disjoint3.insertTriangulation(annulus);
            disjoint3.insertTriangulation(s2);
            disjoint3.setPacketLabel("KB U Annulus U S^2");
        }

        void tearDown() {
        }

        /**
         * Run a given test over all hand-coded cases.
         */
        void testManualAll(Dim2TriangulationTestFunction f) {
            f(&empty);
            f(&s2);
            f(&s2Tet);
            f(&s2Oct);
            f(&torus);
            f(&torus2);
            f(&rp2);
            f(&kb);
            f(&disc);
            f(&annulus);
            f(&mobius);
            f(&disjoint2);
            f(&disjoint3);
        }

        void makeCanonical() {
            testManualAll(verifyMakeCanonical);
        }

        void isomorphismSignature() {
            testManualAll(verifyIsomorphismSignature);
        }

        static void verifyEltMove13(Dim2Triangulation* tri) {
            unsigned long n = tri->getNumberOfTriangles();
            for (unsigned long i = 0; i < n; ++i) {
                Dim2Triangulation large(*tri);
                large.oneThreeMove(large.getTriangle(i));

                if (large.getNumberOfTriangles() != n + 2) {
                    std::ostringstream msg;
                    msg << tri->getPacketLabel() << ", tri " << i << ": "
                        << "1-3 move gives wrong # triangles.";
                    CPPUNIT_FAIL(msg.str());
                }

                if (large.isOrientable() != tri->isOrientable()) {
                    std::ostringstream msg;
                    msg << tri->getPacketLabel() << ", tri " << i << ": "
                        << "1-3 move changes orientability.";
                    CPPUNIT_FAIL(msg.str());
                }

                if (large.isClosed() != tri->isClosed()) {
                    std::ostringstream msg;
                    msg << tri->getPacketLabel() << ", tri " << i << ": "
                        << "1-3 move changes closedness.";
                    CPPUNIT_FAIL(msg.str());
                }

                if (large.getNumberOfBoundaryComponents() !=
                        tri->getNumberOfBoundaryComponents()) {
                    std::ostringstream msg;
                    msg << tri->getPacketLabel() << ", tri " << i << ": "
                        << "1-3 move changes # boundary components.";
                    CPPUNIT_FAIL(msg.str());
                }

                if (large.getEulerChar() != tri->getEulerChar()) {
                    std::ostringstream msg;
                    msg << tri->getPacketLabel() << ", tri " << i << ": "
                        << "1-3 move changes Euler characteristic.";
                    CPPUNIT_FAIL(msg.str());
                }
            }
        }

        void eltMove13() {
            testManualAll(verifyEltMove13);
        }
};

void addDim2Triangulation(CppUnit::TextUi::TestRunner& runner) {
    runner.addTest(Dim2TriangulationTest::suite());
}

