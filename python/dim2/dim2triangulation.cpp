
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
#include "algebra/ngrouppresentation.h"
#include "dim2/dim2isomorphism.h"
#include "dim2/dim2triangulation.h"
#include "../utilities/equality.h"

using namespace boost::python;
using regina::Dim2Triangulation;

namespace {
    regina::Dim2Triangle* (Dim2Triangulation::*newTriangle_void)() =
        &Dim2Triangulation::newTriangle;
    regina::Dim2Triangle* (Dim2Triangulation::*newTriangle_string)(
        const std::string&) = &Dim2Triangulation::newTriangle;
    regina::Dim2Triangle* (Dim2Triangulation::*getTriangle_non_const)(
        unsigned long) = &Dim2Triangulation::getTriangle;

    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OL_oneThreeMove,
        Dim2Triangulation::oneThreeMove, 1, 3);

    boost::python::list Dim2_getTriangles_list(Dim2Triangulation& t) {
        boost::python::list ans;
        for (Dim2Triangulation::TriangleIterator it =
                t.getTriangles().begin(); it != t.getTriangles().end(); ++it)
            ans.append(boost::python::ptr(*it));
        return ans;
    }

    boost::python::list Dim2_getComponents_list(Dim2Triangulation& t) {
        boost::python::list ans;
        for (Dim2Triangulation::ComponentIterator it =
                t.getComponents().begin(); it != t.getComponents().end(); ++it)
            ans.append(boost::python::ptr(*it));
        return ans;
    }

    boost::python::list Dim2_getBoundaryComponents_list(Dim2Triangulation& t) {
        boost::python::list ans;
        for (Dim2Triangulation::BoundaryComponentIterator it =
                t.getBoundaryComponents().begin();
                it != t.getBoundaryComponents().end(); ++it)
            ans.append(boost::python::ptr(*it));
        return ans;
    }

    boost::python::list Dim2_getVertices_list(Dim2Triangulation& t) {
        boost::python::list ans;
        for (Dim2Triangulation::VertexIterator it =
                t.getVertices().begin(); it != t.getVertices().end(); ++it)
            ans.append(boost::python::ptr(*it));
        return ans;
    }

    boost::python::list Dim2_getEdges_list(Dim2Triangulation& t) {
        boost::python::list ans;
        for (Dim2Triangulation::EdgeIterator it =
                t.getEdges().begin(); it != t.getEdges().end(); ++it)
            ans.append(boost::python::ptr(*it));
        return ans;
    }

    regina::Dim2Isomorphism* isIsomorphicTo_ptr(Dim2Triangulation& t,
            Dim2Triangulation& s) {
        return t.isIsomorphicTo(s).release();
    }

    regina::Dim2Isomorphism* isContainedIn_ptr(Dim2Triangulation& t,
            Dim2Triangulation& s) {
        return t.isContainedIn(s).release();
    }

    std::string isoSig_void(const Dim2Triangulation& t) {
        return t.isoSig();
    }

    boost::python::tuple isoSig_relabelling(const Dim2Triangulation& t) {
        regina::Dim2Isomorphism* iso;
        std::string sig = t.isoSig(&iso);
        return boost::python::make_tuple(
            sig,
            boost::python::object(boost::python::handle<>(
                boost::python::manage_new_object::
                apply<regina::Dim2Isomorphism*>::type()(iso))));
    }
}

