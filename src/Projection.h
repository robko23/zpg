//
// Created by robko on 10/18/24.
//

#pragma once

#include "glm/mat4x4.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include <vector>
#include "GLWindow.h"

struct ProjectionMatrix {
    glm::mat4 projectionMatrix;

    inline static ProjectionMatrix _default() {
        return ProjectionMatrix{
                .projectionMatrix = glm::mat4(1)
        };
    }
};

class PerspectiveProjection : public Observer<WindowSize>, public Observable<ProjectionMatrix> {
private:
    float fov = 60;
    int screenWidth;
    int screenHeight;
    float minDistance = 0.1;
    float maxDistance = 100;
    ProjectionMatrix projectionMatrix;

    inline void assertSetSize() const {
        DEBUG_ASSERTF(screenWidth != 0 && screenHeight != 0,
                      "Window dimensions are not set. Did you forget to register windowResize callback?")
    }

    void recalculate() {
        assertSetSize();
        float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
        auto projectionRaw = glm::perspective(glm::radians(fov), aspectRatio, minDistance,
                                              maxDistance);
        projectionMatrix = ProjectionMatrix{.projectionMatrix = projectionRaw};
        notify(projectionMatrix);
    }

public:
    PerspectiveProjection() : Observable<ProjectionMatrix>(ProjectionMatrix::_default()),
                              screenWidth(0),
                              screenHeight(0), projectionMatrix(ProjectionMatrix::_default()) {
    }

    inline void setFov(float val) {
        fov = val;
        recalculate();
    }

    inline void setMaxDistance(float val) {
        maxDistance = val;
        recalculate();
    }

    void update(const WindowSize &action) override {
        screenHeight = action.height;
        screenWidth = action.width;
        recalculate();
    }

    [[nodiscard]] glm::mat4 getProjectionMatrix() const {
        assertSetSize();
        return projectionMatrix.projectionMatrix;
    }
};