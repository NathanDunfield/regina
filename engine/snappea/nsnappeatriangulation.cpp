
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2011, Ben Burton                                   *
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

#include <climits>
#include <cstring>

#include "snappea/nsnappeatriangulation.h"
#include "snappea/kernel/triangulation.h"
#include "snappea/kernel/unix_file_io.h"
#include "triangulation/ntriangulation.h"
#include "utilities/nthread.h"

namespace regina {

namespace {
    /**
     * A mutex to protect kernelMessages.
     */
    static NMutex snapMutex;
}

NSnapPeaTriangulation::NSnapPeaTriangulation(const NSnapPeaTriangulation& tri) :
        ShareableObject() {
    if (tri.snappeaData)
        ::copy_triangulation(tri.snappeaData, &snappeaData);
    else
        snappeaData = 0;
}

NSnapPeaTriangulation::NSnapPeaTriangulation(const NTriangulation& tri,
        bool allowClosed) {
    snappeaData = reginaToSnapPea(tri, allowClosed);
}

NSnapPeaTriangulation::~NSnapPeaTriangulation() {
    ::free_triangulation(snappeaData);
}

NSnapPeaTriangulation::SolutionType NSnapPeaTriangulation::solutionType()
        const {
    if (! snappeaData)
        return NSnapPeaTriangulation::not_attempted;
    return static_cast<SolutionType>(::get_complete_solution_type(snappeaData));
}

double NSnapPeaTriangulation::volume() const {
    if (! snappeaData)
        return 0;
    return ::volume(snappeaData, 0);
}

double NSnapPeaTriangulation::volume(int& precision) const {
    if (! snappeaData)
        return 0;
    return ::volume(snappeaData, &precision);
}

void NSnapPeaTriangulation::saveAsSnapPea(const char* filename) const {
    if (snappeaData)
        save_triangulation(snappeaData, filename);
}

void NSnapPeaTriangulation::writeTextShort(std::ostream& out) const {
    if (snappeaData) {
        out << "SnapPea triangulation with " << snappeaData->num_tetrahedra
            << " tetrahedra.";
    } else {
        out << "Null SnapPea triangulation";
    }
}

::Triangulation* NSnapPeaTriangulation::reginaToSnapPea(
        const NTriangulation& tri, bool allowClosed) {
    // Make sure SnapPea is likely to be comfortable with it.
    if (tri.getNumberOfTetrahedra() == 0)
        return 0;
    if (tri.hasBoundaryFaces())
        return 0;
    if (! tri.isConnected())
        return 0;
    if (! tri.isValid())
        return 0;
    if (! tri.isStandard())
        return 0;
    if (tri.isIdeal()) {
        // If it's ideal, make sure every vertex is ideal.
        if (tri.getNumberOfBoundaryComponents() < tri.getNumberOfVertices())
            return 0;
    } else {
        // No boundary faces, not ideal.. must be closed.
        if (! allowClosed)
            return 0;
        // If closed is okay, at least make sure it's one-vertex.
        if (1 != tri.getNumberOfVertices())
            return 0;
    }
    if (tri.getNumberOfTetrahedra() >= INT_MAX)
        return 0;

    ::TriangulationData data;
    data.name = strdup(tri.getPacketLabel().c_str());
    data.num_tetrahedra = tri.getNumberOfTetrahedra();

    // Fields recalculated by SnapPea:
    data.solution_type = ::not_attempted;
    data.volume = 0;
    data.orientability = ::unknown_orientability;
    data.CS_value_is_known = false;
    data.CS_value = 0;
    data.num_or_cusps = 0;
    data.num_nonor_cusps = 0;
    data.cusp_data = 0;

    data.tetrahedron_data = new ::TetrahedronData[data.num_tetrahedra];
    int tet, face, i, j, k, l;
    NTriangulation::TetrahedronIterator it = tri.getTetrahedra().begin();
    for (tet = 0; tet < data.num_tetrahedra; tet++) {
        for (face = 0; face < 4; face++) {
            data.tetrahedron_data[tet].neighbor_index[face] =
                tri.tetrahedronIndex((*it)->adjacentTetrahedron(face));
            for (i = 0; i < 4; i++)
                data.tetrahedron_data[tet].gluing[face][i] =
                    (*it)->adjacentGluing(face)[i];
        }

        // Other fields are recalculated by SnapPea.
        for (i = 0; i < 4; i++)
            data.tetrahedron_data[tet].cusp_index[i] = -1;
        for (i = 0; i < 2; i++)
            for (j = 0; j < 2; j++)
                for (k = 0; k < 4; k++)
                    for (l = 0; l < 4; l++)
                        data.tetrahedron_data[tet].curve[i][j][k][l] = 0;
        data.tetrahedron_data[tet].filled_shape.real = 0;
        data.tetrahedron_data[tet].filled_shape.imag = 0;

        it++;
    }

    ::Triangulation* ans;
    ::data_to_triangulation(&data, &ans);

    delete[] data.tetrahedron_data;
    free(data.name);

    return ans;
}

bool NSnapPeaTriangulation::kernelMessagesEnabled() {
    NMutex::MutexLock ml(snapMutex);
    return kernelMessages;
}

void NSnapPeaTriangulation::enableKernelMessages(bool enabled) {
    NMutex::MutexLock ml(snapMutex);
    kernelMessages = enabled;
}

void NSnapPeaTriangulation::disableKernelMessages() {
    NMutex::MutexLock ml(snapMutex);
    kernelMessages = false;
}

} // namespace regina