void addDim2Triangulation() {
    scope s = class_<Dim2Triangulation, bases<regina::NPacket>,
            std::auto_ptr<Dim2Triangulation>,
            boost::noncopyable>("Dim2Triangulation")
        .def(init<const Dim2Triangulation&>())
        .def(init<const std::string&>())
        .def("getNumberOfTriangles", &Dim2Triangulation::getNumberOfTriangles)
        .def("getNumberOfSimplices", &Dim2Triangulation::getNumberOfSimplices)
        .def("getTriangles", Dim2_getTriangles_list)
        .def("getSimplices", Dim2_getTriangles_list)
        .def("getTriangle", getTriangle_non_const,
            return_value_policy<reference_existing_object>())
        .def("getSimplex", getTriangle_non_const,
            return_value_policy<reference_existing_object>())
        .def("triangleIndex", &Dim2Triangulation::triangleIndex)
        .def("simplexIndex", &Dim2Triangulation::simplexIndex)
        .def("newTriangle", newTriangle_void,
            return_value_policy<reference_existing_object>())
        .def("newSimplex", newTriangle_void,
            return_value_policy<reference_existing_object>())
        .def("newTriangle", newTriangle_string,
            return_value_policy<reference_existing_object>())
        .def("newSimplex", newTriangle_string,
            return_value_policy<reference_existing_object>())
        .def("removeTriangle", &Dim2Triangulation::removeTriangle)
        .def("removeSimplex", &Dim2Triangulation::removeSimplex)
        .def("removeTriangleAt", &Dim2Triangulation::removeTriangleAt)
        .def("removeSimplexAt", &Dim2Triangulation::removeSimplexAt)
        .def("removeAllTriangles", &Dim2Triangulation::removeAllTriangles)
        .def("removeAllSimplices", &Dim2Triangulation::removeAllSimplices)
        .def("swapContents", &Dim2Triangulation::swapContents)
        .def("moveContentsTo", &Dim2Triangulation::moveContentsTo)
        .def("getNumberOfComponents", &Dim2Triangulation::getNumberOfComponents)
        .def("getNumberOfBoundaryComponents",
            &Dim2Triangulation::getNumberOfBoundaryComponents)
        .def("getNumberOfVertices", &Dim2Triangulation::getNumberOfVertices)
        .def("getNumberOfEdges", &Dim2Triangulation::getNumberOfEdges)
        .def("getComponents", Dim2_getComponents_list)
        .def("getBoundaryComponents", Dim2_getBoundaryComponents_list)
        .def("getVertices", Dim2_getVertices_list)
        .def("getEdges", Dim2_getEdges_list)
        .def("getComponent", &Dim2Triangulation::getComponent,
            return_value_policy<reference_existing_object>())
        .def("getBoundaryComponent", &Dim2Triangulation::getBoundaryComponent,
            return_value_policy<reference_existing_object>())
        .def("getVertex", &Dim2Triangulation::getVertex,
            return_value_policy<reference_existing_object>())
        .def("getEdge", &Dim2Triangulation::getEdge,
            return_value_policy<reference_existing_object>())
        .def("componentIndex", &Dim2Triangulation::componentIndex)
        .def("boundaryComponentIndex",
            &Dim2Triangulation::boundaryComponentIndex)
        .def("vertexIndex", &Dim2Triangulation::vertexIndex)
        .def("edgeIndex", &Dim2Triangulation::edgeIndex)
        .def("isIdenticalTo", &Dim2Triangulation::isIdenticalTo)
        .def("isIsomorphicTo", isIsomorphicTo_ptr,
            return_value_policy<manage_new_object>())
        .def("makeCanonical", &Dim2Triangulation::makeCanonical)
        .def("isContainedIn", isContainedIn_ptr,
            return_value_policy<manage_new_object>())
        .def("isEmpty", &Dim2Triangulation::isEmpty)
        .def("size", &Dim2Triangulation::size)
        .def("isValid", &Dim2Triangulation::isValid)
        .def("getEulerChar", &Dim2Triangulation::getEulerChar)
        .def("isClosed", &Dim2Triangulation::isClosed)
        .def("hasBoundaryEdges", &Dim2Triangulation::hasBoundaryEdges)
        .def("getNumberOfBoundaryEdges",
            &Dim2Triangulation::getNumberOfBoundaryEdges)
        .def("isOrientable", &Dim2Triangulation::isOrientable)
        .def("isIdeal", &Dim2Triangulation::isIdeal)
        .def("isConnected", &Dim2Triangulation::isConnected)
        .def("isMinimal", &Dim2Triangulation::isMinimal)
        .def("oneThreeMove", &Dim2Triangulation::oneThreeMove,
            OL_oneThreeMove())
        .def("insertTriangulation", &Dim2Triangulation::insertTriangulation)
        .def("isoSig", isoSig_void)
        .def("isoSigDetail", isoSig_relabelling)
        .def("fromIsoSig", &Dim2Triangulation::fromIsoSig,
            return_value_policy<manage_new_object>())
        .def("isoSigComponentSize", &Dim2Triangulation::isoSigComponentSize)
        .def("dumpConstruction", &Dim2Triangulation::dumpConstruction)
        EQUAL_BY_PTR(Dim2Triangulation)
        .staticmethod("fromIsoSig")
        .staticmethod("isoSigComponentSize")
    ;

    s.attr("packetType") = regina::PacketType(Dim2Triangulation::packetType);

    implicitly_convertible<std::auto_ptr<Dim2Triangulation>,
        std::auto_ptr<regina::NPacket> >();
}

