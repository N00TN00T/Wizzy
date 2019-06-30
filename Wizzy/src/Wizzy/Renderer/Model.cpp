#include "wzpch.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Wizzy/Renderer/Model.h"

namespace Wizzy {

    /****************************************************
    ****************MESH*********************************
    *****************************************************/

    Mesh::Mesh(const std::vector<Vertex>& vertices,
               const std::vector<u32>& indices,
               const string& name)
        : m_vertices(vertices), m_indices(indices), m_name(name) {
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

    Model::Model(const string& file, const ulib::Bitset& flags)
        : IResource(file, "Model"), m_flags(flags) {

    }

    void Model::Load() {
        WZ_CORE_ASSERT(m_isGarbage, "Item already loaded");
        WZ_CORE_TRACE("Reading model with assimp from '{0}'", this->GetSourceFile());
        Assimp::Importer _importer;

        const aiScene *_scene
                = _importer.ReadFile(this->GetSourceFile().c_str(),
                                     aiProcess_Triangulate | aiProcess_FlipUVs);

        string _sceneError = "";

        if (!_scene) {

            WZ_CORE_ERROR("Failed loading model from file '{0}'...",
                                                        this->GetSourceFile());
            WZ_CORE_ERROR("...Assimp error: {0}", _importer.GetErrorString());

            m_isGarbage = true;
            return;
        }
        WZ_CORE_INFO("Successfully read model from '{0}'", this->GetSourceFile());

        WZ_CORE_TRACE("Verifying scene...");
        if (!VerifyAssimpScene(_scene, &_sceneError)) {

            WZ_CORE_ERROR("Failed loading model from file '{0}'...",
                                                        this->GetSourceFile());
            WZ_CORE_ERROR("...Reason: {0}", _sceneError);

            m_isGarbage = true;
            return;
        }
        WZ_CORE_INFO("Scene was verified successfully");

        m_meshes.reserve(_scene->mNumMeshes);
        WZ_CORE_TRACE("Processing {0} meshes...", _scene->mNumMeshes);
        for (u32 i = 0; i < _scene->mNumMeshes; i++) {
            aiMesh *_mesh = _scene->mMeshes[i];

            bool _meshSuccess = false;
            WZ_CORE_TRACE("Processing mesh {0}/{1}...", i + 1, _scene->mNumMeshes);
            m_meshes.push_back(ProcessMesh(_mesh, &_meshSuccess));

            if (!_meshSuccess) {
                WZ_CORE_ERROR("Failed processing mesh");
                m_meshes.clear();
                m_isGarbage = true;
                return;
            }
            WZ_CORE_INFO("Successfully processed mesh {0}/{1}...", i + 1, _scene->mNumMeshes);
        }

        WZ_CORE_INFO("Model successfully loaded");

        m_isGarbage = false;
    }
    void Model::Unload() {
        m_isGarbage = true;
        m_meshes.clear();
    }
    void Model::Reload() {
        Unload();
        Load();
    }
    void Model::Save() {
        WZ_CORE_WARN("Saving of models is not yet implemented");
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

    Mesh Model::ProcessMesh(aiMesh *assimpMesh, bool *success) {
        *success = true;
        string assimpMeshError = "";
        WZ_CORE_TRACE("Verifying mesh");
        if (!VerifyAssimpMesh(assimpMesh, &assimpMeshError)) {
            WZ_CORE_ERROR("Failed loading mesh: {0}", assimpMeshError);
            m_isGarbage = true;
            *success = false;
            return Mesh();
        }
        WZ_CORE_TRACE("Mesh OK");

        WZ_CORE_TRACE("Reserving memory for {0} vertices and {1} faces ({2} indices) [{2} Bytes]",
                        assimpMesh->mNumVertices, assimpMesh->mNumFaces,
                        assimpMesh->mNumFaces * 3,
                        (sizeof(Vertex) * assimpMesh->mNumVertices + sizeof(u32) * assimpMesh->mNumFaces * 3));

        std::vector<Vertex> _vertices;
        _vertices.reserve(assimpMesh->mNumVertices);
        std::vector<u32> _indices;
        _vertices.reserve(assimpMesh->mNumFaces * 3);
        string _name;

        WZ_CORE_TRACE("Iterating {0} vertices...", assimpMesh->mNumVertices);
        for (u32 i = 0; i < assimpMesh->mNumVertices; i++) {
            if (rand() % 500 == 3) {
                WZ_CORE_TRACE("Vertices progress: {0}%", (int)(((float)i / (float)assimpMesh->mNumVertices) * 100));
            }
            aiVector3D& _pos = assimpMesh->mVertices[i];
            aiVector3D _uv;
            aiVector3D& _normal = assimpMesh->mNormals[i];
            if (assimpMesh->HasTextureCoords(0)) {
                _uv = assimpMesh->mTextureCoords[0][i];
            } else {
                _uv = aiVector3D(0, 0, 0);
            }

            _name = assimpMesh->mName.C_Str();

            _vertices.push_back({
                vec3(_pos.x, _pos.y, _pos.z),
                vec2(_uv.x, _uv.y),
                vec3(_normal.x, _normal.y, _normal.z)
            });
        }

        for (u32 i = 0; i < assimpMesh->mNumFaces; i++) {
            aiFace& _face = assimpMesh->mFaces[i];
            WZ_CORE_ASSERT(_face.mNumIndices == 3, "Model must be triangulated (Should be done automatically at import?)");

            for (u32 j = 0; j < _face.mNumIndices; j++) {
                _indices.push_back(_face.mIndices[j]);
            }
        }

        return { _vertices, _indices, _name };
    }

    Model* Model::Create(const string& file, ulib::Bitset flags) {
        return new Model(file, flags);
    }
}
