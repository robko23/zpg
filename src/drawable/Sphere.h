//
// Created by robko on 10/7/24.
//

#ifndef ZPG_SPHERE_H
#define ZPG_SPHERE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Drawable.h"
#include "../models/sphere.h"

class Sphere: public Drawable {
private:
    GLuint vbo = 0;
    GLuint vao = 0;

public:
    Sphere() {
        //vertex buffer object (VBO)
        glGenBuffers(1, &vbo); // generate the VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(sphere), sphere, GL_STATIC_DRAW);

        //Vertex Array Object (VAO)
        glGenVertexArrays(1, &vao); //generate the VAO
        glBindVertexArray(vao); //bind the VAO
        glEnableVertexAttribArray(0); //enable vertex attributes
        glEnableVertexAttribArray(1); //enable vertex attributes
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));

        DEBUG_ASSERT(0 != vbo);
        DEBUG_ASSERT(0 != vao);
    }

    void draw_raw() override {
        glBindVertexArray(this->vao);
        glDrawArrays(GL_TRIANGLES, 0, 2880);
    }

//    void draw(ShaderProgram &shader) override {
//        shader.withShader([this]() -> void {
//            this->draw_raw();
//        });
//    }
};

#endif //ZPG_SPHERE_H
