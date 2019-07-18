#include "wzpch.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Wizzy/Renderer/Model.h"
#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/Renderer/Material.h"
#include "Wizzy/Renderer/Texture.h"

namespace Wizzy {

    /****************************************************
    ****************MESH*********************************
    *****************************************************/

    Mesh::Mesh(const std::vector<Vertex>& vertices,
               const std::vector<u32>& indices,
               const MaterialHandle& materialHandle,
               const string& name)
        : m_vertices(vertices), m_indices(indices), m_name(name),
          m_materialHandle(materialHandle) {
        Init();
    }

    Mesh::~Mesh() {
        m_vertices.clear();
        m_indices.clear();
        m_VBOs.clear();
    }

    void Mesh::Init() {
        WZ_CORE_TRACE("Initializing mesh '{0}' with {1} vertices and {2} indices...", m_name, m_vertices.size(), m_indices.size());
        WZ_CORE_ASSERT(m_vertices.size() >= 3, "Mesh must have at least 3 vertices");
        WZ_CORE_ASSERT(m_indices.size() >= 3, "Mesh must have at least 3 indices");

        m_VAO = VertexArrayPtr(VertexArray::Create());

        VertexBufferPtr _mainVBO(VertexBuffer::Create(&m_vertices[0],
                             sizeof(Vertex) * m_vertices.size()));

        BufferLayout _layout = {
            { ShaderDataType::FLOAT3, "vertexPosition" },
			{ ShaderDataType::FLOAT2, "vertexUv" },
            { ShaderDataType::FLOAT3, "vertexNormal" }
        };

        _mainVBO->SetLayout(_layout);

        m_VBOs.push_back(_mainVBO);

        m_IBO = IndexBufferPtr(IndexBuffer::Create(&m_indices[0], m_indices.size()));

        for (const auto& _VBO : m_VBOs) {
            m_VAO->PushVertexBuffer(_VBO);
        }
        m_VAO->SetIndexBuffer(m_IBO);
        WZ_CORE_INFO("Successfully initialized mesh '{0}' with {1} vertices and {2} indices ({3} triangles)", m_name, m_vertices.size(), m_indices.size(), m_indices.size() / 3);
    }

    /****************************************************
    ****************MODEL*********************************
    *****************************************************/

    Model::Model(const string& sourceFile, const string& data, const ulib::Bitset& flags)
        : Resource(flags, "Model", WZ_EXTENSION_MODEL) {
        WZ_CORE_TRACE("Reading model with assimp from data");
        Assimp::Importer _importer;
        std::vector<Material*> _materials;
        std::vector<string> _meshNames;

        const aiScene *_scene
                = _importer.ReadFileFromMemory(&data[0], data.size(),
                                     aiProcess_Triangulate |
                                     aiProcess_JoinIdenticalVertices |
                                     aiProcess_GenNormals |
                                     aiProcess_CalcTangentSpace |
                                     aiProcess_SortByPType |
                                     aiProcess_RemoveRedundantMaterials |
                                     aiProcess_OptimizeMeshes |
                                     aiProcess_OptimizeGraph
                                 );

        string _sceneError = "";

        if (!_scene) {

            WZ_CORE_ERROR("Failed loading model, Assimp error: {0}", _importer.GetErrorString());

            m_isValid = false;
            return;
        }
        WZ_CORE_INFO("Successfully read model from data");

        WZ_CORE_TRACE("Verifying scene...");
        if (!VerifyAssimpScene(_scene, &_sceneError)) {

            WZ_CORE_ERROR("Failed loading model from data: '{0}'...", _sceneError);

            m_isValid = false;
            return;
        }
        WZ_CORE_INFO("Scene was verified successfully");

        ProcessNode(_scene->mRootNode, _scene, sourceFile);

        m_isValid = true;
        WZ_CORE_INFO("Model successfully initialized");
    }

    string Model::Serialize() const {
        WZ_CORE_ERROR("Model serializing not yet implemented");
        return "";
    }

    bool Model::VerifyAssimpScene(const aiScene *scene, string *error) {

        *error = "Unknown error";

        if (!scene) {
            *error = "Scene was null, assimp error...";
            return false;
        }

        if (!scene->HasMeshes()) {
            *error = "File has no meshes";
            return false;
        }

        return true;
    }
    bool Model::VerifyAssimpMesh(const aiMesh *mesh, string *error) {

        *error = "Unknown error";

        if (!mesh) {
            *error = "Mesh is null";
            return false;
        }

        if (!mesh->HasFaces()) {
            *error = "Mesh has no indices";
            return false;
        }

        if (!mesh->HasNormals()) {
            *error = "Mesh has no normals";
            return false;
        }

        if (!mesh->HasPositions()) {
            *error = "Mesh has no positions";
            return false;
        }

        return true;
    }

