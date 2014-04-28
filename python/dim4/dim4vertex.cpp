
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
#include "dim4/dim4boundarycomponent.h"
#include "dim4/dim4component.h"
#include "dim4/dim4pentachoron.h"
#include "dim4/dim4triangulation.h"
#include "dim4/dim4vertex.h"
#include "triangulation/ntriangulation.h"
#include "../globalarray.h"

using namespace boost::python;
using regina::Dim4Vertex;
using regina::Dim4VertexEmbedding;

namespace {
    boost::python::list Dim4Vertex_getEmbeddings_list(const Dim4Vertex* v) {
        const std::vector<Dim4VertexEmbedding>& embs = v->getEmbeddings();
        std::vector<Dim4VertexEmbedding>::const_iterator it;

        boost::python::list ans;
        for (it = embs.begin(); it != embs.end(); it++)
            ans.append(*it);
        return ans;
    }

    boost::python::tuple vertex_buildLinkDetail_bool(const Dim4Vertex* v,
            bool labels = true) {
        regina::Dim4Isomorphism* iso;
        regina::NTriangulation* link = v->buildLinkDetail(labels, &iso);
        return make_tuple(
            boost::python::object(boost::python::handle<>(
                boost::python::manage_new_object::
                apply<regina::NTriangulation*>::type()(link))),
            boost::python::object(boost::python::handle<>(
                boost::python::manage_new_object::
                apply<regina::Dim4Isomorphism*>::type()(iso))));
    }

    boost::python::tuple vertex_buildLinkDetail_void(const Dim4Vertex* v) {
        return vertex_buildLinkDetail_bool(v);
    }
}

void addDim4Vertex() {
    class_<Dim4VertexEmbedding>("Dim4VertexEmbedding",
            init<regina::Dim4Pentachoron*, int>())
        .def(init<const Dim4VertexEmbedding&>())
        .def("getPentachoron", &Dim4VertexEmbedding::getPentachoron,
            return_value_policy<reference_existing_object>())
        .def("getVertex", &Dim4VertexEmbedding::getVertex)
        .def("getVertices", &Dim4VertexEmbedding::getVertices)
        .def(self == self)
        .def(self != self)
    ;

    scope s = class_<Dim4Vertex, bases<regina::ShareableObject>,
            std::auto_ptr<Dim4Vertex>, boost::noncopyable>
            ("Dim4Vertex", no_init)
        .def("getEmbeddings", Dim4Vertex_getEmbeddings_list)
        .def("getNumberOfEmbeddings", &Dim4Vertex::getNumberOfEmbeddings)
        .def("getEmbedding", &Dim4Vertex::getEmbedding,
            return_internal_reference<>())
        .def("getTriangulation", &Dim4Vertex::getTriangulation,
            return_value_policy<reference_existing_object>())
        .def("getComponent", &Dim4Vertex::getComponent,
            return_value_policy<reference_existing_object>())
        .def("getBoundaryComponent", &Dim4Vertex::getBoundaryComponent,
            return_value_policy<reference_existing_object>())
        .def("getDegree", &Dim4Vertex::getDegree)
        .def("getLink", &Dim4Vertex::getLink,
            return_value_policy<reference_existing_object>())
        .def("buildLink", &Dim4Vertex::buildLink,
            return_value_policy<reference_existing_object>())
        .def("buildLinkDetail", vertex_buildLinkDetail_void)
        .def("buildLinkDetail", vertex_buildLinkDetail_bool)
        .def("isValid", &Dim4Vertex::isValid)
        .def("isIdeal", &Dim4Vertex::isIdeal)
        .def("isBoundary", &Dim4Vertex::isBoundary)
    ;
}

