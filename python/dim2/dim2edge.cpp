
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

using namespace boost::python;
using regina::Dim2Edge;
using regina::Dim2EdgeEmbedding;
using regina::python::GlobalArray;

namespace {
    GlobalArray<regina::NPerm3> Dim2Edge_ordering(Dim2Edge::ordering, 3);
}

void addDim2Edge() {
    class_<Dim2EdgeEmbedding>("Dim2EdgeEmbedding",
            init<regina::Dim2Triangle*, int>())
        .def(init<const Dim2EdgeEmbedding&>())
        .def("getTriangle", &Dim2EdgeEmbedding::getTriangle,
            return_value_policy<reference_existing_object>())
        .def("getEdge", &Dim2EdgeEmbedding::getEdge)
        .def("getVertices", &Dim2EdgeEmbedding::getVertices)
        .def(self == self)
        .def(self != self)
    ;

    scope s = class_<Dim2Edge, bases<regina::ShareableObject>,
            std::auto_ptr<Dim2Edge>, boost::noncopyable>("Dim2Edge", no_init)
        .def("index", &Dim2Edge::index)
        .def("getNumberOfEmbeddings", &Dim2Edge::getNumberOfEmbeddings)
        .def("getEmbedding", &Dim2Edge::getEmbedding,
            return_internal_reference<>())
        .def("getTriangulation", &Dim2Edge::getTriangulation,
            return_value_policy<reference_existing_object>())
        .def("getComponent", &Dim2Edge::getComponent,
            return_value_policy<reference_existing_object>())
        .def("getBoundaryComponent", &Dim2Edge::getBoundaryComponent,
            return_value_policy<reference_existing_object>())
        .def("getVertex", &Dim2Edge::getVertex,
            return_value_policy<reference_existing_object>())
        .def("isBoundary", &Dim2Edge::isBoundary)
    ;

    s.attr("ordering") = &Dim2Edge_ordering;
}

