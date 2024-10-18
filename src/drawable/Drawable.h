//
// Created by robko on 10/5/24.
//

#ifndef ZPG_DRAWABLE_H
#define ZPG_DRAWABLE_H
//#include "../Shader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Drawable {
public:
//    virtual void draw(ShaderProgram& shader) = 0;
    virtual void draw() = 0;
};

#endif //ZPG_DRAWABLE_H
