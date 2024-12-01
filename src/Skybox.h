#pragma once

#include "assertions.h"
#include <GL/glew.h>

#include "AssetManager.h"
#include "shaders/ShaderSkybox.h"
#include <GL/gl.h>
#include <SOIL.h>
#include <memory>

class Skybox {
  private:
    std::shared_ptr<Cubemap> cubemap;
    std::shared_ptr<ShaderSkybox> shader;
    const static inline float skycube[108] = {
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
        1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
        1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
        1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  -1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,
        1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  -1.0f, 1.0f};

    GLuint VAO = 0;
    GLuint VBO = 0;

  public:
    Skybox(const std::shared_ptr<AssetManager> &assetManager)
        : cubemap(assetManager->loadCubemap()),
          shader(ShaderSkybox::load(assetManager).value()) {
        shader->update(CameraProperties::defaultProps());
        shader->update(ProjectionMatrix::_default());

        // Vertex Array Object (VAO)
        glGenBuffers(1, &VBO); // generate the VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skycube), &skycube[0],
                     GL_STATIC_DRAW);

        glGenVertexArrays(1, &VAO); // generate the VAO
        glBindVertexArray(VAO);     // bind the VAO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // enable vertex attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                              (GLvoid *)0);
        DEBUG_ASSERT(0 != VBO);
        DEBUG_ASSERT(0 != VAO);
    }

    void render() {
        shader->bind();
        shader->modelMatrix(glm::mat4(1));
        shader->setCubemapId(cubemap->getTextureUnit());

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 108);

        glClear(GL_DEPTH_BUFFER_BIT);
        shader->unbind();
    }
};
