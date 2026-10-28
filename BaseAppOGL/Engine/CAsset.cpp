#pragma warning(disable : 4267)
#define STB_IMAGE_IMPLEMENTATION 
#define STBI_WINDOWS_UTF8

#include "CAsset.h"
#include <iostream>
#include <cmath>
#include <utility>
#include <filesystem>
#include <algorithm>
#include <array>

// Mesh Implementation
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, AssetMaterial material)
    : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures)), material(std::move(material))
{
    setupMesh();
}

Mesh::~Mesh() { release(); }

Mesh::Mesh(Mesh&& other) noexcept
    : vertices(std::move(other.vertices)), indices(std::move(other.indices)),
      textures(std::move(other.textures)), VAO(std::exchange(other.VAO, 0)),
      material(std::move(other.material)),
      VBO(std::exchange(other.VBO, 0)), EBO(std::exchange(other.EBO, 0))
{
    other.textures.clear();
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other) {
        release();
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        textures = std::move(other.textures);
        material = std::move(other.material);
        other.textures.clear();
        VAO = std::exchange(other.VAO, 0);
        VBO = std::exchange(other.VBO, 0);
        EBO = std::exchange(other.EBO, 0);
    }
    return *this;
}

void Mesh::release()
{
    // Meshes are destroyed while their owning scene still has an OpenGL context.
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    VAO = VBO = EBO = 0;
    for (auto& texture : textures) if (texture.id) glDeleteTextures(1, &texture.id);
    textures.clear();
}

void Mesh::setupMesh() 
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // Texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	// Tangents
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	// Bitangents
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    

    glBindVertexArray(0);
}

void Mesh::Draw(unsigned int shaderProgram) 
{
    // Bind textures
    unsigned int diffuseNr = 1;
    unsigned int normalNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++) 
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string name = textures[i].type;
        //std::string number = (name == "uDiffuseMap") ? std::to_string(diffuseNr++) : std::to_string(normalNr++);
        //glUniform1i(glGetUniformLocation(shaderProgram, (name + number).c_str()), i);
        glUniform1i(glGetUniformLocation(shaderProgram, (name).c_str()), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
}

void Mesh::DrawWithShadows(unsigned int shaderProgram)
{
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shaderProgram, "uDiffuseTexture"), 1);
    glBindTexture(GL_TEXTURE_2D, textures.empty() ? 0 : textures[0].id);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// CAsset Implementation
CAsset::CAsset(const std::string& path, bool _flipTexture) 
    : flipTexture(_flipTexture)
{
    loadModel(path);
}

CAsset::~CAsset() = default;

void CAsset::Draw(unsigned int shaderProgram, bool shadows)
{
	if (!shadows)
	{
        for (auto& mesh : meshes)
            mesh.Draw(shaderProgram);
	}
    else
    {
        for (auto& mesh : meshes)
            mesh.DrawWithShadows(shaderProgram);
    }
    
}

