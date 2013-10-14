
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
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

/*! \file surfaces/hscoordregistry-impl.h
 *  \brief Contains the registry of all coordinate systems that can be used
 *  to create and store normal hypersurfaces in 4-manifold triangulations.
 *
 *  Each time a new coordinate system is created, this registry must be
 *  updated to:
 *
 *  - add a #include line for the corresponding vector subclass;
 *  - add a corresponding case to each implementation of forCoords().
 *
 *  See hscoordregistry.h for how other routines can use this registry.
 */

#ifndef __HSFLAVOURREGISTRY_IMPL_H
#ifndef __DOXYGEN
#define __HSFLAVOURREGISTRY_IMPL_H
#endif

#include "hypersurface/hscoordregistry.h"
#include "hypersurface/nhsstandard.h"

namespace regina {

template <typename FunctionObject>
inline typename FunctionObject::ReturnType forCoords(
        HyperCoords coords, FunctionObject func,
        typename FunctionObject::ReturnType defaultReturn) {
    switch (coords) {
        case HS_STANDARD : return func(HyperInfo<HS_STANDARD>());
        default: return defaultReturn;
    }
}

template <typename VoidFunctionObject>
inline void forCoords(HyperCoords coords, VoidFunctionObject func) {
    switch (coords) {
        case HS_STANDARD : func(HyperInfo<HS_STANDARD>()); break;
        default: break;
    }
}

/*@}*/

} // namespace regina

#endif // include guard __HSFLAVOURREGISTRY_IMPL_H

