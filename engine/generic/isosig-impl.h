
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

/*! \file generic/isosig-impl.h
 *  \brief Contains some of the implementation details for the
 *  NGenericTriangulation class template.
 *
 *  This file is \e not included automatically by ngenerictriangulation.h.
 *  However, typical end users should never need to include it, since
 *  Regina's calculation engine provides full explicit instantiations
 *  of NGenericTriangulation for \ref stddim "standard dimensions".
 */

#include <algorithm>
#include <string>
#include "generic/ngenerictriangulation.h"
#include "packet/npacket.h"

/**
 * The numbers of base64 characters required to store an index into
 * DimTraits<dim>::Perm::Sn.
 *
 * This is 1 if dim <= 3  (since 4! <= 64), and 2 if dim = 4 (since 5! > 64).
 */
#define CHARS_PER_PERM(dim) ((dim) <= 3 ? 1 : 2)

namespace regina {

namespace {
    /**
     * Determine the integer value represented by the given character in
     * a signature string.
     */
    inline unsigned SVAL(char c) {
        if (c >= 'a' && c <= 'z')
            return (c - 'a');
        if (c >= 'A' && c <= 'Z')
            return (c - 'A' + 26);
        if (c >= '0' && c <= '9')
            return (c - '0' + 52);
        if (c == '+')
            return 62;
        return 63;
    }

    /**
     * Determine the character that represents the given integer value
     * in a signature string.
     */
    inline char SCHAR(unsigned c) {
        if (c < 26)
            return (char(c) + 'a');
        if (c < 52)
            return (char(c - 26) + 'A');
        if (c < 62)
            return (char(c - 52) + '0');
        if (c == 62)
            return '+';
        return '-';
    }

    /**
     * Is the given character a valid character in a signature string?
     */
    inline bool SVALID(char c) {
        return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '+' || c == '-');
    }

    /**
     * Append an encoding of the given integer to the given string.
     * The integer is broken into nChars distinct 6-bit blocks, and the
     * lowest-significance blocks are written first.
     */
    void SAPPEND(std::string& s, unsigned val, unsigned nChars) {
        for ( ; nChars > 0; --nChars) {
            s += SCHAR(val & 0x3F);
            val >>= 6;
        }
    }

    /**
     * Read the integer at the beginning of the given string.
     * Assumes the string has length >= nChars.
     */
    unsigned SREAD(const char* s, unsigned nChars) {
        unsigned ans = 0;
        for (unsigned i = 0; i < nChars; ++i)
            ans += (SVAL(s[i]) << (6 * i));
        return ans;
    }

    /**
     * Append up to three trits (0, 1 or 2) to the given string.
     * These are packed into a single character, with the first trit
     * representing the lowest-significance bits and so on.
     */
    void SAPPENDTRITS(std::string& s, const char* trits, unsigned nTrits) {
        char ans = 0;
        if (nTrits >= 1)
            ans |= trits[0];
        if (nTrits >= 2)
            ans |= (trits[1] << 2);
        if (nTrits >= 3)
            ans |= (trits[2] << 4);
        s += SCHAR(ans);
    }

    /**
     * Reads three trits (0, 1 or 2) from the given character.
     */
    void SREADTRITS(char c, char* result) {
        unsigned val = SVAL(c);
        result[0] = val & 3;
        result[1] = (val >> 2) & 3;
        result[2] = (val >> 4) & 3;
    }
}

