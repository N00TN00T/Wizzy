#pragma once

#include "Wizzy/Resource/Resource.h"
#include "Wizzy/Renderer/Buffers.h"

struct aiMesh;
struct aiScene;
struct aiMaterial;
struct aiNode;

namespace Wizzy {

    class Material;

    typedef ResourceHandle ModelHandle;

    struct Vertex {
        vec3 position;
        vec2 uv;
        vec3 normal;
    };

    class Mesh {
    public:
        Mesh() {
            WZ_CORE_WARN("Default mesh constructor was called");
        }
        ~Mesh();
        Mesh(const std::vector<Vertex>& vertices,
             const std::vector<u32>& indices,
             const string& materialHandle = WZ_NULL_RESOURCE_HANDLE,
             const string& name = "unnamed_mesh");

        inline
        const std::vector<VertexBufferPtr>& GetVBOs() const { return m_VBOs; }
        inline
        const VertexBufferPtr& GetMainVBO() const { return m_VBOs[0]; }
        inline
        const IndexBufferPtr& GetIBO() const { return m_IBO; }
        inline
        const VertexArrayPtr& GetVAO() const { return m_VAO; }
        inline
        const MaterialHandle& GetMaterialHandle() const { return m_materialHandle; }

    private:
        void Init();
    private:
        std::vector<Vertex>             m_vertices;
        std::vector<u32>                m_indices;
        string                          m_name;
        std::vector<VertexBufferPtr>    m_VBOs;
        IndexBufferPtr                  m_IBO;
        VertexArrayPtr                  m_VAO;
        MaterialHandle                  m_materialHandle;
    };

    class Model
        : public Resource {
    public:
        Model(const string& sourceFile, const string& data, const ulib::Bitset& flags);

        inline
        std::vector<Mesh>& GetMeshes() { return m_meshes; }

        virtual
        string Serialize() const override;

        static
        Model* Create(const string& sourceFile, const string& data, ulib::Bitset flags = ulib::Bitset());

    private:
        bool VerifyAssimpScene(const aiScene *scene, string *error);
        bool VerifyAssimpMesh(const aiMesh *mesh, string *error);
        bool ProcessNode(aiNode *node, const aiScene *scene,
                         const string& sourceFile);
        Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene, bool *success,
                         const string& sourceFile, int32 meshIndex);
        MaterialHandle ProcessMaterial(aiMaterial *mat,
                                       const string& meshName,
                                       const aiScene *scene,
                                       const string& sourceFile);
        std::vector<TextureHandle> LoadMaterialTextures(aiMaterial *mat,
                                                        int32 textureType,
                                                        const aiScene *scene,
                                                        const string& sourceFile);


    private:
        std::vector<Mesh> m_meshes;

    };

}
