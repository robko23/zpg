//
// Created by robko on 11/16/24.
//

#pragma once

#include <GL/gl.h>
#include <vector>
#include "../assertions.h"
#include "../gl_utils.h"

template<typename Inner>
class SSBO {
    static_assert(alignof(Inner) == 16);
    static_assert(sizeof(GLuint) == sizeof(unsigned int));
private:
    // https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Shader_storage_blocks
    // https://www.khronos.org/opengl/wiki/Shader_Storage_Buffer_Object
    GLuint m_ssboId = 0;
    std::vector<Inner> m_objects;
    size_t m_allocSize = 0;

    inline void bindGlBuffer() {
        DEBUG_ASSERT(0 != m_ssboId);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboId);
        gl::assertNoError();
    }

    inline void unbindGlBuffer() {
#ifdef DEBUG_ASSERTIONS
        int currentSSBO = gl::getCurrentSSBO();
        DEBUG_ASSERTF(currentSSBO == m_ssboId, "Trying to unbind another SSBO");
        // unbind the buffer
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
#endif
        gl::assertNoError();
    }


public:
    // Reallocates the buffer to match our internal representation
    void realloc() {
        DEBUG_ASSERT(0 != m_ssboId);
        DEBUG_ASSERTF((m_objects.size() * sizeof(Inner)) != m_allocSize,
                      "Sizes didn't change, use updateAt(idx) to change element");
        this->bindGlBuffer();

        m_allocSize = m_objects.size() * sizeof(Inner);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_allocSize, m_objects.data(), GL_DYNAMIC_DRAW);
        gl::assertNoError();

        this->unbindGlBuffer();
    }

    void updateAt(size_t idx) {
        DEBUG_ASSERTF(idx < m_objects.size(), "Trying to update object outside bounds");
        DEBUG_ASSERT(0 != m_ssboId);
        DEBUG_ASSERTF((m_objects.size() * sizeof(Inner)) == m_allocSize,
                      "Sizes changed, use realloc()");
        size_t size = sizeof(Inner);
        size_t offset = size * idx;
        Inner& data = m_objects.at(idx);
        this->bindGlBuffer();
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, &data);
        this->unbindGlBuffer();
    }

    SSBO(const SSBO &other) = delete;

    SSBO(SSBO &&other) noexcept: m_ssboId(other.m_ssboId),
                                 m_objects(std::move(other.m_objects)) {
        other.m_ssboId = 0;
    }

    explicit SSBO() {
        glGenBuffers(1, &m_ssboId);
        DEBUG_ASSERT(0 != m_ssboId);
    }

    /*
     *  Binds the buffer to the binding used in GLSL shader
     */
    void bind(GLuint bindIndex) {
        DEBUG_ASSERT(0 != m_ssboId);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindIndex, m_ssboId);
        gl::assertNoError();
    }

    std::vector<Inner> &objects() {
        return m_objects;
    }

    ~SSBO() {
        if (0 != m_ssboId) {
            glDeleteBuffers(1, &m_ssboId);
        }
    }
};
