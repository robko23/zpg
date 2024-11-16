//
// Created by robko on 10/28/24.
//

#pragma once
#include "assertions.h"
#include <GL/gl.h>

namespace gl {
    static_assert(sizeof(GLint) == sizeof(int));

    static inline void assertNoError() {
#ifdef DEBUG_ASSERTIONS
        GLenum err = glGetError();
        DEBUG_ASSERT(GL_NO_ERROR == err);
#endif
    }

    static inline int getCurrentSSBO() {
        GLint prog = 0;
        glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, &prog);
        return prog;
    }

    static inline int getCurrentProgram() { // NOLINT(*-reserved-identifier)
        GLint prog = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
        return prog;
    }
}
