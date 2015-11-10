
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

/*! \file dim4/dim4tetrahedron.h
 *  \brief Deals with tetrahedra in the 3-skeleton of a 4-manifold
 *  triangulation.
 */

#ifndef __DIM4TETRAHEDRON_H
#ifndef __DOXYGEN
#define __DIM4TETRAHEDRON_H
#endif

#include "regina-core.h"
#include "output.h"
#include "maths/nperm5.h"
#include "utilities/nmarkedvector.h"
#include <boost/noncopyable.hpp>
// NOTE: More #includes follow after the class declarations.

namespace regina {

class Dim4BoundaryComponent;
class Dim4Edge;
class Dim4Triangle;
class Dim4Vertex;

template <int> class Component;
template <int> class Simplex;
template <int> class Triangulation;
typedef Component<4> Dim4Component;
typedef Simplex<4> Dim4Pentachoron;
typedef Triangulation<4> Dim4Triangulation;

/**
 * \weakgroup dim4
 * @{
 */

/**
 * Details how a tetrahedron in the 3-skeleton of a 4-manifold triangulation
 * forms part of an individual pentachoron.
 */
class REGINA_API Dim4TetrahedronEmbedding {
    private:
        Dim4Pentachoron* pent_;
            /**< The pentachoron in which this tetrahedron is contained. */
        int tet_;
            /**< The facet number of the pentachoron that is this
                 tetrahedron. */

    public:
        /**
         * Default constructor.  The embedding descriptor created is
         * unusable until it has some data assigned to it using
         * <tt>operator =</tt>.
         *
         * \ifacespython Not present.
         */
        Dim4TetrahedronEmbedding();

        /**
         * Creates an embedding descriptor containing the given data.
         *
         * @param pent the pentachoron in which this tetrahedron is contained.
         * @param tet the facet number of \a pent that is this tetrahedron.
         */
        Dim4TetrahedronEmbedding(Dim4Pentachoron* pent, int tet);

        /**
         * Creates an embedding descriptor containing the same data as
         * the given embedding descriptor.
         *
         * @param cloneMe the embedding descriptor to clone.
         */
        Dim4TetrahedronEmbedding(const Dim4TetrahedronEmbedding& cloneMe);

        /**
         * Assigns to this embedding descriptor the same data as is
         * contained in the given embedding descriptor.
         *
         * @param cloneMe the embedding descriptor to clone.
         */
        Dim4TetrahedronEmbedding& operator = (
            const Dim4TetrahedronEmbedding& cloneMe);

        /**
         * Returns the pentachoron in which this tetrahedron is contained.
         *
         * @return the pentachoron.
         */
        Dim4Pentachoron* getPentachoron() const;

        /**
         * Returns the facet number within getPentachoron() that is this
         * tetrahedron.
         *
         * @return the facet number that is this tetrahedron.
         */
        int getTetrahedron() const;

        /**
         * Returns a mapping from vertices (0,1,2,3) of this tetrahedron to the
         * corresponding vertex numbers in getPentachoron(), as described
         * in Dim4Pentachoron::getTetrahedronMapping().
         *
         * @return a mapping from the vertices of this tetrahedron to the
         * vertices of getPentachoron().
         */
        NPerm5 getVertices() const;

        /**
         * Tests whether this and the given embedding are identical.
         * Here "identical" means that they refer to the same facet of
         * the same pentachoron.
         *
         * @param rhs the embedding to compare with this.
         * @return \c true if and only if both embeddings are identical.
         */
        bool operator == (const Dim4TetrahedronEmbedding& rhs) const;

