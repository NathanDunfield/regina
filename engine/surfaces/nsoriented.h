
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

/*! \file surfaces/nsoriented.h
 *  \brief Implements normal surface vectors using transversly oriented 
 *  normal surface coordinates.*/

#ifndef __NSORIENTED_H
#ifndef __DOXYGEN
#define __NSORIENTED_H
#endif


#include "regina-core.h"
#include "surfaces/nnormalsurface.h"

namespace regina {
class NMatrixInt;
class NNormalSurfaceVectorOriented;

/**
 * \weakgroup surfaces
 * @{
 */

/**
 * Stores information about transversely oriented standard normal coordinates.
 * See the general NormalInfo template notes for further details.
 *
 * \ifacespython Not present.
 */
template <>
struct NormalInfo<NS_ORIENTED> {
    typedef NNormalSurfaceVectorOriented Class;
    typedef NormalInfo<NS_ORIENTED> Standard;
    typedef NormalInfo<NS_ORIENTED_QUAD> Reduced;
    inline static const char* name() {
        return "Transversely oriented standard normal";
    }
    enum {
        almostNormal = 0,
        spun = 0,
        oriented = 1
    };
};

/**
 * A normal surface vector using transversely oriented standard
 * (triangle-quad) coordinates.
 *
 * If there are \a t tetrahedra in the underlying
 * triangulation, there must be precisely 14<i>t</i> coordinates.
 * For each \a i, coordinates 2<i>i</i> and 2<i>i</i>+1 represent
 * the \c true and \c false orientations for coordinate \a i in the
 * 7<i>t</i>-dimensional standard coordinate system.  See
 * NNormalSurfaceVectorStandard for further details.
 *
 * \warning Support for transversely oriented normal surfaces is still
 * experimental, and some features \b will break (e.g., testing
 * connectedness, disjointness or embeddedness).
 *
 * \ifacespython Not present.
 */
class REGINA_API NNormalSurfaceVectorOriented : public NNormalSurfaceVector {
    REGINA_NORMAL_SURFACE_FLAVOUR(NNormalSurfaceVectorOriented, NS_ORIENTED)

    public:
        /**
         * Creates a new vector all of whose entries are initialised to
         * zero.
         *
         * @param length the number of elements in the new vector.
         */
        NNormalSurfaceVectorOriented(size_t length);
        /**
         * Creates a new vector that is a clone of the given vector.
         *
         * @param cloneMe the vector to clone.
         */
        NNormalSurfaceVectorOriented(const NVector<NLargeInteger>& cloneMe);

        virtual NLargeInteger getTriangleCoord(unsigned long tetIndex,
            int vertex, const NTriangulation* triang) const;
        virtual NLargeInteger getQuadCoord(unsigned long tetIndex,
            int quadType, const NTriangulation* triang) const;
        virtual NLargeInteger getOrientedTriangleCoord(unsigned long tetIndex,
            int vertex, const NTriangulation* triang, bool orientation) const;
        virtual NLargeInteger getOrientedQuadCoord(unsigned long tetIndex,
            int quadType, const NTriangulation* triang, bool orientation) const;
        virtual NLargeInteger getOctCoord(unsigned long tetIndex,
            int octType, const NTriangulation* triang) const;
        virtual NLargeInteger getEdgeWeight(unsigned long edgeIndex,
            const NTriangulation* triang) const;
        virtual NLargeInteger getTriangleArcs(unsigned long triIndex,
            int triVertex, const NTriangulation* triang) const;

        static NNormalSurfaceVector* makeZeroVector(
            const NTriangulation* triangulation);
        static NMatrixInt* makeMatchingEquations(
            const NTriangulation* triangulation);
        static NEnumConstraintList* makeEmbeddedConstraints(
            const NTriangulation* triangulation);
};

/*@}*/

// Inline functions for NNormalSurfaceVectorOriented

inline NNormalSurfaceVectorOriented::NNormalSurfaceVectorOriented(
        size_t length) : NNormalSurfaceVector(length) {
}
inline NNormalSurfaceVectorOriented::NNormalSurfaceVectorOriented(
        const NVector<NLargeInteger>& cloneMe) :
        NNormalSurfaceVector(cloneMe) {
}

inline NLargeInteger NNormalSurfaceVectorOriented::getTriangleCoord(
        unsigned long tetIndex, int vertex, const NTriangulation* tri) const {
    return getOrientedTriangleCoord(tetIndex,vertex,tri, true)
           + getOrientedTriangleCoord(tetIndex,vertex,tri, false);
}
inline NLargeInteger NNormalSurfaceVectorOriented::getQuadCoord(
        unsigned long tetIndex, int quadType, const NTriangulation* tri) const {
    return getOrientedQuadCoord(tetIndex,quadType,tri, true)
           + getOrientedQuadCoord(tetIndex,quadType,tri, false);
}

inline NLargeInteger NNormalSurfaceVectorOriented::getOrientedTriangleCoord(
        unsigned long tetIndex, int vertex, const NTriangulation*,
        bool orientation) const {
    return (*this)[14 * tetIndex + 2 * vertex +
        (orientation ? 0 : 1)];
}
inline NLargeInteger NNormalSurfaceVectorOriented::getOrientedQuadCoord(
        unsigned long tetIndex, int quadType, const NTriangulation*, 
        bool orientation) const {
    return (*this)[14 * tetIndex + 8 + 2 * quadType +
        (orientation ? 0 : 1)];
}
inline NLargeInteger NNormalSurfaceVectorOriented::getOctCoord(
        unsigned long, int, const NTriangulation*) const {
    return zero;
}

} // namespace regina

#endif

