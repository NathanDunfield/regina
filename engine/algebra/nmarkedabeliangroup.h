
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2009, Ben Burton                                   *
 *  For further details contact Ben Burton (bab@debian.org).              *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or         *
 *  modify it under the terms of the GNU General Public License as        *
 *  published by the Free Software Foundation; either version 2 of the    *
 *  License, or (at your option) any later version.                       *
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

/*! \file algebra/nmarkedabeliangroup.h
 *  \brief Deals with abelian groups given by chain complexes.
 */

#ifndef __NMARKEDABELIANGROUP_H
#ifndef __DOXYGEN
#define __NMARKEDABELIANGROUP_H
#endif

#include <vector>
#include "maths/nmatrixint.h"

namespace regina {

/**
 * \weakgroup algebra
 * @{
 */

/**
 * Represents a finitely generated abelian group given by a chain complex.
 *
 * This class is initialized with a chain complex.  The chain complex is given
 * in terms of two integer matrices \a M and \a N such that M*N=0. The abelian
 * group is the kernel of \a M mod the image of \a N.
 *
 * In other words, we are computing the homology of the chain complex
 * <tt>Z^a --N--> Z^b --M--> Z^c</tt>
 * where a=N.columns(), M.columns()=b=N.rows(), and c=M.rows(). 
 *
 * This class allows one to retrieve the invariant factors, the rank, and
 * the corresponding vectors in the kernel of \a M.  Moreover, given a
 * vector in the kernel of \a M, it decribes the homology class of the
 * vector (the free part, and its position in the invariant factors).
 *
 * The purpose of this class is to allow one to not only
 * represent homology groups, but it gives coordinates on the group allowing
 * for the construction of homomorphisms, and keeping track of subgroups.
 *
 * Some routines in this class refer to the internal <i>presentation
 * matrix</i>.  This is a proper presentation matrix for the abelian group,
 * and is created by constructing the product getMRBi() * \a N, and then
 * removing the first getRankM() rows.
 *
 * @author Ryan Budney
 *
 * \todo \optlong Look at using sparse matrices for storage of SNF and
 * the like.
 * \todo Add Z_p coefficients. This should be pretty straightforeward using 
 *    reducedKernelLattice, then tossing in extra relators.  Store mod-p computations 
 *    as some kind of vector or list.
 *    Have all routines adapt -- like getRank(), writeTextShort() have default p=0 
 *    arguments so that the implementations do not change and it's always assumed you
 *    want answers with Z coefficients unless otherwise specified. Could even do a p==-1
 *    for rational coefficients, if interested.  
 */
class NMarkedAbelianGroup : public ShareableObject {
    private:
        /** Internal original M */
        NMatrixInt OM; // copy of initializing M
        /** Internal original N */
        NMatrixInt ON; // copy of initializing N assumes M*N == 0
        /** Internal change of basis. OMC*OM*OMR is the SNF of OM */
        NMatrixInt OMR;  
        /** Internal change of basis. OMC*OM*OMR is the SNF of OM */
        NMatrixInt OMC;
        /** Internal change of basis. OM = OMCi*SNF(OM)*OMRi */
        NMatrixInt OMRi;
        /** Internal change of basis. OM = OMCi*SNF(OM)*OMRi */
        NMatrixInt OMCi;
        /** Internal rank of M */
        unsigned long rankOM; // this is the index of the first zero entry
                              // in the SNF of OM.

        /* Internal reduced N matrix: */
        // In the notes below, ORN refers to the internal presentation
        // matrix [OMRi * ON], where the brackets indicate removal of the
        // first rankOM rows.

        /** Internal change of basis. ornC * ORN * ornR is the SNF(ORN). */
        NMatrixInt ornR;
        /** Internal change of basis. ornRi is the inverse to ornR. */
        NMatrixInt ornRi; // 
        /** Internal change of basis. ornC * ORN * ornR is the SNF(ORN). */
        NMatrixInt ornC;
        /** Internal change of basis. ornCi is the inverse to ornC. */
        NMatrixInt ornCi;

        /** Internal list of invariant factors. */
        std::vector<NLargeInteger> InvFacList;
        /** The number of free generators, from SNF(ORN) */
        unsigned long snfrank;
        /** The row index of the first free generator in SNF(ORN). */
        unsigned long snffreeindex;
        /** Number of invariant factors. */
        unsigned long ifNum;
        /** Row index of invariant factors in SNF(ORN) */
        unsigned long ifLoc;


    public:

        /**
         * Creates a marked abelian group from a chain complex.
         * See the class notes for details.
         *
         * \pre M.columns() = N.rows().
         * \pre The product M*N = 0.
         *
         * @param M the `right' matrix in chain complex
         * @param N `left' matrix in chain complex
         */
        NMarkedAbelianGroup(const NMatrixInt& M, const NMatrixInt& N);

