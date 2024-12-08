//
// Created by robko on 10/28/24.
//

#pragma once
#include "assertions.h"
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef GL_CALL
#undef GL_CALL
#endif

/**
 * Simple wrapper around any OpenGL function call which just calls the specified
 * function and asserts that there is no error.
 */
#define GL_CALL(func, ...)                                                     \
    {                                                                          \
        func(__VA_ARGS__);                                                     \
        gl::assertNoErrorFunc(#func);                                          \
    }

namespace gl {

static_assert(sizeof(GLint) == sizeof(int));

static inline void assertNoErrorFunc(const char *funcName) {
#ifdef DEBUG_ASSERTIONS
    GLenum err = glGetError();
    if (GL_NO_ERROR != err) {
        const GLubyte *string;
        string = gluErrorString(err);
        std::cout << "OpenGL error when calling function " << funcName << ": "
                  << string << std::endl;
    }
    DEBUG_ASSERT(GL_NO_ERROR == err);
#endif
}

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
