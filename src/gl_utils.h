//
// Created by robko on 10/28/24.
//

#pragma once
#include "assertions.h"
#include <GL/gl.h>

namespace gl {
    static inline void assertNoError() {
#ifdef DEBUG_ASSERTIONS
        GLenum err = glGetError();
        DEBUG_ASSERT(GL_NO_ERROR == err);
#endif
    }
}
