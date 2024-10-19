//
// Created by robko on 10/19/24.
//

#pragma once

#include "Scene.h"
#include <vector>
#include <memory>
#include "../assertions.h"

class SceneSwitcher : public Scene {
private:
    std::vector<std::shared_ptr<Scene>> scenes;
    Scene* current = nullptr;
    bool running = true;

    void renderMenu() {
        DEBUG_ASSERT(nullptr == current);
        ImGui::Begin("Scene selector");
        for (auto &scene: scenes) {
            if (ImGui::Button(scene->getId())) {
                std::cout << "Changing scene to: " << scene->getId() << std::endl;
                current = scene.get();
            }
        }
        if (ImGui::Button("Exit")) {
            running = false;
        }
        ImGui::End();
    }

public:
    void addScene(const std::shared_ptr<Scene> &scene) {
        scenes.emplace_back(scene);
    }

    void render() override {
        if (nullptr == current) {
            renderMenu();
        } else {
            current->render();
        }
    }

    [[nodiscard]] bool shouldExit() override {
        if (nullptr == current) {
            return !running;
        } else {
            if (current->shouldExit()) {
                current = nullptr;
            }
        }
        return false;
    }

    const char* getId() override {
        return "scene-switcher";
    }

};