	/**
	 * Creates a free Z_p-module of a given rank using the a direct sum 
	 *  of the standard chain complex 0 --> Z --p--> Z --> 0.  So this group
	 *  is isomorphic to n Z_p, moreover if constructed using the previous
	 *  constructor, M would be zero and N would be diagonal, square with p
	 *  down the diagonal. 
	 *
	 * @param rk is the rank of the group as a Z_p-module, ie if the group is n Z_p, 
	 *        rk is n. 
	 * @param p describes the type of ring that you're using to talk about `free' module. 
	 */
	NMarkedAbelianGroup(const unsigned long &rk, const NLargeInteger &p);

        /**
         * Creates a clone of the given group.
         *
         * @param cloneMe the group to clone.
         */
        NMarkedAbelianGroup(const NMarkedAbelianGroup& cloneMe);

        /**
         * Destroys the group.
         */
        virtual ~NMarkedAbelianGroup();

        /**
         * Returns the rank of the group.
         * This is the number of included copies of <i>Z</i>.
         *
         * @return the rank of the group.
         */
        unsigned getRank() const;

        /**
         * Returns the rank in the group of the torsion term of given degree.
         * If the given degree is <i>d</i>, this routine will return the
         * largest <i>m</i> for which <i>m</i>Z_<i>d</i> is a subgroup
         * of this group.
         *
         * For instance, if this group is Z_6+Z_12, the torsion term of
         * degree 2 has rank 2 (one occurrence in Z_6 and one in Z_12),
         * and the torsion term of degree 4 has rank 1 (one occurrence
         * in Z_12).
         *
         * \pre The given degree is at least 2.
         *
         * @param degree the degree of the torsion term to query.
         * @return the rank in the group of the given torsion term.
         */
        unsigned getTorsionRank(const NLargeInteger& degree) const;

        /**
         * Returns the rank in the group of the torsion term of given degree.
         * If the given degree is <i>d</i>, this routine will return the
         * largest <i>m</i> for which <i>m</i>Z_<i>d</i> is a subgroup
         * of this group.
         *
         * For instance, if this group is Z_6+Z_12, the torsion term of
         * degree 2 has rank 2 (one occurrence in Z_6 and one in Z_12),
         * and the torsion term of degree 4 has rank 1 (one occurrence
         * in Z_12).
         *
         * \pre The given degree is at least 2.
         *
         * @param degree the degree of the torsion term to query.
         * @return the rank in the group of the given torsion term.
         */
        unsigned getTorsionRank(unsigned long degree) const;

        /**
         * Returns the number of invariant factors that describe the
         * torsion elements of this group.  This is the minimal number
         * of torsion generators.
         * See the NMarkedAbelianGroup class notes for further details.
         *
         * @return the number of invariant factors.
         */
        unsigned long getNumberOfInvariantFactors() const;

        /**
         * Returns the given invariant factor describing the torsion
         * elements of this group.
         * See the NMarkedAbelianGroup class notes for further details.
         *
         * If the invariant factors are <i>d0</i>|<i>d1</i>|...|<i>dn</i>,
         * this routine will return <i>di</i> where <i>i</i> is the
         * value of parameter \a index.
         *
         * @param index the index of the invariant factor to return;
         * this must be between 0 and getNumberOfInvariantFactors()-1
         * inclusive.
         * @return the requested invariant factor.
         */
        const NLargeInteger& getInvariantFactor(unsigned long index) const;

        /**
         * Determines whether this is the trivial (zero) group.
         *
         * @return \c true if and only if this is the trivial group.
         */
        bool isTrivial() const;

        /**
         * Determines whether this and the given abelian group are
         * isomorphic.
         *
         * @param the group with which this should be compared.
         * @return \c true if and only if the two groups are isomorphic.
	 * 
         */
	bool isIsomorphicTo(const NMarkedAbelianGroup &other) const;

	/**
	 * Determines whether or not the two NMarkedAbelianGroups are identical, meaning
	 * having exactly the same presentation matrices -- this is useful for determinging
	 * if two NHomMarkedAbelianGroups are composable.  See isIsomorphicTo if all you 
	 * care about is the isomorphism relation among the group defined by the presentation
	 * matrices.
	 *
	 * @param the NMarkedAbelianGroup with which this should be compared.
	 * @return \c true if and only if the two groups have identical chain-complex definitions.
	 */
        bool operator == (const NMarkedAbelianGroup& other) const;

        /**
         * The text representation will be of the form
         * <tt>3 Z + 4 Z_2 + Z_120</tt>.
         * The torsion elements will be written in terms of the
         * invariant factors of the group, as described in the
         * NMarkedAbelianGroup notes.
         *
         * @param out the stream to write to.
         */
        virtual void writeTextShort(std::ostream& out) const;

