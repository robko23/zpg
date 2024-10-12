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
#include "Shader.h"
#include "GLWindow.h"
#include "GLFWContext.h"
#include "gl_info.h"
#include "drawable/Sphere.h"
#include "drawable/Tree.h"
#include "drawable/Suzi.h"

const char* vertex_shader =
        "#version 330\n"
        "layout(location=0) in vec3 vp;"
        "layout(location=1) in vec3 vn;"
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

class Forest {
private:
    Tree tree = Tree();
    float minX = -0.7f;
    float maxX = 0.9f;
    float minY = -1.f;
    float maxY = 0.6f;

public:
    void draw(uint16_t amount, uint16_t rows, ShaderProgram &program,
              const std::unique_ptr<GLWindow> &window) {
        double delta = window->getDelta();
        if (window->isPressed(GLFW_KEY_Q)) {
            minX += delta;
        }
        if (window->isPressed(GLFW_KEY_W)) {
            minX -= delta;
        }
        if (window->isPressed(GLFW_KEY_A)) {
            minY += delta;
        }
        if (window->isPressed(GLFW_KEY_S)) {
            minY -= delta;
        }
        if (window->isPressed(GLFW_KEY_E)) {
            maxX += delta;
        }
        if (window->isPressed(GLFW_KEY_R)) {
            maxX -= delta;
        }
        if (window->isPressed(GLFW_KEY_D)) {
            maxY += delta;
        }
        if (window->isPressed(GLFW_KEY_F)) {
            maxY -= delta;
        }
        if (window->isPressedAndClear(GLFW_KEY_P)) {
            std::cout << "minX: " << minX << std::endl;
            std::cout << "maxX: " << maxX << std::endl;
            std::cout << "minY: " << minY << std::endl;
            std::cout << "maxY: " << maxY << std::endl;
        }
        uint16_t cols = amount / rows;
        float stepCol = (maxX - minX) / cols;
        float stepRow = (maxY - minY) / rows;
        float currentX = minX;
        float currentY = minY;

        for (int i = 0; i < cols; ++i) {
            for (int j = 0; j < rows; ++j) {
                auto mat = glm::mat4(1.0f);
                mat = glm::rotate(mat, 0.596677f, glm::vec3(-1.44582, 0.351348, 0.0501079));
                mat = glm::translate(mat, glm::vec3(currentX, currentY, 0.));
                mat = glm::scale(mat, glm::vec3(0.100465));
                currentX += stepCol;

                program.begin()
                        .bind("modelMatrix", mat)
                        .draw(tree);
            }
            currentX = minX;
            currentY += stepRow;
        }
    }
};

class MovableObject {
private:
    Drawable &drawable;
    bool focused = false;

    float posX = 0;
    float posY = 0;
    float scale = 1;
    float angle = 0;

    float rotX = 0;
    float rotY = 0;
    float rotZ = 0;
    glm::mat4 transMat = glm::mat4(1.0f);
public:
    explicit MovableObject(Drawable &drawable) : drawable(drawable) {}

    void setFocus(bool focus) {
        focused = focus;
    }