void CAsset::Draw(unsigned int shaderProgram, const AssetDrawOptions& options)
{
    if (meshes.empty()) return;
    const std::array<unsigned, 3> units{ options.diffuseUnit, options.normalUnit, options.lightmapUnit };
    GLint maxUnits = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxUnits);
    if (units[0] == units[1] || units[0] == units[2] || units[1] == units[2]
        || units[0] >= unsigned(maxUnits) || units[1] >= unsigned(maxUnits) || units[2] >= unsigned(maxUnits)) {
        std::cerr << "CAsset: invalid or repeated imported texture units." << std::endl;
        return;
    }

    GLint oldActive = GL_TEXTURE0, oldVao = 0;
    std::array<GLint, 3> oldTextures{};
    const bool oldCull = glIsEnabled(GL_CULL_FACE) == GL_TRUE;
    const std::array<const char*, 7> names{ "uHasDiffuse", "uHasNormal", "uHasLightmap", "uDiffuseMap", "uNormalMap", "uLightMap", "uImportedMaterial" };
    std::array<GLint, 7> locations{}, previous{};
    for (std::size_t i = 0; i < names.size(); ++i) {
        locations[i] = glGetUniformLocation(shaderProgram, names[i]);
        if (locations[i] >= 0 && (options.preserveBindings || i == 6))
            glGetUniformiv(shaderProgram, locations[i], &previous[i]);
    }
    const GLint tintLocation = glGetUniformLocation(shaderProgram, "uMaterialTint");
    std::array<GLfloat, 4> previousTint{};
    if (options.preserveBindings) {
        glGetIntegerv(GL_ACTIVE_TEXTURE, &oldActive);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &oldVao);
        for (std::size_t i = 0; i < units.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + units[i]);
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTextures[i]);
        }
        if (tintLocation >= 0) glGetUniformfv(shaderProgram, tintLocation, previousTint.data());
    }
    glUniform1i(locations[3], GLint(units[0]));
    glUniform1i(locations[4], GLint(units[1]));
    glUniform1i(locations[5], GLint(units[2]));
    glUniform1i(locations[6], GL_TRUE);

    for (const auto& mesh : meshes) {
        const bool transparent = mesh.material.Transparent();
        if ((options.pass == AssetPass::Opaque && transparent) || (options.pass == AssetPass::Transparent && !transparent)) continue;
        const auto textureId = [&](const char* type) {
            for (const auto& texture : mesh.textures) if (texture.type == type) return texture.id;
            return 0u;
        };
        const GLuint diffuse = mesh.material.hasDiffuse ? textureId("uDiffuseMap") : 0;
        const GLuint normal = mesh.material.hasNormal ? textureId("uNormalMap") : 0;
        const GLuint lightmap = mesh.material.hasLightmap ? textureId("uLightMap") : 0;
        const std::array<GLuint, 3> textures{ diffuse, normal, lightmap };
        for (std::size_t i = 0; i < units.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + units[i]);
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glUniform1i(locations[i], textures[i] != 0);
        }
        glm::vec4 tint = options.useMaterialColor ? mesh.material.diffuseColor : glm::vec4(1.f);
        tint.a = options.useMaterialOpacity ? mesh.material.opacity * mesh.material.diffuseColor.a : 1.f;
        glUniform4fv(tintLocation, 1, &tint[0]);
        if (oldCull && !mesh.material.twoSided) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);
    }

    if (oldCull) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    // The procedural world must never inherit the imported material's tint/alpha.
    glUniform1i(locations[6], previous[6]);
    if (options.preserveBindings) {
        for (std::size_t i = 0; i < units.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + units[i]);
            glBindTexture(GL_TEXTURE_2D, GLuint(oldTextures[i]));
        }
        glActiveTexture(GLenum(oldActive));
        glBindVertexArray(GLuint(oldVao));
        for (std::size_t i = 0; i < locations.size(); ++i) glUniform1i(locations[i], previous[i]);
        glUniform4fv(tintLocation, 1, previousTint.data());
    } else glBindVertexArray(0);
}

void CAsset::loadModel(const std::string& path) 
{
    Assimp::Importer importer;
    importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, 
        aiComponent_CAMERAS | 
        aiComponent_LIGHTS | 
        aiComponent_COLORS | 
        aiComponent_ANIMATIONS | 
        aiComponent_BONEWEIGHTS
    );

    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | 
        aiProcess_FlipUVs | 
        aiProcess_TransformUVCoords |
        aiProcess_CalcTangentSpace | 
        aiProcess_GenSmoothNormals |
        aiProcess_PreTransformVertices |
        aiProcess_RemoveComponent
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        report.error = importer.GetErrorString();
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    const auto separator = path.find_last_of("/\\");
    directory = separator == std::string::npos ? "." : path.substr(0, separator);
    processNode(scene->mRootNode, scene);
    report.meshCount = meshes.size();
    if (!report.vertexCount || !report.triangleCount) report.error = "Model contains no renderable triangles.";
}

