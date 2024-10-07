//
// Created by robko on 10/5/24.
//

#ifndef ZPG_DRAWABLE_H
#define ZPG_DRAWABLE_H
#include "../Shader.h"

class Drawable {
public:
    virtual void draw(ShaderProgram& shader) = 0;
};

#endif //ZPG_DRAWABLE_H
