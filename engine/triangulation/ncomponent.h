
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

#ifndef __NCOMPONENT_H
#ifndef __DOXYGEN
#define __NCOMPONENT_H
#endif

/*! \file triangulation/ncomponent.h
 *  \brief Deals with components of a triangulation.
 */

#include <vector>
#include "regina-core.h"
#include "shareableobject.h"
#include "utilities/nmarkedvector.h"

namespace regina {

class NTetrahedron;
class NTriangle;
class NEdge;
class NVertex;
class NBoundaryComponent;

/**
 * \weakgroup triangulation
 * @{
 */

/**
 * Represents a component of a triangulation.
 * Components are highly temporary; once a triangulation changes, all
 * its component objects will be deleted and new ones will be created.
 */
class REGINA_API NComponent : public ShareableObject, public NMarkedElement {
    private:
        std::vector<NTetrahedron*> tetrahedra;
            /**< List of tetrahedra in the component. */
        std::vector<NTriangle*> triangles;
            /**< List of triangles in the component. */
        std::vector<NEdge*> edges;
            /**< List of edges in the component. */
        std::vector<NVertex*> vertices;
            /**< List of vertices in the component. */
        std::vector<NBoundaryComponent*> boundaryComponents;
            /**< List of boundary components in the component. */

        bool ideal;
            /**< Is the component ideal? */
        bool orientable;
            /**< Is the component orientable? */

    public:
        /**
         * Default destructor.
         */
        virtual ~NComponent();

        /**
         * Returns the number of tetrahedra in this component.
         *
         * @return the number of tetrahedra.
         */
        unsigned long getNumberOfTetrahedra() const;
        /**
         * A dimension-agnostic alias for getNumberOfTetrahedra().
         * This is to assist with writing dimension-agnostic code that
         * can be reused to work in different dimensions.
         * 
         * Here "simplex" refers to a top-dimensional simplex (which for
         * 3-manifold triangulations means a tetrahedron).
         * 
         * See getNumberOfTetrahedra() for further information.
         */
        unsigned long getNumberOfSimplices() const;

        /**
         * Returns the number of triangles in this component.
         *
         * @return the number of triangles.
         */
        unsigned long getNumberOfTriangles() const;
        /**
         * A deprecated alias for getNumberOfTriangles().
         *
         * This routine returns the number of triangular faces in this
         * component.  See getNumberOfTriangles() for further details.
         *
         * \deprecated This routine will be removed in a future version
         * of Regina.  Please use getNumberOfTriangles() instead.
         *
         * @return the number of triangles.
         */
        unsigned long getNumberOfFaces() const;

        /**
         * Returns the number of edges in this component.
         *
         * @return the number of edges.
         */
        unsigned long getNumberOfEdges() const;

        /**
         * Returns the number of vertices in this component.
         *
         * @return the number of vertices.
         */
        unsigned long getNumberOfVertices() const;

        /**
         * Returns the number of boundary components in this component.
         *
         * @return the number of boundary components.
         */
        unsigned long getNumberOfBoundaryComponents() const;

        /**
         * Returns the requested tetrahedron in this component.
         *
         * @param index the index of the requested tetrahedron in the
         * component.  This should be between 0 and
         * getNumberOfTetrahedra()-1 inclusive.
         * Note that the index of a tetrahedron in the component need
         * not be the index of the same tetrahedron in the entire
         * triangulation.
         * @return the requested tetrahedron.
         */
        NTetrahedron* getTetrahedron(unsigned long index) const;
        /**
         * A dimension-agnostic alias for getTetrahedron().
         * This is to assist with writing dimension-agnostic code that
         * can be reused to work in different dimensions.
         * 
         * Here "simplex" refers to a top-dimensional simplex (which for
         * 3-manifold triangulations means a tetrahedron).
         * 
         * See getTetrahedron() for further information.
         */
        NTetrahedron* getSimplex(unsigned long index) const;

        /**
         * Returns the requested triangle in this component.
         *
         * @param index the index of the requested triangle in the
         * component.  This should be between 0 and
         * getNumberOfTriangles()-1 inclusive.
         * Note that the index of a triangle in the component need
         * not be the index of the same triangle in the entire
         * triangulation.
         * @return the requested triangle.
         */
        NTriangle* getTriangle(unsigned long index) const;
        /**
         * A deprecated alias for getTriangle().
         *
         * This routine returns the requested triangular face in this
         * component.  See getTriangle() for further details.
         *
         * \deprecated This routine will be removed in a future version
         * of Regina.  Please use getTriangle() instead.
         *
         * @param index the index of the requested triangle in the
         * component.  This should be between 0 and
         * getNumberOfTriangles()-1 inclusive.
         * Note that the index of a triangle in the component need
         * not be the index of the same triangle in the entire
         * triangulation.
         * @return the requested triangle.
         */
        NTriangle* getFace(unsigned long index) const;