        /**
         * Returns the requested free generator in the original chain
	 * complex defining the group.
         *
         * As described in the class overview, this marked abelian group
         * is defined by matrices \a M and \a N where M*N = 0.
         * If \a M is an \a m by \a l matrix and \a N is an \a l by \a n
         * matrix, then this routine returns the (\a index)th free
         * generator of ker(M)/img(N) in \a Z^l.
         *
         * \ifacespython The return value will be a python list.
         *
         * @param index specifies which free generator to look up;
         * this must be between 0 and getRank()-1 inclusive.
         * @return the coordinates of the free generator in the nullspace of
         * \a M; this vector will have length M.columns() (or
         * equivalently, N.rows()).
         */
        std::vector<NLargeInteger> getFreeRep(unsigned long index) const;

        /**
         * Returns the requested generator of the torsion subgroup but represented
         * in the original chain complex defining the group.
         *
         * As described in the class overview, this marked abelian group
         * is defined by matrices \a M and \a N where M*N = 0.
         * If \a M is an \a m by \a l matrix and \a N is an \a l by \a n
         * matrix, then this routine returns the (\a index)th torsion
         * generator of ker(M)/img(N) in \a Z^l.
         *
         * \ifacespython The return value will be a python list.
         *
         * @param index specifies which generator in the torsion subgroup;
         * this must be at least 0 and strictly less than the number of
         * non-trivial invariant factors.
         * @return the coordinates of the generator in the nullspace of
         * \a M; this vector will have length M.columns() (or
         * equivalently, N.rows()).
         */
        std::vector<NLargeInteger> getTorsionRep(unsigned long index) const;

	/**
	 * A combination of getFreeRep and getTorsion rep, this routine takes
         * a vector which represents an element in the group in the SNF coordinates
 	 * and returns a corresponding vector in the original chain complex. 
	 *
         * As described in the class overview, this marked abelian group
         * is defined by matrices \a M and \a N where M*N = 0.
         * If \a M is an \a m by \a l matrix and \a N is an \a l by \a n
         * matrix, then this routine returns the (\a index)th free
         * generator of ker(M)/img(N) in \a Z^l. This routine is the inverse
	 * to getSNFIsoRep() described below.
	 */
	std::vector<NLargeInteger> getCCRep(const std::vector<NLargeInteger> SNFRep) const;

	/**
	 * Given a vector, determines if it represents a cycle in chain complex.
	 */
	bool isCycle(const std::vector<NLargeInteger> &input) const;
	
	/**
	 * Returns the rank of the chain complex supporting the homology computation. 
         * This is also obtainable via getOM().columns() and getON().rows() but might
         * as well have something clear for users. 
	 */
	unsigned long getRankCC() const;

        /**
         * Expresses the given vector as a combination of free and torsion
         * generators.
         *
         * Recall that this marked abelian was defined by matrices \a M
         * and \a N with M*N=0; suppose that \a M is an \a m by \a l matrix
         * and \a N is an \a l by \a n matrix.  This abelian group is then
         * the quotient ker(M)/img(N) in \a Z^l.
         *
         * When it is constructed, this group is computed to be isomorphic to
         * some Z_{d0} + ... + Z_{dk} + Z^d, where:
         *
         * - \a d is the number of free generators, as returned by getRank();
         * - \a d1, ..., \a dk are the invariant factors that describe the
         *   torsion elements of the group, where
         *   1 < \a d1 | \a d2 | ... | \a dk.
         *
         * This routine takes a single argument \a v, which must be a
         * vector in \a Z^l.
         *
         * If \a v belongs to ker(M), this routine describes how it
         * projects onto the group ker(M)/img(N).  Specifically, it
         * returns a vector of length \a d + \a k, where:
         *
         * - the first \a k elements describe the projection of \a v
         *   to the torsion component Z_{d1} + ... + Z_{dk}.  These
         *   elements are returned as non-negative integers modulo
         *   \a d1, ..., \a dk respectively.
         * - the remaining \a d elements describe the projection of \a v
         *   to the free component \a Z^d;
         *
         * In other words, suppose \a v belongs to ker(M) and getSNFIsoRep(v)
         * returns the vector (\a b1, ..., \a bk, \a a1, ..., \a ad).
         * Suppose furthermore that the free generators returned
         * by getFreeRep(0..(d-1)) are \a f1, ..., \a fd respectively, and
         * that the torsion generators returned by getTorsionRep(0..(k-1))
         * are \a t1, ..., \a tk respectively.  Then
         * \a v = \a b1.t1 + ... + \a bk.tk + \a a1.f1 + ... + \a ad.fd
         * modulo img(N).
         *
         * If \a v does not belong to ker(M), this routine simply returns
         * the empty vector.
         *
         * \pre Vector \a v has length M.columns(), or equivalently N.rows().
         *
         * \ifacespython Both \a v and the return value are python lists.
         *
         * @param v a vector of length M.columns().
         * @return a vector that describes \a v in the standard
         * Z_{d1} + ... + Z_{dk} + Z^d form, or the empty vector if
         * \a v is not in the kernel of \a M.
	 *
         */
        std::vector<NLargeInteger> getSNFIsoRep(
            const std::vector<NLargeInteger>& v) const;

