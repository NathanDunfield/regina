
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Python Interface                                                      *
 *                                                                        *
 *  Copyright (c) 1999-2005, Ben Burton                                   *
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
 *  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,        *
 *  MA 02111-1307, USA.                                                   *
 *                                                                        *
 **************************************************************************/

/* end stub */

#include "subcomplex/nlayeredloop.h"
#include "triangulation/ncomponent.h"
#include "triangulation/nedge.h"
#include <boost/python.hpp>

using namespace boost::python;
using regina::NLayeredLoop;

void addNLayeredLoop() {
    class_<NLayeredLoop, bases<regina::NStandardTriangulation>,
            std::auto_ptr<NLayeredLoop>, boost::noncopyable>
            ("NLayeredLoop", no_init)
        .def("clone", &NLayeredLoop::clone,
            return_value_policy<manage_new_object>())
        .def("getLength", &NLayeredLoop::getLength)
        .def("getIndex", &NLayeredLoop::getIndex)
        .def("isTwisted", &NLayeredLoop::isTwisted)
        .def("getHinge", &NLayeredLoop::getHinge,
            return_value_policy<reference_existing_object>())
        .def("isLayeredLoop", &NLayeredLoop::isLayeredLoop,
            return_value_policy<manage_new_object>())
        .staticmethod("isLayeredLoop")
    ;

    implicitly_convertible<std::auto_ptr<NLayeredLoop>,
        std::auto_ptr<regina::NStandardTriangulation> >();
}

