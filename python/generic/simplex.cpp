
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
#include "generic/simplex.h"

using namespace boost::python;
using regina::Simplex;

template <int dim>
void addSimplex(const char* name) {
    class_<regina::Simplex<dim>, std::auto_ptr<regina::Simplex<2>>,
            boost::noncopyable>(name, no_init)
        .def("getDescription", &Simplex<dim>::getDescription,
            return_value_policy<return_by_value>())
        .def("setDescription", &Simplex<dim>::setDescription)
        .def("index", &Simplex<dim>::index)
        .def("adjacentSimplex", &Simplex<dim>::adjacentSimplex,
            return_value_policy<reference_existing_object>())
        .def("adjacentGluing", &Simplex<dim>::adjacentGluing)
        .def("adjacentFacet", &Simplex<dim>::adjacentFacet)
        .def("hasBoundary", &Simplex<dim>::hasBoundary)
        .def("joinTo", &Simplex<dim>::joinTo)
        .def("unjoin", &Simplex<dim>::unjoin,
            return_value_policy<reference_existing_object>())
        .def("isolate", &Simplex<dim>::isolate)
        .def("getTriangulation", &Simplex<dim>::getTriangulation,
            return_value_policy<reference_existing_object>())
        .def("str", &Simplex<dim>::str)
        .def("toString", &Simplex<dim>::toString)
        .def("detail", &Simplex<dim>::detail)
        .def("toStringLong", &Simplex<dim>::toStringLong)
        .def("__str__", &Simplex<dim>::str)
    ;
}

void addSimplex() {
    // boost::python::def("helper", regina::helper);

    // addSimplex<4>("Simplex4");
}

