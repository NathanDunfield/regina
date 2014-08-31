
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

#include "engine.h"
#include "shareableobject.h"
#include "regina-config.h"

void addGlobalArray();

void addAlgebra();
void addAngle();
void addCensus();
void addDim2();
void addFile();
void addForeign();
void addManifold();
void addMaths();
void addPacket();
void addProgress();
#ifndef EXCLUDE_SNAPPEA
void addSnapPea();
#endif
void addSplit();
void addSubcomplex();
void addSurfaces();
void addTriangulation();
void addUtilities();

using boost::python::self;
using regina::ShareableObject;

namespace {
    std::string welcome() {
        return std::string(PACKAGE_STRING) +
            "\nSoftware for 3-manifold topology and normal surface theory" +
            "\nCopyright (c) 1999-2014, The Regina development team";
    }

    void shareableWriteTextShort(const ShareableObject& obj) {
        obj.writeTextShort(std::cout);
    }

    void shareableWriteTextLong(const ShareableObject& obj) {
        obj.writeTextLong(std::cout);
    }
}

/**
 * Make == work like pointer equality for ShareableObject.
 */
namespace regina {
    static inline bool operator == (const ShareableObject& a,
            const ShareableObject& b) {
        return &a == &b;
    }

    static inline bool operator != (const ShareableObject& a,
            const ShareableObject& b) {
        return &a != &b;
    }
}

BOOST_PYTHON_MODULE(engine) {
    // Welcome string:

    boost::python::def("welcome", welcome);

    // Wrappers for global array classes:

    addGlobalArray();

    // Core engine routines:

    boost::python::def("getVersionString", regina::getVersionString);
    boost::python::def("getVersionMajor", regina::getVersionMajor);
    boost::python::def("getVersionMinor", regina::getVersionMinor);
    boost::python::def("versionUsesUTF8", regina::versionUsesUTF8);
    boost::python::def("testEngine", regina::testEngine);

    // ShareableObject class:

    boost::python::class_<ShareableObject, boost::noncopyable>
            ("ShareableObject", boost::python::no_init)
        .def("writeTextShort", &shareableWriteTextShort)
        .def("writeTextLong", &shareableWriteTextLong)
        .def("str", &ShareableObject::str)
        .def("toString", &ShareableObject::toString)
        .def("detail", &ShareableObject::detail)
        .def("toStringLong", &ShareableObject::toStringLong)
        .def("__str__", &ShareableObject::str)
        .def(self == self)
        .def(self != self)
    ;

    // Components from subdirectories (in approximate dependency order):

    addUtilities();
    addProgress();
    addMaths();
    addAlgebra();
    addPacket();
    addTriangulation();
    addCensus();
    addFile();
    addForeign();
    addSplit();
#ifndef EXCLUDE_SNAPPEA
    addSnapPea();
#endif
    addSubcomplex();
    addManifold();
    addAngle();
    addSurfaces();
    addDim2();
}

