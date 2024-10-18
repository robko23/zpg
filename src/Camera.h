//
// Created by robko on 10/14/24.
//

#pragma once

#include <deque>
#include <algorithm>
#include "CameraObserver.h"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera {
private:
    std::deque<std::shared_ptr<CameraObserver>> observers;
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up = glm::vec3(1);
    glm::mat4 camera;

    void handleChange() {
        recalculate();
        for (const auto &item: observers) {
            item->onCameraChange(camera);
        }
    }

    void recalculate() {
        auto direction = glm::normalize(m_position - m_target);
        auto right = glm::normalize(glm::cross(m_up, direction));
        auto up = glm::cross(direction, right);
        camera = glm::lookAt(m_target, right, up);
    }

public:
    explicit Camera() : observers(), m_position(0, 0, 3), m_target(1), camera() {
        recalculate();
    }

    explicit Camera(glm::vec3 position, glm::vec3 target)
            : observers(), m_position(position), m_target(target), camera() {
        recalculate();
    }

    void moveForward(float howMuch) {
        m_position += glm::vec3(howMuch, 0, 0);
//        position[0] += howMuch;
        handleChange();
    }

    void moveBack(float howMuch) {
        m_position[0] -= howMuch;
        handleChange();
    }

    void moveLeft(float howMuch) {
        m_position[1] -= howMuch;
    }

    void moveRight(float howMuch) {
        m_position[1] += howMuch;
    }


    void registerCameraObserver(const std::shared_ptr<CameraObserver> &observer) noexcept {
        observers.emplace_back(observer);
    }

    void unregisterObserver(const std::shared_ptr<CameraObserver> &observer) {
        auto it = std::find_if(observers.begin(), observers.end(),
                               [&observer](const std::shared_ptr<CameraObserver> &o) {
                                   return *o == *observer;
                               });
        if (it != observers.end()) {
            observers.erase(it);
        }
    }
};

