
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

#include "surfaces/coordregistry.h"
#include "surfaces/nnormalsurfacelist.h"
#include "triangulation/ntriangulation.h"
#include "utilities/xmlutils.h"

namespace regina {

namespace {
    // Since legacy coordinate systems don't appear in the coordinate system
    // registry, give them a consistent name here.
    const char* AN_LEGACY_NAME =
        "Legacy standard almost normal (pruned tri-quad-oct)";
}

const NormalCoords NNormalSurfaceList::STANDARD = NS_STANDARD;;
const NormalCoords NNormalSurfaceList::QUAD = NS_QUAD;
const NormalCoords NNormalSurfaceList::AN_LEGACY = NS_AN_LEGACY;
const NormalCoords NNormalSurfaceList::AN_QUAD_OCT = NS_AN_QUAD_OCT;
const NormalCoords NNormalSurfaceList::AN_STANDARD = NS_AN_STANDARD;
const NormalCoords NNormalSurfaceList::EDGE_WEIGHT = NS_EDGE_WEIGHT;
const NormalCoords NNormalSurfaceList::FACE_ARCS = NS_TRIANGLE_ARCS;
const NormalCoords NNormalSurfaceList::ORIENTED = NS_ORIENTED;
const NormalCoords NNormalSurfaceList::ORIENTED_QUAD = NS_ORIENTED_QUAD;

void NNormalSurfaceList::writeAllSurfaces(std::ostream& out) const {
    unsigned long n = getNumberOfSurfaces();
    out << "Number of surfaces is " << n << '\n';
    for (unsigned long i = 0; i < n; i++) {
        getSurface(i)->writeTextShort(out);
        out << '\n';
    }
}

namespace {
    struct ZeroVector : public Returns<NNormalSurfaceVector*> {
        const NTriangulation* tri_;

        ZeroVector(const NTriangulation* tri) : tri_(tri) {}

        template <typename Coords>
        inline NNormalSurfaceVector* operator() (Coords) {
            return Coords::Class::makeZeroVector(tri_);
        }
    };
}

NNormalSurfaceVector* makeZeroVector(const NTriangulation* triangulation,
        NormalCoords coords) {
    return forCoords(coords, ZeroVector(triangulation), 0);
}

namespace {
    struct MatchingEquations : public Returns<NMatrixInt*> {
        const NTriangulation* tri_;

        MatchingEquations(const NTriangulation* tri) : tri_(tri) {}

        template <typename Coords>
        inline NMatrixInt* operator() (Coords) {
            return Coords::Class::makeMatchingEquations(tri_);
        }
    };
}

NMatrixInt* makeMatchingEquations(const NTriangulation* triangulation,
        NormalCoords coords) {
    return forCoords(coords, MatchingEquations(triangulation), 0);
}

namespace {
    struct EmbeddedConstraints : public Returns<NEnumConstraintList*> {
        const NTriangulation* tri_;

        EmbeddedConstraints(const NTriangulation* tri) : tri_(tri) {}

