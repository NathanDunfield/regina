
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  Computational Engine                                                  *
 *                                                                        *
 *  Copyright (c) 1999-2002, Ben Burton                                   *
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

#include "progress/nprogressmanager.h"
#include "jnitools.h"

using namespace regina;
using namespace regina::jni;

REGJNIEXPORT jobject JNICALL
        Java_normal_engine_implementation_jni_progress_NJNIProgressManager_getProgress
        (JNIEnv *env, jobject me) {
    return CREATE_WRAPPER_OBJECT(env,
        (NProgress*)GET_ENGINE_OBJECT(env,
        NProgressManager, me)->getProgress(),
        "normal/engine/implementation/jni/progress/NJNIProgress");
}

REGJNIEXPORT jboolean JNICALL
        Java_normal_engine_implementation_jni_progress_NJNIProgressManager_isFinished
        (JNIEnv *env, jobject me) {
    return GET_ENGINE_OBJECT(env, NProgressManager, me)->isFinished();
}
 
REGJNIEXPORT jboolean JNICALL
        Java_normal_engine_implementation_jni_progress_NJNIProgressManager_isStarted
        (JNIEnv *env, jobject me) {
    return GET_ENGINE_OBJECT(env, NProgressManager, me)->isStarted();
}
 
REGJNIEXPORT void JNICALL
        Java_normal_engine_implementation_jni_progress_NJNIProgressManager_newNProgressManager
        (JNIEnv *env, jobject me) {
    ASSIGN_ENGINE_OBJECT(env, new NProgressManager(), me);
}
 