template <int dim>
std::string NGenericTriangulation<dim>::isoSigFrom(
        const typename DimTraits<dim>::Triangulation& tri,
        unsigned simp,
        const typename DimTraits<dim>::Perm& vertices,
        typename DimTraits<dim>::Isomorphism* relabelling) {
    // These typedefs are already present in the class declaration,
    // but gcc 4.4 seems to break unless we include them here also.
    typedef typename DimTraits<dim>::Perm Perm;
    typedef typename DimTraits<dim>::Simplex Simplex;

    // Only process the component that simp belongs to.

    // ---------------------------------------------------------------------
    // Data for reconstructing a triangulation from an isomorphism signature
    // ---------------------------------------------------------------------

    // The number of simplices.
    unsigned nSimp = tri.getNumberOfSimplices();

    // What happens to each new facet that we encounter?
    // Options are:
    //   0 -> boundary
    //   1 -> joined to a simplex not yet seen [gluing perm = identity]
    //   2 -> joined to a simplex already seen
    // These actions are stored in lexicographical order by (simplex, facet),
    // but only once for each facet (so we "skip" gluings that we've
    // already seen from the other direction).
    char* facetAction = new char[tri.template getNumberOfFaces<dim-1>()];

    // What are the destination simplices and gluing permutations for
    // each facet under case #2 above?
    // For gluing permutations, we store the index of the permutation in
    // Perm::orderedSn.
    unsigned* joinDest = new unsigned[tri.template getNumberOfFaces<dim-1>()];
    unsigned* joinGluing = new unsigned[tri.template getNumberOfFaces<dim-1>()];

    // ---------------------------------------------------------------------
    // Data for finding the unique canonical isomorphism from this
    // connected component that maps (simplex, vertices) -> (0, 0..dim)
    // ---------------------------------------------------------------------

    // The image for each simplex and its vertices:
    int* image = new int[nSimp];
    Perm* vertexMap = new Perm[nSimp];

    // The preimage for each simplex:
    int* preImage = new int[nSimp];

    // ---------------------------------------------------------------------
    // Looping variables
    // ---------------------------------------------------------------------
    unsigned facetPos, joinPos, nextUnusedSimp;
    unsigned simpImg, facetImg;
    unsigned simpSrc, facetSrc, dest;
    const Simplex* s;

    // ---------------------------------------------------------------------
    // The code!
    // ---------------------------------------------------------------------

    std::fill(image, image + nSimp, -1);
    std::fill(preImage, preImage + nSimp, -1);

    image[simp] = 0;
    vertexMap[simp] = vertices.inverse();
    preImage[0] = simp;

    facetPos = 0;
    joinPos = 0;
    nextUnusedSimp = 1;

    // To obtain a canonical isomorphism, we must run through the simplices
    // and their facets in image order, not preimage order.
    //
    // This main loop is guaranteed to exit when (and only when) we have
    // exhausted a single connected component of the triangulation.
    for (simpImg = 0; simpImg < nSimp && preImage[simpImg] >= 0; ++simpImg) {
        simpSrc = preImage[simpImg];
        s = tri.getSimplex(simpSrc);

        for (facetImg = 0; facetImg <= dim; ++facetImg) {
            facetSrc = vertexMap[simpSrc].preImageOf(facetImg);

            // INVARIANTS (held while we stay within a single component):
            // - nextUnusedSimp > simpImg
            // - image[simpSrc], preImage[image[simpSrc]] and vertexMap[simpSrc]
            //   are already filled in.

            // Work out what happens to our source facet.
            if (! s->adjacentSimplex(facetSrc)) {
                // A boundary facet.
                facetAction[facetPos++] = 0;
                continue;
            }

            // We have a real gluing.  Is it a gluing we've already seen
            // from the other side?
            dest = tri.simplexIndex(s->adjacentSimplex(facetSrc));

            if (image[dest] >= 0)
                if (image[dest] < image[simpSrc] ||
                        (dest == simpSrc &&
                         vertexMap[simpSrc][s->adjacentFacet(facetSrc)]
                         < vertexMap[simpSrc][facetSrc])) {
                    // Yes.  Just skip this gluing entirely.
                    continue;
                }

            // Is it a completely new simplex?
            if (image[dest] < 0) {
                // Yes.  The new simplex takes the next available
                // index, and the canonical gluing becomes the identity.
                image[dest] = nextUnusedSimp++;
                preImage[image[dest]] = dest;
                vertexMap[dest] = vertexMap[simpSrc] *
                    s->adjacentGluing(facetSrc).inverse();

                facetAction[facetPos++] = 1;
                continue;
            }

            // It's a simplex we've seen before.  Record the gluing.
            joinDest[joinPos] = image[dest];
            joinGluing[joinPos] = (vertexMap[dest] *
                s->adjacentGluing(facetSrc) * vertexMap[simpSrc].inverse()).
                orderedSnIndex();
            ++joinPos;

            facetAction[facetPos++] = 2;
        }
    }

    // We have all we need.  Pack it all together into a string.
    // We need to encode:
    // - the number of simplices in this component;
    // - facetAction[i], 0 <= i < facetPos;
    // - joinDest[i], 0 <= i < joinPos;
    // - joinGluing[i], 0 <= i < joinPos.
    std::string ans;

    // Keep it simple for small triangulations (1 character per integer).
    // For large triangulations, start with a special marker followed by
    // the number of chars per integer.
    unsigned nCompSimp = simpImg;
    unsigned nChars;
    if (nCompSimp < 63)
        nChars = 1;
    else {
        nChars = 0;
        unsigned tmp = nCompSimp;
        while (tmp > 0) {
            tmp >>= 6;
            ++nChars;
        }

        ans = SCHAR(63);
        ans += SCHAR(nChars);
    }

    // Off we go.
    unsigned i;
    SAPPEND(ans, nCompSimp, nChars);
    for (i = 0; i < facetPos; i += 3)
        SAPPENDTRITS(ans, facetAction + i,
            (facetPos >= i + 3 ? 3 : facetPos - i));
    for (i = 0; i < joinPos; ++i)
        SAPPEND(ans, joinDest[i], nChars);
    for (i = 0; i < joinPos; ++i)
        SAPPEND(ans, joinGluing[i], CHARS_PER_PERM(dim));

    // Record the canonical isomorphism if required.
    if (relabelling)
        for (i = 0; i < nCompSimp; ++i) {
            relabelling->simpImage(i) = image[i];
            relabelling->facetPerm(i) = vertexMap[i];
        }

    // Done!
    delete[] image;
    delete[] vertexMap;
    delete[] preImage;
    delete[] facetAction;
    delete[] joinDest;
    delete[] joinGluing;

    return ans;
}

