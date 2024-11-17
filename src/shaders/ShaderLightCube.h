//
// Created by robko on 11/16/24.
//

#pragma once

#include "ShaderCommon.h"

class ShaderLightCube
        : public ShaderCommon<ShaderLightCube, "lightCube.glsl", "lightCube.glsl"> {
    using ShaderCommon::ShaderCommon;
public:
    void setLightColor(glm::vec4 value) {
        auto bound = program.isBound();
        if (!bound) {
            program.bind();
        }
        program.bindParam("lightColor", value);
        if (!bound) {
            program.unbind();
        }
    }
};
