
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

#include "hypersurface/hsflavourregistry.h"

namespace regina {

#define __HSFLAVOUR_REGISTRY_BODY

// Bring in routines from the flavour registry.

#define REGISTER_HSFLAVOUR(id_name, class, n) \
    NNormalHypersurfaceVector* class::clone() const { \
        return new class(*this); \
    }

#include "hypersurface/hsflavourregistry.h"

// Tidy up.
#undef REGISTER_HSFLAVOUR
#undef __HSFLAVOUR_REGISTRY_BODY

} // namespace regina