        /**
         * Returns a change-of-basis matrix for the Smith normal form of \a M.
         *
         * This is one of several routines that returns information on
         * how we determine the isomorphism-class of this group.
         *
         * Recall from the class overview that this marked abelian group
         * is defined by matrices \a M and \a N, where M*N = 0.
         *
         * - getMCB() * M * getMRB() is the Smith normal form of \a M;
         * - getMCBi() and getMRBi() are the inverses of getMCB() and getMRB()
         *   respectively.
         *
         * @return the matrix getMRB() as described above.
         */
        const NMatrixInt& getMRB() const;
        /**
         * Returns an inverse change-of-basis matrix for the Smith normal
         * form of \a M.
         *
         * This is one of several routines that returns information on
         * how we determine the isomorphism-class of this group.
         *
         * Recall from the class overview that this marked abelian group
         * is defined by matrices \a M and \a N, where M*N = 0.
         *
         * - getMCB() * M * getMRB() is the Smith normal form of \a M;
         * - getMCBi() and getMRBi() are the inverses of getMCB() and getMRB()
         *   respectively.
         *
         * @return the matrix getMRBi() as described above.
         */
        const NMatrixInt& getMRBi() const;
        /**
         * Returns a change-of-basis matrix for the Smith normal form of \a M.
         *
         * This is one of several routines that returns information on
         * how we determine the isomorphism-class of this group.
         *
         * Recall from the class overview that this marked abelian group
         * is defined by matrices \a M and \a N, where M*N = 0.
         *
         * - getMCB() * M * getMRB() is the Smith normal form of \a M;
         * - getMCBi() and getMRBi() are the inverses of getMCB() and getMRB()
         *   respectively.
         *
         * @return the matrix getMCB() as described above.
         */
        const NMatrixInt& getMCB() const;
        /**
         * Returns an inverse change-of-basis matrix for the Smith normal
         * form of \a M.
         *
         * This is one of several routines that returns information on
         * how we determine the isomorphism-class of this group.
         *
         * Recall from the class overview that this marked abelian group
         * is defined by matrices \a M and \a N, where M*N = 0.
         *
         * - getMCB() * M * getMRB() is the Smith normal form of \a M;
         * - getMCBi() and getMRBi() are the inverses of getMCB() and getMRB()
         *   respectively.
         *
         * @return the matrix getMCBi() as described above.
         */
        const NMatrixInt& getMCBi() const;

        /**
         * Returns a change-of-basis matrix for the Smith normal form of
         * the internal presentation matrix.
         *
         * This is one of several routines that returns information on
         * how we determine the isomorphism-class of this group.
         *
         * For details on the internal presentation matrix, see the class
         * overview.  If \a P is the internal presentation matrix, then:
         *
         * - getNCB() * P * getNRB() is the Smith normal form of \a P;
         * - getNCBi() and getNRBi() are the inverses of getNCB() and getNRB()
         *   respectively.
         *
         * @return the matrix getNRB() as described above.
         */
        const NMatrixInt& getNRB() const;

        /**
         * Returns an inverse change-of-basis matrix for the Smith normal
         * form of the internal presentation matrix.
         *
         * This is one of several routines that returns information on
         * how we determine the isomorphism-class of this group.
         *
         * For details on the internal presentation matrix, see the class
         * overview.  If \a P is the internal presentation matrix, then:
         *
         * - getNCB() * P * getNRB() is the Smith normal form of \a P;
         * - getNCBi() and getNRBi() are the inverses of getNCB() and getNRB()
         *   respectively.
         *
         * @return the matrix getNRBi() as described above.
         */
        const NMatrixInt& getNRBi() const;

        /**
         * Returns a change-of-basis matrix for the Smith normal form of
         * the internal presentation matrix.
         *
         * This is one of several routines that returns information on
         * how we determine the isomorphism-class of this group.
         *
         * For details on the internal presentation matrix, see the class
         * overview.  If \a P is the internal presentation matrix, then:
         *
         * - getNCB() * P * getNRB() is the Smith normal form of \a P;
         * - getNCBi() and getNRBi() are the inverses of getNCB() and getNRB()
         *   respectively.
         *
         * @return the matrix getNCB() as described above.
         */
        const NMatrixInt& getNCB() const;

