
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Python Interface                                                      *
 *                                                                        *
 *  Copyright (c) 1999-2011, Ben Burton                                   *
 *  For further details contact Ben Burton (bab@debian.org).              *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or         *
 *  modify it under the terms of the GNU General Public License as        *
 *  published by the Free Software Foundation; either version 2 of the    *
 *  License, or (at your option) any later version.                       *
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
#include "dim4/dim4isomorphism.h"
#include "dim4/dim4triangulation.h"

using namespace boost::python;
using regina::Dim4Triangulation;

namespace {
    regina::Dim4Pentachoron* (Dim4Triangulation::*newPentachoron_void)() =
        &Dim4Triangulation::newPentachoron;
    regina::Dim4Pentachoron* (Dim4Triangulation::*newPentachoron_string)(
        const std::string&) = &Dim4Triangulation::newPentachoron;
    regina::Dim4Pentachoron* (Dim4Triangulation::*getPentachoron_non_const)(
        unsigned long) = &Dim4Triangulation::getPentachoron;
    bool (Dim4Triangulation::*twoZeroMove_triangle)(regina::Dim4Triangle*,
        bool, bool) = &Dim4Triangulation::twoZeroMove;
    bool (Dim4Triangulation::*twoZeroMove_edge)(regina::Dim4Edge*,
        bool, bool) = &Dim4Triangulation::twoZeroMove;

    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OL_simplifyToLocalMinimum,
        Dim4Triangulation::simplifyToLocalMinimum, 0, 1);
    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OL_fourTwoMove,
        Dim4Triangulation::fourTwoMove, 1, 3);
    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OL_threeThreeMove,
        Dim4Triangulation::threeThreeMove, 1, 3);
    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OL_twoFourMove,
        Dim4Triangulation::twoFourMove, 1, 3);
    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OL_twoZeroMove,
        Dim4Triangulation::twoZeroMove, 1, 3);
    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OL_openBook,
        Dim4Triangulation::openBook, 1, 3);
    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OL_shellBoundary,
        Dim4Triangulation::shellBoundary, 1, 3);
    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OL_collapseEdge,
        Dim4Triangulation::collapseEdge, 1, 3);
    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OL_isoSig,
        Dim4Triangulation::isoSig, 0, 1);

    boost::python::list Dim4_getPentachora_list(Dim4Triangulation& t) {
        boost::python::list ans;
        for (Dim4Triangulation::PentachoronIterator it =
                t.getPentachora().begin(); it != t.getPentachora().end(); ++it)
            ans.append(boost::python::ptr(*it));
        return ans;
    }

    boost::python::list Dim4_getComponents_list(Dim4Triangulation& t) {
        boost::python::list ans;
        for (Dim4Triangulation::ComponentIterator it =
                t.getComponents().begin(); it != t.getComponents().end(); ++it)
            ans.append(boost::python::ptr(*it));
        return ans;
    }

    boost::python::list Dim4_getBoundaryComponents_list(Dim4Triangulation& t) {
        boost::python::list ans;
        for (Dim4Triangulation::BoundaryComponentIterator it =
                t.getBoundaryComponents().begin();
                it != t.getBoundaryComponents().end(); ++it)
            ans.append(boost::python::ptr(*it));
        return ans;
    }

    boost::python::list Dim4_getVertices_list(Dim4Triangulation& t) {
        boost::python::list ans;
        for (Dim4Triangulation::VertexIterator it =
                t.getVertices().begin(); it != t.getVertices().end(); ++it)
            ans.append(boost::python::ptr(*it));
        return ans;
    }

    boost::python::list Dim4_getEdges_list(Dim4Triangulation& t) {
        boost::python::list ans;
        for (Dim4Triangulation::EdgeIterator it =
                t.getEdges().begin(); it != t.getEdges().end(); ++it)
            ans.append(boost::python::ptr(*it));
        return ans;
    }

    boost::python::list Dim4_getTriangles_list(Dim4Triangulation& t) {
        boost::python::list ans;
        for (Dim4Triangulation::TriangleIterator it =
                t.getTriangles().begin(); it != t.getTriangles().end(); ++it)
            ans.append(boost::python::ptr(*it));
        return ans;
    }

    boost::python::list Dim4_getTetrahedra_list(Dim4Triangulation& t) {
        boost::python::list ans;
        for (Dim4Triangulation::TetrahedronIterator it =
                t.getTetrahedra().begin(); it != t.getTetrahedra().end(); ++it)
            ans.append(boost::python::ptr(*it));
        return ans;
    }

    regina::Dim4Isomorphism* isIsomorphicTo_ptr(Dim4Triangulation& t,
            Dim4Triangulation& s) {
        return t.isIsomorphicTo(s).release();
    }

    regina::Dim4Isomorphism* isContainedIn_ptr(Dim4Triangulation& t,
            Dim4Triangulation& s) {
        return t.isContainedIn(s).release();
    }

    void simplifiedFundamentalGroup_own(Dim4Triangulation& tri,
            std::auto_ptr<regina::NGroupPresentation> group) {
        tri.simplifiedFundamentalGroup(group.release());
    }
}

