#pragma once
#include "drawable/DynamicModel.h"
#include "shaders/Shader.h"

#include "Texture.h"
#include "assertions.h"
#include "gl_utils.h"
#include <GL/gl.h>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <unordered_map>

enum AssetType : uint8_t {
    ASSET_VERTEX_SHADER,
    ASSET_FRAGMENT_SHADER,
    ASSET_TEXTURE,
    ASSET_MODEL,
};

class AssetManager {
  private:
    std::filesystem::path basePath;
    size_t maxTextures = 0;
    size_t currentTexture = 0;
    std::unordered_map<std::filesystem::path, std::shared_ptr<Texture>>
        loadedTextures;
    std::optional<std::shared_ptr<Cubemap>> loadedCubemap = {};

    std::unordered_map<std::filesystem::path, std::shared_ptr<DynamicModel>>
        loadedModels;

    constexpr std::filesystem::path getAssetPrefix(AssetType type) const {
        switch (type) {
        case ASSET_VERTEX_SHADER:
            return "shaders/vertex";
        case ASSET_FRAGMENT_SHADER:
            return "shaders/fragment";
        case ASSET_TEXTURE:
            return "textures";
        case ASSET_MODEL:
            return "models/";
        }
        UNREACHABLE("Invalid AssetType: %d", type);
    }
    std::filesystem::path getAssetPath(AssetType type,
                                       const char *fileName) const {
        auto relPath = getAssetPrefix(type) / fileName;
        return basePath / relPath;
    }

    std::optional<std::string>
    readFileString(std::filesystem::path fullPath) const {
        std::optional<std::vector<uint8_t>> buffer = readFileBinary(fullPath);
        if (!buffer.has_value()) {
            return {};
        }
        std::string result(
            reinterpret_cast<const char *>(buffer.value().data()),
            buffer.value().size());
        return result;
    }

    std::optional<std::vector<uint8_t>>
    readFileBinary(std::filesystem::path fullPath) const {
        auto file = std::ifstream(fullPath.string());

        DEBUG_ASSERTF(file.is_open(), "No asset at '%s'", fullPath.c_str());
        file.seekg(0, std::ios::end);
        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        DEBUG_ASSERTF(fileSize != 0, "Failed to determine file size for %s",
                      fullPath.c_str());
        std::vector<uint8_t> buffer(fileSize);
        if (!file.read(reinterpret_cast<char *>(buffer.data()), fileSize)) {
            UNREACHABLE("Failed to read file at %s", fullPath.c_str());
        }
        file.close();
        return std::move(buffer);
    }

  public:
    AssetManager(const std::string &basePath)
        : basePath(basePath), maxTextures(gl::getMaxTextureUnits()) {
        DEBUG_ASSERTF(maxTextures != 0, "Max texture units is 0");
    }

    AssetManager(const AssetManager &) = delete;

    std::optional<FragmentShader> loadFragment(const char *path) {
        auto fullPath = getAssetPath(AssetType::ASSET_FRAGMENT_SHADER, path);
        auto content = readFileString(fullPath);
        if (!content.has_value()) {
            return {};
        }

        return FragmentShader::compile(content.value());
    }

    std::optional<VertexShader> loadVertex(const char *path) const {
        auto fullPath = getAssetPath(AssetType::ASSET_VERTEX_SHADER, path);
        auto content = readFileString(fullPath);
        if (!content.has_value()) {
            return {};
        }

        return VertexShader::compile(content.value());
    }

    std::shared_ptr<Texture> loadTexture(const char *name) {
        auto fullPath = getAssetPath(AssetType::ASSET_TEXTURE, name);
        if (loadedTextures.find(fullPath) != loadedTextures.end()) {
            std::cout << "Texture at " << fullPath << " is already loaded"
                      << std::endl;
            return loadedTextures[fullPath];
        }
        std::cout << "Texture at " << fullPath << " is not loaded, loading"
                  << std::endl;

        auto maybeBuf = readFileBinary(fullPath);
        DEBUG_ASSERTF(maybeBuf.has_value(), "Failed to read texture %s", name);

        DEBUG_ASSERTF(currentTexture <= maxTextures,
                      "Exceeded max textures: %zu", maxTextures);

        auto buf = maybeBuf.value();
        auto it = Texture::load(buf, currentTexture);
        currentTexture = currentTexture + 1;
        loadedTextures[fullPath] = it;
        return it;
    }

    std::shared_ptr<Cubemap> loadCubemap() {
        if (loadedCubemap.has_value()) {
            std::cout << "Cubemap/skybox is already loaded" << std::endl;
            return loadedCubemap.value();
        }
        std::cout << "Loading cubemap/skybox" << std::endl;
        auto cubemapBase = getAssetPath(AssetType::ASSET_TEXTURE, "skybox");

        auto xpos = readFileBinary(cubemapBase / "posx.jpg").value();
        auto xneg = readFileBinary(cubemapBase / "negx.jpg").value();
        auto ypos = readFileBinary(cubemapBase / "posy.jpg").value();
        auto yneg = readFileBinary(cubemapBase / "negy.jpg").value();
        auto zpos = readFileBinary(cubemapBase / "posz.jpg").value();
        auto zneg = readFileBinary(cubemapBase / "negz.jpg").value();

        DEBUG_ASSERTF(currentTexture <= maxTextures,
                      "Exceeded max textures: %zu", maxTextures);

        auto it =
            Cubemap::load(xpos, xneg, ypos, yneg, zpos, zneg, currentTexture);
        currentTexture = currentTexture + 1;
        loadedCubemap = it;
        return it;
    }

    std::shared_ptr<DynamicModel> loadModel(const std::string &path) {
        auto fullPath = getAssetPath(AssetType::ASSET_MODEL, path.c_str());
        if (loadedModels.find(fullPath) != loadedModels.end()) {
            std::cout << "Model at " << fullPath << " is already loaded"
                      << std::endl;
            return loadedModels[fullPath];
        }
        std::cout << "Model at " << fullPath << " is not loaded, loading"
                  << std::endl;

        auto maybeBuf = readFileBinary(fullPath);
        DEBUG_ASSERTF(maybeBuf.has_value(), "Failed to read model %s",
                      path.c_str());

        auto buf = maybeBuf.value();
        auto it = DynamicModel::load(buf);
        loadedModels[fullPath] = it;
        return it;
    }
};
