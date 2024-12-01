#pragma once

#include "assertions.h"
#include "gl_utils.h"
#include <GL/gl.h>
#include <SOIL.h>
#include <cstdint>
#include <memory>
#include <vector>

class Texture {
    static_assert(sizeof(GLuint) == sizeof(int));

  private:
    int textureId = 0;
    uint32_t boundTextureUnit = UINT32_MAX;

    Texture(int textureId, int boundTextureUnit)
        : textureId(textureId), boundTextureUnit(boundTextureUnit) {
        DEBUG_ASSERTF(textureId != 0, "Texture id is 0");
        DEBUG_ASSERTF(boundTextureUnit != UINT32_MAX,
                      "Bound texture unit is UINT32_MAX");
    }

  public:
    Texture(Texture &) = delete;
    Texture(Texture &&other) noexcept
        : textureId(other.textureId), boundTextureUnit(other.boundTextureUnit) {
        other.textureId = 0;
        other.boundTextureUnit = UINT32_MAX;
    }

    static std::shared_ptr<Texture> load(std::vector<uint8_t> buf,
                                         size_t textureUnit) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        gl::assertNoError();
        GLuint textureId = SOIL_load_OGL_texture_from_memory(
            buf.data(), buf.size(), SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID,
            SOIL_FLAG_INVERT_Y);
        if (0 == textureId) {
            auto err = SOIL_last_result();
            UNREACHABLE("Failed to load image: %s", err);
        }
        glBindTexture(GL_TEXTURE_2D, textureId);
        auto self =
            std::shared_ptr<Texture>(new Texture(textureId, textureUnit));
        return self;
    }

    [[nodiscard]] uint32_t getTextureUnit() const noexcept {
        return boundTextureUnit;
    }

    [[nodiscard]] int getTextureId() const noexcept { return textureId; }
};

class Cubemap {
    static_assert(sizeof(GLuint) == sizeof(int));

  private:
    int cubemapId = 0;
    uint32_t boundTextureUnit = UINT32_MAX;

    Cubemap(int cubemapId, int boundTextureUnit)
        : cubemapId(cubemapId), boundTextureUnit(boundTextureUnit) {
        DEBUG_ASSERTF(cubemapId != 0, "Cubemap id is 0");
        DEBUG_ASSERTF(boundTextureUnit != UINT32_MAX,
                      "Bound texture unit is UINT32_MAX");
    }

  public:
    Cubemap(Cubemap &) = delete;
    Cubemap(Cubemap &&other) noexcept
        : cubemapId(other.cubemapId), boundTextureUnit(other.boundTextureUnit) {
        other.cubemapId = 0;
        other.boundTextureUnit = UINT32_MAX;
    }

    static std::shared_ptr<Cubemap>
    load(std::vector<uint8_t> xPosBuf, std::vector<uint8_t> xNegBuf,
         std::vector<uint8_t> yPosBuf, std::vector<uint8_t> yNegBuf,
         std::vector<uint8_t> zPosBuf, std::vector<uint8_t> zNegBuf,
         size_t textureUnit) {
        std::cout << "Loading skybox into texture unit " << textureUnit
                  << std::endl;
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        gl::assertNoError();
        GLuint cubemapId = SOIL_load_OGL_cubemap_from_memory(
            xPosBuf.data(), xPosBuf.size(), xNegBuf.data(), xNegBuf.size(),
            yPosBuf.data(), yPosBuf.size(), yNegBuf.data(), yNegBuf.size(),
            zPosBuf.data(), zPosBuf.size(), zNegBuf.data(), zNegBuf.size(),
            SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
        if (0 == cubemapId) {
            auto err = SOIL_last_result();
            UNREACHABLE("Failed to load cubemap: %s", err);
        }
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);
        gl::assertNoError();
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        gl::assertNoError();
        auto self =
            std::shared_ptr<Cubemap>(new Cubemap(cubemapId, textureUnit));
        return self;
    }

    [[nodiscard]] uint32_t getTextureUnit() const noexcept {
        return boundTextureUnit;
    }

    [[nodiscard]] int getCubemapId() const noexcept { return cubemapId; }
};
