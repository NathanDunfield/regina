
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

/*! \file surfaces/filterregistry-impl.h
 *  \brief Contains the registry of all normal surface filter classes that can
 *  be used to filter lists of normal surfaces in 3-manifold triangulations.
 *
 * Each time a new filter is created, this registry must be updated to:
 *
 * - add a #include line for the corresponding filter class;
 * - add a corresponding case to each implementation of forFilter().
 *
 * See filterregistry.h for how other routines can use this registry.
 */

#ifndef __FILTERREGISTRY_IMPL_H
#ifndef __DOXYGEN
#define __FILTERREGISTRY_IMPL_H
#endif

#include "surfaces/nsurfacefilter.h"
#include "surfaces/sfproperties.h"
#include "surfaces/sfcombination.h"

namespace regina {

template <typename FunctionObject>
inline typename FunctionObject::ReturnType forFilter(
        SurfaceFilterType filter, FunctionObject func,
        typename FunctionObject::ReturnType defaultReturn) {
    switch (filter) {
        case NS_FILTER_DEFAULT :
            return func(SurfaceFilterInfo<NS_FILTER_DEFAULT>());
        case NS_FILTER_PROPERTIES :
            return func(SurfaceFilterInfo<NS_FILTER_PROPERTIES>());
        case NS_FILTER_COMBINATION :
            return func(SurfaceFilterInfo<NS_FILTER_COMBINATION>());
        default: return defaultReturn;
    }
}

template <typename VoidFunctionObject>
inline void forFilter(SurfaceFilterType filter, VoidFunctionObject func) {
    switch (filter) {
        case NS_FILTER_DEFAULT :
            func(SurfaceFilterInfo<NS_FILTER_DEFAULT>()); break;
        case NS_FILTER_PROPERTIES :
            func(SurfaceFilterInfo<NS_FILTER_PROPERTIES>()); break;
        case NS_FILTER_COMBINATION :
            func(SurfaceFilterInfo<NS_FILTER_COMBINATION>()); break;
        default: break;
    }
}

} // namespace regina

#endif

