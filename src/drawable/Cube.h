//
// Created by robko on 11/16/24.
//

#pragma once

#include "Drawable.h"
#include "../assertions.h"

class Cube: public Drawable {
private:
    static inline const GLfloat data[] = {
            -1.0f,-1.0f,-1.0f, 1, 1, 1,
            -1.0f,-1.0f, 1.0f, 1, 1, 1,
            -1.0f, 1.0f, 1.0f, 1, 1, 1,
            1.0f, 1.0f,-1.0f, 1, 1, 1,
            -1.0f,-1.0f,-1.0f, 1, 1, 1,
            -1.0f, 1.0f,-1.0f, 1, 1, 1,
            1.0f,-1.0f, 1.0f, 1, 1, 1,
            -1.0f,-1.0f,-1.0f, 1, 1, 1,
            1.0f,-1.0f,-1.0f, 1, 1, 1,
            1.0f, 1.0f,-1.0f, 1, 1, 1,
            1.0f,-1.0f,-1.0f, 1, 1, 1,
            -1.0f,-1.0f,-1.0f, 1, 1, 1,
            -1.0f,-1.0f,-1.0f, 1, 1, 1,
            -1.0f, 1.0f, 1.0f, 1, 1, 1,
            -1.0f, 1.0f,-1.0f, 1, 1, 1,
            1.0f,-1.0f, 1.0f, 1, 1, 1,
            -1.0f,-1.0f, 1.0f, 1, 1, 1,
            -1.0f,-1.0f,-1.0f, 1, 1, 1,
            -1.0f, 1.0f, 1.0f, 1, 1, 1,
            -1.0f,-1.0f, 1.0f, 1, 1, 1,
            1.0f,-1.0f, 1.0f, 1, 1, 1,
            1.0f, 1.0f, 1.0f, 1, 1, 1,
            1.0f,-1.0f,-1.0f, 1, 1, 1,
            1.0f, 1.0f,-1.0f, 1, 1, 1,
            1.0f,-1.0f,-1.0f, 1, 1, 1,
            1.0f, 1.0f, 1.0f, 1, 1, 1,
            1.0f,-1.0f, 1.0f, 1, 1, 1,
            1.0f, 1.0f, 1.0f, 1, 1, 1,
            1.0f, 1.0f,-1.0f, 1, 1, 1,
            -1.0f, 1.0f,-1.0f, 1, 1, 1,
            1.0f, 1.0f, 1.0f, 1, 1, 1,
            -1.0f, 1.0f,-1.0f, 1, 1, 1,
            -1.0f, 1.0f, 1.0f, 1, 1, 1,
            1.0f, 1.0f, 1.0f, 1, 1, 1,
            -1.0f, 1.0f, 1.0f, 1, 1, 1,
            1.0f,-1.0f, 1.0f,  1, 1, 1
    };

    GLuint vbo = 0;
    GLuint vao = 0;
public:
    Cube() {
        //vertex buffer object (VBO)
        glGenBuffers(1, &vbo); // generate the VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

        //Vertex Array Object (VAO)
        glGenVertexArrays(1, &vao); //generate the VAO
        glBindVertexArray(vao); //bind the VAO
        glEnableVertexAttribArray(0); //enable vertex attributes
        glEnableVertexAttribArray(1); //enable normal attributes
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*) 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                              (GLvoid*) (3 * sizeof(float)));

        DEBUG_ASSERT(0 != vbo);
        DEBUG_ASSERT(0 != vao);
    }
    void draw() override {
        glBindVertexArray(this->vao);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);
    }
};