        /**
         * Returns an inverse change-of-basis matrix for the Smith normal
         * form of the internal presentation matrix.
         *
         * This is one of several routines that returns information on
         * how we determine the isomorphism-class of this group.
         *
         * For details on the internal presentation matrix, see the class
         * overview.  If \a P is the internal presentation matrix, then:
         *
         * - getNCB() * P * getNRB() is the Smith normal form of \a P;
         * - getNCBi() and getNRBi() are the inverses of getNCB() and getNRB()
         *   respectively.
         *
         * @return the matrix getNCBi() as described above.
         */
        const NMatrixInt& getNCBi() const;

        /**
         * Returns the rank of the defining matrix \a M.
         *
         * The matrix \a M is the `right' matrix used in defining the chain
         * complex.  See the class overview for further details.
         *
         * @return the rank of the defining matrix \a M.
         */
        unsigned long getRankM() const;

        /**
         * Returns the index of the first free generator in the Smith
         * normal form of the internal presentation matrix.  See the class
         * overview for details.
         *
         * @return the index of the first free generator.
         */
        unsigned long getFreeLoc() const; // internal: snffreeindex

        /**
         * Returns the index of the first torsion generator in the Smith
         * normal form of the internal presentation matrix.  See the class
         * overview for details.
         *
         * @return the index of the first torsion generator.
         */
        unsigned long getTorsionLoc() const; // internal: ifLoc

        /**
         * Returns the `right' matrix used in defining the chain complex.
         * Our group was defined as the kernel of \a M mod the image of \a N.  This
	 * is that matrix, \a M. 
         *
         * This is a copy of the matrix \a M that was originally passed to the
         * class constructor.  See the class overview for further details on
         * matrices \a M and \a N and their roles in defining the chain complex.
         *
         * @return a reference to the defining matrix M.
         */
        const NMatrixInt& getM() const;
        /**
         * Returns the `left' matrix used in defining the chain complex.
	 * Our group was defined as the kernel of \a M mod the image of \a N. 
	 * this is the matrix \a N. 
         *
         * This is a copy of the matrix \a N that was originally passed to the
         * class constructor.  See the class overview for further details on
         * matrices \a M and \a N and their roles in defining the chain complex.
         *
         * @return a reference to the defining matrix N.
         */
        const NMatrixInt& getN() const;
};

/**
 * Represents a homomorphism of finitely generated abelian groups.
 *
 * One initializes such a homomorphism by providing:
 *
 * - two finitely generated abelian groups, which act as domain and range;
 * - a matrix describing the linear map between the free abelian
 *   groups in the centres of the respective chain complexes that were
 *   used to define the domain and range.
 *
 * So for example, if the domain was initialized by the chain complex
 * <tt>Z^a --A--> Z^b --B--> Z^c</tt> and the range was initialized by
 * <tt>Z^d --D--> Z^e --E--> Z^f</tt>, then the matrix needs to be an
 * e-by-b matrix.  Furthermore, you only obtain a well-defined
 * homomorphism if this matrix extends to a chain map (which this class
 * assumes).
 *
 * \todo preImageOf in CC and SNF coordinates.  This routine would return a generating
 *          list of elements in the preimage, thought of as an affine subspace. Or maybe
 *          just one element together with the kernel inclusion.  IMO smarter to be a list
 *          because that way there's a more pleasant way to make it empty. Or could have a 
 *          variety of routines among these themes.  Store some minimal data for efficient
 *          computations of preImage -- eventually replacing the internals of inverseHom() 
 *          with a more flexible set of tools.
 * \todo isInImage in various coordinates -- essentially a wrapper for preimage of.
 *          Or could compute this by taking the image of the element into the cokernel and
 *          asking if this is zero. 
 * \todo add routine to check all the various chain complexes we define are really chain complexes, 
 *          and the maps between the chain complexes that induce maps on homology are actually chain
 *          maps.  Add to test suite. isCycle() is first step in this.
 * \todo add induced homomorphism on Z_p coefficients once Z_p coefficients implemented in 
 *       NMarkedAbelianGroup.
 *
 * @author Ryan Budney
 */
class NHomMarkedAbelianGroup : public ShareableObject {
    private:
        /** internal rep of domain of the homomorphism */
        NMarkedAbelianGroup domain;
        /** internal rep of range of the homomorphism */
        NMarkedAbelianGroup range;
        /** matrix describing map from domain to range, in the coordinates
            of the chain complexes used to construct domain and range, see
	    above description */
        NMatrixInt matrix;

