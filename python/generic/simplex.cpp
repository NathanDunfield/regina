
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Python Interface                                                      *
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

#include <boost/python.hpp>
#include "generic/simplex.h"
#include "generic/triangulation.h"
#include "maths/nperm5.h" // Specialisation needed for 4-D case.
#include "../helpers.h"
#include "../generic/facehelper.h"

using namespace boost::python;
using regina::Simplex;

namespace {
    template <int dim, int maxsubdim>
    struct face_aliases :
            boost::python::def_visitor<face_aliases<dim, maxsubdim>> {
        friend class boost::python::def_visitor_access;

        template <typename Class>
        void visit(Class& c) const {
            c.def(face_aliases<dim, maxsubdim - 1>());
        }
    };

    template <int dim>
    struct face_aliases<dim, 0> :
            boost::python::def_visitor<face_aliases<dim, 0>> {
        friend class boost::python::def_visitor_access;

        template <typename Class>
        void visit(Class& c) const {
            c.def("vertex", &Simplex<dim>::vertex,
                return_value_policy<reference_existing_object>());
            c.def("getVertex", &Simplex<dim>::getVertex,
                return_value_policy<reference_existing_object>());
            c.def("vertexMapping", &Simplex<dim>::vertexMapping);
            c.def("getVertexMapping", &Simplex<dim>::getVertexMapping);
        }
    };

    template <int dim>
    struct face_aliases<dim, 1> :
            boost::python::def_visitor<face_aliases<dim, 1>> {
        friend class boost::python::def_visitor_access;

        template <typename Class>
        void visit(Class& c) const {
            c.def("edge", &Simplex<dim>::edge,
                return_value_policy<reference_existing_object>());
            c.def("getEdge", &Simplex<dim>::getEdge,
                return_value_policy<reference_existing_object>());
            c.def("edgeMapping", &Simplex<dim>::edgeMapping);
            c.def("getEdgeMapping", &Simplex<dim>::getEdgeMapping);
            c.def(face_aliases<dim, 0>());
        }
    };

    template <int dim>
    struct face_aliases<dim, 2> :
            boost::python::def_visitor<face_aliases<dim, 2>> {
        friend class boost::python::def_visitor_access;

        template <typename Class>
        void visit(Class& c) const {
            c.def("triangle", &Simplex<dim>::triangle,
                return_value_policy<reference_existing_object>());
            c.def("getTriangle", &Simplex<dim>::getTriangle,
                return_value_policy<reference_existing_object>());
            c.def("triangleMapping", &Simplex<dim>::triangleMapping);
            c.def("getTriangleMapping", &Simplex<dim>::getTriangleMapping);
            c.def(face_aliases<dim, 1>());
        }
    };

    template <int dim>
    struct face_aliases<dim, 3> :
            boost::python::def_visitor<face_aliases<dim, 3>> {
        friend class boost::python::def_visitor_access;

        template <typename Class>
        void visit(Class& c) const {
            c.def("tetrahedron", &Simplex<dim>::tetrahedron,
                return_value_policy<reference_existing_object>());
            c.def("getTetrahedron", &Simplex<dim>::getTetrahedron,
                return_value_policy<reference_existing_object>());
            c.def("tetrahedronMapping", &Simplex<dim>::tetrahedronMapping);
            c.def("getTetrahedronMapping",
                &Simplex<dim>::getTetrahedronMapping);
            c.def(face_aliases<dim, 2>());
        }
    };

    template <int dim>
    struct face_aliases<dim, 4> :
            boost::python::def_visitor<face_aliases<dim, 4>> {
        friend class boost::python::def_visitor_access;

        template <typename Class>
        void visit(Class& c) const {
            c.def("pentachoron", &Simplex<dim>::pentachoron,
                return_value_policy<reference_existing_object>());
            c.def("getPentachoron", &Simplex<dim>::getPentachoron,
                return_value_policy<reference_existing_object>());
            c.def("pentachoronMapping", &Simplex<dim>::pentachoronMapping);
            c.def("getPentachoronMapping",
                &Simplex<dim>::getPentachoronMapping);
            c.def(face_aliases<dim, 3>());
        }
    };
}

template <int dim>
void addSimplex(const char* name) {
    class_<regina::Simplex<dim>, std::auto_ptr<regina::Simplex<dim>>,
            boost::noncopyable>(name, no_init)
        .def("getDescription", &Simplex<dim>::getDescription,
            return_value_policy<return_by_value>())
        .def("setDescription", &Simplex<dim>::setDescription)
        .def("index", &Simplex<dim>::index)
        .def("adjacentSimplex", &Simplex<dim>::adjacentSimplex,
            return_value_policy<reference_existing_object>())
        .def("adjacentGluing", &Simplex<dim>::adjacentGluing)
        .def("adjacentFacet", &Simplex<dim>::adjacentFacet)
        .def("hasBoundary", &Simplex<dim>::hasBoundary)
        .def("joinTo", &Simplex<dim>::joinTo)
        .def("join", &Simplex<dim>::join)
        .def("unjoin", &Simplex<dim>::unjoin,
            return_value_policy<reference_existing_object>())
        .def("isolate", &Simplex<dim>::isolate)
        .def("triangulation", &Simplex<dim>::triangulation,
            return_value_policy<reference_existing_object>())
        .def("getTriangulation", &Simplex<dim>::getTriangulation,
            return_value_policy<reference_existing_object>())
        .def("component", &Simplex<dim>::component,
            return_value_policy<reference_existing_object>())
        .def("getComponent", &Simplex<dim>::getComponent,
            return_value_policy<reference_existing_object>())
        .def("face", &regina::python::face<Simplex<dim>, dim, int>)
        .def("getFace", &regina::python::face<Simplex<dim>, dim, int>)
        .def("faceMapping", &regina::python::faceMapping<Simplex<dim>, dim>)
        .def("getFaceMapping", &regina::python::faceMapping<Simplex<dim>, dim>)
        .def(face_aliases<dim, dim - 1>())
        .def("orientation", &Simplex<dim>::orientation)
        .def("facetInMaximalForest", &Simplex<dim>::facetInMaximalForest)
        .def("str", &Simplex<dim>::str)
        .def("toString", &Simplex<dim>::toString)
        .def("detail", &Simplex<dim>::detail)
        .def("toStringLong", &Simplex<dim>::toStringLong)
        .def("__str__", &Simplex<dim>::str)
        .def(regina::python::add_eq_operators())
    ;
}

void addSimplex() {
    addSimplex<4>("Simplex4");
    addSimplex<5>("Simplex5");
    addSimplex<6>("Simplex6");
    addSimplex<7>("Simplex7");
    addSimplex<8>("Simplex8");
    addSimplex<9>("Simplex9");
    addSimplex<10>("Simplex10");
    addSimplex<11>("Simplex11");
    addSimplex<12>("Simplex12");
    addSimplex<13>("Simplex13");
    addSimplex<14>("Simplex14");
    addSimplex<15>("Simplex15");
}