        /**
         * Tests whether this and the given embedding are different.
         * Here "different" means that they do not refer to the same facet of
         * the same pentachoron.
         *
         * @param rhs the embedding to compare with this.
         * @return \c true if and only if both embeddings are identical.
         */
        bool operator != (const Dim4TetrahedronEmbedding& rhs) const;
};

/**
 * Represents a tetrahedron in the 3-skeleton of a 4-manifold triangulation.
 * Tetrahedra are highly temporary; once a triangulation changes, all its
 * tetrahedron objects will be deleted and new ones will be created.
 */
class REGINA_API Dim4Tetrahedron :
        public Output<Dim4Tetrahedron>,
        public boost::noncopyable,
        public NMarkedElement {
    public:
        /**
         * An array that maps tetrahedron numbers within a pentachoron
         * (i.e., facet numbers) to the canonical ordering of the
         * individual pentachoron vertices that form each tetrahedron.
         *
         * This means that the vertices of tetrahedron \a i in a pentachoron
         * are, in canonical order, <tt>ordering[i][0..3]</tt>.  As an
         * immediate consequence, we obtain <tt>ordering[i][4] == i</tt>.
         *
         * Regina defines canonical order to be \e increasing order.
         * That is, <tt>ordering[i][0] &lt; ... &lt; ordering[i][3]</tt>.
         *
         * This table does \e not describe the mapping from specific
         * tetrahedra within a triangulation into individual pentachora
         * (for that, see Dim4Pentachoron::getTetrahedronMapping() instead).
         * This table merely provides a neat and consistent way of
         * listing the vertices of any given pentachoron facet.
         */
        static const NPerm5 ordering[5];

    private:
        Dim4TetrahedronEmbedding emb_[2];
            /**< A list of descriptors telling how this tetrahedron forms a
                 part of each individual pentachoron that it belongs to. */
        unsigned nEmb_;
            /**< The number of descriptors stored in the list \a emb_.
                 This will never exceed two. */
        Dim4Component* component_;
            /**< The component that this tetrahedron is a part of. */
        Dim4BoundaryComponent* boundaryComponent_;
            /**< The boundary component that this tetrahedron is a part of,
                 or 0 if this tetrahedron is internal. */

    public:
        /**
         * Returns the index of this tetrahedron in the underlying
         * triangulation.  This is identical to calling
         * <tt>getTriangulation()->tetrahedronIndex(this)</tt>.
         *
         * @return the index of this tetrahedron.
         */
        unsigned long index() const;

        /**
         * Returns the number of descriptors available through getEmbedding().
         * Note that this number will never be greater than two.
         *
         * @return the number of embedding descriptors.
         */
        unsigned getNumberOfEmbeddings() const;

        /**
         * Returns the requested descriptor detailing how this tetrahedron
         * forms a part of a particular pentachoron in the triangulation.
         * Note that if this tetrahedron represents multiple facets of a
         * particular pentachoron, then there will be multiple embedding
         * descriptors available regarding that pentachoron.
         *
         * @param index the index of the requested descriptor.  This
         * should be between 0 and getNumberOfEmbeddings()-1 inclusive.
         * @return the requested embedding descriptor.
         */
        const Dim4TetrahedronEmbedding& getEmbedding(unsigned index) const;

        /**
         * Returns the triangulation to which this tetrahedron belongs.
         *
         * @return the triangulation containing this tetrahedron.
         */
        Dim4Triangulation* getTriangulation() const;

        /**
         * Returns the component of the triangulation to which this
         * tetrahedron belongs.
         *
         * @return the component containing this tetrahedron.
         */
        Dim4Component* getComponent() const;

        /**
         * Returns the boundary component of the triangulation to which
         * this tetrahedron belongs.
         *
         * @return the boundary component containing this tetrahedron, or 0
         * if this tetrahedron does not lie entirely within the boundary of
         * the triangulation.
         */
        Dim4BoundaryComponent* getBoundaryComponent() const;

        /**
         * Returns the vertex of the 4-manifold triangulation corresponding
         * to the given vertex of this tetrahedron.
         *
         * @param vertex the vertex of this tetrahedron to examine.  This
         * should be between 0 and 3 inclusive.
         * @return the corresponding vertex of the 4-manifold triangulation.
         */
        Dim4Vertex* getVertex(int vertex) const;

        /**
         * Returns the edge of the 4-manifold triangulation corresponding
         * to the given edge of this tetrahedron.
         *
         * Edges of a tetrahedron are numbered from 0 to 5, as described
         * by the arrays NEdge::edgeNumber and NEdge::edgeVertex.
         * Edge \a i of a tetrahedron is always opposite edge \a 5-i.
         *
         * @param edge the edge of this tetrahedron to examine.  This should
         * be between 0 and 5 inclusive.
         * @return the corresponding edge of the 4-manifold triangulation.
         */
        Dim4Edge* getEdge(int edge) const;

        /**
         * Returns the triangle of the 4-manifold triangulation corresponding
         * to the given triangular face of this tetrahedron.
         *
         * Note that triangle \a i of a tetrahedron is opposite vertex \a i of
         * the tetrahedron.
         *
         * @param tri the triangle of this tetrahedron to examine.  This
         * should be between 0 and 3 inclusive.
         * @return the corresponding triangle of the 4-manifold triangulation.
         */
        Dim4Triangle* getTriangle(int tri) const;

        /**
         * Examines the given edge of this tetrahedron, and returns a mapping
         * from the "canonical" vertices of the corresponding edge of
         * the triangulation to the vertices of this tetrahedron.
         *
         * This routine behaves much the same way as
         * Dim4Pentachoron::getEdgeMapping(), except that it maps the
         * edge vertices into a tetrahedron, not into a pentachoron.  See
         * Dim4Pentachoron::getEdgeMapping() for a more detailed
         * explanation of precisely what this mapping means.
         *
         * This routine differs from Dim4Pentachoron::getEdgeMapping()
         * in how it handles the images of 2, 3 and 4.  This routine
         * will always map 2 and 3 to the remaining vertices of this
         * tetrahedron (in arbitrary order), and will always map 4 to itself.
         *
         * @param edge the edge of this tetrahedron to examine.  This should
         * be between 0 and 5 inclusive.
         * @return a mapping from vertices (0,1) of the requested edge
         * to the vertices of this tetrahedron.
         */
        NPerm5 getEdgeMapping(int edge) const;

        /**
         * Examines the given triangle of this tetrahedron, and returns a
         * mapping from the "canonical" vertices of the corresponding triangle
         * of the triangulation to the vertices of this tetrahedron.
         *
         * This routine behaves much the same way as
         * Dim4Pentachoron::getTriangleMapping(), except that it maps the
         * triangle vertices into a tetrahedron, not into a pentachoron.  See
         * Dim4Pentachoron::getTriangleMapping() for a more detailed
         * explanation of precisely what this mapping means.
         *
         * This routine differs from Dim4Pentachoron::getTriangleMapping()
         * in how it handles the images of 3 and 4.  This routine
         * will always map 3 to the remaining vertex of this tetrahedron (which
         * is equal to the argument \a tri), and will always map 4 to itself.
         *
         * @param tri the triangle of this tetrahedron to examine.  This should
         * be between 0 and 3 inclusive.
         * @return a mapping from vertices (0,1,2) of the requested triangle
         * to the vertices of this tetrahedron.
         */
        NPerm5 getTriangleMapping(int tri) const;

        /**
         * Determines if this tetrahedron lies entirely on the boundary of the
         * triangulation.
         *
         * @return \c true if and only if this tetrahedron lies on the boundary.
         */
        bool isBoundary() const;

        /**
         * Determines whether this tetrahedron represents a dual edge in the
         * maximal forest that has been chosen for the dual 1-skeleton of the
         * triangulation.
         *
         * When the skeletal structure of a triangulation is first computed,
         * a maximal forest in the dual 1-skeleton of the triangulation is
         * also constructed.  Each dual edge in this maximal forest
         * represents a tetrahedron of the (primal) triangulation.
         *
         * This maximal forest will remain fixed until the triangulation
         * changes, at which point it will be recomputed (as will all
         * other skeletal objects, such as connected components and so on).
         * There is no guarantee that, when it is recomputed, the
         * maximal forest will use the same dual edges as before.
         *
         * This routine identifies whether this tetrahedron belongs to the
         * dual forest.  In this sense it performs a similar role to
         * Simplex::facetInMaximalForest(), but this routine is typically
         * easier to use.
         *
         * If the skeleton has already been computed, then this routine is
         * very fast (since it just returns a precomputed answer).
         *
         * @return \c true if and only if this tetrahedron represents a
         * dual edge in the maximal forest.
         */
        bool inMaximalForest() const;

        /**
         * Writes a short text representation of this object to the
         * given output stream.
         *
         * \ifacespython Not present.
         *
         * @param out the output stream to which to write.
         */
        void writeTextShort(std::ostream& out) const;
        /**
         * Writes a detailed text representation of this object to the
         * given output stream.
         *
         * \ifacespython Not present.
         *
         * @param out the output stream to which to write.
         */
        void writeTextLong(std::ostream& out) const;

    private:
        /**
         * Creates a new tetrahedron and marks it as belonging to the
         * given triangulation component.
         *
         * @param component the triangulation component to which this
         * tetrahedron belongs.
         */
        Dim4Tetrahedron(Dim4Component* component);

    friend class Triangulation<4>;
        /**< Allow access to private members. */
};

/*@}*/

} // namespace regina
// Some more headers that are required for inline functions:
#include "dim4/dim4pentachoron.h"
namespace regina {

// Inline functions for Dim4TetrahedronEmbedding

inline Dim4TetrahedronEmbedding::Dim4TetrahedronEmbedding() : pent_(0) {
}

inline Dim4TetrahedronEmbedding::Dim4TetrahedronEmbedding(
        Dim4Pentachoron* pent, int tet) :
        pent_(pent), tet_(tet) {
}

inline Dim4TetrahedronEmbedding::Dim4TetrahedronEmbedding(
        const Dim4TetrahedronEmbedding& cloneMe) :
        pent_(cloneMe.pent_), tet_(cloneMe.tet_) {
}

inline Dim4TetrahedronEmbedding& Dim4TetrahedronEmbedding::operator =
        (const Dim4TetrahedronEmbedding& cloneMe) {
    pent_ = cloneMe.pent_;
    tet_ = cloneMe.tet_;
    return *this;
}

inline Dim4Pentachoron* Dim4TetrahedronEmbedding::getPentachoron() const {
    return pent_;
}

inline int Dim4TetrahedronEmbedding::getTetrahedron() const {
    return tet_;
}

inline NPerm5 Dim4TetrahedronEmbedding::getVertices() const {
    return pent_->getTetrahedronMapping(tet_);
}

inline bool Dim4TetrahedronEmbedding::operator ==
        (const Dim4TetrahedronEmbedding& other) const {
    return ((pent_ == other.pent_) && (tet_ == other.tet_));
}

inline bool Dim4TetrahedronEmbedding::operator !=
        (const Dim4TetrahedronEmbedding& other) const {
    return ((pent_ != other.pent_) || (tet_ != other.tet_));
}

// Inline functions for Dim4Tetrahedron

inline Dim4Tetrahedron::Dim4Tetrahedron(Dim4Component* component) :
        nEmb_(0), component_(component), boundaryComponent_(0) {
}

inline unsigned long Dim4Tetrahedron::index() const {
    return markedIndex();
}

inline unsigned Dim4Tetrahedron::getNumberOfEmbeddings() const {
    return nEmb_;
}

inline const Dim4TetrahedronEmbedding& Dim4Tetrahedron::getEmbedding(
        unsigned index) const {
    return emb_[index];
}

inline Dim4Triangulation* Dim4Tetrahedron::getTriangulation() const {
    return emb_[0].getPentachoron()->getTriangulation();
}

inline Dim4Component* Dim4Tetrahedron::getComponent() const {
    return component_;
}

inline Dim4BoundaryComponent* Dim4Tetrahedron::getBoundaryComponent() const {
    return boundaryComponent_;
}

inline Dim4Vertex* Dim4Tetrahedron::getVertex(int vertex) const {
    return emb_[0].getPentachoron()->getVertex(emb_[0].getVertices()[vertex]);
}

inline bool Dim4Tetrahedron::isBoundary() const {
    return (boundaryComponent_ != 0);
}

inline bool Dim4Tetrahedron::inMaximalForest() const {
    return emb_[0].getPentachoron()->facetInMaximalForest(emb_[0].getTetrahedron());
}

inline void Dim4Tetrahedron::writeTextShort(std::ostream& out) const {
    out << (boundaryComponent_ ? "Boundary " : "Internal ") << "tetrahedron";
}

} // namespace regina

#endif
