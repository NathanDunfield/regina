
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2001, Ben Burton                                   *
 *  For further details contact Ben Burton (benb@acm.org).                *
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

/*! \file npropertyholder.h
 *  \brief Deals with objects that possess calculable properties.
 */

#ifndef __NPROPERTYHOLDER_H
#ifndef __DOXYGEN
#define __NPROPERTYHOLDER_H
#endif

#include <fstream.h>

class NFile;

/**
 * Represents an object that can have properties, and whose properties
 * can be written to and read from file.
 *
 * Each property for such an object should have a unique integer
 * identifier which should be positive; this will be referred to as the
 * <i>property type</i>.  When reading properties from
 * file, if a property of unknown type is reached, it will be
 * skipped.
 *
 * Properties should only be calculated when necessary.  Properties thus
 * should be able to take the value "unknown".
 *
 * The function clearAllProperties() should be called <b>every</b> time
 * the contents of the property holder change in such a way that the
 * properties might change their values.  Ideally this call will
 * be placed in the various edit access functions for the derived
 * class.
 * The function initialiseAllProperties() should never be called but its
 * functionality should be mirrored by the property holder's constructor.
 *
 * To read properties from file, simply call readProperties().
 * To write properties to file, do the following:
 * <ul>
 *   <li> For each property:
 *     <ul>
 *       <li> Call writePropertyHeader();
 *       <li> Write the property details to file in a way that mirrors
 *         the information read during readIndividualProperty(),
 *         noting that the property type and bookmarking details should
 *         <b>not</b> be written;
 *       <li> Call writePropertyFooter(), passing the \c streampos
 *         that was returned from writePropertyHeader().
 *     </ul>
 *   <li> Call writeAllPropertiesFooter().
 * </ul>
 *
 * Note that if initialiseAllProperties() or readIndividualProperty() are
 * ever overridden, the subclass implementations must <b>always</b> begin with
 * a call to the superclass implementations.
 *
 * \todo \featurelong Complete property overhaul.  Be able to lock
 * properties (specify that they won't change during this operation even
 * though clearAllProperties() has been called).  Allow locking and
 * unlocking in groups (such as skeletal properties, topological
 * properties, etc).
 * \todo \tidy Document what to do when adding a new property.
 *
 * \ifaces Not present.
 */
class NPropertyHolder {
    public:
        /**
         * Default constructor that does nothing.
         */
        NPropertyHolder();

        /**
         * Default destructor that does nothing.
         */
        virtual ~NPropertyHolder();

        /**
         * Reads in all properties from file.
         * Properties of unknown type will simply be skipped.
         * If a individual property read routine leaves the file read
         * pointer at the wrong location, this will be rectified and
         * later properties will not be affected.
         *
         * @param infile the file from which to read the properties.
         * This should be open for reading and at the correct position.
         */
        void readProperties(NFile& infile);

        /**
         * Writes a header to file for a property, containing the
         * property type and some bookmarking details.
         * The bookmark returned should later be passed to
         * writePropertyFooter() for housekeeping.
         *
         * @param outfile the file to which the property header will be
         * written.  This should be open for writing.
         * If this is the first property, the file should be at the position
         * from which readProperties() will be called.  Otherwise it
         * should be at the position immediately after the previous
         * property footer was written.
         * @return bookmarking information that should later be passed
         * to writePropertyFooter().
         */
        streampos writePropertyHeader(NFile& outfile, unsigned propType) const;

        /**
         * Writes a footer to file for a property containing
         * bookmarking details.
         *
         * @param outfile the file to which the footer will be written.
         * This should be open for writing and at the position
         * immediately after the internal property details were written.
         * @param bookmark the \c streampos returned from
         * writePropertyHeader().
         */
        void writePropertyFooter(NFile& outfile, streampos bookmark) const;

        /**
         * Writes a footer to file to signify that all properties have
         * now been written.
         *
         * @param outfile the file to which the footer will be written.
         * This should be open for writing and at the position
         * immediately after the last property footer was written.
         */
        void writeAllPropertiesFooter(NFile& outfile) const;

    protected:
        /**
         * Reads an individual property from file.
         * The property type and bookmarking details should <b>not</b>
         * read; merely the internal property details that are written
         * to file between writePropertyHeader() and
         * writePropertyFooter().
         *
         * The property type of the property to be read will be passed
         * in \a propType.  If the property type is unrecognised, this
         * routine may simply do nothing and return.
         *
         * Note that if this routine is being overridden, the
         * subclass implementation must <b>always</b> begin with a call to
         * the superclass implementation.
         *
         * @param infile the file from which to read the property.  This
         * should be open for reading and at the position immediately after
         * writePropertyHeader() would have been called during the
         * corresponding save operation.
         * @param propType the property type of the property about to be
         * read.
         */
        virtual void readIndividualProperty(NFile& infile,
            unsigned propType) = 0;

        /**
         * Clears any calculated properties and declares them all
         * unknown.  All dynamic memory used for storing known
         * properties should be deallocated here.
         *
         * Whenever this routine is overridden, it should end with a call
         * to the superclass implementation.  These superclass calls will
         * travel up the class hierarchy until
         * NPropertyHolder::clearAllProperties() is reached.  At this
         * point initialiseAllProperties() will be called and all
         * properties for all subclasses will be reinitialised to their
         * default values.
         *
         * Note that overriding implementations should not call
         * initialiseAllProperties() directly.  Note also that if no
         * memory needs to be deallocated for this specific subclass of
         * NPropertyHolder, there is no need to override this routine.
         *
         * This routine should be called <b>every</b> time the contents of the
         * property holder change in such a way that the properties might
         * change their values.
         */
        virtual void clearAllProperties();

        /**
         * Declares all properties unknown.
         *
         * This should never be called from any routine other than
         * NPropertyHolder::clearAllProperties().
         * Its functionality however should be mirrored in the
         * constructor of each derived class of NPropertyHolder.
         *
         * Note that if this routine is being overridden, the
         * subclass implementation must <b>always</b> begin with a call to
         * the superclass implementation.
         */
        virtual void initialiseAllProperties() = 0;
};

// Inline functions for NPropertyHolder

inline NPropertyHolder::NPropertyHolder() {
}

inline NPropertyHolder::~NPropertyHolder() {
}

inline void NPropertyHolder::clearAllProperties() {
    initialiseAllProperties();
}

#endif

