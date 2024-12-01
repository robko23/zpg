//
// Created by robko on 11/16/24.
//

#pragma once

#include "ShaderCommon.h"

class ShaderBasicTexture
    : public ShaderCommon<ShaderBasicTexture, "basicTexture.glsl",
                          "basicTexture.glsl"> {
    using ShaderCommon::ShaderCommon;

  public:
    void setTextureId(int32_t textureUnitId) {
        auto bound = program.isBound();
        if (!bound) {
            program.bind();
        }
        program.bindParam("textureUnitId", textureUnitId);
        if (!bound) {
            program.unbind();
        }
    }
};
