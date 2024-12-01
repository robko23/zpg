//
// Created by robko on 10/28/24.
//

#pragma once
#include "assertions.h"
#include <GL/gl.h>
#include <GL/glu.h>

namespace gl {

static_assert(sizeof(GLint) == sizeof(int));

static inline void assertNoError() {
#ifdef DEBUG_ASSERTIONS
    GLenum err = glGetError();
    if (GL_NO_ERROR != err) {
        const GLubyte *string;
        string = gluErrorString(err);
		std::cout << "OpenGL error: " << string << std::endl;
    }
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

static inline int getMaxTextureUnits() {
    int val;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &val);
    return val;
}

static inline int getCurrentTexture(int textureUnit) {
    int val;
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &val);
    glActiveTexture(0);
    return val;
}

} // namespace gl
