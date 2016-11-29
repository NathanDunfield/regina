/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KSYNTAXHIGHLIGHTING_STATE_H
#define KSYNTAXHIGHLIGHTING_STATE_H

#include <memory>

namespace KSyntaxHighlighting {

class StateData;

/** Opaque handle to the state of the highlighting engine.
 *  This needs to be fed into AbstractHighlighter for every line of text
 *  and allows concrete highlighter implementations to store state per
 *  line for fast re-highlighting of specific lines (e.g. during editing).
 */
class State
{
public:
    /** Creates an initial state, ie. what should be used for the first line
     *  in a document.
     */
    State();
    State(const State &other);
    ~State();
    /**
     * Performs a shallow copy.
     */
    State& operator=(const State &rhs);
    /**
     * Performs a deep copy.
     */
    State copy() const;

    /** Compares two states for equality.
     *  For two equal states and identical text input, AbstractHighlighter
     *  guarantees to produce equal results. This can be used to only
     *  re-highlight as many lines as necessary during editing.
     */
    bool operator==(const State &other) const;
    /** Compares two states for inequality.
     *  This is the opposite of operator==().
     */
    bool operator!=(const State &other) const;

private:
    friend class StateData;
    std::shared_ptr<StateData> d;
};

}

#endif // KSYNTAXHIGHLIGHTING_STATE_H