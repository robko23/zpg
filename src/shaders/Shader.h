//
// Created by robko on 10/1/24.
//

#ifndef ZPG_SHADER_H
#define ZPG_SHADER_H

#include <string>
#include <expected>
#include <iostream>
#include <optional>
#include "../drawable/Drawable.h"
#include <GL/gl.h>
#include <memory>
#include "../assertions.h"
#include "ShaderLoader.h"
#include "../gl_utils.h"
#include <glm/matrix.hpp>

template<typename Derived>
class ShaderBase {
private:
    GLuint id;

    explicit ShaderBase(GLuint id) : id(id) {};

    virtual ~ShaderBase() = default;

    friend class FragmentShader;

    friend class VertexShader;

    friend class ShaderProgram;

public:
    /*
     * Tries to compile new program. If compilation fails, it returns null and
     * logs error to stderr
     */
    static std::optional<Derived> compile(const std::string &code) {
        const auto code_c = code.c_str();
        return compile(code_c);
    }

    static std::optional<Derived> compile(const char* const code) {
        const GLenum shader_type = Derived::getShaderType();
        GLuint shader_id = glCreateShader(shader_type);
        glShaderSource(shader_id, 1, &code, nullptr);
        glCompileShader(shader_id);

        GLint status;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
        if (GL_FALSE == status) {
            GLint infoLogLength;
            glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &infoLogLength);
            auto infoLog = std::make_unique<char>(infoLogLength + 1);
            glGetShaderInfoLog(shader_id, 512, nullptr, infoLog.get());
            infoLog.get()[infoLogLength] = '\0';
            std::cerr << "ERROR: failed to compile shader: " << infoLog << std::endl;
            return {};
        }

        return Derived(shader_id);
    }
};

#define DEFINE_SHADER($NAME, $TYPE) \
class $NAME final : public ShaderBase<$NAME> { \
private: \
    friend class ShaderBase; \
    explicit $NAME(GLuint id) : ShaderBase(id) {} \
private: \
    static constexpr GLenum getShaderType() { \
        return $TYPE; \
    } \
};

DEFINE_SHADER(FragmentShader, GL_FRAGMENT_SHADER)

DEFINE_SHADER(VertexShader, GL_VERTEX_SHADER)

#undef DEFINE_SHADER

class ShaderProgram: public ShaderLoader {
private:
    GLuint program_id;
    bool bound;

    explicit ShaderProgram(
            GLuint programId) :
            program_id(programId), bound(false) {
        DEBUG_ASSERT(0 != program_id);
    }

public:

    ShaderProgram(ShaderProgram &other) : program_id(other.program_id), bound(program_id) {
        other.program_id = 0;
    }

    ShaderProgram(ShaderProgram &&other) noexcept: program_id(other.program_id), bound(program_id) {
        other.program_id = 0;
    }

    /*
     * Links fragment shader and vertex shader into complete program.
     * Returns null on failure and logs the error to stderr
     */
    static std::optional<ShaderProgram>
    link(const FragmentShader &fragment, const VertexShader &vertex) {

        GLuint program = glCreateProgram();
        glAttachShader(program, fragment.id);
        glAttachShader(program, vertex.id);
        glLinkProgram(program);

        GLint status;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (GL_FALSE == status) {
            GLint infoLogLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
            auto infoLog = std::make_unique<char>(infoLogLength + 1);
            glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.get());
            std::cerr << "ERROR: failed to link shader program: " << infoLog << std::endl;
            return {};
        }

        return ShaderProgram(program);
    }

    inline void bind() {
        bound = true;
#ifdef DEBUG_ASSERTIONS
        DEBUG_ASSERTF(gl::getCurrentProgram() == 0,
                      "Another shader program is bound already");
#endif

        DEBUG_ASSERT(0 != this->program_id);
        glUseProgram(this->program_id);
    }

    inline void unbind() {
        bound = false;
#ifdef DEBUG_ASSERTIONS
        DEBUG_ASSERTF(gl::getCurrentProgram() == this->program_id,
                      "Trying to unbind shader of another instance");

        glUseProgram(0);
#endif
    }

    [[nodiscard]] inline bool isBound() const {
        if (this->bound) {
#ifdef DEBUG_ASSERTIONS
            GLint prog = 0;
            glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
            DEBUG_ASSERT(this->program_id == prog);
#endif
        }
        return this->bound;
    }

private:
    void bindInner(const char* name, const std::function<void(GLint)> &function) const {
        DEBUG_ASSERT(this->isBound());

        GLint id = glGetUniformLocation(program_id, name);
        DEBUG_ASSERTF(-1 != id, "Parameter %s may not exist in the shader", name);

#ifdef DEBUG_ASSERTIONS
        {
            GLenum err = glGetError();
            DEBUG_ASSERT(err != GL_INVALID_VALUE);
            DEBUG_ASSERT(err != GL_INVALID_OPERATION);
        };
#endif

        function(id);

#ifdef DEBUG_ASSERTIONS
        {
            GLenum err = glGetError();
            DEBUG_ASSERT(err != GL_INVALID_VALUE);
            DEBUG_ASSERT(err != GL_INVALID_OPERATION);
        }
#endif
    }

public:

    void bindParam(const char* name, const glm::mat4 &mat) {
        bindInner(name, [&mat](GLint id) {
            glUniformMatrix4fv(id, 1, GL_FALSE, &mat[0][0]);
        });
    }

    void bindParam(const char* name, const glm::mat3 &mat) {
        bindInner(name, [&mat](GLint id) {
            glUniformMatrix3fv(id, 1, GL_FALSE, &mat[0][0]);
        });
    }

    void bindParam(const char* name, const glm::vec4 &vec) {
        bindInner(name, [&vec](GLint id) {
            glUniform4fv(id, 1, &vec[0]);
        });
    }

    void bindParam(const char* name, const glm::vec3 &vec) {
        bindInner(name, [&vec](GLint id) {
            glUniform3fv(id, 1, &vec[0]);
        });
    }

    void bindParam(const char* name, float val) {
        bindInner(name, [val](GLint id) {
            glUniform1f(id, val);
        });
    }

    void bindParam(const char* name, int32_t val) {
        static_assert(sizeof(int32_t) == sizeof(GLint));
        bindInner(name, [val](GLint id) {
            glUniform1i(id, val);
        });
    }

    bool operator==(const ShaderProgram &rhs) const {
        return program_id == rhs.program_id;
    }

    bool operator!=(const ShaderProgram &rhs) const {
        return !(rhs == *this);
    }

    ~ShaderProgram() {
        if (0 != program_id) {
            glDeleteProgram(program_id);
#ifdef DEBUG_ASSERTIONS
            GLenum err = glGetError();
            DEBUG_ASSERT(err != GL_INVALID_VALUE);
#endif
        }
    }
};

class Shader {
public:
    virtual void bind() = 0;

    virtual void unbind() = 0;

    virtual bool isBound() = 0;

#ifdef DEBUG_ASSERTIONS

    inline static bool isInShaderContext() {
        return gl::getCurrentProgram() != 0;
    }

#endif
};

#endif //ZPG_SHADER_H
