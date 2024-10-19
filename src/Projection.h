//
// Created by robko on 10/18/24.
//

#pragma once

#include "glm/mat4x4.hpp"
#include "ResizeObserver.h"
#include <vector>

class Projection {
protected:

public:
    [[nodiscard]] virtual glm::mat4 getProjectionMatrix() const = 0;

    virtual ~Projection() = default;
};

class PerspectiveProjection : public ResizeObserver, public Projection {
private:
    float fov = 60;
    int screenWidth;
    int screenHeight;
    float minDistance = 0.1;
    float maxDistance = 100;
    glm::mat4 projection;

    void recalculate() {
        float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
        projection = glm::perspective(glm::radians(fov), aspectRatio, minDistance, maxDistance);
    }

public:
    PerspectiveProjection(int screenWidth, int screenHeight) : screenWidth(screenWidth),
                                                               screenHeight(screenHeight),
                                                               projection() {
        recalculate();
    }

    inline void setFov(float val) {
        fov = val;
        recalculate();
    }

    inline void setMaxDistance(float val) {
        maxDistance = val;
        recalculate();
    }

    void onWindowResize(int width, int height) override {
        screenHeight = height;
        screenWidth = width;
        recalculate();
    }

    [[nodiscard]] glm::mat4 getProjectionMatrix() const override {
        return projection;
    }
};