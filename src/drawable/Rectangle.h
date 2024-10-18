//
// Created by robko on 10/5/24.
//

#ifndef ZPG_RECTANGLE_H
#define ZPG_RECTANGLE_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Drawable.h"
#include "../assertions.h"

class Rectangle: public Drawable {
private:
    static inline float points[] = {
            0.6, 0.8, 0.,
            0.6, 0.6, 0.,
            0.3, 0.6, -1.f,

            0.6, 0.8, 0.0,
            0.3, 0.6, -1.0,
            0.3, 0.8, -1.0,
    };
    GLuint vbo = 0;
    GLuint vao = 0;

public:
    Rectangle() {
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

    void draw() override {
        glBindVertexArray(this->vao);
        glDrawArrays(GL_TRIANGLES, 0, 6); //mode,first,count
    }

//    void draw(ShaderProgram &shader) override {
//        shader.withShader([this]() -> void {
//            this->draw_raw();
//        });
//    }
};

#endif //ZPG_RECTANGLE_H
