
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  KDE User Interface                                                    *
 *                                                                        *
 *  Copyright (c) 1999-2003, Ben Burton                                   *
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
 *  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,        *
 *  MA 02111-1307, USA.                                                   *
 *                                                                        *
 **************************************************************************/

/* end stub */

/*! \file pythoninterpreter.h
 *  \brief Provides an interface to the python interpreter and
 *  subinterpreters.
 */

#ifndef __PYTHONINTERPRETER_H
#define __PYTHONINTERPRETER_H

#include <Python.h>
#include <string>

#include "utilities/nthread.h"

class PythonOutputStream;

namespace regina {
    class NPacket;
}

/**
 * A single python subinterpreter.  Multiple subinterpreters are independent
 * and may exist simultaneously.
 *
 * Each new subinterpreter corresponds to a new call to
 * Py_NewInterpreter().  The global routines Py_Initialize() and
 * Py_Finalize() are called when the first interpreter is created and
 * the last interpreter is destroyed respectively.
 */
class PythonInterpreter {
    private:
        static long nInterpreters;
            /**< The number of currently active interpreters. */
        static bool createdGIL;
            /**< Have we created the python global interpreter lock? */
        static regina::NMutex globalMutex;
            /**< A mutual exclusion device for the creation and
                 destruction of subinterpreters. */

        PyThreadState* state;
            /**< The first thread state created in this particular
                 subinterpreter. */

        PyObject* mainModule;
            /**< The __main__ module. */
        PyObject* mainNamespace;
            /**< The global namespace. */

        std::string currentCode;
            /**< Any previous statements (such as loop openings) that are
                 waiting to be completed. */

    public:
        /**
         * Constructor and destructor.
         */
        PythonInterpreter(PythonOutputStream* pyStdOut = 0,
                PythonOutputStream* pyStdErr = 0);
        ~PythonInterpreter();

        /**
         * Execute the given line in the subinterpreter.
         * Returns \c true if no further input is required, or
         * \c false if the interpreter is waiting on further lines of code.
         */
        bool executeLine(const std::string& command);

        /**
         * Configuration of the subinterpreter.
         */
        void importRegina();
        void setVar(const char* name, regina::NPacket* value);

    private:
        /**
         * Is the given command comment or only whitespace?
         */
        static bool isEmptyCommand(const std::string& command);

        /**
         * Write the given error message to std::cerr along with a
         * request to report the problem.
         */
        static void pleaseReport(const char* msg);

        /**
         * Extract the current error message.  A new python reference is
         * returned.
         */
        static PyObject* extractErrMsg();
};

#endif
