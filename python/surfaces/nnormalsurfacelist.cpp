
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

#include "regina-config.h" // For EXCLUDE_NORMALIZ

#include <boost/python.hpp>
#include "maths/nmatrixint.h"
#include "progress/nprogresstracker.h"
#include "surfaces/nnormalsurfacelist.h"
#include "triangulation/ntriangulation.h"
#include "../utilities/equality.h"

using namespace boost::python;
using regina::NNormalSurfaceList;

namespace {
    void writeAllSurfaces_stdio(const NNormalSurfaceList& s) {
        s.writeAllSurfaces(std::cout);
    }

    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OL_saveCSVStandard,
        NNormalSurfaceList::saveCSVStandard, 1, 2);
    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(OL_saveCSVEdgeWeight,
        NNormalSurfaceList::saveCSVEdgeWeight, 1, 2);

    // Write manual overload wrappers since these are static member functions.
    NNormalSurfaceList* unified_2(regina::NTriangulation* owner,
            regina::NormalCoords coords) {
        return NNormalSurfaceList::enumerate(owner, coords);
    }
    NNormalSurfaceList* unified_3(regina::NTriangulation* owner,
            regina::NormalCoords coords, regina::NormalList which) {
        return NNormalSurfaceList::enumerate(owner, coords, which);
    }
    NNormalSurfaceList* unified_4(regina::NTriangulation* owner,
            regina::NormalCoords coords, regina::NormalList which,
            regina::NormalAlg algHints) {
        return NNormalSurfaceList::enumerate(owner, coords, which, algHints);
    }
    NNormalSurfaceList* unified_5(regina::NTriangulation* owner,
            regina::NormalCoords coords, regina::NormalList which,
            regina::NormalAlg algHints, regina::NProgressTracker* tracker) {
        return NNormalSurfaceList::enumerate(owner, coords, which, algHints,
            tracker);
    }

    NNormalSurfaceList* enumerate_3(regina::NTriangulation* owner,
            regina::NormalCoords coords, bool embedded) {
        return NNormalSurfaceList::enumerate(owner, coords, embedded);
    }
    NNormalSurfaceList* enumerate_4(regina::NTriangulation* owner,
            regina::NormalCoords coords, bool embedded,
            regina::NProgressTracker* tracker) {
        return NNormalSurfaceList::enumerate(owner, coords, embedded, tracker);
    }

#ifndef EXCLUDE_NORMALIZ
    NNormalSurfaceList* enumerateFundPrimal_2(regina::NTriangulation* owner,
            regina::NormalCoords coords) {
        return NNormalSurfaceList::enumerateFundPrimal(owner, coords);
    }
    NNormalSurfaceList* enumerateFundPrimal_3(regina::NTriangulation* owner,
            regina::NormalCoords coords, bool embedded) {
        return NNormalSurfaceList::enumerateFundPrimal(owner, coords,
            embedded);
    }
    NNormalSurfaceList* enumerateFundPrimal_4(regina::NTriangulation* owner,
            regina::NormalCoords coords, bool embedded,
            regina::NNormalSurfaceList* vtxSurfaces) {
        return NNormalSurfaceList::enumerateFundPrimal(owner, coords,
            embedded, vtxSurfaces);
    }
    NNormalSurfaceList* enumerateFundPrimal_5(regina::NTriangulation* owner,
            regina::NormalCoords coords, bool embedded,
            regina::NNormalSurfaceList* vtxSurfaces,
            regina::NProgressTracker* tracker) {
        return NNormalSurfaceList::enumerateFundPrimal(owner, coords,
            embedded, vtxSurfaces, tracker);
    }
