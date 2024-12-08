
#include "../LightsCollection.h"
#include "ShaderCommon.h"
#include "ShaderLights.h"

class ShaderLightTexture
    : public ShaderCommon<ShaderLightTexture, "basicTexture.glsl",
                          "textureLight.glsl"> {
  private:
    std::shared_ptr<LightsCollection> lights;

  protected:
    void onCameraPositionChange(glm::vec3 cameraPosition) override {
        DEBUG_ASSERT(program.isBound());
        program.bindParam("cameraPosition", cameraPosition);
    }

    using ShaderCommon::ShaderCommon;

  public:
    void setLightCollection(const std::shared_ptr<LightsCollection> &val) {
        lights = val;
    }

    [[nodiscard]] std::shared_ptr<LightsCollection> getLightCollection() const {
        return lights;
    }

    ShaderLightTexture(const ShaderLights &other) = delete;

    ShaderLightTexture(ShaderLightTexture &&other) noexcept
        : ShaderCommon(std::move(other)), lights(std::move(other.lights)) {}

    void setMaterial(const Material &material) {
        program.bind();
        program.bindParam("material.ambient", material.getAmbient());
        program.bindParam("material.diffuse", material.getDiffuse());
        program.bindParam("material.specular", material.getSpecular());
        program.bindParam("material.shininess", material.getShininess());
        program.unbind();
    }

    void bind() override {
        ShaderCommon::bind();
        lights->bind(0);
    }

    void setTextureUnitId(int32_t textureUnitId) {
        program.bindParam("textureUnitId", textureUnitId);
    }
};
