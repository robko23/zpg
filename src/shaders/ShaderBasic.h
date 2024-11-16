#pragma once

#include "../Camera.h"
#include "Shader.h"
#include "../ShaderLoaderV2.h"
#include <optional>
#include <memory>
#include "glm/mat4x4.hpp"
#include "../Projection.h"
#include <glm/gtx/string_cast.hpp>
#include "../Observer.h"
#include "ShaderCommon.h"

class ShaderBasic
        : public ShaderCommon<ShaderBasic, "basic.glsl", "basic.glsl"> {
public:
    using ShaderCommon::ShaderCommon;
};