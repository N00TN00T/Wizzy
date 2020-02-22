#include "wzpch.h"
//
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//
//#include "Wizzy/WizzyExceptions.h"
//#include "Wizzy/Renderer/Model.h"
//#include "Wizzy/Resource/ResourceManagement.h"
//#include "Wizzy/Renderer/Shader.h"
//#include "Wizzy/Renderer/Material.h"
//#include "Wizzy/Renderer/Texture.h"
//
//namespace Wizzy {
//
//    /****************************************************
//    ****************MESH*********************************
//    *****************************************************/
//
//    Mesh::Mesh(const std::vector<Vertex>& vertices,
//               const std::vector<u32>& indices,
//               const Material::Handle& materialHandle,
//               const string& name)
//        : m_vertices(vertices), m_indices(indices), m_name(name),
//          m_materialHandle(materialHandle) {
//        Init();
//    }
//
//    Mesh::~Mesh() {
//        m_vertices.clear();
//        m_indices.clear();
//        m_VBOs.clear();
//    }
//
//    void Mesh::Init() {
//        WZ_CORE_TRACE("Initializing mesh '{0}' with {1} vertices and {2} indices...", m_name, m_vertices.size(), m_indices.size());
//        WZ_CORE_ASSERT(m_vertices.size() >= 3, "Mesh must have at least 3 vertices");
//        WZ_CORE_ASSERT(m_indices.size() >= 3, "Mesh must have at least 3 indices");
//
//        m_VAO = VertexArrayPtr(VertexArray::Create());
//
//        VertexBufferPtr _mainVBO(VertexBuffer::Create(&m_vertices[0],
//                             sizeof(Vertex) * m_vertices.size()));
//
//        BufferLayout _layout = {
//            { ShaderDataType::FLOAT3, "vertexPosition" },
//			{ ShaderDataType::FLOAT2, "vertexUv" },
//            { ShaderDataType::FLOAT3, "vertexNormal" }
//        };
//
//        _mainVBO->SetLayout(_layout);
//
//        m_VBOs.push_back(_mainVBO);
//
//        m_IBO = IndexBufferPtr(IndexBuffer::Create(&m_indices[0], m_indices.size()));
//
//       /* for (const auto& _VBO : m_VBOs) {
//            m_VAO->PushVertexBuffer(_VBO);
//        }*/
//        m_VAO->SetIndexBuffer(m_IBO);
//        WZ_CORE_INFO("Successfully initialized mesh '{0}' with {1} vertices and {2} indices ({3} triangles)", m_name, m_vertices.size(), m_indices.size(), m_indices.size() / 3);
//    }
//
//    /****************************************************
//    ****************MODEL*********************************
//    *****************************************************/
//
//    Model::Model(const string& data, const PropertyLibrary& props)
//        : Resource(props) {
//
//		WZ_CORE_TRACE("Decoding model with assimp");
//		AssimpLoad(data, props);
//    }
//
//    string Model::Serialize() const {
//        /*WZ_CORE_ERROR("Model serializing not yet implemented");
//        return "";*/
//    }
//
//	void Model::AssimpLoad(const string& data, const PropertyLibrary& props) {
//		//WZ_CORE_TRACE("Reading model with assimp from data");
//		//Assimp::Importer _importer;
//
//		//const aiScene* _scene
//		//	= _importer.ReadFileFromMemory(&data[0], data.size()-1,
//		//		aiProcess_Triangulate //|
//		//		//aiProcess_JoinIdenticalVertices |
//		//		//aiProcess_GenNormals |
//		//		//aiProcess_CalcTangentSpace |
//		//		//aiProcess_SortByPType |
//		//		//aiProcess_RemoveRedundantMaterials |
//		//		//aiProcess_OptimizeMeshes |
//		//		//aiProcess_OptimizeGraph
//		//	);
//
//		//string _sceneError = "";
//
//		//if (!_scene) {
//
//  //          WZ_THROW("Failed loading model, Assimp error:\n    " + string(_importer.GetErrorString()));
//		//	return;
//		//}
//		//WZ_CORE_INFO("Successfully read model from data");
//
//		//WZ_CORE_TRACE("Verifying scene...");
//		//if (!VerifyAssimpScene(_scene, &_sceneError)) {
//
//  //          WZ_THROW("Failed loading model from data:\n    " + _sceneError);
//		//	return;
//		//}
//		//WZ_CORE_INFO("Scene was verified successfully");
//
//		//ProcessAssimpNode(_scene->mRootNode, _scene);
//	}
//
//    bool Model::VerifyAssimpScene(const aiScene *scene, string *error) {
//
//        *error = "Unknown error";
//
//        if (!scene) {
//            *error = "Scene was null, assimp error...";
//            return false;
//        }
//
//        if (!scene->HasMeshes()) {
//            *error = "File has no meshes";
//            return false;
//        }
//
//        return true;
//    }
//    bool Model::VerifyAssimpMesh(const aiMesh *mesh, string *error) {
//
//        *error = "Unknown error";
//
//        if (!mesh) {
//            *error = "Mesh is null";
//            return false;
//        }
//
//        if (!mesh->HasFaces()) {
//            *error = "Mesh has no indices";
//            return false;
//        }
//
//        if (!mesh->HasNormals()) {
//            *error = "Mesh has no normals";
//            return false;
//        }
//
//        if (!mesh->HasPositions()) {
//            *error = "Mesh has no positions";
//            return false;
//        }
//
//        return true;
//    }
//
//    const PropertyLibrary& Model::GetTemplateProps() const
//    {
//        
//    }
//
//    void Model::Deserialize(const string& data)
//    {
//    }
//
//    bool Model::ProcessAssimpNode(aiNode *node, const aiScene *scene) {
//        WZ_CORE_TRACE("Processing ai node '{0}'...", node->mName.C_Str());
//        WZ_CORE_TRACE("Processing {0} meshes...", node->mNumMeshes);
//        m_meshes.reserve(node->mNumMeshes);
//        for (u32 i = 0; i < node->mNumMeshes; i++) {
//            WZ_CORE_TRACE("Processing mesh {0}/{1}", i + 1, node->mNumMeshes);
//            aiMesh *_mesh = scene->mMeshes[node->mMeshes[i]];
//            bool _meshSuccess = false;
//            auto _processedMesh = ProcessAssimpMesh(_mesh, scene, &_meshSuccess, i);
//            if (_meshSuccess) {
//                m_meshes.push_back(_processedMesh);
//            } else {
//                WZ_CORE_ERROR("Failed processing node, error when processing mesh.");
//                return false;
//            }
//            WZ_CORE_INFO("Successfully processed mesh {0}/{1}", i + 1, node->mNumMeshes);
//        }
//
//        WZ_CORE_TRACE("Processing {0} node children...", node->mNumChildren);
//        bool _anyFailure = false;
//        for (u32 i = 0; i < node->mNumChildren; i++) {
//            WZ_CORE_TRACE("Processing child {0}/{1}", i + 1, node->mNumChildren);
//            if (!ProcessAssimpNode(node->mChildren[i], scene)) {
//                _anyFailure = true;
//                break;
//            }
//        }
//
//        if (_anyFailure) {
//            return false;
//        }
//        WZ_CORE_INFO("Successfully processed ai node '{0}'", node->mName.C_Str());
//        return true;
//    }
//
//    Mesh Model::ProcessAssimpMesh(aiMesh *mesh, const aiScene *scene, bool *success, int32 meshIndex) {
//        *success = true;
//        string meshError = "";
//        std::vector<Vertex> _vertices;
//        std::vector<u32> _indices;
//
//        WZ_CORE_TRACE("Verifying mesh");
//        if (!VerifyAssimpMesh(mesh, &meshError)) {
//            WZ_CORE_ERROR("Failed processing mesh: {0}", meshError);
//            *success = false;
//            return Mesh();
//        }
//        WZ_CORE_TRACE("Mesh OK");
//
//        WZ_CORE_TRACE("Reserving memory for {0} vertices and {1} faces ({2} indices) [{2} Bytes]",
//                        mesh->mNumVertices, mesh->mNumFaces,
//                        mesh->mNumFaces * 3,
//                        (sizeof(Vertex) * mesh->mNumVertices + sizeof(u32) * mesh->mNumFaces * 3));
//
//        //_vertices.reserve(mesh->mNumVertices);
//        /*_indices.reserve(mesh->mNumFaces * 3);
//
//        if (mesh->HasTextureCoords(0)) {
//            WZ_CORE_TRACE("Mesh has uv's");
//        } else {
//            WZ_CORE_TRACE("Mesh doesn't have uv's");
//        }
//
//        if (mesh->HasNormals()) {
//            WZ_CORE_TRACE("Mesh has normals");
//        } else {
//            WZ_CORE_TRACE("Mesh doesn't have normals");
//        }
//
//        WZ_CORE_TRACE("Iterating {0} vertices...", mesh->mNumVertices);
//        for (u32 i = 0; i < mesh->mNumVertices; i++) {
//            if (rand() % 500 == 3) {
//                WZ_CORE_TRACE("Vertices progress: {0}%", (int)(((float)i / (float)mesh->mNumVertices) * 100));
//            }
//            Vertex _vertex;
//
//            _vertex.position.x = mesh->mVertices[i].x;
//            _vertex.position.y = mesh->mVertices[i].y;
//            _vertex.position.z = mesh->mVertices[i].z;
//
//            if (mesh->mTextureCoords[0]) {
//                _vertex.uv.x = (float)mesh->mTextureCoords[0][i].x;
//                _vertex.uv.y = (float)mesh->mTextureCoords[0][i].y;
//            }
//
//            if (mesh->HasNormals()) {
//
//                _vertex.normal.x = mesh->mNormals[i].x;
//                _vertex.normal.y = mesh->mNormals[i].y;
//                _vertex.normal.z = mesh->mNormals[i].z;
//            }
//
//            _vertices.push_back(_vertex);
//        }
//
//        WZ_CORE_TRACE("Iterating {0} triangles", mesh->mNumFaces);
//        for (u32 i = 0; i < mesh->mNumFaces; i++) {
//            aiFace& _face = mesh->mFaces[i];
//            WZ_CORE_ASSERT(_face.mNumIndices == 3, "Model must be triangulated (Should be done automatically at import?)");
//
//            _indices.push_back(_face.mIndices[0]);
//            _indices.push_back(_face.mIndices[1]);
//            _indices.push_back(_face.mIndices[2]);
//        }
//
//        string _meshName = "_mesh_" + std::to_string(meshIndex) + string("_") + mesh->mName.C_Str();
//
//        return { _vertices, _indices, "", _meshName };*/
//    }
//
//    Model* Model::Create(const string& data, const PropertyLibrary& props) {
//        return new Model(data, props);
//    }
//}
