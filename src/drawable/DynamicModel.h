#pragma once
#include <GL/glew.h>

#include <GL/gl.h>

#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

#include "../assertions.h"

#include "assimp/Importer.hpp"
#include "assimp/color4.h"
#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"

#include "glm/ext/vector_float4.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "../Material.h"
#include "../gl_utils.h"
#include "Drawable.h"

struct Vertex {
    float Position[3];
    float Normal[3];
    float Texture[2];
    float Tangent[3];
};

class DynamicModel : public Drawable {
    static_assert(sizeof(GLuint) == sizeof(uint32_t));

  private:
    uint32_t VAO = 0;
    uint32_t VBO = 0;
    uint32_t IBO = 0;
    size_t indiciesCount = 0;
    Material material;

    const static inline uint32_t importOptions =
        aiProcess_Triangulate | aiProcess_OptimizeMeshes |
        aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace;

    static inline glm::vec4 aiColToGlm(const aiColor4D &col) {
        return {col.r, col.g, col.b, col.a};
    }

    DynamicModel(uint32_t vao, uint32_t vbo, uint32_t ibo, size_t indiciesCount,
                 Material material)
        : VAO(vao), VBO(vbo), IBO(ibo), indiciesCount(indiciesCount),
          material(material) {}

  public:
    DynamicModel(DynamicModel &) = delete;
    DynamicModel &operator=(const DynamicModel &) = delete;
    DynamicModel(DynamicModel &&other) noexcept
        : VAO(other.VAO), VBO(other.VBO), IBO(other.IBO),
          indiciesCount(other.indiciesCount), material(other.material) {
        other.VAO = 0;
        other.VBO = 0;
        other.IBO = 0;
    }

    static std::shared_ptr<DynamicModel> load(const std::vector<uint8_t> &buf) {
        Assimp::Importer importer;
        const aiScene *scene =
            importer.ReadFileFromMemory(buf.data(), buf.size(), importOptions);
        DEBUG_ASSERTF(nullptr != scene, "Failed to load model");
        std::cout << "Number of meshes: " << scene->mNumMeshes
                  << "; number of materials: " << scene->mNumMaterials
                  << std::endl;
        Material material(glm::vec4(0), glm::vec4(0), glm::vec4(0), 32);
        for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
            const aiMaterial *mat = scene->mMaterials[i];
            DEBUG_ASSERT_NOT_NULL(mat);
            aiString name;
            mat->Get(AI_MATKEY_NAME, name);
            std::cout << "Material " << i << " name: " << name.C_Str()
                      << std::endl;
            aiColor4D col;
            if (AI_SUCCESS ==
                aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &col)) {
                material.setAmbient(aiColToGlm(col));
            } else {
                UNREACHABLE("Failed to read ambinet material color")
            }
            if (AI_SUCCESS ==
                aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &col)) {
                auto val = aiColToGlm(col);
                std::cout << "Diffuse color in material: "
                          << glm::to_string(val) << std::endl;
                material.setDiffuse(val);
            } else {
                UNREACHABLE("Failed to read diffuse material color")
            }
            if (AI_SUCCESS ==
                aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &col)) {
                material.setSpecular(aiColToGlm(col));
            } else {
                UNREACHABLE("Failed to read specular material color")
            }
        }

        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[i];

            auto *pVertices = new Vertex[mesh->mNumVertices];
            DEBUG_ASSERT_NOT_NULL(pVertices);
            std::memset(pVertices, 0, sizeof(Vertex) * mesh->mNumVertices);

            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                if (mesh->HasPositions()) {
                    pVertices[i].Position[0] = mesh->mVertices[i].x;
                    pVertices[i].Position[1] = mesh->mVertices[i].y;
                    pVertices[i].Position[2] = mesh->mVertices[i].z;
                }

                if (mesh->HasNormals()) {
                    pVertices[i].Normal[0] = mesh->mNormals[i].x;
                    pVertices[i].Normal[1] = mesh->mNormals[i].y;
                    pVertices[i].Normal[2] = mesh->mNormals[i].z;
                }

                if (mesh->HasTextureCoords(0)) {
                    pVertices[i].Texture[0] = mesh->mTextureCoords[0][i].x;
                    pVertices[i].Texture[1] = mesh->mTextureCoords[0][i].y;
                }

                if (mesh->HasTangentsAndBitangents()) {
                    pVertices[i].Tangent[0] = mesh->mTangents[i].x;
                    pVertices[i].Tangent[1] = mesh->mTangents[i].y;
                    pVertices[i].Tangent[2] = mesh->mTangents[i].z;
                }
            }

            unsigned int *pIndices = nullptr;
            if (mesh->HasFaces()) {
                pIndices = new unsigned int[mesh->mNumFaces * 3];
                for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                    pIndices[i * 3] = mesh->mFaces[i].mIndices[0];
                    pIndices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
                    pIndices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
                }
            }

            GLuint vao;
            GLuint vbo;
            GLuint ibo;
            size_t indiciesCount;
            GL_CALL(glGenVertexArrays, 1, &vao);

            DEBUG_ASSERT(0 != vao);

            GL_CALL(glGenBuffers, 1, &vbo);
            DEBUG_ASSERT(0 != vbo);

            GL_CALL(glGenBuffers, 1, &ibo);
            DEBUG_ASSERT(0 != ibo);

            GL_CALL(glBindVertexArray, vao);
            GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, vbo);
            GL_CALL(glBufferData, GL_ARRAY_BUFFER,
                    sizeof(Vertex) * mesh->mNumVertices, pVertices,
                    GL_STATIC_DRAW);
            GL_CALL(glEnableVertexAttribArray, 0);
            GL_CALL(glVertexAttribPointer, 0, 3, GL_FLOAT, GL_FALSE,
                    sizeof(Vertex), (GLvoid *)nullptr);
            GL_CALL(glEnableVertexAttribArray, 1);
            GL_CALL(glVertexAttribPointer, 1, 3, GL_FLOAT, GL_FALSE,
                    sizeof(Vertex), (GLvoid *)(3 * sizeof(GLfloat)));
            GL_CALL(glEnableVertexAttribArray, 2);
            GL_CALL(glVertexAttribPointer, 2, 2, GL_FLOAT, GL_FALSE,
                    sizeof(Vertex), (GLvoid *)(6 * sizeof(GLfloat)));

            // Tangent for Normal Map
            GL_CALL(glEnableVertexAttribArray, 3);
            GL_CALL(glVertexAttribPointer, 3, 3, GL_FLOAT, GL_FALSE,
                    sizeof(Vertex), (GLvoid *)(8 * sizeof(GLfloat)));

            // Index Buffer
            GL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, ibo);
            GL_CALL(glBufferData, GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(GLuint) * mesh->mNumFaces * 3, pIndices,
                    GL_STATIC_DRAW);
            GL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
            GL_CALL(glBindVertexArray, vao);

            indiciesCount = mesh->mNumFaces * 3;
            delete[] pVertices;
            delete[] pIndices;

            GL_CALL(glBindVertexArray, 0);

            return std::shared_ptr<DynamicModel>(
                new DynamicModel(vao, vbo, ibo, indiciesCount, material));
        }
        UNREACHABLE("At least one ")
    }

    [[nodiscard]] const Material &getMaterial() const { return material; }

    void draw() override {
        GL_CALL(glBindVertexArray, VAO);
        GL_CALL(glDrawElements, GL_TRIANGLES, indiciesCount, GL_UNSIGNED_INT,
                nullptr);

#ifdef DEBUG_ASSERTIONS
        GL_CALL(glBindVertexArray, 0);
#endif // DEBUG_ASSERTIONS
    }
};
