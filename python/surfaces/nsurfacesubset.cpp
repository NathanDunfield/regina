
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
#include "surfaces/nsurfacefilter.h"
#include "surfaces/nsurfacesubset.h"
#include "triangulation/ntriangulation.h"
#include "../utilities/equality.h"

using namespace boost::python;
using regina::NSurfaceSubset;

namespace {
    void writeAllSurfaces_stdio(const regina::NNormalSurfaceList& s) {
        s.writeAllSurfaces(std::cout);
    }
}

void addNSurfaceSubset() {
    scope s = class_<NSurfaceSubset, std::auto_ptr<NSurfaceSubset>,
            boost::noncopyable>("NSurfaceSubset",
            init<const regina::NNormalSurfaceList&,
                const regina::NSurfaceFilter&>())
        .def("getFlavour", &NSurfaceSubset::getFlavour)
        .def("coords", &NSurfaceSubset::coords)
        .def("allowsAlmostNormal", &NSurfaceSubset::allowsAlmostNormal)
        .def("allowsSpun", &NSurfaceSubset::allowsSpun)
        .def("allowsOriented", &NSurfaceSubset::allowsOriented)
        .def("isEmbeddedOnly", &NSurfaceSubset::isEmbeddedOnly)
        .def("getTriangulation", &NSurfaceSubset::getTriangulation,
            return_value_policy<reference_existing_object>())
        .def("size", &NSurfaceSubset::size)
        .def("getNumberOfSurfaces", &NSurfaceSubset::getNumberOfSurfaces)
        .def("getSurface", &NSurfaceSubset::getSurface,
            return_internal_reference<>())
        .def("writeAllSurfaces", writeAllSurfaces_stdio)
        .def("str", &NSurfaceSubset::str)
        .def("toString", &NSurfaceSubset::toString)
        .def("detail", &NSurfaceSubset::detail)
        .def("toStringLong", &NSurfaceSubset::toStringLong)
        .def("__str__", &NSurfaceSubset::str)
        EQUAL_BY_PTR(NSurfaceSubset)
    ;
}

