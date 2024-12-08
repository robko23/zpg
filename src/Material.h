#pragma once

#include "glm/ext/vector_float4.hpp"
class Material {
  private:
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float shininess;

  public:
    Material(const glm::vec4 &ambient, const glm::vec4 &diffuse,
             const glm::vec4 &specular, float shininess)
        : ambient(ambient), diffuse(diffuse), specular(specular),
          shininess(shininess) {}

    [[nodiscard]] const glm::vec4 &getAmbient() const { return ambient; }

    void setAmbient(const glm::vec4 &val) { ambient = val; }

    [[nodiscard]] const glm::vec4 &getDiffuse() const { return diffuse; }

    void setDiffuse(const glm::vec4 &val) { diffuse = val; }

    [[nodiscard]] const glm::vec4 &getSpecular() const { return specular; }

    void setSpecular(const glm::vec4 &val) { specular = val; }

    [[nodiscard]] float getShininess() const { return shininess; }

    void setShininess(float val) { shininess = val; }
};