void addDim4Triangulation() {
    scope s = class_<Dim4Triangulation, bases<regina::NPacket>,
            std::auto_ptr<Dim4Triangulation>,
            boost::noncopyable>("Dim4Triangulation")
        .def(init<const Dim4Triangulation&>())
        .def("getNumberOfPentachora", &Dim4Triangulation::getNumberOfPentachora)
        .def("getNumberOfSimplices", &Dim4Triangulation::getNumberOfSimplices)
        .def("getPentachora", Dim4_getPentachora_list)
        .def("getSimplices", Dim4_getPentachora_list)
        .def("getPentachoron", getPentachoron_non_const,
            return_value_policy<reference_existing_object>())
        .def("getSimplex", getPentachoron_non_const,
            return_value_policy<reference_existing_object>())
        .def("pentachoronIndex", &Dim4Triangulation::pentachoronIndex)
        .def("simplexIndex", &Dim4Triangulation::simplexIndex)
        .def("newPentachoron", newPentachoron_void,
            return_value_policy<reference_existing_object>())
        .def("newSimplex", newPentachoron_void,
            return_value_policy<reference_existing_object>())
        .def("newPentachoron", newPentachoron_string,
            return_value_policy<reference_existing_object>())
        .def("newSimplex", newPentachoron_string,
            return_value_policy<reference_existing_object>())
        .def("removePentachoron", &Dim4Triangulation::removePentachoron)
        .def("removeSimplex", &Dim4Triangulation::removeSimplex)
        .def("removePentachoronAt", &Dim4Triangulation::removePentachoronAt)
        .def("removeSimplexAt", &Dim4Triangulation::removeSimplexAt)
        .def("removeAllPentachora", &Dim4Triangulation::removeAllPentachora)
        .def("removeAllSimplices", &Dim4Triangulation::removeAllSimplices)
        .def("swapContents", &Dim4Triangulation::swapContents)
        .def("moveContentsTo", &Dim4Triangulation::moveContentsTo)
        .def("getNumberOfComponents", &Dim4Triangulation::getNumberOfComponents)
        .def("getNumberOfBoundaryComponents",
            &Dim4Triangulation::getNumberOfBoundaryComponents)
        .def("getNumberOfVertices", &Dim4Triangulation::getNumberOfVertices)
        .def("getNumberOfEdges", &Dim4Triangulation::getNumberOfEdges)
        .def("getNumberOfTriangles", &Dim4Triangulation::getNumberOfTriangles)
        .def("getNumberOfTetrahedra", &Dim4Triangulation::getNumberOfTetrahedra)
        .def("getComponents", Dim4_getComponents_list)
        .def("getBoundaryComponents", Dim4_getBoundaryComponents_list)
        .def("getVertices", Dim4_getVertices_list)
        .def("getEdges", Dim4_getEdges_list)
        .def("getTriangles", Dim4_getTriangles_list)
        .def("getTetrahedra", Dim4_getTetrahedra_list)
        .def("getComponent", &Dim4Triangulation::getComponent,
            return_value_policy<reference_existing_object>())
        .def("getBoundaryComponent", &Dim4Triangulation::getBoundaryComponent,
            return_value_policy<reference_existing_object>())
        .def("getVertex", &Dim4Triangulation::getVertex,
            return_value_policy<reference_existing_object>())
        .def("getEdge", &Dim4Triangulation::getEdge,
            return_value_policy<reference_existing_object>())
        .def("getTriangle", &Dim4Triangulation::getTriangle,
            return_value_policy<reference_existing_object>())
        .def("getTetrahedron", &Dim4Triangulation::getTetrahedron,
            return_value_policy<reference_existing_object>())
        .def("componentIndex", &Dim4Triangulation::componentIndex)
        .def("boundaryComponentIndex",
            &Dim4Triangulation::boundaryComponentIndex)
        .def("vertexIndex", &Dim4Triangulation::vertexIndex)
        .def("edgeIndex", &Dim4Triangulation::edgeIndex)
        .def("triangleIndex", &Dim4Triangulation::triangleIndex)
        .def("tetrahedronIndex", &Dim4Triangulation::tetrahedronIndex)
        .def("isIsomorphicTo", isIsomorphicTo_ptr,
            return_value_policy<manage_new_object>())
        .def("makeCanonical", &Dim4Triangulation::makeCanonical)
        .def("isContainedIn", isContainedIn_ptr,
            return_value_policy<manage_new_object>())
        .def("getEulerCharTri", &Dim4Triangulation::getEulerCharTri)
        .def("getEulerCharManifold", &Dim4Triangulation::getEulerCharManifold)
        .def("isValid", &Dim4Triangulation::isValid)
        .def("isIdeal", &Dim4Triangulation::isIdeal)
        .def("hasBoundaryTetrahedra", &Dim4Triangulation::hasBoundaryTetrahedra)
        .def("isClosed", &Dim4Triangulation::isClosed)
        .def("isOrientable", &Dim4Triangulation::isOrientable)
        .def("isConnected", &Dim4Triangulation::isConnected)
        .def("getFundamentalGroup", &Dim4Triangulation::getFundamentalGroup,
            return_internal_reference<>())
        .def("simplifiedFundamentalGroup", simplifiedFundamentalGroup_own)
        .def("getHomologyH1", &Dim4Triangulation::getHomologyH1,
            return_internal_reference<>())
        .def("getHomologyH2", &Dim4Triangulation::getHomologyH2,
            return_internal_reference<>())
        .def("intelligentSimplify", &Dim4Triangulation::intelligentSimplify)
        .def("simplifyToLocalMinimum",
            &Dim4Triangulation::simplifyToLocalMinimum,
            OL_simplifyToLocalMinimum())
        .def("fourTwoMove", &Dim4Triangulation::fourTwoMove, OL_fourTwoMove())
        .def("threeThreeMove", &Dim4Triangulation::threeThreeMove,
            OL_threeThreeMove())
        .def("twoFourMove", &Dim4Triangulation::twoFourMove, OL_twoFourMove())
        .def("twoZeroMove", twoZeroMove_triangle, OL_twoZeroMove())
        .def("twoZeroMove", twoZeroMove_edge, OL_twoZeroMove())
        .def("openBook", &Dim4Triangulation::openBook, OL_openBook())
        .def("shellBoundary", &Dim4Triangulation::shellBoundary,
            OL_shellBoundary())
        .def("collapseEdge", &Dim4Triangulation::collapseEdge,
            OL_collapseEdge())
        .def("barycentricSubdivision",
            &Dim4Triangulation::barycentricSubdivision)
        .def("insertTriangulation", &Dim4Triangulation::insertTriangulation)
        .def("isoSig", &Dim4Triangulation::isoSig, OL_isoSig())
        .def("fromIsoSig", &Dim4Triangulation::fromIsoSig,
            return_value_policy<manage_new_object>())
        .def("dumpConstruction", &Dim4Triangulation::dumpConstruction)
        .staticmethod("fromIsoSig")
    ;

    s.attr("packetType") = Dim4Triangulation::packetType;

    implicitly_convertible<std::auto_ptr<Dim4Triangulation>,
        std::auto_ptr<regina::NPacket> >();
}

