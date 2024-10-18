//
// Created by robko on 10/11/24.
//

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Drawable.h"
#include "../assertions.h"
#include "../models/suzi_smooth.h"

class Suzi: public Drawable {
private:
    GLuint vbo = 0;
    GLuint vao = 0;

public:
    Suzi() {
        //vertex buffer object (VBO)
        glGenBuffers(1, &vbo); // generate the VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(suziSmooth), suziSmooth, GL_STATIC_DRAW);

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

    void draw() override {
        glBindVertexArray(this->vao);
        glDrawArrays(GL_TRIANGLES, 0, 2904);
    }

};

