
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

/*! \file surfaces/filterregistry.h
 *  \brief Provides access to a registry of all normal surface filter classes
 *  that can be used to filter lists of normal surfaces in 3-manifold
 *  triangulations.
 *
 *  Each time a new filter is created, the file filterregistry-impl.h must be
 *  updated to include it.  Instructions on how to do this are included in
 *  filterregistry-impl.h.
 *
 *  External routines can access the registry by calling one of the
 *  forFilter() template functions defined in filterregistry.h.
 *
 *  \warning You should not include this header unless it is necessary,
 *  since it will automatically import every header for every filter class
 *  in the registry.
 */

// The old registry macros will silently compile but do nothing.
// This could lead to nasty surprises, so throw an error if it looks like
// people are still trying to use them.
#ifdef __FILTER_REGISTRY_BODY
#error "The old REGISTER_FILTER macros have been removed.  Use forFilter() instead."
#endif

#ifndef __FILTERREGISTRY_H
#ifndef __DOXYGEN
#define __FILTERREGISTRY_H
#endif

#include "surfaces/surfacefiltertype.h"
#include "utilities/registryutils.h"

namespace regina {

/**
 * \weakgroup surfaces
 * @{
 */

/**
 * Allows the user to call a template function whose template parameter
 * matches a given value of SurfaceFilterType, which is not known
 * until runtime.  In essence, this routine contains a switch/case statement
 * that runs through all possible normal surface filter types.
 *
 * The advantages of this routine are that (i) the user does not need to
 * repeatedly type such switch/case statements themselves; and (ii) if
 * a new filter type is added then only a small amount of code
 * needs to be extended to incorporate it.
 *
 * In detail: the function object \a func must define a templated
 * unary bracket operator, so that <tt>func(SurfaceFilterInfo<t>)</tt> is
 * defined for any valid SurfaceFilterType enum value \a t.  Then,
 * when the user calls <tt>forFilter(filter, func, defaultReturn)</tt>,
 * this routine will call <tt>func(SurfaceFilterInfo<filter>)</tt> and pass
 * back the corresponding return value.  If \a filter does not denote a valid
 * filter type, then forFilter() will pass back \a defaultReturn instead.
 *
 * There is also a two-argument variant of forFilter() that works with
 * void functions.
 *
 * \pre The function object must have a typedef \a ReturnType indicating
 * the return type of the corresponding templated unary bracket operator.
 * Inheriting from Returns<...> is a convenient way to ensure this.
 *
 * \ifacespython Not present.
 *
 * @param filter the given type of normal surface filter.
 * @param func the function object whose unary bracket operator we will
 * call with a SurfaceFilterInfo<filter> object.
 * @param defaultReturn the value to return if the given filter type
 * is not valid.
 * @return the return value from the corresponding unary bracket
 * operator of \a func, or \a defaultReturn if the given filter type
 * is not valid.
 */
template <typename FunctionObject>
typename FunctionObject::ReturnType forFilter(
        SurfaceFilterType filter, FunctionObject func,
        typename FunctionObject::ReturnType defaultReturn);

/**
 * Allows the user to call a template function whose template parameter
 * matches a given value of SurfaceFilterType, which is not known
 * until runtime.  In essence, this routine contains a switch/case statement
 * that runs through all possible normal surface filter types.
 *
 * The advantages of this routine are that (i) the user does not need to
 * repeatedly type such switch/case statements themselves; and (ii) if
 * a new filter type is added then only a small amount of code
 * needs to be extended to incorporate it.
 *
 * In detail: the function object \a func must define a templated
 * unary bracket operator, so that <tt>func(SurfaceFilterInfo<t>)</tt> is
 * defined for any valid SurfaceFilterType enum value \a t.  Then,
 * when the user calls <tt>forFilter(filter, func)</tt>,
 * this routine will call <tt>func(SurfaceFilterInfo<filter>)</tt> in turn.
 * If \a filter does not denote a valid filter type, then forFilter()
 * will do nothing.
 *
 * There is also a three-argument variant of forFilter() that works with
 * functions with return values.
 *
 * \ifacespython Not present.
 *
 * @param filter the given type of normal surface filter.
 * @param func the function object whose unary bracket operator we will
 * call with a SurfaceFilterInfo<filter> object.
 */
template <typename VoidFunctionObject>
void forFilter(SurfaceFilterType filter, VoidFunctionObject func);

/*@}*/

} // namespace regina

// Import template implementations:
#include "surfaces/filterregistry-impl.h"

#endif

