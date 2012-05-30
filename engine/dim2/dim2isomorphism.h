
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2011, Ben Burton                                   *
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

/*! \file dim2/dim2isomorphism.h
 *  \brief Deals with combinatorial isomorphisms of 2-manifold triangulations.
 */

#ifndef __DIM2ISOMORPHISM_H
#ifndef __DOXYGEN
#define __DIM2ISOMORPHISM_H
#endif

#include "regina-core.h"
#include "shareableobject.h"
#include "triangulation/nfacetspec.h"
#include "maths/nperm3.h"

namespace regina {

class Dim2Triangulation;

/**
 * \weakgroup dim2
 * @{
 */

/**
 * Represents a combinatorial isomorphism from one 4-manifold triangulation
 * into another.
 *
 * A combinatorial isomorphism from triangulation T to triangulation U
 * is a one-to-one map f from the individual face edges of T to
 * the individual face edges of U for which the following
 * conditions hold:
 *
 *   - if edges x and y belong to the same face of T then
 *     edges f(x) and f(y) belong to the same face of U;
 *   - if edges x and y are identified in T then edges f(x) and f(y)
 *     are identified in U.
 *
 * Isomorphisms can be <i>boundary complete</i> or
 * <i>boundary incomplete</i>.  A boundary complete isomorphism
 * satisfies the additional condition:
 *
 *   - if edge x is a boundary edge of T then edge f(x) is a boundary
 *     edge of U.
 *
 * A boundary complete isomorphism thus indicates that a copy of
 * triangulation T is present as an entire component (or components) of U,
 * whereas a boundary incomplete isomorphism represents an embedding of a
 * copy of triangulation T as a subcomplex of some possibly larger component
 * (or components) of U.
 *
 * Note that in all cases triangulation U may contain more faces
 * than triangulation T.
 */
class REGINA_API Dim2Isomorphism : public ShareableObject {
    protected:
        unsigned nFaces_;
            /**< The number of faces in the source triangulation. */
        int* faceImage_;
            /**< The face of the destination triangulation that
                 each face of the source triangulation maps to. */
        NPerm3* edgePerm_;
            /**< The permutation applied to the three edges of each
                 source face. */

    public:
        /**
         * Creates a new isomorphism with no initialisation.
         *
         * \ifacespython Not present.
         *
         * @param sourceFaces the number of faces in the source
         * triangulation associated with this isomorphism; this may be zero.
         */
        Dim2Isomorphism(unsigned sourceFaces);
        /**
         * Creates a new isomorphism identical to the given isomorphism.
         *
         * @param cloneMe the isomorphism upon which to base the new
         * isomorphism.
         */
        Dim2Isomorphism(const Dim2Isomorphism& cloneMe);
        /**
         * Destroys this isomorphism.
         */
        ~Dim2Isomorphism();

        /**
         * Returns the number of faces in the source triangulation
         * associated with this isomorphism.  Note that this is always
         * less than or equal to the number of faces in the
         * destination triangulation.
         *
         * @return the number of faces in the source triangulation.
         */
        unsigned getSourceFaces() const;

        /**
         * Determines the image of the given source face under
         * this isomorphism.
         *
         * \ifacespython Not present, though the read-only version of this
         * routine is.
         *
         * @param sourceFace the index of the source face; this must
         * be between 0 and <tt>getSourceFaces()-1</tt> inclusive.
         * @return a reference to the index of the destination face
         * that the source face maps to.
         */
        int& faceImage(unsigned sourceFace);
        /**
         * Determines the image of the given source face under
         * this isomorphism.
         *
         * @param sourceFace the index of the source face; this must
         * be between 0 and <tt>getSourceFaces()-1</tt> inclusive.
         * @return the index of the destination face
         * that the source face maps to.
         */
        int faceImage(unsigned sourceFace) const;
        /**
         * Returns a read-write reference to the permutation that is
         * applied to the three edges of the given source face
         * under this isomorphism.
         * Edge \a i of source face \a sourceFace will be mapped to
         * edge <tt>edgePerm(sourceFace)[i]</tt> of face
         * <tt>faceImage(sourceFace)</tt>.
         *
         * \ifacespython Not present, though the read-only version of this
         * routine is.
         *
         * @param sourceFace the index of the source face containing
         * the original three edges; this must be between 0 and
         * <tt>getSourceFaces()-1</tt> inclusive.
         * @return a read-write reference to the permutation applied to the
         * three edges of the source face.
         */
        NPerm3& edgePerm(unsigned sourceFace);
        /**
         * Determines the permutation that is applied to the three edges
         * of the given source face under this isomorphism.
         * Edge \a i of source face \a sourceFace will be mapped to
         * face <tt>edgePerm(sourceFace)[i]</tt> of face
         * <tt>faceImage(sourceFace)</tt>.
         *
         * @param sourceFace the index of the source face containing
         * the original three edges; this must be between 0 and
         * <tt>getSourceFaces()-1</tt> inclusive.
         * @return the permutation applied to the three edges of the
         * source face.
         */
        NPerm3 edgePerm(unsigned sourceFace) const;
        /**
         * Determines the image of the given source face edge
         * under this isomorphism.  Note that a value only is returned; this
         * routine cannot be used to alter the isomorphism.
         *
         * @param source the given source face edge; this must
         * be one of the three edges of one of the getSourceFaces()
         * faces in the source triangulation.
         * @return the image of the source face edge under this
         * isomorphism.
         */
        NFacetSpec<2> operator [] (const NFacetSpec<2>& source) const;