        /** short description of matrix in SNF coordinates -- this means we've
	    conjugated matrix by the relevant change-of-basis maps in both the
	    domain and range so that we are using the coordinates of Smith Normal
	    form.  We also truncate off the trivial Z/Z factors so that reducedMatrix
	    will not have the same dimensions as matrix. This means the torsion factors
	    appear first, followed by the free factors. */
        NMatrixInt* reducedMatrix;
        /** pointer to kernel of map */
        NMarkedAbelianGroup* kernel;
        /** pointer to coKernel of map */
        NMarkedAbelianGroup* coKernel;
        /** pointer to image */
        NMarkedAbelianGroup* image;
        /** pointer to a lattice which describes the kernel of the
            homomorphism. */
        NMatrixInt* reducedKernelLattice;

        /** compute the ReducedKernelLattice */
        void computeReducedKernelLattice();
        /** compute the ReducedMatrix */
        void computeReducedMatrix();
        /** compute the Kernel */
        void computeKernel();
        /** compute the Cokernel */
        void computeCokernel();
        /** compute the Image */
        void computeImage();

    public:

        /**
         * Constructs a homomorphism from two marked abelian groups and
         * a matrix that indicates where the generators are sent.
         * The roles of the two groups and the matrix are described in
         * detail in the NHomMarkedAbelianGroup class overview.
         *
         * The matrix must be given in the chain-complex coordinates.
         * Specifically, if \a domain was defined via the chain complex
         * <tt>Z^a --N1--> Z^b --M1--> Z^c</tt> and \a range was
         * defined via <tt>Z^d --N2--> Z^e --M2--> Z^f</tt>, then \a mat is
         * an e-by-b matrix that describes a homomorphism from Z^b to Z^e.
         *
         * In order for this to make sense as a homomorphism of the groups
         * represented by \a domain and \a range respectively, one requires
         * img(mat*N1) to be a subset of img(N2).  Similarly, ker(M1) must
         * be sent into ker(M2).  These facts are not checked, but are
         * assumed as preconditions of this constructor.
         *
         * \pre The matrix \a mat has the required dimensions e-by-b,
         * gives img(mat*N1) as a subset of img(N2), and sends ker(M1)
         * into ker(M2), as explained in the detailed notes above.
         *
         * @param domain the domain group.
         * @param range the range group.
         * @param mat the matrix that describes the homomorphism from 
         * \a domain to \a range.
         */
        NHomMarkedAbelianGroup(const NMarkedAbelianGroup& dom,
                const NMarkedAbelianGroup& ran,
                const NMatrixInt &mat);

	/**
	 * For those situations where you want to define an NHomMarkedAbelianGroup
	 * from its reduced matrix, not from a chain map.  This is in the situation where
         * the SNF coordinates have particular meaning to the user.  At present I only use this
         * for NHomMarkedAbelianGroup::inverseHom() 
	 */
	NHomMarkedAbelianGroup(const NMatrixInt &tobeRedMat, 
			       const NMarkedAbelianGroup &dom, 
			       const NMarkedAbelianGroup &ran);
 
        /**
         * Copy constructor.
         *
         * @param h the homomorphism to clone.
         */
        NHomMarkedAbelianGroup(const NHomMarkedAbelianGroup& h);
        /**
         * Destructor.
         */
        virtual ~NHomMarkedAbelianGroup();

	/**
	 * Is this at least a chain map?  If not, pretty much any further computations you try with
	 * this class will be give you nothing more than carefully-crafted garbage.
	 */
	bool isChainMap() const;

        /**
         * Is this an epic homomorphism?
         *
         * @return true if this homomorphism is epic.
         */
        bool isEpic() const;
        /**
         * Is this a monic homomorphism?
         *
         * @return true if this homomorphism is monic.
         */
        bool isMonic() const;
        /**
         * Is this an isomorphism?
         *
         * @return true if this homomorphism is an isomorphism.
         */
        bool isIso() const;
        /**
         * Is this the zero map?
         *
         * @return true if this homomorphism is the zero map.
         */
        bool isZero() const;
	/**
	 * Is this the identity automorphism?  
	 *
	 * @return true if and only if the domain and range are defined via the same chain
	 *         complexes and the induced map on homology is the identity.
	 */
	bool isIdentity() const;

        /**
         * Returns the kernel of this homomorphism.
         *
         * @return the kernel of the homomorphism, as a marked abelian group.
         */
        const NMarkedAbelianGroup& getKernel() const;
        /**
         * Returns the cokernel of this homomorphism.
         *
         * @return the cokernel of the homomorphism, as a marked abelian group.
         */
        const NMarkedAbelianGroup& getCokernel() const;
        /**
         * Returns the image of this homomorphism.
         *
         * @return the image of the homomorphism, as a marked abelian group.
         */
        const NMarkedAbelianGroup& getImage() const;

