//
// Created by robko on 10/14/24.
//

#pragma once

#include <deque>
#include <algorithm>
#include "CameraObserver.h"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <memory>

class Camera {
private:
    std::deque<std::shared_ptr<CameraObserver>> observers;
    glm::vec3 m_eye;
    glm::vec3 m_target;
    glm::vec3 m_up;
    double yaw = 0;
    double pitch = 0;
    double sensitivity;
    glm::vec2 prev_pos = glm::vec2(0);
    bool firstMouse = true;
    glm::vec<2, double> prevPos = glm::vec2(0);

    glm::mat4 viewMatrix = glm::mat4(1);

    void handleChange() {
        recalculate();
        for (const auto &item: observers) {
            item->onCameraChange(viewMatrix);
        }
    }

    void recalculate() {
        viewMatrix = glm::lookAt(m_eye, m_eye + m_target, m_up);
    }

public:
    explicit Camera(double sensitivity, glm::vec3 initialPosition = glm::vec3(-3, 3, -3))
            : observers(), m_eye(initialPosition), m_target(0), m_up(0, 1, 0),
              sensitivity(sensitivity) {
        recalculate();
    }

    void moveForward(float speed) {
        m_eye += speed * m_target;
        handleChange();
    }

    void moveBack(float speed) {
        m_eye -= speed * m_target;
        handleChange();
    }

    void moveLeft(float speed) {
        m_eye -= (glm::normalize(glm::cross(m_target, m_up))) * speed;
        handleChange();
    }

    void moveRight(float speed) {
        m_eye += (glm::normalize(glm::cross(m_target, m_up))) * speed;
        handleChange();
    }

    void setSensitivity(double val) {
        sensitivity = val;
    }

    void onMouseMove(double mouseX, double mouseY) {
        if (firstMouse) {
            prevPos = glm::vec2(mouseX, mouseY);
            firstMouse = false;
        } else {
            double xOffset = mouseX - prev_pos.x;
            double yOffset = prev_pos.y - mouseY;
            prev_pos = glm::vec2(mouseX, mouseY);
            yaw += xOffset * sensitivity;
            pitch += yOffset * sensitivity;

            if (pitch > 89.0f) {
                pitch = 89.0f;
            } else if (pitch < -89.0f) {
                pitch = -89.0f;
            }
            auto direction = glm::vec<3, double>(0);

            direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction.y = sin(glm::radians(pitch));
            direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            m_target = glm::normalize(direction);
        }
        recalculate();
        handleChange();
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

