//
// Created by robko on 10/14/24.
//

#ifndef ZPG_SHADERLOADER_H
#define ZPG_SHADERLOADER_H

#include "shaders/Shader.h"
#include <optional>
#include <filesystem>
#include <iostream>
#include <fstream>

class ShaderLoader {
    std::filesystem::path basePath;

    std::optional<std::string> load(const char* type, const char* path) const {
        std::filesystem::path fullPath = basePath / type / path;

        auto file = std::ifstream(fullPath.string());

        if (!file.is_open()) {
            std::cerr << "No " << type << " shader at path " << fullPath << std::endl;
            return {};
        }
        std::string content((std::istreambuf_iterator<char>(file)),
                            (std::istreambuf_iterator<char>()));
        return content;
    }

public:
    explicit ShaderLoader(const std::string &basePath) : basePath(basePath) {}

    std::optional<FragmentShader> loadFragment(const char* path) const {
        auto content = load("fragment", path);
        if (!content.has_value()) {
            return {};
        }

        return FragmentShader::compile(content.value());
    }


    std::optional<VertexShader> loadVertex(const char* path) const {
        auto content = load("vertex", path);
        if (!content.has_value()) {
            return {};
        }

        return VertexShader::compile(content.value());
    }
};


#endif //ZPG_SHADERLOADER_H