#endif

    NNormalSurfaceList* enumerateFundDual_2(regina::NTriangulation* owner,
            regina::NormalCoords coords) {
        return NNormalSurfaceList::enumerateFundDual(owner, coords);
    }
    NNormalSurfaceList* enumerateFundDual_3(regina::NTriangulation* owner,
            regina::NormalCoords coords, bool embedded) {
        return NNormalSurfaceList::enumerateFundDual(owner, coords,
            embedded);
    }
    NNormalSurfaceList* enumerateFundDual_4(regina::NTriangulation* owner,
            regina::NormalCoords coords, bool embedded,
            regina::NProgressTracker* tracker) {
        return NNormalSurfaceList::enumerateFundDual(owner, coords,
            embedded, tracker);
    }

#ifndef EXCLUDE_NORMALIZ
    NNormalSurfaceList* enumerateFundFullCone_2(regina::NTriangulation* owner,
            regina::NormalCoords coords) {
        return NNormalSurfaceList::enumerateFundFullCone(owner, coords);
    }
    NNormalSurfaceList* enumerateFundFullCone_3(regina::NTriangulation* owner,
            regina::NormalCoords coords, bool embedded) {
        return NNormalSurfaceList::enumerateFundFullCone(owner, coords,
            embedded);
    }
#endif

    NNormalSurfaceList* enumerateFundCD_2(regina::NTriangulation* owner,
            regina::NormalCoords coords) {
        return NNormalSurfaceList::enumerateFundCD(owner, coords);
    }
    NNormalSurfaceList* enumerateFundCD_3(regina::NTriangulation* owner,
            regina::NormalCoords coords, bool embedded) {
        return NNormalSurfaceList::enumerateFundCD(owner, coords, embedded);
    }
}

