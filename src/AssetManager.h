#pragma once

#include "assertions.h"
#include "shaders/Shader.h"
#include <cstdint>
#include <filesystem>
#include <optional>

enum AssetType : uint8_t {
    ASSET_VERTEX_SHADER,
    ASSET_FRAGMENT_SHADER,
    ASSET_TEXTURE,
};

class AssetManager {
  private:
    std::filesystem::path basePath;

    constexpr std::filesystem::path getAssetPrefix(AssetType type) const {
        switch (type) {
        case ASSET_VERTEX_SHADER:
            return "shaders/vertex";
        case ASSET_FRAGMENT_SHADER:
            return "shaders/fragment";
        case ASSET_TEXTURE:
            return "textures";
        }
        UNREACHABLE("Invalid AssetType: %d", type);
    }

    std::optional<std::string> readFile(AssetType type,
                                        const char *fileName) const {
        auto relPath = getAssetPrefix(type) / fileName;
        std::filesystem::path fullPath = basePath / relPath;

        auto file = std::ifstream(fullPath.string());

        DEBUG_ASSERTF(file.is_open(), "No asset at '%s'", relPath.c_str());
        std::string content((std::istreambuf_iterator<char>(file)),
                            (std::istreambuf_iterator<char>()));
        return content;
    }

  public:
    AssetManager(const std::string &basePath) : basePath(basePath) {}

    AssetManager(const AssetManager &) = delete;

    std::optional<FragmentShader> loadFragment(const char *path) {
        auto content = readFile(AssetType::ASSET_FRAGMENT_SHADER, path);
        if (!content.has_value()) {
            return {};
        }

        return FragmentShader::compile(content.value());
    }

    std::optional<VertexShader> loadVertex(const char *path) const {
        auto content = readFile(AssetType::ASSET_VERTEX_SHADER, path);
        if (!content.has_value()) {
            return {};
        }

        return VertexShader::compile(content.value());
    }
};
