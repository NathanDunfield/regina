
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
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

#include <vector>
#include "algebra/nxmlalgebrareader.h"
#include "triangulation/nxmltrireader.h"
#include "utilities/stringutils.h"

namespace regina {

/**
 * A unique namespace containing various specific-task packet readers.
 */
namespace {
    /**
     * Reads a single tetrahedron and its name and gluings.
     */
    class NTetrahedronReader : public NXMLElementReader {
        private:
            NTriangulation* tri;
            NTetrahedron* tet;

        public:
            NTetrahedronReader(NTriangulation* newTri, unsigned whichTet) :
                    tri(newTri), tet(tri->getTetrahedra()[whichTet]) {
            }

            virtual void startElement(const std::string&,
                    const regina::xml::XMLPropertyDict& props,
                    NXMLElementReader*) {
                tet->setDescription(props.lookup("desc"));
            }

            virtual void initialChars(const std::string& chars) {
                std::vector<std::string> tokens;
                if (basicTokenise(back_inserter(tokens), chars) != 8)
                    return;

                long tetIndex, permCode;
                NPerm4 perm;
                NTetrahedron* adjTet;
                int adjFace;
                for (int k = 0; k < 4; k ++) {
                    if (! valueOf(tokens[2 * k], tetIndex))
                        continue;
                    if (! valueOf(tokens[2 * k + 1], permCode))
                        continue;

                    if (tetIndex < 0 || tetIndex >=
                            static_cast<int>(tri->size()))
                        continue;
                    if (! NPerm4::isPermCode(
                            static_cast<unsigned char>(permCode)))
                        continue;

                    perm.setPermCode(static_cast<unsigned char>(permCode));
                    adjTet = tri->getTetrahedra()[tetIndex];
                    adjFace = perm[k];
                    if (adjTet == tet && adjFace == k)
                        continue;
                    if (tet->adjacentTetrahedron(k))
                        continue;
                    if (adjTet->adjacentTetrahedron(adjFace))
                        continue;

                    tet->joinTo(k, adjTet, perm);
                }
            }
    };

    /**
     * Reads an entire set of tetrahedra with their names and gluings.
     */
    class NTetrahedraReader : public NXMLElementReader {
        private:
            NTriangulation* tri;
            unsigned readTets;

        public:
            NTetrahedraReader(NTriangulation* newTri) : tri(newTri),
                    readTets(0) {
            }

            virtual void startElement(const std::string& /* tagName */,
                    const regina::xml::XMLPropertyDict& props,
                    NXMLElementReader*) {
                long nTets;
                if (valueOf(props.lookup("ntet"), nTets))
                    for ( ; nTets > 0; nTets--)
                        tri->newTetrahedron();
            }

            virtual NXMLElementReader* startSubElement(
                    const std::string& subTagName,
                    const regina::xml::XMLPropertyDict&) {
                if (subTagName == "tet") {
                    if (readTets < tri->size())
                        return new NTetrahedronReader(tri, readTets++);
                    else
                        return new NXMLElementReader();
                } else
                    return new NXMLElementReader();
            }
    };

    /**
     * Reads an abelian group property.
     */
    class NAbelianGroupPropertyReader : public NXMLElementReader {
        public:
            typedef NProperty<NAbelianGroup, StoreManagedPtr> PropType;

        private:
            PropType& prop;

        public:
            NAbelianGroupPropertyReader(PropType& newProp) : prop(newProp) {
            }

            virtual NXMLElementReader* startSubElement(
                    const std::string& subTagName,
                    const regina::xml::XMLPropertyDict&) {
                if (subTagName == "abeliangroup")
                    if (! prop.known())
                        return new NXMLAbelianGroupReader();
                return new NXMLElementReader();
            }

            virtual void endSubElement(const std::string& subTagName,
                    NXMLElementReader* subReader) {
                if (subTagName == "abeliangroup") {
                    NAbelianGroup* ans =
                        dynamic_cast<NXMLAbelianGroupReader*>(subReader)->
                        group();
                    if (ans)
                        prop = ans;
                }
            }
    };

    /**
     * Reads a group presentation property.
     */
    class NGroupPresentationPropertyReader : public NXMLElementReader {
        public:
            typedef NProperty<NGroupPresentation, StoreManagedPtr> PropType;

        private:
            PropType& prop;

        public:
            NGroupPresentationPropertyReader(PropType& newProp) :
                    prop(newProp) {
            }

            virtual NXMLElementReader* startSubElement(
                    const std::string& subTagName,
                    const regina::xml::XMLPropertyDict&) {
                if (subTagName == "group")
                    if (! prop.known())
                        return new NXMLGroupPresentationReader();
                return new NXMLElementReader();
            }

            virtual void endSubElement(const std::string& subTagName,
                    NXMLElementReader* subReader) {
                if (subTagName == "group") {
                    NGroupPresentation* ans =
                        dynamic_cast<NXMLGroupPresentationReader*>(subReader)->
                        group();
                    if (ans)
                        prop = ans;
                }
            }
    };
}

NXMLElementReader* NXMLTriangulationReader::startContentSubElement(
        const std::string& subTagName,
        const regina::xml::XMLPropertyDict& props) {
    // We don't read boundary component properties since they're stored
    // across multiple property tags and they're easy to calculate
    // anyway.
    if (subTagName == "tetrahedra")
        return new NTetrahedraReader(tri);
    else if (subTagName == "zeroeff") {
        bool b;
        if (valueOf(props.lookup("value"), b))
            tri->zeroEfficient_ = b;
    } else if (subTagName == "splitsfce") {
        bool b;
        if (valueOf(props.lookup("value"), b))
            tri->splittingSurface_ = b;
    } else if (subTagName == "threesphere") {
        bool b;
        if (valueOf(props.lookup("value"), b))
            tri->threeSphere_ = b;
    } else if (subTagName == "threeball") {
        bool b;
        if (valueOf(props.lookup("value"), b))
            tri->threeBall_ = b;
    } else if (subTagName == "solidtorus") {
        bool b;
        if (valueOf(props.lookup("value"), b))
            tri->solidTorus_ = b;
    } else if (subTagName == "irreducible") {
        bool b;
        if (valueOf(props.lookup("value"), b))
            tri->irreducible_ = b;
    } else if (subTagName == "compressingdisc") {
        bool b;
        if (valueOf(props.lookup("compressingdisc"), b))
            tri->compressingDisc_ = b;
    } else if (subTagName == "haken") {
        bool b;
        if (valueOf(props.lookup("haken"), b))
            tri->haken_ = b;
    } else if (subTagName == "H1")
        return new NAbelianGroupPropertyReader(tri->H1_);
    else if (subTagName == "H1Rel")
        return new NAbelianGroupPropertyReader(tri->H1Rel_);
    else if (subTagName == "H1Bdry")
        return new NAbelianGroupPropertyReader(tri->H1Bdry_);
    else if (subTagName == "H2")
        return new NAbelianGroupPropertyReader(tri->H2_);
    else if (subTagName == "fundgroup")
        return new NGroupPresentationPropertyReader(tri->fundamentalGroup_);
    return new NXMLElementReader();
}

void NXMLTriangulationReader::endContentSubElement(const std::string&,
        NXMLElementReader*) {
}

NXMLPacketReader* NTriangulation::xmlReader(NPacket*,
        NXMLTreeResolver& resolver) {
    return new NXMLTriangulationReader(resolver);
}

} // namespace regina