void CAsset::processNode(aiNode* node, const aiScene* scene) 
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++) 
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) 
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh CAsset::processMesh(aiMesh* mesh, const aiScene* scene) 
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    AssetMaterial importedMaterial;
    bool meshUVsValid = mesh->HasTextureCoords(0);
    vertices.reserve(mesh->mNumVertices);
    if (mesh->HasNormals()) ++report.meshesWithNormals;
    if (mesh->HasTextureCoords(0)) ++report.meshesWithUVs;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) 
    {
        Vertex vertex{};
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (!std::isfinite(vertex.Position.x) || !std::isfinite(vertex.Position.y) || !std::isfinite(vertex.Position.z)) {
            report.positionsFinite = false;
            vertex.Position = glm::vec3(0.f);
        }
        if (!report.hasBounds) { report.boundsMin = report.boundsMax = vertex.Position; report.hasBounds = true; }
        else { report.boundsMin = (glm::min)(report.boundsMin, vertex.Position); report.boundsMax = (glm::max)(report.boundsMax, vertex.Position); }
        vertex.Normal = mesh->HasNormals()
            ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
            : glm::vec3(0.f, 1.f, 0.f);
        const float normalLength = glm::dot(vertex.Normal, vertex.Normal);
        if (!mesh->HasNormals() || !std::isfinite(normalLength) || normalLength <= 1e-10f) {
            report.normalsFinite = false;
            ++report.normalFallbackVertices;
        }
        vertex.Normal = std::isfinite(normalLength) && normalLength > 1e-10f
            ? vertex.Normal / std::sqrt(normalLength) : glm::vec3(0.f, 1.f, 0.f);

        if (mesh->HasTextureCoords(0))
        {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else 
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        if (!mesh->HasTextureCoords(0) || !std::isfinite(vertex.TexCoords.x) || !std::isfinite(vertex.TexCoords.y)) {
            meshUVsValid = false;
            report.uvsFinite = false;
            ++report.uvFallbackVertices;
            vertex.TexCoords = glm::vec2(0.f);
        }

        // Assimp cannot calculate tangents without UVs. Construct a finite basis
        // instead of dereferencing optional arrays on untextured classroom models.
        const glm::vec3 helper = std::fabs(vertex.Normal.y) < .9f
            ? glm::vec3(0.f, 1.f, 0.f) : glm::vec3(1.f, 0.f, 0.f);
        const glm::vec3 fallback = glm::normalize(glm::cross(helper, vertex.Normal));
        vertex.Tangent = fallback;
        float handedness = 1.f;
        if (mesh->HasTangentsAndBitangents()) {
            glm::vec3 tangent(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            tangent -= vertex.Normal * glm::dot(vertex.Normal, tangent);
            const float tangentLength = glm::dot(tangent, tangent);
            if (std::isfinite(tangentLength) && tangentLength > 1e-10f)
                vertex.Tangent = tangent / std::sqrt(tangentLength);
            const glm::vec3 bitangent(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
            if (glm::dot(glm::cross(vertex.Normal, vertex.Tangent), bitangent) < 0.f) handedness = -1.f;
        }
        vertex.Bitangent = glm::cross(vertex.Normal, vertex.Tangent) * handedness;

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) 
    {
        aiFace face = mesh->mFaces[i];
        if (face.mNumIndices != 3) continue;
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex < scene->mNumMaterials)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        if (std::find(usedMaterialIndices.begin(), usedMaterialIndices.end(), mesh->mMaterialIndex) == usedMaterialIndices.end())
            usedMaterialIndices.push_back(mesh->mMaterialIndex);
        report.materialCount = usedMaterialIndices.size();
        aiColor4D diffuse(1.f, 1.f, 1.f, 1.f);
        aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
        const auto component = [](float value) { return std::isfinite(value) ? (std::clamp)(value, 0.f, 1.f) : 1.f; };
        importedMaterial.diffuseColor = glm::vec4(component(diffuse.r), component(diffuse.g), component(diffuse.b), component(diffuse.a));
        ai_real opacity = 1.f;
        material->Get(AI_MATKEY_OPACITY, opacity);
        importedMaterial.opacity = component(float(opacity));
        int twoSided = 0;
        material->Get(AI_MATKEY_TWOSIDED, twoSided);
        importedMaterial.twoSided = twoSided != 0;
        aiString materialName;
        if (material->Get(AI_MATKEY_NAME, materialName) == AI_SUCCESS) importedMaterial.name = materialName.C_Str();
        auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "uDiffuseMap", &importedMaterial.hasDiffuseAlpha);
        importedMaterial.hasDiffuse = !diffuseMaps.empty();
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        auto normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "uNormalMap");
        importedMaterial.hasNormal = !normalMaps.empty();
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        auto heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "uHeightMap");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        auto lightMaps = loadMaterialTextures(material, aiTextureType_LIGHTMAP, "uLightMap");
        importedMaterial.hasLightmap = !lightMaps.empty();
        textures.insert(textures.end(), lightMaps.begin(), lightMaps.end());
    }
    report.meshMaterials.push_back(importedMaterial);
    if (importedMaterial.hasDiffuse) ++report.meshesWithDiffuse; else ++report.colorOnlyMeshes;
    if (importedMaterial.hasNormal) ++report.meshesWithNormalMap;
    if (importedMaterial.hasLightmap) ++report.meshesWithLightmap;
    if (importedMaterial.Transparent()) ++report.transparentMeshes;
    if (importedMaterial.twoSided) ++report.twoSidedMeshes;
    if ((importedMaterial.hasDiffuse || importedMaterial.hasNormal || importedMaterial.hasLightmap) && !meshUVsValid)
        ++report.texturedMeshesMissingUVs;

    numVertices += mesh->mNumVertices;
	numIndices += static_cast<unsigned int>(indices.size());
    numFaces += static_cast<unsigned int>(indices.size() / 3);
    report.vertexCount += vertices.size();
    report.triangleCount += indices.size() / 3;

    return Mesh(std::move(vertices), std::move(indices), std::move(textures), std::move(importedMaterial));
}

