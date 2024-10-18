//
// Created by robko on 10/14/24.
//

#pragma once

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <deque>

class Transformation {
public:
    virtual glm::mat4 apply(glm::mat4 source) = 0;
    virtual ~Transformation() = default;
};

class TransformationRotate : public Transformation {
private:
    glm::vec3 axis = glm::vec3(1, 1, 1);
    float angle = 0;
public:
    explicit TransformationRotate(const glm::vec3 &axis, const float angle) : axis(axis),
                                                                              angle(angle) {}

    glm::mat4 apply(glm::mat4 source) override {
        return glm::rotate(source, angle, axis);
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
};

class TransformationTranslate : public Transformation {
private:
    glm::vec3 position = glm::vec3(1);
public:
    explicit TransformationTranslate(const glm::vec3 &position) : position(position) {}

    glm::mat4 apply(glm::mat4 source) override {
        return glm::translate(source, position);
    }
};

class TransformationBuilder {
private:
    std::deque<std::unique_ptr<Transformation>> transformations;

public:
    TransformationBuilder(TransformationBuilder &other) : transformations(
            std::move(other.transformations)) {}

    explicit TransformationBuilder() : transformations() {}

    TransformationBuilder scale(float scale) {
        transformations.emplace_back(std::make_unique<TransformationScale>(glm::vec3(scale)));
        return *this;
    }

    TransformationBuilder rotateX(float angle) {
        transformations.emplace_back(
                std::make_unique<TransformationRotate>(glm::vec3(1, 0, 0), angle));
        return *this;
    }

    TransformationBuilder rotateY(float angle) {
        transformations.emplace_back(
                std::make_unique<TransformationRotate>(glm::vec3(0, 1, 0), angle));
        return *this;
    }

    TransformationBuilder rotateZ(float angle) {
        transformations.emplace_back(
                std::make_unique<TransformationRotate>(glm::vec3(0, 0, 1), angle));
        return *this;
    }

    TransformationBuilder moveX(float moveBy) {
        transformations.emplace_back(
                std::make_unique<TransformationTranslate>(glm::vec3(moveBy, 0, 0)));
        return *this;
    }

    TransformationBuilder moveY(float moveBy) {
        transformations.emplace_back(
                std::make_unique<TransformationTranslate>(glm::vec3(0, moveBy, 0)));
        return *this;
    }

    TransformationBuilder moveZ(float moveBy) {
        transformations.emplace_back(
                std::make_unique<TransformationTranslate>(glm::vec3(0, 0, moveBy)));
        return *this;
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