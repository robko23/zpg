//
// Created by robko on 10/14/24.
//

#pragma once

#include <deque>
#include <algorithm>
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <memory>
#include "Projection.h"
#include "Observer.h"

struct ViewMatrix {
    glm::mat4 viewMatrix;
};

class Camera : public Observable<ViewMatrix> {
private:
    glm::vec3 m_eye;
    glm::vec3 m_target;
    glm::vec3 m_up;
    double yaw = 0;
    double pitch = 0;
    double sensitivity;
    glm::vec2 prev_pos = glm::vec2(0);
    bool firstMouse = true;
    glm::vec<2, double> prevPos = glm::vec2(0);
    std::shared_ptr<PerspectiveProjection> perspectiveProjection;
    std::shared_ptr<GLWindow> window;

    glm::mat4 viewMatrix = glm::mat4(1);

    void handleChange() {
        recalculate();
        notify(ViewMatrix{.viewMatrix = viewMatrix});
    }

    void recalculate() {
        viewMatrix = glm::lookAt(m_eye, m_eye + m_target, m_up);
    }

public:
    explicit Camera(double sensitivity, const std::shared_ptr<GLWindow> &window,
                    glm::vec3 initialPosition = glm::vec3(-3, 3, -3))
            : Observable<ViewMatrix>(ViewMatrix{.viewMatrix = glm::mat4(1)}), m_eye(initialPosition),
              m_target(0), m_up(0, 1, 0), sensitivity(sensitivity),
              window(window) {
        perspectiveProjection = std::make_shared<PerspectiveProjection>();
        window->attach(perspectiveProjection);
        handleChange();
    }

    [[nodiscard]] PerspectiveProjection* projection() {
        auto ptr = perspectiveProjection.get();
        DEBUG_ASSERT_NOT_NULL(ptr)
        return ptr;
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

    ~Camera() override {
        window->detach(perspectiveProjection);
    };
};

