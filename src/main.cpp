#include <GL/glew.h>
//Include GLFW
#include <GLFW/glfw3.h>

////Include GLM
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
//
////Include the standard C++ headers
#include <cstdlib>
#include <cstdio>
#include <iostream>
//
#include "Application.h"
#include "Shader.h"
#include "GLFWContext.h"
#include "gl_info.h"
#include "drawable/Sphere.h"

const char* vertex_shader =
        "#version 330\n"
        "layout(location=0) in vec3 vp;"
        "layout(location=0) in vec3 vn;"
        "out vec3 color;"
        "uniform mat4 modelMatrix;"
        "void main () {"
        "     color = vn;"
        "     gl_Position = modelMatrix * vec4 (vp, 1.0);"
        "}";


const char* fragment_shader =
        "#version 330\n"
        "out vec4 frag_colour;"
        "in vec3 color;"
        "void main () {"
        "     frag_colour = vec4 (color, 1.0);"
        "}";


int main(void) {
    GLFWcontext::inContext([]() {
        if (!Application::init()) {
            fprintf(stderr, "ERROR: Failed to initialize application");
            exit(EXIT_FAILURE);
        }

        Application::get_instance()->window.inContext([]() {
            print_gl_info();

            Sphere sphere;

            //create and compile shaders
            auto maybeVertexShader = VertexShader::compile(vertex_shader);
            auto maybeFragmentShader = FragmentShader::compile(fragment_shader);

            // todo: handle maybe value
            auto vertexShader = maybeVertexShader.value();
            auto fragmentShader = maybeFragmentShader.value();

            auto shaderProgram = ShaderProgram::link(fragmentShader, vertexShader);

            glm::mat4 M = glm::mat4(1.0f); // construct identity matrix

            float angle = 0.9;
            float myView = 0.2;
            M = glm::rotate(M, angle, glm::vec3(1.0f, 1.0f, 1.0f));
//            M = glm::rotate(M, angle, glm::vec3(1.0f, 0.0f, 0.0f));
            M = glm::translate(M, glm::vec3(0.0f, 0.0f, myView));
            M = glm::scale(M, glm::vec3(0.5f));

            GLint idModelTransform = glGetUniformLocation(shaderProgram->getId(), "modelMatrix");

            Application::get_instance()->run(
                    [&shaderProgram, &sphere, idModelTransform, M]() -> void {
                        shaderProgram->withShader([&sphere, &shaderProgram, &idModelTransform, &M](){
                            glUniformMatrix4fv(idModelTransform, 1, GL_FALSE, &M[0][0]);
                            //location, count, transpose, *value
                            sphere.draw(shaderProgram.value());
                        });
                    });
        });

    });

    return EXIT_SUCCESS;
}
