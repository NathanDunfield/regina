
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Python Interface                                                      *
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

#include <boost/python.hpp>
#include "dim4/dim4component.h"
#include "dim4/dim4edge.h"
#include "dim4/dim4pentachoron.h"
#include "dim4/dim4tetrahedron.h"
#include "dim4/dim4triangle.h"
#include "dim4/dim4triangulation.h"
#include "dim4/dim4vertex.h"

using namespace boost::python;
using regina::Dim4Pentachoron;

void addDim4Pentachoron() {
    class_<Dim4Pentachoron, bases<regina::ShareableObject>,
            std::auto_ptr<Dim4Pentachoron>, boost::noncopyable>(
            "Dim4Pentachoron", no_init)
        .def("getDescription", &Dim4Pentachoron::getDescription,
            return_value_policy<return_by_value>())
        .def("setDescription", &Dim4Pentachoron::setDescription)
        .def("adjacentPentachoron", &Dim4Pentachoron::adjacentPentachoron,
            return_value_policy<reference_existing_object>())
        .def("adjacentSimplex", &Dim4Pentachoron::adjacentSimplex,
            return_value_policy<reference_existing_object>())
        .def("adjacentGluing", &Dim4Pentachoron::adjacentGluing)
        .def("adjacentFacet", &Dim4Pentachoron::adjacentFacet)
        .def("hasBoundary", &Dim4Pentachoron::hasBoundary)
        .def("joinTo", &Dim4Pentachoron::joinTo)
        .def("unjoin", &Dim4Pentachoron::unjoin,
            return_value_policy<reference_existing_object>())
        .def("isolate", &Dim4Pentachoron::isolate)
        .def("getTriangulation", &Dim4Pentachoron::getTriangulation,
            return_value_policy<reference_existing_object>())
        .def("getComponent", &Dim4Pentachoron::getComponent,
            return_value_policy<reference_existing_object>())
        .def("getVertex", &Dim4Pentachoron::getVertex,
            return_value_policy<reference_existing_object>())
        .def("getEdge", &Dim4Pentachoron::getEdge,
            return_value_policy<reference_existing_object>())
        .def("getTriangle", &Dim4Pentachoron::getTriangle,
            return_value_policy<reference_existing_object>())
        .def("getTetrahedron", &Dim4Pentachoron::getTetrahedron,
            return_value_policy<reference_existing_object>())
        .def("getVertexMapping", &Dim4Pentachoron::getVertexMapping)
        .def("getEdgeMapping", &Dim4Pentachoron::getEdgeMapping)
        .def("getTriangleMapping", &Dim4Pentachoron::getTriangleMapping)
        .def("getTetrahedronMapping", &Dim4Pentachoron::getTetrahedronMapping)
        .def("orientation", &Dim4Pentachoron::orientation)
    ;
}