        template <typename Coords>
        inline NEnumConstraintList* operator() (Coords) {
            return Coords::Class::makeEmbeddedConstraints(tri_);
        }
    };
}

NEnumConstraintList* makeEmbeddedConstraints(
        const NTriangulation* triangulation, NormalCoords coords) {
    return forCoords(coords, EmbeddedConstraints(triangulation), 0);
}

NTriangulation* NNormalSurfaceList::getTriangulation() const {
    return dynamic_cast<NTriangulation*>(getTreeParent());
}

namespace {
    struct AlmostNormalFunction : public Returns<bool> {
        template <typename Coords>
        inline bool operator() (Coords f) { return f.almostNormal; }
    };
}

bool NNormalSurfaceList::allowsAlmostNormal() const {
    if (coords_ == NS_AN_LEGACY)
        return true;
    else
        return forCoords(coords_, AlmostNormalFunction(), false);
}

namespace {
    struct SpunFunction : public Returns<bool> {
        template <typename Coords>
        inline bool operator() (Coords f) { return f.spun; }
    };
}

bool NNormalSurfaceList::allowsSpun() const {
    // Both the default and the NS_AN_LEGACY cases should return false.
    return forCoords(coords_, SpunFunction(), false);
}

namespace {
    struct OrientedFunction : public Returns<bool> {
        template <typename Coords>
        inline bool operator() (Coords f) { return f.oriented; }
    };
}

bool NNormalSurfaceList::allowsOriented() const {
    // Both the default and the NS_AN_LEGACY cases should return false.
    return forCoords(coords_, OrientedFunction(), false);
}

namespace {
    struct NameFunction : public Returns<const char*> {
        template <typename Coords>
        inline const char* operator() (Coords f) { return f.name(); }
    };
}

void NNormalSurfaceList::writeTextShort(std::ostream& out) const {
    out << surfaces.size();

    if (which_.has(regina::NS_EMBEDDED_ONLY))
        out << " embedded,";
    else if (which_.has(regina::NS_IMMERSED_SINGULAR))
        out << " embedded / immersed / singular,";
    else
        out << " unknown,";

    if (which_.has(regina::NS_VERTEX))
        out << " vertex";
    else if (which_.has(regina::NS_FUNDAMENTAL))
        out << " fundamental";
    else if (which_.has(regina::NS_CUSTOM))
        out << " custom";
    else if (which_.has(regina::NS_LEGACY))
        out << " legacy";
    else
        out << " unknown";

    out << " surface";
    if (surfaces.size() != 1)
        out << 's';

    out << " (";
    if (coords_ == NS_AN_LEGACY)
        out << AN_LEGACY_NAME;
    else
        out << forCoords(coords_, NameFunction(), "Unknown");
    out << ')';
}

void NNormalSurfaceList::writeTextLong(std::ostream& out) const {
    if (which_.has(regina::NS_EMBEDDED_ONLY))
        out << "Embedded,";
    else if (which_.has(regina::NS_IMMERSED_SINGULAR))
        out << "Embedded / immersed / singular,";
    else
        out << "Unknown,";

    if (which_.has(regina::NS_VERTEX))
        out << " vertex";
    else if (which_.has(regina::NS_FUNDAMENTAL))
        out << " fundamental";
    else if (which_.has(regina::NS_CUSTOM))
        out << " custom";
    else if (which_.has(regina::NS_LEGACY))
        out << " legacy";
    else
        out << " unknown";

    out << " surfaces\n";

    out << "Coordinates: ";
    if (coords_ == NS_AN_LEGACY)
        out << AN_LEGACY_NAME << '\n';
    else
        out << forCoords(coords_, NameFunction(), "Unknown") << '\n';

    writeAllSurfaces(out);
}

void NNormalSurfaceList::writeXMLPacketData(std::ostream& out) const {
    // Write the surface list parameters.
    out << "  <params "
        << "type=\"" << which_.intValue() << "\" "
        << "algorithm=\"" << algorithm_.intValue() << "\" "
        << "flavourid=\"" << coords_ << "\"\n";
    out << "\tflavour=\"";
    if (coords_ == NS_AN_LEGACY)
        out << regina::xml::xmlEncodeSpecialChars(AN_LEGACY_NAME);
    else
        out << regina::xml::xmlEncodeSpecialChars(forCoords(
            coords_, NameFunction(), "Unknown"));
    out << "\"/>\n";

    // Write the individual surfaces.
    std::vector<NNormalSurface*>::const_iterator it;
    for (it = surfaces.begin(); it != surfaces.end(); it++)
        (*it)->writeXMLData(out);
}

NPacket* NNormalSurfaceList::internalClonePacket(NPacket* /* parent */) const {
    NNormalSurfaceList* ans = new NNormalSurfaceList(
        coords_, which_, algorithm_);
    transform(surfaces.begin(), surfaces.end(), back_inserter(ans->surfaces),
        FuncNewClonePtr<NNormalSurface>());
    return ans;
}

} // namespace regina

