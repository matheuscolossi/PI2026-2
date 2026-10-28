#pragma once
#include <string>
#include <vector>
#include <cstddef>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Defines.h"
#include "stb_image.h"

//struct Vertex
//{
//    glm::vec3 Position;
//    glm::vec3 Normal;
//    glm::vec2 TexCoords;
//    glm::vec3 Tangent;
//    glm::vec3 Bitangent;
//};
//
//struct Texture
//{
//    unsigned int id;
//    std::string type;
//    std::string path;
//};


enum class AssetPass { All, Opaque, Transparent };

struct AssetDrawOptions
{
    unsigned diffuseUnit = 4, normalUnit = 5, lightmapUnit = 6;
    AssetPass pass = AssetPass::All;
    bool preserveBindings = true;
    bool useMaterialColor = true, useMaterialOpacity = true;
};

struct AssetMaterial
{
    glm::vec4 diffuseColor{1.f};
    float opacity = 1.f;
    bool twoSided = false;
    bool hasDiffuse = false, hasNormal = false, hasLightmap = false;
    bool hasDiffuseAlpha = false;
    std::string name;
    bool Transparent() const { return opacity * diffuseColor.a < .999f || hasDiffuseAlpha; }
};

class Mesh 
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO = 0;
    AssetMaterial material;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, AssetMaterial material = {});
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;
    void Draw(unsigned int shaderProgram);
    void DrawWithShadows(unsigned int shaderProgram);
    

private:
    unsigned int VBO = 0, EBO = 0;
    void release();
    void setupMesh();
};

// Aggregates every transformed mesh, unlike the legacy first-mesh getters.
// Attribute flags describe imported/generated data before safe fallbacks.
struct AssetReport
{
    std::size_t meshCount = 0, vertexCount = 0, triangleCount = 0;
    glm::vec3 boundsMin{0.f}, boundsMax{0.f};
    bool hasBounds = false;
    bool positionsFinite = true, normalsFinite = true, uvsFinite = true;
    std::size_t meshesWithUVs = 0, meshesWithNormals = 0;
    std::size_t normalFallbackVertices = 0, uvFallbackVertices = 0;
    std::size_t diffuseTexturesLoaded = 0;
    std::size_t materialCount = 0, meshesWithDiffuse = 0, meshesWithNormalMap = 0, meshesWithLightmap = 0;
    std::size_t colorOnlyMeshes = 0, transparentMeshes = 0, twoSidedMeshes = 0, texturedMeshesMissingUVs = 0;
    std::size_t normalTexturesLoaded = 0, lightmapTexturesLoaded = 0;
    std::vector<AssetMaterial> meshMaterials;
    std::vector<std::string> diffuseTexturePaths, missingTextures;
    std::string error;
};

class CAsset 
{
public:
    CAsset(const std::string& path, bool _flipTexture = false);
    ~CAsset();
    CAsset(const CAsset&) = delete;
    CAsset& operator=(const CAsset&) = delete;

    void Draw(unsigned int shaderProgram, bool shadows = false);
    // Uses reserved texture slots and per-mesh material uniforms. The caller owns
    // blend/depth state and transparent-instance sorting. Pass filters describe
    // native materials; use All when instance Color supplies glass/water alpha.
    void Draw(unsigned int shaderProgram, const AssetDrawOptions& options);

    unsigned int numVertices = 0;
    unsigned int numIndices = 0;
    unsigned int numFaces = 0;

	std::vector<Vertex> GetVertices() const { return meshes.empty() ? std::vector<Vertex>{} : meshes[0].vertices; }
	std::vector<unsigned int> GetIndices() const { return meshes.empty() ? std::vector<unsigned int>{} : meshes[0].indices; }
    const AssetReport& GetReport() const { return report; }
    // Prefer a portable bitmap beside the model over an exporter's absolute path.
    // Inputs and returned filename use UTF-8. Empty result means no regular file.
    static std::string ResolveTexturePath(const std::string& reference, const std::string& modelDirectory);

private:
    std::vector<Mesh> meshes;
    std::string directory;
    bool flipTexture;
    AssetReport report;
    std::vector<unsigned> usedMaterialIndices;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, bool* alpha = nullptr);
    unsigned int TextureFromFile(const std::string& filename, bool* alpha = nullptr);
};


