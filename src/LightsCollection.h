#pragma once

#include "LightGLSL.h"
#include "assertions.h"
#include "shaders/SSBO.h"
#include <algorithm>

class LightsCollection {
  private:
    SSBO<LightGLSL> lights;

  public:
	explicit LightsCollection() = default;
    /*
     * Adds a light to the shader and sends it to the SSBO
     * @returns Index where the light is located. You can use it to modify it
     */
    size_t addLight(LightGLSL light) {
        size_t idx = lights.objects().size();
        light.setId(idx);
        lights.objects().emplace_back(std::move(light));
        lights.realloc();
        return idx;
    }

    void removeLight(size_t id) {
        auto &obj = lights.objects();
        auto it = std::ranges::find_if(
            obj, [id](const LightGLSL &val) { return val.getId() == id; });
        DEBUG_ASSERTF(it != obj.end(),
                      "Trying to remove non existent light: %zu", id);
        obj.erase(it);
        lights.realloc();
    }

    LightGLSL &getLight(size_t id) {
        auto &obj = lights.objects();
        for (auto &item : obj) {
            if (item.getId() == id) {
                return item;
            }
        }
        UNREACHABLE("Light with id %zu does not exists", id);
    }

    void updateLight(size_t id) {
        auto &obj = lights.objects();
        for (size_t i = 0; i < obj.size(); i++) {
            if (obj[i].getId() == id) {
                lights.updateAt(i);
                return;
            }
        }
    }

    void bind(uint32_t bindingId) { lights.bind(bindingId); }
};
