
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

#include <sstream>
#include <cppunit/extensions/HelperMacros.h>

#include "maths/nsparsegrid.h"
#include "maths/nmatrixint.h"

#include "triangulation/nexampletriangulation.h"
#include "triangulation/ntriangulation.h"
#include "dim4/dim4triangulation.h"

#include "manifold/nlensspace.h"

#include "algebra/ncellulardata.h"
#include "algebra/nbilinearform.h"
#include "algebra/nmarkedabeliangroup.h"

#include "testsuite/utilities/testutilities.h"
#include "testsuite/testparams.h"

using regina::NLargeInteger;
using regina::NTriangulation;
using regina::Dim4Triangulation;
using regina::NExampleTriangulation;
using regina::NMarkedAbelianGroup;
using regina::NHomMarkedAbelianGroup;
using regina::NCellularData;
using regina::NBilinearForm;
using regina::NMatrixInt;
using regina::NLensSpace;
using regina::NMatrixRing; 
using regina::NSVPolynomialRing; 


// export REGINA_DETAILED_TESTS=yes
// to enable detailed (slow!) tests.

class NCellularDataTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(NCellularDataTest);

    // Poincare Polynomial, Euler Characteristic
    CPPUNIT_TEST(basic_tests); 
    // Comparisons between NTriangulation, Dim4Triangulation and NCellularData H1 computations
    CPPUNIT_TEST(absolute_h1_comparisons);
    // verify chain complexes are really chain complexes.
    CPPUNIT_TEST(chain_complex_tests); 
    // verify chain maps are really maps of chain complexes.
    CPPUNIT_TEST(chain_maps_tests); 
    // maps between standard, dual and mixed
    CPPUNIT_TEST(coordinate_isomorphisms_tests);  
    // homology long exact sequence
    CPPUNIT_TEST(homology_LES_tests); 
    // poincare duality
    CPPUNIT_TEST(poincare_duality_tests); 
    // intersection forms
    CPPUNIT_TEST(intersectionform_tests); 
    // check lens space classification, torsion linking form
    CPPUNIT_TEST(lensspacehomotopyclassification_tests); 
    // check alexander polynomials
    CPPUNIT_TEST(alexpoly_tests);
    // torsion linking form for 3-manifold with non-cyclic torsion in H1
    CPPUNIT_TEST(noncyclic_TLF_tests);

    CPPUNIT_TEST_SUITE_END();

    void copyAndDelete(NTriangulation& dest, NTriangulation* source) {
            dest.insertTriangulation(*source);
            delete source; }
    void copyAndDelete(Dim4Triangulation& dest, Dim4Triangulation* source) {
            dest.insertTriangulation(*source);
            delete source; }

    private:
       std::vector< NTriangulation* > t3List;
       std::vector< Dim4Triangulation* > t4List;
       std::vector< NTriangulation* > knotList;
       std::vector< std::string > knotPolyList;
       std::vector< NCellularData* > m3List; // NCellularData of 3-manifolds
       std::vector< NCellularData* > m4List; // NCellularData of 4-manifolds
       std::vector< std::string > polyList;

    public:
        void setUp() {     
          unsigned long numTests3( detailedTests() ? 17 : 6 ); 
          t3List.resize(numTests3); for (unsigned long i=0; i<t3List.size(); i++) t3List[i] = new NTriangulation();          
	  // todo: once we have dim4Triangulation dehydration strings we can add some 4-dimensional triangulations.
          t3List[0]->insertRehydration("cabbbbaei");     t3List[1]->insertRehydration("cabbbbapt");
          t3List[2]->insertRehydration("dagacccfwkn");   t3List[3]->insertRehydration("ebdbcdddaqhie");
          t3List[4]->insertRehydration("eahbcdddhsssj"); t3List[5]->insertRehydration("ebdbcdddddddx");
          if (detailedTests()) {
          t3List[6]->insertRehydration("baaaade");       t3List[7]->insertRehydration("cabbbbabw");
          t3List[8]->insertRehydration("cabbbbcdw");     t3List[9]->insertRehydration("eahdccddakfhq");
          t3List[10]->insertRehydration("ebdbcdddcemre");t3List[11]->insertRehydration("eahbcdddjxxxj");
          copyAndDelete(*t3List[12],     NExampleTriangulation::smallClosedOrblHyperbolic());
          copyAndDelete(*t3List[13],     NExampleTriangulation::smallClosedNonOrblHyperbolic());
          copyAndDelete(*t3List[14],     NExampleTriangulation::weberSeifert());
	  t3List[15]->insertRehydration("gepaadcefeffnkkanax");
	  t3List[16]->insertRehydration("jgofiaaaceedfhiiifkxkfnbtxe"); }

          unsigned long numKnotTests( 5 );
          knotList.resize(numKnotTests); for (unsigned long i=0; i<knotList.size(); i++)
           knotList[i] = new NTriangulation();
          knotPolyList.resize(numKnotTests);
          copyAndDelete(*knotList[0], NTriangulation::fromIsoSig("cPcbbbiht"));  // figure-8 4_1 knot
          knotPolyList[0] = "-1+3t-t^2"; // -t^-1 + 3 - t
          copyAndDelete(*knotList[1], NTriangulation::fromIsoSig("dLQbcccdero")); // 3-twist 5_2 knot
          knotPolyList[1] = "2-3t+2t^2"; // 2t^-1 - 3 + 2t
          copyAndDelete(*knotList[2], NTriangulation::fromIsoSig("eLPkbcddddcwjb")); // Stevedore 6_1
          knotPolyList[2] = "-2+5t-2t^2"; // -2t^-1 + 5 - 2t
          copyAndDelete(*knotList[3], NTriangulation::fromIsoSig("fLLQcbcdeeemgopdp")); // Miller Inst knot 6_2
          knotPolyList[3] = "-1+3t-3t^2+3t^3-t^4"; // -t^-2 + 3t^-1 - 3 + 3t - t^2
          copyAndDelete(*knotList[4], NTriangulation::fromIsoSig("gLLPQccdefffhggaacv")); // 6_3 knot
          knotPolyList[4] = "1-3t+5t^2-3t^3+t^4"; // t^2 - 3t^-1 + 5 - 3t + t^2

          unsigned long numTests4( detailedTests() ? 7 : 4);
          t4List.resize(numTests4); for (unsigned long i=0; i<t4List.size(); i++) t4List[i] = new Dim4Triangulation();
          copyAndDelete(*t4List[0], Dim4Triangulation::fromIsoSig("baa")); // single pentachoron
          copyAndDelete(*t4List[1], Dim4Triangulation::fromIsoSig("cHkbbbRb3asb")); // Poincare dodecahedral space x interval
          copyAndDelete(*t4List[2], Dim4Triangulation::fromIsoSig("cMkabbb+aAa3blb")); // simplest 2-knot 2-pentachoron
          copyAndDelete(*t4List[3], Dim4Triangulation::fromIsoSig("eLMQcaccddcd1aaa2a4aaa1aca"));  // 4-pentachoron knot
          if (detailedTests()) {
          copyAndDelete(*t4List[4], Dim4Triangulation::fromIsoSig("eLAQcbbbdddd0baa0bhahaDaDa"));  // another 
          copyAndDelete(*t4List[5], Dim4Triangulation::fromIsoSig("gLLAQQccddeffeffaayaNaNaPbzb0aPbIaxa")); // 6-pentachoron knot
          copyAndDelete(*t4List[6], Dim4Triangulation::fromIsoSig("eAMMcaabccdd+aoa+aAaqbyaca")); } // CP2

          m3List.resize( t3List.size() ); for (unsigned long i=0; i<m3List.size(); i++) m3List[i]=new NCellularData(*t3List[i]);
          m4List.resize( t4List.size() ); for (unsigned long i=0; i<m4List.size(); i++) m4List[i]=new NCellularData(*t4List[i]);          

	  polyList.resize(17);
	  polyList[0] = "1+t";       polyList[1] = "1+t";        polyList[2] = "1+t";    polyList[3] = "1+t";
 	  polyList[4] = "1+2t+t^2";  polyList[5] = "1+2t+t^2";   polyList[6] = "1+t";    polyList[7] = "1+t";
	  polyList[8] = "1+t";       polyList[9] = "1+t";        polyList[10] = "1+t";   polyList[11] = "1+2t+t^2";
	  polyList[12] = "1+t^3";    polyList[13] = "1+t";       polyList[14] = "1+t^3"; polyList[15] = "1+3t+3t^2+t^3";
          polyList[16] = "1+t^3";
        }

        void tearDown() {
	 for (unsigned long i=0; i<t3List.size(); i++) delete t3List[i];
         for (unsigned long i=0; i<t4List.size(); i++) delete t4List[i];
	 for (unsigned long i=0; i<m3List.size(); i++) delete m3List[i];
         for (unsigned long i=0; i<m4List.size(); i++) delete m4List[i];
         for (unsigned long i=0; i<knotList.size(); i++) delete knotList[i];
	}

	void basic_tests() {  // euler char, poincare poly, cell counts
		for (unsigned long i=0; i<m3List.size(); i++)
		 if (m3List[i]->eulerChar() != 0) CPPUNIT_FAIL("Euler characteristic error.");
		for (unsigned long i=0; i<m3List.size(); i++)
	         if (m3List[i]->poincarePolynomial().toString() != polyList[i]) 
                        CPPUNIT_FAIL("Poincare polynomial error.");
	}

        void absolute_h1_comparisons() {
                for (unsigned long i=0; i<m3List.size(); i++)
                {
                 std::stringstream str1; t3List[i]->getHomologyH1().writeTextShort(str1); 
                        std::string S1( str1.str() );  
                 NCellularData::GroupLocator h1s( 1, NCellularData::coVariant, NCellularData::STD_coord, 0 );
                 std::stringstream str2; m3List[i]->markedGroup( h1s )->writeTextShort(str2); 
                        std::string S2( str2.str() );
                 NCellularData::GroupLocator h2s( 1, NCellularData::coVariant, NCellularData::DUAL_coord, 0 );
                 std::stringstream str3; m3List[i]->markedGroup( h2s )->writeTextShort(str3); 
                        std::string S3( str3.str() );
                 NCellularData::GroupLocator h3s( 1, NCellularData::coVariant, NCellularData::MIX_coord, 0 );
                 std::stringstream str4; m3List[i]->markedGroup( h3s )->writeTextShort(str4); 
                        std::string S4( str4.str() );
                 if ( (S1!=S2) || (S2!=S3) || (S3!=S4) ) 
                        CPPUNIT_FAIL("4-Way H1 comparison failed (3). "+S1+" "+S2+" "+S3+" "+S4);
               }
                for (unsigned long i=0; i<m4List.size(); i++)
                {
                 std::stringstream str1; t4List[i]->getHomologyH1().writeTextShort(str1); std::string S1( str1.str() );  
                 NCellularData::GroupLocator h1s( 1, NCellularData::coVariant, NCellularData::STD_coord, 0 );
                 std::stringstream str2; m4List[i]->markedGroup( h1s )->writeTextShort(str2); std::string S2( str2.str() );
                 NCellularData::GroupLocator h2s( 1, NCellularData::coVariant, NCellularData::DUAL_coord, 0 );
                 std::stringstream str3; m4List[i]->markedGroup( h2s )->writeTextShort(str3); std::string S3( str3.str() );
                 NCellularData::GroupLocator h3s( 1, NCellularData::coVariant, NCellularData::MIX_coord, 0 );
                 std::stringstream str4; m4List[i]->markedGroup( h3s )->writeTextShort(str4); std::string S4( str4.str() );
                 if ( (S1!=S2) || (S2!=S3) || (S3!=S4) ) CPPUNIT_FAIL("4-Way H1 comparison failed (4). "+S1+" "+S2+" "+S3+" "+S4);
                }
        }

        void chain_complex_tests() {	
        	for (unsigned long i=0; i<m3List.size(); i++)
                 if (!m3List[i]->chainComplexesVerified()) CPPUNIT_FAIL("Chain complex error (3).");
         	for (unsigned long i=0; i<m4List.size(); i++)
                 if (!m4List[i]->chainComplexesVerified()) CPPUNIT_FAIL("Chain complex error (4).");
       } 

        void chain_maps_tests() {        	
                for (unsigned long i=0; i<m3List.size(); i++)
                 if (!m3List[i]->chainMapsVerified()) CPPUNIT_FAIL("Chain map error (3).");
                for (unsigned long i=0; i<m4List.size(); i++)
                 if (!m4List[i]->chainMapsVerified()) CPPUNIT_FAIL("Chain map error (4).");
        }

	void coordinate_isomorphisms_tests() { 
		if (! detailedTests()) // this is Ben's magic speed-up flag true iff
			return;        // REGINA_DETAILED_TESTS initialized and non-empty
		for (unsigned long i=0; i<m3List.size(); i++)
		 if (!m3List[i]->coordinateIsomorphismsVerified()) CPPUNIT_FAIL("Coordinate isomorphisms error (3).");
		for (unsigned long i=0; i<m4List.size(); i++)
		 if (!m4List[i]->coordinateIsomorphismsVerified()) CPPUNIT_FAIL("Coordinate isomorphisms error (4).");

	}
	void homology_LES_tests() { 
		for (unsigned long i=0; i<m3List.size(); i++)
		 if (!m3List[i]->homologyLESVerified()) CPPUNIT_FAIL("Homology LES error (3).");
		for (unsigned long i=0; i<m4List.size(); i++)
		 if (!m4List[i]->homologyLESVerified()) CPPUNIT_FAIL("Homology LES error (4).");
	}
	void poincare_duality_tests() { 
		for (unsigned long i=0; i<m3List.size(); i++)
		 if (!m3List[i]->poincareDualityVerified()) CPPUNIT_FAIL("Poincare Duality error (3).");
		for (unsigned long i=0; i<m4List.size(); i++)
		 if (!m4List[i]->poincareDualityVerified()) CPPUNIT_FAIL("Poincare Duality error (4).");
	}
	void intersectionform_tests() {
		for (unsigned long i=0; i<m3List.size(); i++)
		 if (!m3List[i]->intersectionFormsVerified()) CPPUNIT_FAIL("Intersection forms misbehaving (3).");
		for (unsigned long i=0; i<m4List.size(); i++)
		 if (!m4List[i]->intersectionFormsVerified()) CPPUNIT_FAIL("Intersection forms misbehaving (4).");
	}

    void noncyclic_TLF_tests() {
      // TODO: perhaps add 
      NTriangulation* tri1(NTriangulation::fromIsoSig("gLLPQaceedffjkxknnn")); 
      NTriangulation* tri2(NTriangulation::fromIsoSig("nLvAAAvPQkcffggghjlkmmlmaaaaaoskkvxisp")); 
      NCellularData cDat1( *tri1 ); NCellularData cDat2( *tri2 ); 
      if (cDat1.stringInfo(NCellularData::TORFORM_embinfo) != 
        std::string("The torsion linking form is of hyperbolic type.  Manifold is a rational homology sphere.")) 
        CPPUNIT_FAIL("Non-cyclic TLF test fails (1).");
      if (cDat2.stringInfo(NCellularData::TORFORM_embinfo) != 
        std::string("The torsion linking form is of hyperbolic type.")) 
        CPPUNIT_FAIL("Non-cyclic TLF test fails (2).");

      delete tri1; delete tri2;
    }


        void lensspacehomotopyclassification_tests() {
         NLargeInteger maxP(22); 
         // produce some random lens spaces, compute intersection form and 
         //   check it is [\pm r^2 q/p] \in Q/Z
         // p will be anywhere from 2 to 19, q will be chosen to be coprime, 
         //  how? just choose randomly 0 < q < p, 
         // until you find something coprime, I suppose. 
         maxP = maxP - 2;
         bool testpassed(true);
         std::ostringstream msg;
         NTriangulation* lens(NULL);
         const NBilinearForm* tif(NULL);
         for (unsigned long i=0; i<8; i++) // 3 random lens spaces to test.
          { 
           NLargeInteger p( maxP.randomBoundedByThis() + 2 ); // should be at least 2
           NLargeInteger q( p.randomBoundedByThis() );
           while ( q.gcd(p) != 1 ) q = p.randomBoundedByThis();
           lens = new NTriangulation(*NLensSpace(p.longValue(),q.longValue()).construct());
           // we have our random triangulation, lets test it. 
           NCellularData ncd( *lens );
           // TODO: put in variant with STD_coord, STD_coord
           NCellularData::GroupLocator h1L( 1, NCellularData::coVariant, NCellularData::DUAL_coord, 0 );
           NCellularData::FormLocator tifL( NCellularData::torsionlinkingForm, h1L, h1L );
           tif = new NBilinearForm( *ncd.bilinearForm(tifL) );
           // have the form, if it evaluates to a/p we need to make sure a/p = +- r^2 q/p mod 1 
           //  for some r == 1,2,...,p-1
           bool itestpassed(false);
           std::vector<NLargeInteger> idV(1, NLargeInteger::one);
           std::vector<NLargeInteger> eval(tif->evalCC(idV, idV));
           NLargeInteger A(eval[0]); unsigned long P( p.longValue() );
          for (unsigned long r=1; r < P; r++)
            {
             NLargeInteger temp(q); 
             temp *= (r*r); 
             if ( (temp - A) % p == 0 ) itestpassed=true;
             if ( (temp + A) % p == 0 ) itestpassed=true; 
            }
           // if failed, append a message for output
           if (itestpassed==false) { msg << "L("<<p<<","<<q<<") "; testpassed=false; }
           // tidy up.
           delete tif;
           delete lens;
          } // for i loop
         if (!testpassed) CPPUNIT_FAIL("Lens space(s): "+msg.str()+"failed TLF test.");
	} // lensspacehomotopyclassification_tests()

        void alexpoly_tests() {
        for (unsigned long i=0; i<knotList.size(); i++)
         {
         // construct corresponding NCellularData from knotList[i]
         NCellularData knotNCD(*knotList[i]);

         //  TODO: Check maximal tree has right number of cells.  No way to access this yet. 
         const NMatrixRing< NSVPolynomialRing< NLargeInteger > >* cm1(NULL), *cm2(NULL);
         cm1 = knotNCD.alexanderChainComplex( 
                NCellularData::ChainComplexLocator(1, NCellularData::DUAL_coord ) );
         cm2 = knotNCD.alexanderChainComplex( 
                NCellularData::ChainComplexLocator(2, NCellularData::DUAL_coord ) );

         std::auto_ptr< NMatrixRing< NSVPolynomialRing< NLargeInteger > > > prod((*cm1)*(*cm2));
        if (!prod->isZero()) CPPUNIT_FAIL("NCellularData::alexander module chain complex error.");
         //  (3) check a wider variety of alex polys symmetric and evaluate to +1 or -1 at 1. 
        std::auto_ptr< std::list< regina::NSVPolynomialRing< regina::NLargeInteger > > > 
                ideal( knotNCD.alexanderIdeal() );
        if (ideal->size()!=1) CPPUNIT_FAIL("Alexander ideal failed to be principal."); 
        regina::NSVPolynomialRing< regina::NLargeInteger > alexP( ideal->front() );
        // normalize polynomial so that firstAPterm.first = 0
        // done automatically now prettifyPolynomial(alexP);
        // check symmetric, another test.
        if (!alexP.isSymmetric()) CPPUNIT_FAIL("Alexander polynomial of knot in S^3 fails to be symmetric.");

        // normalize so that eval at 1 is non-negative
        // check eval at 1 is 1. 
        regina::NLargeInteger alexPone(alexP.eval(NLargeInteger::one));
        if (!((alexPone!=NLargeInteger::one) || (-alexPone!=NLargeInteger::one)))
                CPPUNIT_FAIL("Alexander polynomial of knot in S^3 fails to evaluate to +1 or -1 at +1.");
         //  (4) check alex polys against what we expect them to be for several cases
        // compare to stored value
        if (alexP.toString() != knotPolyList[i]) 
                CPPUNIT_FAIL("Alexander polynomial of knot fails to agree with pre-computed value.");

         }
        // let's also do one text for the alexander module of a 4-manifold. Eventually more!

         // construct corresponding NCellularData from knotList[i]
         NCellularData knotNCD(*m4List[2]); // this is a Cappell-Shaneson knot

         //  TODO: Check maximal tree has right number of cells.  No way to access this yet. 
         const NMatrixRing< NSVPolynomialRing< NLargeInteger > >* cm1(NULL), *cm2(NULL);
         cm1 = knotNCD.alexanderChainComplex( 
                NCellularData::ChainComplexLocator(1, NCellularData::DUAL_coord ) );
         cm2 = knotNCD.alexanderChainComplex( 
                NCellularData::ChainComplexLocator(2, NCellularData::DUAL_coord ) );

         std::auto_ptr< NMatrixRing< NSVPolynomialRing< NLargeInteger > > > prod((*cm1)*(*cm2));
        if (!prod->isZero()) CPPUNIT_FAIL("NCellularData::alexander module chain complex error for CS 2-knot.");
        // normalize so that eval at 1 is non-negative
        // check eval at 1 is 1. 
        std::auto_ptr< std::list< regina::NSVPolynomialRing< regina::NLargeInteger > > > 
                ideal( knotNCD.alexanderIdeal() );
        regina::NSVPolynomialRing< regina::NLargeInteger > alexP( ideal->front() );
        // normalize polynomial so that firstAPterm.first = 0
        regina::NLargeInteger alexPone(alexP.eval(NLargeInteger::one));
        if (!((alexPone!=NLargeInteger::one) || (-alexPone!=NLargeInteger::one)))
                CPPUNIT_FAIL("Alexander polynomial of knot in S^3 fails to evaluate to +1 or -1 at +1. CS 2-knot.");
         //  (4) check alex polys against what we expect them to be for several cases
        // compare to stored value
        if (alexP.toString() != std::string("-1+t^2+t^3")) 
        {
                CPPUNIT_FAIL("Alexander polynomial of knot fails to agree with pre-computed value. CS 2-knot.");
                std::cout<<alexP.toString()<<" vs "<<std::string("-1+t^2+t^3\n");
                       std::cout.flush();
        }


        } // end alexpoly_tests()

};

void addNCellularData(CppUnit::TextUi::TestRunner& runner) {
    runner.addTest(NCellularDataTest::suite());
}