void addNNormalSurfaceList() {
    def("makeMatchingEquations",
        regina::makeMatchingEquations,
        return_value_policy<manage_new_object>());

    scope s = class_<NNormalSurfaceList,
            bases<regina::NPacket>,
            std::auto_ptr<NNormalSurfaceList>, boost::noncopyable>
            ("NNormalSurfaceList", no_init)
        .def("getFlavour", &NNormalSurfaceList::getFlavour)
        .def("flavour", &NNormalSurfaceList::flavour)
        .def("coords", &NNormalSurfaceList::coords)
        .def("which", &NNormalSurfaceList::which)
        .def("algorithm", &NNormalSurfaceList::algorithm)
        .def("allowsAlmostNormal", &NNormalSurfaceList::allowsAlmostNormal)
        .def("allowsSpun", &NNormalSurfaceList::allowsSpun)
        .def("allowsOriented", &NNormalSurfaceList::allowsOriented)
        .def("isEmbeddedOnly", &NNormalSurfaceList::isEmbeddedOnly)
        .def("getTriangulation", &NNormalSurfaceList::getTriangulation,
            return_value_policy<reference_existing_object>())
        .def("size", &NNormalSurfaceList::size)
        .def("getNumberOfSurfaces", &NNormalSurfaceList::getNumberOfSurfaces)
        .def("getSurface", &NNormalSurfaceList::getSurface,
            return_internal_reference<>())
        .def("writeAllSurfaces", writeAllSurfaces_stdio)
        .def("enumerate", unified_2,
            return_value_policy<reference_existing_object>())
        .def("enumerate", unified_3,
            return_value_policy<reference_existing_object>())
        .def("enumerate", unified_4,
            return_value_policy<reference_existing_object>())
        .def("enumerate", unified_5,
            return_value_policy<reference_existing_object>())
        .def("enumerate", enumerate_3,
            return_value_policy<reference_existing_object>())
        .def("enumerate", enumerate_4,
            return_value_policy<reference_existing_object>())
#ifndef EXCLUDE_NORMALIZ
        .def("enumerateFundPrimal", enumerateFundPrimal_2,
            return_value_policy<reference_existing_object>())
        .def("enumerateFundPrimal", enumerateFundPrimal_3,
            return_value_policy<reference_existing_object>())
        .def("enumerateFundPrimal", enumerateFundPrimal_4,
            return_value_policy<reference_existing_object>())
        .def("enumerateFundPrimal", enumerateFundPrimal_5,
            return_value_policy<reference_existing_object>())
#endif
        .def("enumerateFundDual", enumerateFundDual_2,
            return_value_policy<reference_existing_object>())
        .def("enumerateFundDual", enumerateFundDual_3,
            return_value_policy<reference_existing_object>())
        .def("enumerateFundDual", enumerateFundDual_4,
            return_value_policy<reference_existing_object>())
#ifndef EXCLUDE_NORMALIZ
        .def("enumerateFundFullCone", enumerateFundFullCone_2,
            return_value_policy<reference_existing_object>())
        .def("enumerateFundFullCone", enumerateFundFullCone_3,
            return_value_policy<reference_existing_object>())
#endif
        .def("enumerateFundCD", enumerateFundCD_2,
            return_value_policy<reference_existing_object>())
        .def("enumerateFundCD", enumerateFundCD_3,
            return_value_policy<reference_existing_object>())
        .def("enumerateStandardDirect",
            &NNormalSurfaceList::enumerateStandardDirect,
            return_value_policy<reference_existing_object>())
        .def("enumerateStandardANDirect",
            &NNormalSurfaceList::enumerateStandardANDirect,
            return_value_policy<reference_existing_object>())
        .def("quadToStandard", &NNormalSurfaceList::quadToStandard,
            return_value_policy<reference_existing_object>())
        .def("quadOctToStandardAN", &NNormalSurfaceList::quadOctToStandardAN,
            return_value_policy<reference_existing_object>())
        .def("standardToQuad", &NNormalSurfaceList::standardToQuad,
            return_value_policy<reference_existing_object>())
        .def("standardANToQuadOct", &NNormalSurfaceList::standardANToQuadOct,
            return_value_policy<reference_existing_object>())
        .def("filterForLocallyCompatiblePairs",
            &NNormalSurfaceList::filterForLocallyCompatiblePairs,
            return_value_policy<reference_existing_object>())
        .def("filterForDisjointPairs",
            &NNormalSurfaceList::filterForDisjointPairs,
            return_value_policy<reference_existing_object>())
        .def("filterForPotentiallyIncompressible",
            &NNormalSurfaceList::filterForPotentiallyIncompressible,
            return_value_policy<reference_existing_object>())
        .def("recreateMatchingEquations",
            &NNormalSurfaceList::recreateMatchingEquations,
            return_value_policy<manage_new_object>())
        .def("saveCSVStandard", &NNormalSurfaceList::saveCSVStandard,
            OL_saveCSVStandard())
        .def("saveCSVEdgeWeight", &NNormalSurfaceList::saveCSVEdgeWeight,
            OL_saveCSVEdgeWeight())
        EQUAL_BY_PTR(NNormalSurfaceList)
        .staticmethod("enumerate")
        .staticmethod("enumerateStandardDirect")
        .staticmethod("enumerateStandardANDirect")
#ifndef EXCLUDE_NORMALIZ
        .staticmethod("enumerateFundPrimal")
#endif
        .staticmethod("enumerateFundDual")
#ifndef EXCLUDE_NORMALIZ
        .staticmethod("enumerateFundFullCone")
#endif
        .staticmethod("enumerateFundCD")
    ;

    s.attr("packetType") = regina::PacketType(NNormalSurfaceList::packetType);
    s.attr("STANDARD") = regina::NS_STANDARD;
    s.attr("AN_STANDARD") = regina::NS_AN_STANDARD;
    s.attr("QUAD") = regina::NS_QUAD;
    s.attr("AN_QUAD_OCT") = regina::NS_AN_QUAD_OCT;
    s.attr("EDGE_WEIGHT") = regina::NS_EDGE_WEIGHT;
    s.attr("FACE_ARCS") = regina::NS_TRIANGLE_ARCS;
    s.attr("AN_LEGACY") = regina::NS_AN_LEGACY;
    s.attr("ORIENTED") = regina::NS_ORIENTED;
    s.attr("ORIENTED_QUAD") = regina::NS_ORIENTED_QUAD;

    implicitly_convertible<std::auto_ptr<NNormalSurfaceList>,
        std::auto_ptr<regina::NPacket> >();
}

