//
// Created by robko on 10/5/24.
//

#ifndef ZPG_TRIANGLE_H
#define ZPG_TRIANGLE_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../assertions.h"
#include "Drawable.h"

class Triangle : public Drawable {
private:
    inline static float points[] = {
            0.0f, 0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f
    };
    GLuint vbo = 0;
    GLuint vao = 0;

public:
    Triangle() {
        //vertex buffer object (VBO)
        glGenBuffers(1, &vbo); // generate the VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

        //Vertex Array Object (VAO)
        glGenVertexArrays(1, &vao); //generate the VAO
        glBindVertexArray(vao); //bind the VAO
        glEnableVertexAttribArray(0); //enable vertex attributes
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        DEBUG_ASSERT(0 != vbo);
        DEBUG_ASSERT(0 != vao);
    }

    void draw_raw() override {
        glBindVertexArray(this->vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

//    void draw(ShaderProgram &shader) override {
//        shader.withShader([this]() -> void {
//            this->draw_raw();
//        });
//    }
};


#endif //ZPG_TRIANGLE_H