template <int dim>
std::string NGenericTriangulation<dim>::isoSig(
        typename DimTraits<dim>::Isomorphism** relabelling) const {
    // These typedefs are already present in the class declaration,
    // but gcc 4.7.3 seems to break unless we include them here also.
    typedef typename DimTraits<dim>::Perm Perm;
    typedef typename DimTraits<dim>::Triangulation Triangulation;
    typedef typename DimTraits<dim>::Isomorphism Isomorphism;

    const Triangulation& tri(static_cast<const Triangulation&>(*this));

    // Make sure the user is not trying to do something illegal.
    if (relabelling && tri.getNumberOfComponents() != 1) {
        *relabelling = 0; // Return 0 to the user...
        relabelling = 0;  // ... and forget they ever asked for an isomorphism.
    }

    Isomorphism* currRelabelling = 0;
    if (relabelling) {
        *relabelling = new Isomorphism(tri.getNumberOfSimplices());
        currRelabelling = new Isomorphism(tri.getNumberOfSimplices());
    }

    if (tri.getSimplices().empty()) {
        char c[2];
        c[0] = SCHAR(0);
        c[1] = 0;
        return c;
    }

    // The triangulation is non-empty.  Get a signature string for each
    // connected component.
    unsigned i;
    typename Triangulation::ComponentIterator it;
    unsigned cSimp;
    unsigned simp, perm;
    std::string curr;

    std::string* comp = new std::string[tri.getNumberOfComponents()];
    for (it = tri.getComponents().begin(), i = 0;
            it != tri.getComponents().end(); ++it, ++i) {
        cSimp = (*it)->getNumberOfSimplices();

        for (simp = 0; simp < (*it)->getNumberOfSimplices(); ++simp)
            for (perm = 0; perm < Perm::nPerms; ++perm) {
                curr = isoSigFrom(tri, (*it)->getSimplex(simp)->markedIndex(),
                    Perm::orderedSn[perm], currRelabelling);
                if ((simp == 0 && perm == 0) || (curr < comp[i])) {
                    comp[i].swap(curr);
                    if (relabelling)
                        std::swap(*relabelling, currRelabelling);
                }
            }
    }

    // Pack the components together.
    std::sort(comp, comp + tri.getNumberOfComponents());

    std::string ans;
    for (i = 0; i < tri.getNumberOfComponents(); ++i)
        ans += comp[i];

    delete[] comp;
    delete currRelabelling;
    return ans;
}