        /**
         * Short text representation.  This will state some basic
         * properties of the homomorphism, such as:
         *
         * - whether the map is an isomorphism;
         * - whether the map is monic or epic;
         * - if it is not monic, describes the kernel;
         * - if it is not epic, describes the co-kernel;
         * - if it is neither monic nor epic, describes the image.
         *
         * @param out the stream to write to.
         */
        virtual void writeTextShort(std::ostream& out) const;

        /**
         * Returns the domain of this homomorphism.
         *
         * @return the domain that was used to define the homomorphism.
         */
        const NMarkedAbelianGroup& getDomain() const;
        /**
         * Returns the range of this homomorphism.
         *
         * @return the range that was used to define the homomorphism.
         */
        const NMarkedAbelianGroup& getRange() const;
        /**
         * Returns the defining matrix for the homomorphism.
         *
         * @return the matrix that was used to define the homomorphism.
         */
        const NMatrixInt& getDefiningMatrix() const;

        /**
         * Returns the internal reduced matrix representing the
         * homomorphism. This is where the rows/columns of the matrix represent
         * first the free generators, then the torsion summands in the order
         * of the invariant factors:
	 *
         *             Z^d + Z_{d0} + ... + Z_{dk}
	 * where:
         *
         * - \a d is the number of free generators, as returned by getRank();
         * - \a d1, ..., \a dk are the invariant factors that describe the
         *   torsion elements of the group, where
         *   1 < \a d1 | \a d2 | ... | \a dk.
         *
         * @return a copy of the internal representation of the homomorphism.
         */
        const NMatrixInt& getReducedMatrix() const;

	/**
	 * Evaluate, in the original chain complex's coordinates. This is multiplication
	 * by the defining matrix, returning the empty vector if the input is not a cycle. 
	 */
	std::vector<NLargeInteger> evalCC(const std::vector<NLargeInteger> &input) const; 
	/**
	 * Evaluate, in the SNF coordinates. This is just multiplication by the reduced matrix, 
	 * returning the empty vector if the input vector has the wrong dimensions.
	 */
	std::vector<NLargeInteger> evalSNF(const std::vector<NLargeInteger> &input) const;

	/**
	 * Returns the inverse to an NHomMarkedAbelianGroup. If not invertible, returns
	 * the zero homomorphism.
	 *
	 * TODO: in future, store the inverse and have this routine return a constant refernce.
	 *
	 * @pre Must be invertible. 
         */
        NHomMarkedAbelianGroup inverseHom() const;

	/**
	 * Composite of NHomMarkedAbelianGroups
	 *
	 * @pre the homomorphisms must be composable, meaning the X.range
	 *      must have the same presentation matrices as the this->domain 
	 */
	NHomMarkedAbelianGroup operator * (const NHomMarkedAbelianGroup &X) const;