    void draw(const std::unique_ptr<GLWindow> &window, ShaderProgram &program) {
        if (focused) {
            double delta = window->getDelta();
            if (window->isPressed(GLFW_KEY_W)) {
                posY += 2 * delta;
            }
            if (window->isPressed(GLFW_KEY_A)) {
                posX -= 2 * delta;
            }
            if (window->isPressed(GLFW_KEY_S)) {
                posY -= 2 * delta;
            }
            if (window->isPressed(GLFW_KEY_D)) {
                posX += 2 * delta;
            }
            if (window->isPressed(GLFW_KEY_Z)) {
                scale += 2 * delta;
            }
            if (window->isPressed(GLFW_KEY_X)) {
                scale -= 2 * delta;
            }
            if (window->isPressed(GLFW_KEY_E)) {
                angle += 2 * delta;
            }
            if (window->isPressed(GLFW_KEY_R)) {
                angle -= 2 * delta;
            }
            if (window->isPressed(GLFW_KEY_T)) {
                rotX += 2 * delta;
            }
            if (window->isPressed(GLFW_KEY_Y)) {
                rotX -= 2 * delta;
            }
            if (window->isPressed(GLFW_KEY_G)) {
                rotY += 2 * delta;
            }
            if (window->isPressed(GLFW_KEY_H)) {
                rotY -= 2 * delta;
            }
            if (window->isPressed(GLFW_KEY_B)) {
                rotZ += 2 * delta;
            }
            if (window->isPressed(GLFW_KEY_N)) {
                rotZ -= 2 * delta;
            }
            if (window->isPressedAndClear(GLFW_KEY_P)) {
                std::cout << "--------------------------------" << std::endl;
                std::cout << "M = glm::rotate(M, " << angle << ", glm::vec3(" << rotX << ", "
                          << rotY << ", " << rotZ << "));" << std::endl;
                std::cout << "M = glm::translate(M, glm::vec3(" << posX << ", " << posY
                          << ", 0.));" << std::endl;
                std::cout << "M = glm::scale(M, glm::vec3(" << scale << "));" << std::endl;
            }

            transMat = glm::mat4(1.0f);
            transMat = glm::rotate(transMat, angle, glm::vec3(rotX, rotY, rotZ));
            // X - horizontal ---
            // Y - vertical |
            // Z - ??
            transMat = glm::translate(transMat, glm::vec3(posX, posY, 0.));
            transMat = glm::scale(transMat, glm::vec3(scale));
        }

        program.begin().bind("modelMatrix", transMat).draw(drawable);
    }
};

int main(void) {
    GLFWcontext::inContext([]() {

        auto window = GLWindow::create("ZPG").value();

        window->inContext([&window]() -> void {
            print_gl_info();

//            Sphere sphere;
//            Tree tree;
            Forest forest;
//            Suzi suzi;

            //create and compile shaders
            auto maybeVertexShader = VertexShader::compile(vertex_shader);
            auto maybeFragmentShader = FragmentShader::compile(fragment_shader);

            // todo: handle maybe value
            auto vertexShader = maybeVertexShader.value();
            auto fragmentShader = maybeFragmentShader.value();

            auto shaderProgram = ShaderProgram::link(fragmentShader, vertexShader).value();

//            MovableObject movableObject(tree);
//            movableObject.setFocus(true);

            uint16_t amount = 50;
            uint16_t rows = 7;
            while (!window->shouldClose()) {
//                auto delta = window->getDelta();
                // clear color and depth buffer
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // Top left
//                M = glm::rotate(M, 0.596677, glm::vec3(-1.44582, 0.351348, 0.0501079));
//                M = glm::translate(M, glm::vec3(-0.699883, 0.199051, 0.));
//                M = glm::scale(M, glm::vec3(0.100465));

//                M = glm::rotate(M, -0.333257, glm::vec3(0.66695, 0.500851, 0));
//                M = glm::translate(M, glm::vec3(0.432456, -0.165958, 0.));
//                M = glm::scale(M, glm::vec3(0.132511));
//                --------------------------------
//                M = glm::rotate(M, -0.333257, glm::vec3(0.66695, 0.500851, 0));
//                M = glm::translate(M, glm::vec3(-0.368396, -0.165958, 0.));
//                M = glm::scale(M, glm::vec3(0.132511));

                if (window->isPressedAndClear(GLFW_KEY_T)) {
                    amount += 1;
                }
                if (window->isPressedAndClear(GLFW_KEY_Y)) {
                    amount -= 1;
                }
                if (window->isPressedAndClear(GLFW_KEY_G)) {
                    rows += 1;
                }
                if (window->isPressedAndClear(GLFW_KEY_H)) {
                    rows -= 1;
                }

                forest.draw(amount, rows, shaderProgram, window);
//                movableObject.draw(window, shaderProgram);

                // update other events like input handling
                glfwPollEvents();
                window->endFrame();
            }
        });

    });

    return EXIT_SUCCESS;
}
