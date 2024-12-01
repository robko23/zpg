#pragma once

#include "ShaderCommon.h"
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif // !GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

class ShaderBasic
    : public ShaderCommon<ShaderBasic, "basic.glsl", "basic.glsl"> {
  public:
    using ShaderCommon::ShaderCommon;
};
