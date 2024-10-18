//
// Created by robko on 10/14/24.
//

#pragma once

#include "glm/mat4x4.hpp"

class CameraObserver {
public:
    virtual void onCameraChange(glm::mat4) = 0;

    virtual bool eq(CameraObserver &other) = 0;

    bool operator==(CameraObserver &other) {
        return eq(other);
    }

    virtual ~CameraObserver() = default;
};
