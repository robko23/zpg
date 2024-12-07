//
// Created by robko on 10/14/24.
//

#pragma once

#include "assertions.h"
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <deque>
#include <memory>

class Transformation {
public:
    virtual glm::mat4 apply(glm::mat4 source) = 0;

    virtual ~Transformation() = default;
};

class TransformationRotate : public Transformation {
private:
    glm::vec3 axis = glm::vec3(1, 1, 1);
    float angleRadians = 0;
public:
    explicit TransformationRotate(const glm::vec3 &axis, const float angle) : axis(axis),
                                                                              angleRadians(angle) {}

    glm::mat4 apply(glm::mat4 source) override {
        return glm::rotate(source, angleRadians, axis);
    }

    [[nodiscard]] inline float getAngleRadians() const {
        return angleRadians;
    }

    inline void setAngleRadians(float rad) {
        angleRadians = rad;
    }
};

class TransformationScale : public Transformation {
private:
    glm::vec3 scale = glm::vec3(1);
public:
    explicit TransformationScale(const glm::vec3 &scale) : scale(scale) {}

    glm::mat4 apply(glm::mat4 source) override {
        return glm::scale(source, scale);
    }

	void setScale(float val) {
		scale = glm::vec3(val);
	}
};

class TransformationTranslate : public Transformation {
private:
    glm::vec3 position = glm::vec3(1);
public:
    explicit TransformationTranslate(const glm::vec3 &position) : position(position) {}

    [[nodiscard]] const glm::vec3 &getPosition() const {
        return position;
    }

    void setPosition(const glm::vec3 &value) {
        position = value;
    }

    glm::mat4 apply(glm::mat4 source) override {
        return glm::translate(source, position);
    }
};

class TransformationBuilder {
private:
    std::deque<std::shared_ptr<Transformation>> transformations;

public:
    TransformationBuilder(TransformationBuilder &&other) : transformations(
            std::move(other.transformations)) {}

    TransformationBuilder(TransformationBuilder &other) = default;

    explicit TransformationBuilder() : transformations() {}

    // Copy assign contractor
    TransformationBuilder &operator=(const TransformationBuilder &other) = delete;

    // Move assign constructor
    TransformationBuilder &operator=(TransformationBuilder &&other) noexcept {
        if (this != &other) {
            transformations = std::move(other.transformations);
        }
        return *this;
    }

    TransformationBuilder scale(float scale) {
        transformations.emplace_back(std::make_shared<TransformationScale>(glm::vec3(scale)));
        return *this;
    }

    TransformationBuilder rotateX(float angleRadians) {
        transformations.emplace_back(
                std::make_shared<TransformationRotate>(glm::vec3(1, 0, 0), angleRadians));
        return *this;
    }

    TransformationBuilder rotateY(float angleRadians) {
        transformations.emplace_back(
                std::make_shared<TransformationRotate>(glm::vec3(0, 1, 0), angleRadians));
        return *this;
    }

    TransformationBuilder rotateZ(float angleRadians) {
        transformations.emplace_back(
                std::make_shared<TransformationRotate>(glm::vec3(0, 0, 1), angleRadians));
        return *this;
    }

    TransformationBuilder translate(float x, float y, float z) {
        transformations.emplace_back(
                std::make_shared<TransformationTranslate>(glm::vec3(x, y, z)));
        return *this;
    }

    TransformationBuilder translate(glm::vec3 pos) {
        transformations.emplace_back(
                std::make_shared<TransformationTranslate>(pos));
        return *this;
    }

    TransformationBuilder moveX(float moveBy) {
        transformations.emplace_back(
                std::make_shared<TransformationTranslate>(glm::vec3(moveBy, 0, 0)));
        return *this;
    }

    TransformationBuilder moveY(float moveBy) {
        transformations.emplace_back(
                std::make_shared<TransformationTranslate>(glm::vec3(0, moveBy, 0)));
        return *this;
    }

    TransformationBuilder moveZ(float moveBy) {
        transformations.emplace_back(
                std::make_shared<TransformationTranslate>(glm::vec3(0, 0, moveBy)));
        return *this;
    }

    [[nodiscard]] Transformation* at(size_t idx) const {
        DEBUG_ASSERTF(idx <= this->transformations.size(), "Trying to get element outside bouds");
        auto item = this->transformations.at(idx).get();
        DEBUG_ASSERT_NOT_NULL(item);
        return item;
    }

    std::shared_ptr<Transformation> replace(size_t idx, std::unique_ptr<Transformation> newTrans) {
        DEBUG_ASSERTF(idx <= this->transformations.size(), "Trying to get element outside bouds");
        auto item = std::move(this->transformations.at(idx));
        this->transformations[idx] = std::move(newTrans);
        return item;
    }

    glm::mat4 build(glm::mat4 source) {
        glm::mat4 tmp = source;
        for (const auto &item: this->transformations) {
            tmp = item->apply(tmp);
        }
        return tmp;
    }

    glm::mat4 build() {
        auto tmp = glm::mat4(1);
        for (const auto &item: this->transformations) {
            tmp = item->apply(tmp);
        }
        return tmp;
    }
};
