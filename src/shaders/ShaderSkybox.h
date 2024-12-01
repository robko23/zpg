//
// Created by robko on 11/16/24.
//

#pragma once

#include "ShaderCommon.h"

class ShaderSkybox
    : public ShaderCommon<ShaderSkybox, "skybox.glsl",
                          "skybox.glsl"> {
    using ShaderCommon::ShaderCommon;

  public:
    void setCubemapId(int32_t textureUnitId) {
        auto bound = program.isBound();
        if (!bound) {
            program.bind();
        }
        program.bindParam("UISky", textureUnitId);
        if (!bound) {
            program.unbind();
        }
    }
};
