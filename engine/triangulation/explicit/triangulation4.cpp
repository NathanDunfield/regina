
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2018, Ben Burton                                   *
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

#include "triangulation/detail/isosig-impl.h"
#include "triangulation/detail/pachner-impl.h"
#include "triangulation/detail/skeleton-impl.h"
#include "triangulation/dim4.h"

namespace regina { namespace detail {

template REGINA_API std::string TriangulationBase<4>::isoSigFrom(
    size_t, const Perm<5>&, Isomorphism<4>*) const;
template REGINA_API std::string TriangulationBase<4>::isoSig(
    Isomorphism<4>**) const;
template REGINA_API Triangulation<4>* TriangulationBase<4>::fromIsoSig(
    const std::string&);
template REGINA_API size_t TriangulationBase<4>::isoSigComponentSize(
    const std::string&);

template REGINA_API void TriangulationBase<4>::calculateSkeleton();
template REGINA_API void TriangulationBase<4>::clearBaseProperties();
template REGINA_API void TriangulationBase<4>::swapBaseProperties(
    TriangulationBase<4>&);

template REGINA_API bool PachnerHelper<4, 0>::pachner(
    Triangulation<4>*, Face<4, 0>*, bool, bool);
template REGINA_API bool PachnerHelper<4, 1>::pachner(
    Triangulation<4>*, Face<4, 1>*, bool, bool);
template REGINA_API bool PachnerHelper<4, 2>::pachner(
    Triangulation<4>*, Face<4, 2>*, bool, bool);
template REGINA_API bool PachnerHelper<4, 3>::pachner(
    Triangulation<4>*, Face<4, 3>*, bool, bool);
template REGINA_API bool PachnerHelper<4, 4>::pachner(
    Triangulation<4>*, Face<4, 4>*, bool, bool);
template REGINA_API bool PachnerHelper<4, 4>::pachnerOld(
    Triangulation<4>*, Face<4, 4>*, bool, bool);

} }
