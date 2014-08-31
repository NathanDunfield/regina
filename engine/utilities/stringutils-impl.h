
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

/* To be included from stringutils.h. */

#include <cctype>
#include <string>
#include "regina-core.h"
#include "maths/ninteger.h"

namespace regina {

template <bool supportInfinity>
bool valueOf(const std::string& str, NIntegerBase<supportInfinity>& dest) {
    bool valid;
    dest = NIntegerBase<supportInfinity>(str.c_str(), 10, &valid);
    return valid;
}

template <class OutputIterator>
unsigned basicTokenise(OutputIterator results, const std::string& str) {
    std::string::size_type len = str.length();
    std::string::size_type pos = 0;

    // Skip initial whitespace.
    while (pos < len && isspace(str[pos]))
        pos++;

    if (pos == len)
        return 0;

    // Extract each token.
    std::string::size_type total = 0;
    std::string::size_type tokStart;
    while (pos < len) {
        // Find the characters making up this token.
        tokStart = pos;
        while (pos < len && ! isspace(str[pos]))
            pos++;
        *results++ = str.substr(tokStart, pos - tokStart);
        total++;

        // Skip the subsequent whitespace.
        while (pos < len && isspace(str[pos]))
            pos++;
    }

    return static_cast<unsigned>(total);
}

} // namespace regina

