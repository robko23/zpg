//
// Created by robko on 10/19/24.
//

#pragma once

#include "Scene.h"
#include <memory>
#include <chrono>

class SceneFpsDisplay : public Scene {
    std::unique_ptr<Scene> s;
    double minFps = 999;
public:
    explicit SceneFpsDisplay(std::unique_ptr<Scene> s) : s(std::move(s)) {}

    void render() override {
        auto start = std::chrono::high_resolution_clock::now();
        s->render();
        auto stop = std::chrono::high_resolution_clock::now();
        auto diff = stop - start;
        auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
        auto duration_s = static_cast<double>(duration_ms) / 1000.;
        auto fps = 1 / duration_s;
        minFps = std::min(fps, minFps);

        ImGui::Begin("Performance");
        ImGui::Text("Frame time: %ld ms", duration_ms);
        ImGui::Text("Fps: %f", fps);
        ImGui::Text("Min fps: %f", minFps);
        ImGui::End();
    }

    [[nodiscard]] bool shouldExit() const override {
        return s->shouldExit();
    }
};
