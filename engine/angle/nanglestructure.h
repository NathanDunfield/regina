
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2009, Ben Burton                                   *
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
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,       *
 *  MA 02110-1301, USA.                                                   *
 *                                                                        *
 **************************************************************************/

/* end stub */

/*! \file angle/nanglestructure.h
 *  \brief Deals with angle structures on triangulations.
 */

#ifndef __NANGLESTRUCTURE_H
#ifndef __DOXYGEN
#define __NANGLESTRUCTURE_H
#endif

#include "regina-core.h"
#include "shareableobject.h"
#include "file/nfilepropertyreader.h"
#include "maths/nrational.h"
#include "maths/nray.h"

namespace regina {

class NTriangulation;
class NXMLAngleStructureReader;

/**
 * \addtogroup angle Angle Structures
 * Angle structures on triangulations.
 * @{
 */

/**
 * A vector of integers used to indirectly store the individual angles
 * in an angle structure.
 *
 * This vector will contain one member per angle plus a final scaling
 * member; to obtain the actual angle in the angle structure one should
 * divide the corresonding angle member by the scaling member and then
 * multiply by <i>pi</i>.
 *
 * The reason for using this obfuscated representation is so we can
 * use the NDoubleDescription vertex enumeration routines to
 * calculate vertex angle structures.
 *
 * If there are \a t tetrahedra in the underlying triangulation, there
 * will be precisely 3<i>t</i>+1 elements in this vector.  The first
 * three elements will be the angle members for the first tetrahedron,
 * the next three for the second tetrahedron and so on.  For each
 * tetraheron, the three individual elements are the angle members
 * for vertex splittings 0, 1 and 2 (see NAngleStructure::getAngle()).
 * The final element of the vector is the scaling member as described
 * above.
 *
 * \testpart
 *
 * \ifacespython Not present.
 */
class REGINA_API NAngleStructureVector : public NRay {
    public:
        /**
         * Creates a new vector all of whose entries are initialised to
         * zero.
         *
         * @param length the number of elements in the new vector.
         */
        NAngleStructureVector(unsigned length);
        /**
         * Creates a new vector that is a clone of the given vector.
         *
         * @param cloneMe the vector to clone.
         */
        NAngleStructureVector(const NVector<NLargeInteger>& cloneMe);