std::vector<Texture> CAsset::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, bool* alpha)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) 
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        Texture texture{};
        texture.path = ResolveTexturePath(str.C_Str(), directory);
        bool textureAlpha = false;
        if (!texture.path.empty()) texture.id = TextureFromFile(texture.path, &textureAlpha);
        if (!texture.id) {
            report.missingTextures.push_back(typeName + ": " + str.C_Str());
            std::cerr << "Texture unavailable: " << str.C_Str() << std::endl;
            continue;
        }
        texture.type = typeName;
        if (alpha && textureAlpha) *alpha = true;
        if (type == aiTextureType_DIFFUSE) {
            ++report.diffuseTexturesLoaded;
            report.diffuseTexturePaths.push_back(texture.path);
        }
        if (type == aiTextureType_NORMALS) ++report.normalTexturesLoaded;
        if (type == aiTextureType_LIGHTMAP) ++report.lightmapTexturesLoaded;
        textures.push_back(texture);
    }
    return textures;
}


std::string CAsset::ResolveTexturePath(const std::string& reference, const std::string& modelDirectory)
{
    if (reference.empty() || reference.front() == '*') return {};
    try {
        std::string normalized = reference;
        std::replace(normalized.begin(), normalized.end(), '\\', '/');
        const auto bitmap = std::filesystem::path(std::u8string(normalized.begin(), normalized.end()));
        const std::string folderName = modelDirectory.empty() ? "." : modelDirectory;
        const auto folder = std::filesystem::path(std::u8string(folderName.begin(), folderName.end()));
        const std::filesystem::path candidates[]{ folder / bitmap.filename(), bitmap.is_absolute() ? bitmap : folder / bitmap };
        for (const auto& candidate : candidates) {
            std::error_code error;
            if (!std::filesystem::is_regular_file(candidate, error) || error) continue;
            const auto utf8 = candidate.lexically_normal().generic_u8string();
            return std::string(utf8.begin(), utf8.end());
        }
    } catch (const std::filesystem::filesystem_error&) { return {}; }
    return {};
}

unsigned int CAsset::TextureFromFile(const std::string& filename, bool* alpha)
{

    // Flip the image vertically during loading
    stbi_set_flip_vertically_on_load(flipTexture);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    unsigned int textureID = 0;
    if (data && width > 0 && height > 0 && nrComponents >= 1 && nrComponents <= 4)
    {
        if (alpha && (nrComponents == 2 || nrComponents == 4)) {
            const std::size_t pixels = std::size_t(width) * std::size_t(height);
            for (std::size_t pixel = 0; pixel < pixels; ++pixel)
                if (data[pixel * nrComponents + nrComponents - 1] < 255) { *alpha = true; break; }
        }
        GLenum format = GL_RED;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 2)
            format = GL_RG;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glGenTextures(1, &textureID);
        if (!textureID) { stbi_image_free(data); return 0; }
        GLint oldUnpack = 4, oldTexture = 0;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldUnpack);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTexture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glPixelStorei(GL_UNPACK_ALIGNMENT, oldUnpack);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Check if anisotropic filtering is supported and get the maximum anisotropy level
        GLfloat maxAnisotropy = 0.0f;
        if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
        {
            // Get the maximum anisotropy level supported by the GPU
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

            // Set anisotropic filtering
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
        }

        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(oldTexture));
        stbi_image_free(data);
    }
    else 
    {
        std::cerr << "Texture failed to load at path: " << filename << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