template <int dim>
typename DimTraits<dim>::Triangulation*
        NGenericTriangulation<dim>::fromIsoSig(const std::string& sig) {
    // These typedefs are already present in the class declaration,
    // but gcc 4.4 seems to break unless we include them here also.
    typedef typename DimTraits<dim>::Perm Perm;
    typedef typename DimTraits<dim>::Simplex Simplex;
    typedef typename DimTraits<dim>::Triangulation Triangulation;

    std::unique_ptr<Triangulation> ans(new Triangulation());

    NPacket::ChangeEventSpan span(ans.get());

    const char* c = sig.c_str();

    // Skip any leading whitespace.
    while (*c && ::isspace(*c))
        ++c;

    // Find the end of the string.
    const char* end = c;
    while (*end && ! ::isspace(*end))
        ++end;

    // Initial check for invalid characters.
    const char* d;
    for (d = c; d != end; ++d)
        if (! SVALID(*d))
            return 0;
    for (d = end; *d; ++d)
        if (! ::isspace(*d))
            return 0;

    unsigned j;
    unsigned pos, nSimp, nChars;
    while (c != end) {
        // Read one component at a time.
        nSimp = SVAL(*c++);
        if (nSimp < 63)
            nChars = 1;
        else {
            if (c == end)
                return 0;
            nChars = SVAL(*c++);
            if (c + nChars > end)
                return 0;
            nSimp = SREAD(c, nChars);
            c += nChars;
        }

        if (nSimp == 0) {
            // Empty component.
            continue;
        }

        // Non-empty component; keep going.
        char* facetAction = new char[(dim+1) * nSimp + 2];
        unsigned nFacets = 0;
        unsigned facetPos = 0;
        unsigned nJoins = 0;

        for ( ; nFacets < (dim+1) * nSimp; facetPos += 3) {
            if (c == end) {
                delete[] facetAction;
                return 0;
            }
            SREADTRITS(*c++, facetAction + facetPos);
            for (j = 0; j < 3; ++j) {
                // If we're already finished, make sure the leftover trits
                // are zero.
                if (nFacets == (dim+1) * nSimp) {
                    if (facetAction[facetPos + j] != 0) {
                        delete[] facetAction;
                        return 0;
                    }
                    continue;
                }

                if (facetAction[facetPos + j] == 0)
                    ++nFacets;
                else if (facetAction[facetPos + j] == 1)
                    nFacets += 2;
                else if (facetAction[facetPos + j] == 2) {
                    nFacets += 2;
                    ++nJoins;
                } else {
                    delete[] facetAction;
                    return 0;
                }
                if (nFacets > (dim+1) * nSimp) {
                    delete[] facetAction;
                    return 0;
                }
            }
        }

        unsigned* joinDest = new unsigned[nJoins + 1];
        for (pos = 0; pos < nJoins; ++pos) {
            if (c + nChars > end) {
                delete[] facetAction;
                delete[] joinDest;
                return 0;
            }

            joinDest[pos] = SREAD(c, nChars);
            c += nChars;
        }

        unsigned* joinGluing = new unsigned[nJoins + 1];
        for (pos = 0; pos < nJoins; ++pos) {
            if (c + CHARS_PER_PERM(dim) > end) {
                delete[] facetAction;
                delete[] joinDest;
                delete[] joinGluing;
                return 0;
            }

            joinGluing[pos] = SREAD(c, CHARS_PER_PERM(dim));
            c += CHARS_PER_PERM(dim);

            if (joinGluing[pos] >= Perm::nPerms) {
                delete[] facetAction;
                delete[] joinDest;
                delete[] joinGluing;
                return 0;
            }
        }

        // End of component!
        Simplex** simp = new Simplex*[nSimp];
        for (pos = 0; pos < nSimp; ++pos)
            simp[pos] = ans->newSimplex();

        facetPos = 0;
        unsigned nextUnused = 1;
        unsigned joinPos = 0;
        for (pos = 0; pos < nSimp; ++pos)
            for (j = 0; j <= dim; ++j) {
                // Already glued from the other side:
                if (simp[pos]->adjacentSimplex(j))
                    continue;

                if (facetAction[facetPos] == 0) {
                    // Boundary facet.
                } else if (facetAction[facetPos] == 1) {
                    // Join to new simplex.
                    if (nextUnused >= nSimp) {
                        delete[] facetAction;
                        delete[] joinDest;
                        delete[] joinGluing;
                        delete[] simp;
                        return 0;
                    }
                    simp[pos]->joinTo(j, simp[nextUnused++], Perm());
                } else {
                    // Join to existing simplex.
                    if (joinDest[joinPos] >= nextUnused ||
                            simp[joinDest[joinPos]]->adjacentSimplex(
                            Perm::orderedSn[joinGluing[joinPos]][j])) {
                        delete[] facetAction;
                        delete[] joinDest;
                        delete[] joinGluing;
                        delete[] simp;
                        return 0;
                    }
                    simp[pos]->joinTo(j, simp[joinDest[joinPos]],
                        Perm::orderedSn[joinGluing[joinPos]]);
                    ++joinPos;
                }

                ++facetPos;
            }

        delete[] facetAction;
        delete[] joinDest;
        delete[] joinGluing;
        delete[] simp;
    }

    return ans.release();
}

template <int dim>
size_t NGenericTriangulation<dim>::isoSigComponentSize(const std::string& sig) {
    const char* c = sig.c_str();

    // Examine the first character.
    // Note that SVALID also ensures that *c is non-null (i.e., it
    // detects premature end of string).
    if (! SVALID(*c))
        return 0;
    size_t nSimp = SVAL(*c);
    if (nSimp < 63)
        return nSimp;

    // The number of simplices is so large that it requires several
    // characters to store.
    ++c;
    if (! *c)
        return 0;
    size_t nChars = SVAL(*c++);

    for (const char* d = c; d < c + nChars; ++d)
        if (! SVALID(*d))
            return 0;
    return SREAD(c, nChars);
}

} // namespace regina

