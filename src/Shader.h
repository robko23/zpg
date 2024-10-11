//
// Created by robko on 10/1/24.
//

#ifndef ZPG_SHADER_H
#define ZPG_SHADER_H

#include <GL/gl.h>
#include <string>
#include <expected>
#include <iostream>
#include <optional>
#include "drawable/Drawable.h"

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

class ShaderRunner {
    friend class ShaderProgram;
private:
    GLuint program_id;
    explicit ShaderRunner(GLuint program_id): program_id(program_id) {
        DEBUG_ASSERT(0 != program_id);
        glUseProgram(this->program_id);
    }

public:
    ShaderRunner bind(const char* name, const glm::mat4& mat) const {
        GLint id = glGetUniformLocation(program_id, name);
        DEBUG_ASSERTF(-1 != id, "Parameter %s may not exist in the shader", name);

#ifdef DEBUG_ASSERTIONS
        {
            GLenum err = glGetError();
            DEBUG_ASSERT(err != GL_INVALID_VALUE);
            DEBUG_ASSERT(err != GL_INVALID_OPERATION);
        };
#endif

        glUniformMatrix4fv(id, 1, GL_FALSE, &mat[0][0]);

#ifdef DEBUG_ASSERTIONS
        {
            GLenum err = glGetError();
            DEBUG_ASSERT(err != GL_INVALID_VALUE);
            DEBUG_ASSERT(err != GL_INVALID_OPERATION);
        };
#endif
        return *this;
    }

    void draw(Drawable& drawable) const {
        DEBUG_ASSERT(0 != program_id)
        drawable.draw_raw();
        glUseProgram(0);
    }
};

class ShaderProgram {
private:
    GLuint program_id;

    explicit ShaderProgram(
            GLuint programId) :
            program_id(programId) {
        DEBUG_ASSERT(0 != program_id);
    }

public:

    /*
     * Links fragment shader and vertex shader into complete program.
     * Returns null on failure and logs the error to stderr
     */
    static std::optional<ShaderProgram> link(const FragmentShader& fragment, const VertexShader& vertex) {

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

    [[nodiscard]] ShaderRunner begin() const {
        return ShaderRunner(program_id);
    }
};


#endif //ZPG_SHADER_H