    bool Model::ProcessNode(aiNode *node, const aiScene *scene, const string& sourceFile) {
        WZ_CORE_TRACE("Processing ai node '{0}'...", node->mName.C_Str());
        WZ_CORE_TRACE("Processing {0} meshes...", node->mNumMeshes);
        m_meshes.reserve(node->mNumMeshes);
        for (u32 i = 0; i < node->mNumMeshes; i++) {
            WZ_CORE_TRACE("Processing mesh {0}/{1}", i + 1, node->mNumMeshes);
            aiMesh *_mesh = scene->mMeshes[node->mMeshes[i]];
            bool _meshSuccess = false;
            auto _processedMesh = ProcessMesh(_mesh, scene, &_meshSuccess, sourceFile, i);
            if (_meshSuccess) {
                m_meshes.push_back(_processedMesh);
            } else {
                WZ_CORE_ERROR("Failed processing node, error when processing mesh.");
                return false;
            }
            WZ_CORE_INFO("Successfully processed mesh {0}/{1}", i + 1, node->mNumMeshes);
        }

        WZ_CORE_TRACE("Processing {0} node children...", node->mNumChildren);
        bool _anyFailure = false;
        for (u32 i = 0; i < node->mNumChildren; i++) {
            WZ_CORE_TRACE("Processing child {0}/{1}", i + 1, node->mNumChildren);
            if (!ProcessNode(node->mChildren[i], scene, sourceFile)) {
                _anyFailure = true;
                break;
            }
        }

        if (_anyFailure) {
            return false;
        }
        WZ_CORE_INFO("Successfully processed ai node '{0}'", node->mName.C_Str());
        return true;
    }

    Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene, bool *success, const string& sourceFile, int32 meshIndex) {
        *success = true;
        string meshError = "";
        std::vector<Vertex> _vertices;
        std::vector<u32> _indices;

        WZ_CORE_TRACE("Verifying mesh");
        if (!VerifyAssimpMesh(mesh, &meshError)) {
            WZ_CORE_ERROR("Failed processing mesh: {0}", meshError);
            *success = false;
            return Mesh();
        }
        WZ_CORE_TRACE("Mesh OK");

        WZ_CORE_TRACE("Reserving memory for {0} vertices and {1} faces ({2} indices) [{2} Bytes]",
                        mesh->mNumVertices, mesh->mNumFaces,
                        mesh->mNumFaces * 3,
                        (sizeof(Vertex) * mesh->mNumVertices + sizeof(u32) * mesh->mNumFaces * 3));

        //_vertices.reserve(mesh->mNumVertices);
        _indices.reserve(mesh->mNumFaces * 3);

        if (mesh->HasTextureCoords(0)) {
            WZ_CORE_TRACE("Mesh has uv's");
        } else {
            WZ_CORE_TRACE("Mesh doesn't have uv's");
        }

        if (mesh->HasNormals()) {
            WZ_CORE_TRACE("Mesh has normals");
        } else {
            WZ_CORE_TRACE("Mesh doesn't have normals");
        }

        WZ_CORE_TRACE("Iterating {0} vertices...", mesh->mNumVertices);
        for (u32 i = 0; i < mesh->mNumVertices; i++) {
            if (rand() % 500 == 3) {
                WZ_CORE_TRACE("Vertices progress: {0}%", (int)(((float)i / (float)mesh->mNumVertices) * 100));
            }
            Vertex _vertex;

            _vertex.position.x = mesh->mVertices[i].x;
            _vertex.position.y = mesh->mVertices[i].y;
            _vertex.position.z = mesh->mVertices[i].z;

            if (mesh->mTextureCoords[0]) {
                _vertex.uv.x = (float)mesh->mTextureCoords[0][i].x;
                _vertex.uv.y = (float)mesh->mTextureCoords[0][i].y;
            }

            if (mesh->HasNormals()) {

                _vertex.normal.x = mesh->mNormals[i].x;
                _vertex.normal.y = mesh->mNormals[i].y;
                _vertex.normal.z = mesh->mNormals[i].z;
            }

            _vertices.push_back(_vertex);
        }

        WZ_CORE_TRACE("Iterating {0} triangles", mesh->mNumFaces);
        for (u32 i = 0; i < mesh->mNumFaces; i++) {
            aiFace& _face = mesh->mFaces[i];
            WZ_CORE_ASSERT(_face.mNumIndices == 3, "Model must be triangulated (Should be done automatically at import?)");

            _indices.push_back(_face.mIndices[0]);
            _indices.push_back(_face.mIndices[1]);
            _indices.push_back(_face.mIndices[2]);
        }

        aiMaterial *_aiMat = scene->mMaterials[mesh->mMaterialIndex];
        string _meshName = ulib::File::name_of(sourceFile) + "_mesh_" + std::to_string(meshIndex) + string("_") + mesh->mName.C_Str();

        auto _material = ProcessMaterial(_aiMat, _meshName, scene, sourceFile);
        if (_material == WZ_NULL_RESOURCE_HANDLE) {
            WZ_CORE_ERROR("Failed processing mesh, couldn't create material");
            *success = false;
            return Mesh();
        }

        return { _vertices, _indices, _material, _meshName };
    }

    MaterialHandle Model::ProcessMaterial(aiMaterial *mat, const string& meshName, const aiScene *scene, const string& sourceFile) {
        string _materialHandle = meshName + "_material";
        Material *_material = new Material(WZ_DEFAULT_SHADER_HANDLE);

        auto _diffuseTextures = LoadMaterialTextures(mat, (int32)aiTextureType_DIFFUSE, scene, sourceFile);
        if (_diffuseTextures.size() == 0) {
            return WZ_NULL_RESOURCE_HANDLE;
        }
        auto _mainDiffuseTexture = _diffuseTextures[0];

        _material->diffuseMapHandle = _mainDiffuseTexture;

        ResourceManagement::Add(_material, _materialHandle);

        return _materialHandle;
    }

    std::vector<TextureHandle> Model::LoadMaterialTextures(aiMaterial *mat, int32 textureType, const aiScene *scene, const string& sourceFile) {
        WZ_CORE_TRACE("Handling textures of texture type {0} for material...", textureType);
        std::vector<TextureHandle> _textures;
        u32 _count = mat->GetTextureCount((aiTextureType)textureType);
        WZ_CORE_DEBUG(mat->GetTextureCount(aiTextureType_SPECULAR));
        string _fileDirectory = ulib::File::directory_of(sourceFile);
        string _fileName = ulib::File::without_extension(ulib::File::name_of(sourceFile));
        if (_count == 0) {
            WZ_CORE_TRACE("No texture found in model file...");

            WZ_CORE_TRACE("Searching for common disk textures in model directory...");
            static string _fmts[] = {
                ".jpg", ".jpeg", ".png", ".tga", ".bmp", ".psd", ".gid", ".hdr", ".pic"
            }; // TODO

            string _diffuseFile = _fileDirectory + "/diffuse";
            bool _foundDiffuse = false;
            for (const auto& _fmt : _fmts) {
                if (ulib::File::exists(_diffuseFile + _fmt)) {
                    WZ_CORE_TRACE("Found '{0}'", _diffuseFile + _fmt);
                    _diffuseFile += _fmt;
                    _foundDiffuse = true;
                    break;
                }
            }

            if (_foundDiffuse) {
                string _handle = _fileName + "_" + std::to_string(textureType) + "_" + ulib::File::without_extension(ulib::File::name_of(_diffuseFile));
                ResourceManagement::Load<Texture>(_diffuseFile, _handle);
                _textures.push_back(_handle);
            } else {

                aiColor3D _aiColor(0.0f, 0.0f, 0.0f);

                switch ((aiTextureType)textureType) {
                    case aiTextureType_DIFFUSE:
                        mat->Get(AI_MATKEY_COLOR_DIFFUSE, _aiColor);
                        break;
                    default: WZ_CORE_ASSERT(false, "Unimplemented aiTextureType"); break;
                }

                if (_aiColor.IsBlack()) {
                    WZ_CORE_TRACE("No color property found, using unloaded texture...");
                    _textures.push_back(Texture::UnloadedTexture());
                } else {
                    WZ_CORE_TRACE("Color property found, creating 1x1 texture...");
                    auto _color = Color(_aiColor.r, _aiColor.g, _aiColor.b, 1.f);
                    Texture *_texture = Texture::Create((byte*)&_color, 1, 1);
                    auto _handle = ulib::File::name_of(sourceFile) + "_" + std::to_string(textureType) + "_1x1_texture";
                    ResourceManagement::Add(_texture, _handle);
                    _textures.push_back(_handle);
                }

            }

        } else {
            WZ_CORE_TRACE("{0} Textures found...", _count);
            _textures.reserve(_count);
            for (u32 i = 0; i < _count; i++) {
                aiString _path;
                mat->GetTexture((aiTextureType)textureType, i ,&_path);
                string _file = ulib::File::directory_of(sourceFile) + "/" +
                        ulib::File::name_of(ulib::File::to_portable_path(_path.C_Str()));
                string _handle = ulib::File::name_of(sourceFile) + "_" + std::to_string(textureType) + "_" + ulib::File::name_of(_file);

                WZ_CORE_ASSERT(ulib::File::exists(_file), "Embedded textures not yet implemnted");
                ResourceManagement::Load<Texture>(_file, _handle);
                _textures.push_back(_handle);
            }
        }

        if (_textures.size() == 0) {
            WZ_CORE_ERROR("Something went wrong when processing textures, applying invalid texture");
            _textures.push_back(Texture::InvalidTexture());
        }

        return _textures;
    }

    Model* Model::Create(const string& sourceFile, const string& data, ulib::Bitset flags) {
        return new Model(sourceFile, data, flags);
    }
}