        virtual NVector<NLargeInteger>* clone() const;
};

/**
 * Represents an angle structure on a triangulation.
 * Once the underlying triangulation changes, this angle structure
 * is no longer valid.
 *
 * \testpart
 */
class REGINA_API NAngleStructure :
        public ShareableObject, public NFilePropertyReader {
    private:
        NAngleStructureVector* vector;
            /**< Stores (indirectly) the individual angles in this angle
             *   structure. */
        NTriangulation* triangulation;
            /**< The triangulation on which this angle structure is placed. */

        mutable unsigned long flags;
            /**< Stores a variety of angle structure properties as
             *   described by the flag constants in this class.
             *   Flags can be combined using bitwise OR. */
        static const unsigned long flagStrict;
            /**< Signals that this angle structure is strict. */
        static const unsigned long flagTaut;
            /**< Signals that this angle structure is taut. */
        static const unsigned long flagCalculatedType;
            /**< Signals that the type (strict/taut) has been calculated. */

    public:
        /**
         * Creates a new angle structure on the given triangulation with
         * the given coordinate vector.
         *
         * \ifacespython Not present.
         *
         * @param triang the triangulation on which this angle structure lies.
         * @param newVector a vector containing the individual angles in the
         * angle structure.
         */
        NAngleStructure(NTriangulation* triang,
            NAngleStructureVector* newVector);
        /**
         * Destroys this angle structure.
         * The underlying vector of angles will also be deallocated.
         */
        virtual ~NAngleStructure();

        /**
         * Creates a newly allocated clone of this angle structure.
         *
         * @return a clone of this angle structure.
         */
        NAngleStructure* clone() const;

        /**
         * Returns the requested angle in this angle structure.
         * The angle returned will be scaled down; the actual angle is
         * the returned value multiplied by <i>pi</i>.
         *
         * @param tetIndex the index in the triangulation of the
         * tetrahedron in which the requested angle lives; this should
         * be between 0 and NTriangulation::getNumberOfTetrahedra()-1
         * inclusive.
         * @param edgePair the number of the vertex splitting
         * representing the pair of edges holding the requested angle;
         * this should be between 0 and 2 inclusive.  See ::vertexSplit
         * and ::vertexSplitDefn for details regarding vertex
         * splittings.
         * @return the requested angle scaled down by <i>pi</i>.
         */
        NRational getAngle(unsigned long tetIndex, int edgePair) const;

        /**
         * Returns the triangulation on which this angle structure lies.
         *
         * @return the underlying triangulation.
         */
        NTriangulation* getTriangulation() const;

        /**
         * Determines whether this is a strict angle structure.
         * A strict angle structure has all angles strictly between (not
         * including) 0 and <i>pi</i>.
         *
         * @return \c true if and only if this is a strict angle structure.
         */
        bool isStrict() const;

        /**
         * Determines whether this is a taut structure.
         * A taut structure contains only angles 0 and <i>pi</i>.
         *
         * @return \c true if and only if this is a taut structure.
         */
        bool isTaut() const;

        void writeTextShort(std::ostream& out) const;

        /**
         * Writes a chunk of XML containing this angle structure and all
         * of its properties.  This routine will be called from within
         * NAngleStructureList::writeXMLPacketData().
         *
         * \ifacespython Not present.
         *
         * @param out the output stream to which the XML should be written.
         */
        void writeXMLData(std::ostream& out) const;

        /**
         * Writes this angle structure and all of its properties to the
         * given old-style binary file.
         *
         * This routine writes precisely what readFromFile() reads.
         *
         * \deprecated For the preferred way to write data to file, see
         * writeXMLData() instead.
         *
         * \pre The given file is currently opened for writing.
         *
         * \ifacespython Not present.
         *
         * @param out the file to which to write.
         */
        void writeToFile(NFile& out) const;

        /**
         * Reads an angle structure and all its properties from the
         * given old-style binary file.
         *
         * This routine reads precisely what writeToFile() writes.
         *
         * \deprecated For the preferred way of reading angle structures
         * from file, see class NXMLAngleStructureReader instead.
         *
         * \pre The given file is currently opened for reading.
         *
         * \ifacespython Not present.
         *
         * @param in the file from which to read.
         * @param triangulation the triangulation on which this angle
         * structure will lie.
         * @return a newly allocated angle structure containing the
         * information read from file.
         */
        static NAngleStructure* readFromFile(NFile& in,
            NTriangulation* triangulation);

    protected:
        virtual void readIndividualProperty(NFile& infile, unsigned propType);

        /**
         * Calculates the structure type (strict or taut) and stores it
         * as a property.
         */
        void calculateType() const;

    friend class regina::NXMLAngleStructureReader;
};

/*@}*/

// Inline functions for NAngleStructureVector

inline NAngleStructureVector::NAngleStructureVector(unsigned length) :
        NRay(length) {
}

inline NAngleStructureVector::NAngleStructureVector(
        const NVector<NLargeInteger>& cloneMe) : NRay(cloneMe) {
}

inline NVector<NLargeInteger>* NAngleStructureVector::clone() const {
    return new NAngleStructureVector(*this);
}

// Inline functions for NAngleStructure

inline NAngleStructure::NAngleStructure(NTriangulation* triang,
        NAngleStructureVector* newVector) : vector(newVector),
        triangulation(triang), flags(0) {
}

inline NAngleStructure::~NAngleStructure() {
    delete vector;
}

inline NTriangulation* NAngleStructure::getTriangulation() const {
    return triangulation;
}

inline bool NAngleStructure::isStrict() const {
    if ((flags & flagCalculatedType) == 0)
        calculateType();
    return ((flags & flagStrict) != 0);
}

inline bool NAngleStructure::isTaut() const {
    if ((flags & flagCalculatedType) == 0)
        calculateType();
    return ((flags & flagTaut) != 0);
}

} // namespace regina

#endif