        /**
         * Returns the requested edge in this component.
         *
         * @param index the index of the requested edge in the
         * component.  This should be between 0 and
         * getNumberOfEdges()-1 inclusive.
         * Note that the index of an edge in the component need
         * not be the index of the same edge in the entire
         * triangulation.
         * @return the requested edge.
         */
        NEdge* getEdge(unsigned long index) const;

        /**
         * Returns the requested vertex in this component.
         *
         * @param index the index of the requested vertex in the
         * component.  This should be between 0 and
         * getNumberOfVertices()-1 inclusive.
         * Note that the index of a vertex in the component need
         * not be the index of the same vertex in the entire
         * triangulation.
         * @return the requested vertex.
         */
        NVertex* getVertex(unsigned long index) const;

        /**
         * Returns the requested boundary component in this component.
         *
         * @param index the index of the requested boundary component in
         * this component.  This should be between 0 and
         * getNumberOfBoundaryComponents()-1 inclusive.
         * Note that the index of a boundary component in the component
         * need not be the index of the same boundary component in the
         * entire triangulation.
         * @return the requested boundary component.
         */
        NBoundaryComponent* getBoundaryComponent(unsigned long index) const;

        /**
         * Determines if this component is ideal.
         * This is the case if and only if it contains an ideal vertex
         * as described by NVertex::isIdeal().
         *
         * @return \c true if and only if this component is ideal.
         */
        bool isIdeal() const;

        /**
         * Determines if this component is orientable.
         * 
         * @return \c true if and only if this component is orientable.
         */
        bool isOrientable() const;

        /**
         * Determines if this component is closed.
         * This is the case if and only if it has no boundary.
         * Note that ideal components are not closed.
         *
         * @return \c true if and only if this component is closed.
         */
        bool isClosed() const;
        /**
         * Returns the number of boundary triangles in this component.
         *
         * @return the number of boundary triangles.
         */
        unsigned long getNumberOfBoundaryTriangles() const;

        void writeTextShort(std::ostream& out) const;
        void writeTextLong(std::ostream& out) const;

    private:
        /**
         * Default constructor.
         */
        NComponent();

    friend class NTriangulation;
        /**< Allow access to private members. */
};

/*@}*/

// Inline functions for NComponent

inline NComponent::NComponent() : ideal(false), orientable(true) {
}

inline NComponent::~NComponent() {
}

inline unsigned long NComponent::getNumberOfTetrahedra() const {
    return tetrahedra.size();
}

inline unsigned long NComponent::getNumberOfSimplices() const {
    return tetrahedra.size();
}

inline unsigned long NComponent::getNumberOfTriangles() const {
    return triangles.size();
}

inline unsigned long NComponent::getNumberOfFaces() const {
    return triangles.size();
}

inline unsigned long NComponent::getNumberOfEdges() const {
    return edges.size();
}

inline unsigned long NComponent::getNumberOfVertices() const {
    return vertices.size();
}

inline unsigned long NComponent::getNumberOfBoundaryComponents() const {
    return boundaryComponents.size();
}

inline NTetrahedron* NComponent::getTetrahedron(unsigned long index) const {
    return tetrahedra[index];
}

inline NTetrahedron* NComponent::getSimplex(unsigned long index) const {
    return tetrahedra[index];
}

inline NTriangle* NComponent::getTriangle(unsigned long index) const {
    return triangles[index];
}

inline NTriangle* NComponent::getFace(unsigned long index) const {
    return triangles[index];
}

inline NEdge* NComponent::getEdge(unsigned long index) const {
    return edges[index];
}

inline NVertex* NComponent::getVertex(unsigned long index) const {
    return vertices[index];
}

inline NBoundaryComponent* NComponent::getBoundaryComponent(unsigned long index)
        const {
    return boundaryComponents[index];
}

inline bool NComponent::isIdeal() const {
    return ideal;
}

inline bool NComponent::isOrientable() const {
    return orientable;
}

inline bool NComponent::isClosed() const {
    return (boundaryComponents.empty());
}

inline unsigned long NComponent::getNumberOfBoundaryTriangles() const {
    return 2 * triangles.size() - 4 * tetrahedra.size();
}

} // namespace regina

#endif

