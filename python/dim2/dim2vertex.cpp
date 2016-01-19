
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
#include "dim2/dim2triangle.h"
#include "dim2/dim2triangulation.h"
#include "dim2/dim2vertex.h"
#include "../globalarray.h"
#include "../helpers.h"

using namespace boost::python;
using regina::Dim2Vertex;
using regina::Dim2VertexEmbedding;
using regina::Face;
using regina::FaceEmbedding;

namespace {
    boost::python::list Dim2Vertex_getEmbeddings_list(const Dim2Vertex* v) {
        boost::python::list ans;
        for (auto& emb: *v)
            ans.append(emb);
        return ans;
    }
}

void addDim2Vertex() {
    class_<FaceEmbedding<2, 0>>("FaceEmbedding2_0",
            init<regina::Dim2Triangle*, int>())
        .def(init<const Dim2VertexEmbedding&>())
        .def("simplex", &Dim2VertexEmbedding::simplex,
            return_value_policy<reference_existing_object>())
        .def("getSimplex", &Dim2VertexEmbedding::getSimplex,
            return_value_policy<reference_existing_object>())
        .def("triangle", &Dim2VertexEmbedding::triangle,
            return_value_policy<reference_existing_object>())
        .def("getTriangle", &Dim2VertexEmbedding::getTriangle,
            return_value_policy<reference_existing_object>())
        .def("face", &Dim2VertexEmbedding::face)
        .def("getFace", &Dim2VertexEmbedding::getFace)
        .def("vertex", &Dim2VertexEmbedding::vertex)
        .def("getVertex", &Dim2VertexEmbedding::getVertex)
        .def("vertices", &Dim2VertexEmbedding::vertices)
        .def("getVertices", &Dim2VertexEmbedding::getVertices)
        .def("str", &Dim2VertexEmbedding::str)
        .def("toString", &Dim2VertexEmbedding::toString)
        .def("detail", &Dim2VertexEmbedding::detail)
        .def("toStringLong", &Dim2VertexEmbedding::toStringLong)
        .def("__str__", &Dim2VertexEmbedding::str)
        .def(regina::python::add_eq_operators())
    ;

    class_<Face<2, 0>, std::auto_ptr<Face<2, 0>>, boost::noncopyable>
            ("Face2_0", no_init)
        .def("index", &Dim2Vertex::index)
        .def("isValid", &Dim2Vertex::isValid)
        .def("isLinkOrientable", &Dim2Vertex::isLinkOrientable)
        .def("embeddings", Dim2Vertex_getEmbeddings_list)
        .def("getEmbeddings", Dim2Vertex_getEmbeddings_list)
        .def("embedding", &Dim2Vertex::embedding,
            return_internal_reference<>())
        .def("getEmbedding", &Dim2Vertex::getEmbedding,
            return_internal_reference<>())
        .def("front", &Dim2Vertex::front,
            return_internal_reference<>())
        .def("back", &Dim2Vertex::back,
            return_internal_reference<>())
        .def("triangulation", &Dim2Vertex::triangulation,
            return_value_policy<reference_existing_object>())
        .def("getTriangulation", &Dim2Vertex::getTriangulation,
            return_value_policy<reference_existing_object>())
        .def("component", &Dim2Vertex::component,
            return_value_policy<reference_existing_object>())
        .def("getComponent", &Dim2Vertex::getComponent,
            return_value_policy<reference_existing_object>())
        .def("boundaryComponent", &Dim2Vertex::boundaryComponent,
            return_value_policy<reference_existing_object>())
        .def("getBoundaryComponent", &Dim2Vertex::getBoundaryComponent,
            return_value_policy<reference_existing_object>())
        .def("degree", &Dim2Vertex::degree)
        .def("getDegree", &Dim2Vertex::getDegree)
        .def("isBoundary", &Dim2Vertex::isBoundary)
        .def("str", &Dim2Vertex::str)
        .def("toString", &Dim2Vertex::toString)
        .def("detail", &Dim2Vertex::detail)
        .def("toStringLong", &Dim2Vertex::toStringLong)
        .def("__str__", &Dim2Vertex::str)
        .def("ordering", &Dim2Vertex::ordering)
        .def("faceNumber", &Dim2Vertex::faceNumber)
        .def("containsVertex", &Dim2Vertex::containsVertex)
        .def(regina::python::add_eq_operators())
        .staticmethod("ordering")
        .staticmethod("faceNumber")
        .staticmethod("containsVertex")
    ;

    scope().attr("Dim2VertexEmbedding") = scope().attr("FaceEmbedding2_0");
    scope().attr("Dim2Vertex") = scope().attr("Face2_0");
}

