
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

/*! \file maths/nvectordense.h
 *  \brief Deals with dense vectors of various types.
 */

#ifndef __NVECTORDENSE_H
#ifndef __DOXYGEN
#define __NVECTORDENSE_H
#endif

#include "maths/nvector.h"

namespace regina {

/**
 * \weakgroup maths
 * @{
 */

/**
 * A dense vector of objects of type T.
 * The vector is represented as a simple array on the heap.
 *
 * This vector class is flexible but slow; for a fast but inflexible
 * vector class that is better suited for heavy computation, see
 * NFastVector instead.
 *
 * \pre See the preconditions for NVector<T>.
 *
 * \ifacespython Not present.
 */
template <class T>
class NVectorDense : public NVector<T> {
    protected:
        T* elements;
            /**< The internal array containing all vector elements. */
        unsigned vectorSize;
            /**< The size of the vector, possibly including zero
             *   elements. */
    public:
        /**
         * Creates a new vector.
         * Its elements will not be initialised.
         *
         * @param newVectorSize the number of elements in the new
         * vector; this must be strictly positive.
         */
        NVectorDense(unsigned newVectorSize) :
                elements(new T[newVectorSize]), vectorSize(newVectorSize) {
        }
        /**
         * Creates a new vector and initialises every element to the
         * given value.
         *
         * @param newVectorSize the number of elements in the new
         * vector; this must be strictly positive.
         * @param initValue the value to assign to every element of the
         * vector.
         */
        NVectorDense(unsigned newVectorSize, const T& initValue) : 
                elements(new T[newVectorSize]), vectorSize(newVectorSize) {
            for (unsigned i=0; i<vectorSize; i++)
                elements[i] = initValue;
        }
        /**
         * Creates a new vector that is a clone of the given vector.
         *
         * @param cloneMe the vector to clone.
         */
        NVectorDense(const NVector<T>& cloneMe) : NVector<T>(),
                vectorSize(cloneMe.size()) {
            elements = new T[vectorSize];
            for (unsigned i=0; i<vectorSize; i++)
                elements[i] = cloneMe[i];
        }
        /**
         * Creates a new vector that is a clone of the given vector.
         *
         * @param cloneMe the vector to clone.
         */
        NVectorDense(const NVectorDense<T>& cloneMe) : NVector<T>(),
                vectorSize(cloneMe.size()) {
            elements = new T[vectorSize];
            for (unsigned i=0; i<vectorSize; i++)
                elements[i] = cloneMe.elements[i];
        }
        /**
         * Destroys this vector.
         */
        virtual ~NVectorDense() {
            delete[] elements;
        }
        virtual NVector<T>* clone() const {
            return new NVectorDense<T>(*this);
        }
        virtual unsigned size() const {
            return vectorSize;
        }
        virtual const T& operator[](unsigned index) const {
            return elements[index];
        }
        virtual void setElement(unsigned index, const T& value) {
            elements[index] = value;
        }
    
        virtual void operator = (const NVector<T>& cloneMe) {
            for (unsigned i=0; i<vectorSize; i++)
                elements[i] = cloneMe[i];
        }
        virtual void operator += (const NVector<T>& other) {
            for (unsigned i=0; i<vectorSize; i++)
                elements[i] += other[i];
        }
        virtual void operator -= (const NVector<T>& other) {
            for (unsigned i=0; i<vectorSize; i++)
                elements[i] -= other[i];
        }
        virtual void operator *= (const T& factor) {
            if (factor == NVector<T>::one)
                return;
            for (unsigned i=0; i<vectorSize; i++)
                elements[i] *= factor;
        }
        virtual void negate() {
            for (unsigned i=0; i<vectorSize; i++)
                elements[i] = -elements[i];
        }
        virtual void addCopies(const NVector<T>& other,
                const T& multiple) {
            if (multiple == NVector<T>::zero)
                return;
            if (multiple == NVector<T>::one) {
                (*this) += other;
                return;
            }
            if (multiple == NVector<T>::minusOne) {
                (*this) -= other;
                return;
            }
            T term;
            for (unsigned i=0; i<vectorSize; i++) {
                term = other[i];
                term *= multiple;
                elements[i] += term;
            }
        }
        virtual void subtractCopies(const NVector<T>& other,
                const T& multiple) {
            if (multiple == NVector<T>::zero)
                return;
            if (multiple == NVector<T>::one) {
                (*this) -= other;
                return;
            }
            if (multiple == NVector<T>::minusOne) {
                (*this) += other;
                return;
            }
            T term;
            for (unsigned i=0; i<vectorSize; i++) {
                term = other[i];
                term *= multiple;
                elements[i] -= term;
            }
        }
};

/*@}*/

} // namespace regina

#endif

