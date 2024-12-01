#pragma once
#include "../assertions.h"
#include "Drawable.h"

class PlaneWithTexture : public Drawable {
  private:
    static inline const float points[] = {
        0.000000f,  -0.500000f, 0.500000f,  -0.872900f, 0.218200f,  0.436400f,
        0.836598f,  0.477063f,  0.000000f,  0.500000f,  0.000000f,  -0.872900f,
        0.218200f,  0.436400f,  0.399527f,  0.286309f,  -0.500000f, -0.500000f,
        -0.500000f, -0.872900f, 0.218200f,  0.436400f,  0.836598f,  0.000179f,
        -0.500000f, -0.500000f, -0.500000f, 0.000000f,  -1.000000f, 0.000000f,
        0.381686f,  0.999821f,  0.500000f,  -0.500000f, -0.500000f, 0.000000f,
        -1.000000f, 0.000000f,  0.000179f,  0.809067f,  0.000000f,  -0.500000f,
        0.500000f,  0.000000f,  -1.000000f, 0.000000f,  0.381686f,  0.522937f,
        0.500000f,  -0.500000f, -0.500000f, 0.872900f,  0.218200f,  0.436400f,
        0.399169f,  0.000179f,  0.000000f,  0.500000f,  0.000000f,  0.872900f,
        0.218200f,  0.436400f,  0.399169f,  0.522579f,  0.000000f,  -0.500000f,
        0.500000f,  0.872900f,  0.218200f,  0.436400f,  0.000179f,  0.261379f,
        -0.500000f, -0.500000f, -0.500000f, 0.000000f,  0.447200f,  -0.894400f,
        0.788901f,  0.477421f,  0.000000f,  0.500000f,  0.000000f,  0.000000f,
        0.447200f,  -0.894400f, 0.788901f,  0.999821f,  0.500000f,  -0.500000f,
        -0.500000f, 0.000000f,  0.447200f,  -0.894400f, 0.399527f,  0.651554f};
    GLuint vbo = 0;
    GLuint vao = 0;

  public:
    PlaneWithTexture() {

        glGenBuffers(1, &vbo); // generate the VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

        // Vertex Array Object (VAO)
        glGenVertexArrays(1, &vao); // generate the VAO
        glBindVertexArray(vao);     // bind the VAO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // enable vertex attributes
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              (GLvoid *)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              (GLvoid *)(sizeof(float) * 3));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              (GLvoid *)(sizeof(float) * 6));

        DEBUG_ASSERT(0 != vbo);
        DEBUG_ASSERT(0 != vao);
    }

    void draw() override {
        glBindVertexArray(this->vao);
        glDrawArrays(GL_TRIANGLES, 0, 2 * 3);
    }
};

class TestModel : public Drawable {
  private:
    const static inline float triangle[48] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f};

    GLuint vbo = 0;
    GLuint vao = 0;

  public:
    TestModel() {
        glGenBuffers(1, &vbo); // generate the VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), &triangle[0],
                     GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao); // generate the VAO
        glBindVertexArray(vao);     // bind the VAO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // enable vertex attributes
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              (GLvoid *)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              (GLvoid *)(sizeof(float) * 3));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              (GLvoid *)(sizeof(float) * 6));

        DEBUG_ASSERT(0 != vbo);
        DEBUG_ASSERT(0 != vao);
    }

    void draw() override {
        glBindVertexArray(this->vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};
