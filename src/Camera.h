//
// Created by robko on 10/14/24.
//

#pragma once

#include "Observer.h"
#include "Projection.h"
#include "glm/ext/matrix_transform.hpp"
#include <memory>

struct CameraProperties {
    glm::mat4 viewMatrix;
    glm::vec3 cameraPosition;

    [[nodiscard]] static inline CameraProperties defaultProps() {
        return CameraProperties{.viewMatrix = glm::mat4(1),
                                .cameraPosition = glm::vec3(1)};
    }
};

class Camera : public Observable<CameraProperties> {
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
        notify(CameraProperties{.viewMatrix = viewMatrix,
                                .cameraPosition = m_eye});
    }

    void recalculate() {
        if (pitch > 89.0f) {
            pitch = 89.0f;
        } else if (pitch < -89.0f) {
            pitch = -89.0f;
        }

        if (yaw > 360) {
            yaw = 0;
        } else if (yaw < 0) {
            yaw = 360;
        }
        auto direction = glm::vec<3, double>(0);

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        m_target = glm::normalize(direction);

        viewMatrix = glm::lookAt(m_eye, m_eye + m_target, m_up);
    }

  public:
    explicit Camera(double sensitivity, const std::shared_ptr<GLWindow> &window,
                    glm::vec3 initialPosition = glm::vec3(-3, 3, -3))
        : Observable<CameraProperties>(CameraProperties{
              .viewMatrix = glm::mat4(1), .cameraPosition = initialPosition}),
          m_eye(initialPosition), m_target(0), m_up(0, 1, 0),
          sensitivity(sensitivity), window(window) {
        perspectiveProjection = std::make_shared<PerspectiveProjection>();
        window->attach(perspectiveProjection);
        handleChange();
    }

    [[nodiscard]] PerspectiveProjection *projection() {
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
        m_eye -= (glm::normalize(glm::cross(m_target, m_up)))*speed;
        handleChange();
    }

    void moveRight(float speed) {
        m_eye += (glm::normalize(glm::cross(m_target, m_up)))*speed;
        handleChange();
    }

    void setSensitivity(double val) { sensitivity = val; }

    void setPosition(glm::vec3 position) {
        m_eye = position;
        handleChange();
    }

    glm::vec3 getPosition() { return m_eye; }

    void setYaw(float val) {
        yaw = val;
        handleChange();
    }

    void setPitch(float val) {
        pitch = val;
        handleChange();
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

            recalculate();
            handleChange();
        }
    }

    ~Camera() override { window->detach(perspectiveProjection); };
};
