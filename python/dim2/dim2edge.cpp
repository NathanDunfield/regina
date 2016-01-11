
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
#include "dim2/dim2boundarycomponent.h"
#include "dim2/dim2component.h"
#include "dim2/dim2edge.h"
#include "dim2/dim2triangle.h"
#include "dim2/dim2triangulation.h"
#include "dim2/dim2vertex.h"
#include "../globalarray.h"
#include "../helpers.h"
#include "../generic/facehelper.h"

using namespace boost::python;
using regina::Dim2Edge;
using regina::Dim2EdgeEmbedding;
using regina::Face;
using regina::FaceEmbedding;
using regina::python::GlobalArray;

namespace {
    boost::python::list Dim2Edge_getEmbeddings_list(const Dim2Edge* e) {
        boost::python::list ans;
        for (auto& emb: *e)
            ans.append(emb);
        return ans;
    }
}

void addDim2Edge() {
    class_<FaceEmbedding<2, 1>>("FaceEmbedding2_1",
            init<regina::Dim2Triangle*, int>())
        .def(init<const Dim2EdgeEmbedding&>())
        .def("simplex", &Dim2EdgeEmbedding::simplex,
            return_value_policy<reference_existing_object>())
        .def("getSimplex", &Dim2EdgeEmbedding::getSimplex,
            return_value_policy<reference_existing_object>())
        .def("triangle", &Dim2EdgeEmbedding::triangle,
            return_value_policy<reference_existing_object>())
        .def("getTriangle", &Dim2EdgeEmbedding::getTriangle,
            return_value_policy<reference_existing_object>())
        .def("face", &Dim2EdgeEmbedding::face)
        .def("getFace", &Dim2EdgeEmbedding::getFace)
        .def("edge", &Dim2EdgeEmbedding::edge)
        .def("getEdge", &Dim2EdgeEmbedding::getEdge)
        .def("vertices", &Dim2EdgeEmbedding::vertices)
        .def("getVertices", &Dim2EdgeEmbedding::getVertices)
        .def("str", &Dim2EdgeEmbedding::str)
        .def("toString", &Dim2EdgeEmbedding::toString)
        .def("detail", &Dim2EdgeEmbedding::detail)
        .def("toStringLong", &Dim2EdgeEmbedding::toStringLong)
        .def("__str__", &Dim2EdgeEmbedding::str)
        .def(regina::python::add_eq_operators())
    ;

    class_<Face<2, 1>, std::auto_ptr<Face<2, 1>>, boost::noncopyable>
            ("Face2_1", no_init)
        .def("index", &Dim2Edge::index)
        .def("isValid", &Dim2Edge::isValid)
        .def("isLinkOrientable", &Dim2Edge::isLinkOrientable)
        .def("embeddings", Dim2Edge_getEmbeddings_list)
        .def("getEmbeddings", Dim2Edge_getEmbeddings_list)
        .def("degree", &Dim2Edge::degree)
        .def("getDegree", &Dim2Edge::getDegree)
        .def("embedding", &Dim2Edge::embedding,
            return_internal_reference<>())
        .def("getEmbedding", &Dim2Edge::getEmbedding,
            return_internal_reference<>())
        .def("front", &Dim2Edge::front,
            return_internal_reference<>())
        .def("back", &Dim2Edge::back,
            return_internal_reference<>())
        .def("triangulation", &Dim2Edge::triangulation,
            return_value_policy<reference_existing_object>())
        .def("getTriangulation", &Dim2Edge::getTriangulation,
            return_value_policy<reference_existing_object>())
        .def("component", &Dim2Edge::component,
            return_value_policy<reference_existing_object>())
        .def("getComponent", &Dim2Edge::getComponent,
            return_value_policy<reference_existing_object>())
        .def("getBoundaryComponent", &Dim2Edge::getBoundaryComponent,
            return_value_policy<reference_existing_object>())
        .def("face", &regina::python::face<Dim2Edge, 1, int>)
        .def("getFace", &regina::python::face<Dim2Edge, 1, int>)
        .def("vertex", &Dim2Edge::vertex,
            return_value_policy<reference_existing_object>())
        .def("getVertex", &Dim2Edge::getVertex,
            return_value_policy<reference_existing_object>())
        .def("faceMapping", &regina::python::faceMapping<Dim2Edge, 1, 3>)
        .def("getFaceMapping", &regina::python::faceMapping<Dim2Edge, 1, 3>)
        .def("vertexMapping", &Dim2Edge::vertexMapping)
        .def("getVertexMapping", &Dim2Edge::getVertexMapping)
        .def("isBoundary", &Dim2Edge::isBoundary)
        .def("inMaximalForest", &Dim2Edge::inMaximalForest)
        .def("str", &Dim2Edge::str)
        .def("toString", &Dim2Edge::toString)
        .def("detail", &Dim2Edge::detail)
        .def("toStringLong", &Dim2Edge::toStringLong)
        .def("__str__", &Dim2Edge::str)
        .def("ordering", &Dim2Edge::ordering)
        .def("faceNumber", &Dim2Edge::faceNumber)
        .def("containsVertex", &Dim2Edge::containsVertex)
        .def(regina::python::add_eq_operators())
        .staticmethod("ordering")
        .staticmethod("faceNumber")
        .staticmethod("containsVertex")
    ;

    scope().attr("Dim2EdgeEmbedding") = scope().attr("FaceEmbedding2_1");
    scope().attr("Dim2Edge") = scope().attr("Face2_1");
}

