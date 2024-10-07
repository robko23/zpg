//
// Created by robko on 10/5/24.
//

#ifndef ZPG_RECTANGLE_H
#define ZPG_RECTANGLE_H

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

    void draw(ShaderProgram &shader) override {
        shader.withShader([this]() -> void {
            glBindVertexArray(this->vao);
            // draw triangles
            glDrawArrays(GL_TRIANGLES, 0, 6); //mode,first,count
//            glDrawArrays(GL_TRIANGLES, 3, 3); //mode,first,count
        });
    }
};

#endif //ZPG_RECTANGLE_H