        /**
         * Determines whether or not this is an identity isomorphism.
         *
         * In an identity isomorphism, each face image is itself,
         * and within each face the edge/vertex permutation is
         * the identity on (0,1,2).
         *
         * @return \c true if this is an identity isomorphism, or
         * \c false otherwise.
         */
        bool isIdentity() const;

        /**
         * Applies this isomorphism to the given triangulation and
         * returns the result as a new triangulation.
         *
         * The given triangulation (call this T) is not modified in any way.
         * A new triangulation (call this S) is returned, so that this
         * isomorphism represents a one-to-one, onto and boundary complete
         * isomorphism from T to S.  That is, T and S are combinatorially
         * identical triangulations, and this isomorphism describes the
         * corresponding mapping between faces and face edges.
         *
         * The resulting triangulation S is newly created, and must be
         * destroyed by the caller of this routine.
         *
         * There are several preconditions to this routine.  This
         * routine does a small amount of sanity checking (and returns 0
         * if an error is detected), but it certainly does not check the
         * entire set of preconditions.  It is up to the caller of this
         * routine to verify that all of the following preconditions are
         * met.
         *
         * \pre The number of faces in the given triangulation is
         * precisely the number returned by getSourceFaces() for
         * this isomorphism.
         * \pre This is a valid isomorphism (i.e., it has been properly
         * initialised, so that all face images are non-negative
         * and distinct, and all edge permutations are real permutations
         * of (0,1,2).
         * \pre Each face image for this isomorphism lies
         * between 0 and <tt>getSourceFaces()-1</tt> inclusive
         * (i.e., this isomorphism does not represent a mapping from a
         * smaller triangulation into a larger triangulation).
         *
         * @param original the triangulation to which this isomorphism
         * should be applied.
         * @return the resulting new triangulation, or 0 if a problem
         * was encountered (i.e., an unmet precondition was noticed).
         */
        Dim2Triangulation* apply(const Dim2Triangulation* original) const;

        /**
         * Applies this isomorphism to the given triangulation,
         * modifying the given triangulation directly.
         *
         * This is similar to apply(), except that instead of creating a
         * new triangulation, the faces and vertices of the given
         * triangulation are modified directly.
         *
         * See apply() for further details on how this operation is performed.
         *
         * As with apply(), there are several preconditions to this routine.
         * This routine does a small amount of sanity checking (and returns
         * without changes if an error is detected), but it certainly does
         * not check the entire set of preconditions.  It is up to the
         * caller of this routine to verify that all of the following
         * preconditions are met.
         *
         * \pre The number of faces in the given triangulation is
         * precisely the number returned by getSourceFaces() for
         * this isomorphism.
         * \pre This is a valid isomorphism (i.e., it has been properly
         * initialised, so that all face images are non-negative
         * and distinct, and all edge permutations are real permutations
         * of (0,1,2).
         * \pre Each face image for this isomorphism lies
         * between 0 and <tt>getSourceFaces()-1</tt> inclusive
         * (i.e., this isomorphism does not represent a mapping from a
         * smaller triangulation into a larger triangulation).
         *
         * @param tri the triangulation to which this isomorphism
         * should be applied.
         */
        void applyInPlace(Dim2Triangulation* tri) const;

        void writeTextShort(std::ostream& out) const;
        void writeTextLong(std::ostream& out) const;

        /**
         * Returns a random isomorphism for the given number of
         * faces.  This isomorphism will reorder faces
         * 0 to <tt>nFaces-1</tt> in a random fashion, and for
         * each face a random permutation of its three vertices
         * will be selected.
         *
         * The isomorphism returned is newly constructed, and must be
         * destroyed by the caller of this routine.
         *
         * Note that both the STL random number generator and the
         * standard C function rand() are used in this routine.  All
         * possible isomorphisms for the given number of faces are
         * equally likely.
         *
         * @param nFaces the number of faces that the new
         * isomorphism should operate upon.
         * @return the newly constructed random isomorphism.
         */
        static Dim2Isomorphism* random(unsigned nFaces);
};

/*@}*/

// Inline functions for Dim2Isomorphism

inline Dim2Isomorphism::Dim2Isomorphism(unsigned sourceFaces) :
        nFaces_(sourceFaces),
        faceImage_(sourceFaces > 0 ? new int[sourceFaces] : 0),
        edgePerm_(sourceFaces > 0 ? new NPerm3[sourceFaces] : 0) {
}
inline Dim2Isomorphism::~Dim2Isomorphism() {
    // Always safe to delete null.
    delete[] faceImage_;
    delete[] edgePerm_;
}

inline unsigned Dim2Isomorphism::getSourceFaces() const {
    return nFaces_;
}

inline int& Dim2Isomorphism::faceImage(unsigned sourceFace) {
    return faceImage_[sourceFace];
}
inline int Dim2Isomorphism::faceImage(unsigned sourceFace) const {
    return faceImage_[sourceFace];
}
inline NPerm3& Dim2Isomorphism::edgePerm(unsigned sourceFace) {
    return edgePerm_[sourceFace];
}
inline NPerm3 Dim2Isomorphism::edgePerm(unsigned sourceFace) const {
    return edgePerm_[sourceFace];
}
inline NFacetSpec<2> Dim2Isomorphism::operator [] (
        const NFacetSpec<2>& source) const {
    return NFacetSpec<2>(faceImage_[source.simp],
        edgePerm_[source.simp][source.facet]);
}

} // namespace regina

#endif