        /**
         * Writes a human-readable version of the reduced matrix to the
         * given output stream.  This is a description of the homomorphism
         * in some specific coordinates at present only meant to be
         * internal to NHomMarkedAbelianGroup.  At present, these coordinates 
	 * have the torsion factors of the group appearing first, followed by the
	 * free factors.
         *
         * \ifacespython The \a out argument is missing; instead this is
         * assumed to be standard output.
         *
         * @param out the output stream.
         */
        void writeReducedMatrix(std::ostream& out) const;
};

/*@}*/

// Inline functions for NMarkedAbelianGroup

// copy constructor
inline NMarkedAbelianGroup::NMarkedAbelianGroup(const NMarkedAbelianGroup& g) :
        ShareableObject(),
        OM(g.OM), ON(g.ON), OMR(g.OMR), OMC(g.OMC), OMRi(g.OMRi), OMCi(g.OMCi),
        rankOM(g.rankOM),
        ornR(g.ornR), ornRi(g.ornRi), ornC(g.ornC), ornCi(g.ornCi),
        InvFacList(g.InvFacList), snfrank(g.snfrank),
        snffreeindex(g.snffreeindex),
        ifNum(g.ifNum), ifLoc(g.ifLoc) {
}

// destructor
inline NMarkedAbelianGroup::~NMarkedAbelianGroup() {}

inline unsigned NMarkedAbelianGroup::getTorsionRank(unsigned long degree)
        const {
    return getTorsionRank(NLargeInteger(degree));
}

inline unsigned long NMarkedAbelianGroup::getNumberOfInvariantFactors() const {
    return InvFacList.size();
}

inline const NLargeInteger& NMarkedAbelianGroup::getInvariantFactor(
        unsigned long index) const {
    return InvFacList[index];
}

inline unsigned NMarkedAbelianGroup::getRank() const {
    return snfrank;
}

inline unsigned long NMarkedAbelianGroup::getRankCC() const {
	return OM.columns();
}


inline bool NMarkedAbelianGroup::isTrivial() const {
    return ! ( (snfrank>0) || (InvFacList.size()>0) );
}

inline bool NMarkedAbelianGroup::operator == (
        const NMarkedAbelianGroup& other) const {
    return ( (OM == other.OM) && (ON == other.ON) );
}

inline bool NMarkedAbelianGroup::isIsomorphicTo(const NMarkedAbelianGroup &other) const {
    return ((InvFacList == other.InvFacList) && (snfrank == other.snfrank));
}


inline const NMatrixInt& NMarkedAbelianGroup::getMRB() const {
    return OMR;
}
inline const NMatrixInt& NMarkedAbelianGroup::getMRBi() const {
    return OMRi;
}
inline const NMatrixInt& NMarkedAbelianGroup::getMCB() const {
    return OMC;
}
inline const NMatrixInt& NMarkedAbelianGroup::getMCBi() const {
    return OMCi;
}
inline const NMatrixInt& NMarkedAbelianGroup::getNRB() const {
    return ornR;
}
inline const NMatrixInt& NMarkedAbelianGroup::getNRBi() const {
    return ornRi;
}
inline const NMatrixInt& NMarkedAbelianGroup::getNCB() const {
    return ornC;
}
inline const NMatrixInt& NMarkedAbelianGroup::getNCBi() const {
    return ornCi;
}

inline unsigned long NMarkedAbelianGroup::getRankM() const {
    return rankOM;
}
inline unsigned long NMarkedAbelianGroup::getFreeLoc() const {
    return snffreeindex;
}
inline unsigned long NMarkedAbelianGroup::getTorsionLoc() const {
    return ifLoc;
}

inline const NMatrixInt& NMarkedAbelianGroup::getM() const {
    return OM;
}
inline const NMatrixInt& NMarkedAbelianGroup::getN() const {
    return ON;
}

// Inline functions for NHomMarkedAbelianGroup

inline NHomMarkedAbelianGroup::NHomMarkedAbelianGroup(
        const NMarkedAbelianGroup& dom,
        const NMarkedAbelianGroup& ran,
        const NMatrixInt &mat) :
        domain(dom), range(ran), matrix(mat),
        reducedMatrix(0), kernel(0), coKernel(0), image(0),
        reducedKernelLattice(0) {
}

inline NHomMarkedAbelianGroup::~NHomMarkedAbelianGroup() {
    if (reducedMatrix)
        delete reducedMatrix;
    if (kernel)
        delete kernel;
    if (coKernel)
        delete coKernel;
    if (image)
        delete image;
    if (reducedKernelLattice)
        delete reducedKernelLattice;
}

inline const NMarkedAbelianGroup& NHomMarkedAbelianGroup::getDomain() const {
    return domain;
}
inline const NMarkedAbelianGroup& NHomMarkedAbelianGroup::getRange() const {
    return range;
}
inline const NMatrixInt& NHomMarkedAbelianGroup::getDefiningMatrix() const {
    return matrix;
}

inline const NMatrixInt& NHomMarkedAbelianGroup::getReducedMatrix() const {
    // Cast away const to compute the reduced matrix -- the only reason we're
    // changing data members now is because we delayed calculations
    // until they were really required.
    const_cast<NHomMarkedAbelianGroup*>(this)->computeReducedMatrix();
    return *reducedMatrix;
}

inline bool NHomMarkedAbelianGroup::isEpic() const {
    return getCokernel().isTrivial();
}

inline bool NHomMarkedAbelianGroup::isMonic() const {
    return getKernel().isTrivial();
}

inline bool NHomMarkedAbelianGroup::isIso() const {
    return (getCokernel().isTrivial() && getKernel().isTrivial());
}

inline bool NHomMarkedAbelianGroup::isZero() const {
    return getImage().isTrivial();
}

inline const NMarkedAbelianGroup& NHomMarkedAbelianGroup::getKernel() const {
    // Cast away const to compute the kernel -- the only reason we're
    // changing data members now is because we delayed calculations
    // until they were really required.
    const_cast<NHomMarkedAbelianGroup*>(this)->computeKernel();
    return *kernel;
}

inline const NMarkedAbelianGroup& NHomMarkedAbelianGroup::getImage() const {
    // Cast away const to compute the kernel -- the only reason we're
    // changing data members now is because we delayed calculations
    // until they were really required.
    const_cast<NHomMarkedAbelianGroup*>(this)->computeImage();
    return *image;
}

inline const NMarkedAbelianGroup& NHomMarkedAbelianGroup::getCokernel() const {
    // Cast away const to compute the kernel -- the only reason we're
    // changing data members now is because we delayed calculations
    // until they were really required.
    const_cast<NHomMarkedAbelianGroup*>(this)->computeCokernel();
    return *coKernel;
}

} // namespace regina

#endif

