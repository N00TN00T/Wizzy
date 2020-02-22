//#pragma once
//
//#include "Wizzy/Resource/Resource.h"
//#include "Wizzy/Renderer/Buffers.h"
//#include "Wizzy/Renderer/Material.h"
//
//struct aiMesh;
//struct aiScene;
//struct aiMaterial;
//struct aiNode;
//
//namespace Wizzy {
//
//    class Material;
//
//    struct Vertex {
//        vec3 position;
//        vec2 uv;
//        vec3 normal;
//    };
//
//    class Mesh {
//    public:
//        Mesh() {
//            WZ_CORE_WARN("Default mesh constructor was called");
//        }
//        ~Mesh();
//        Mesh(const std::vector<Vertex>& vertices,
//             const std::vector<u32>& indices,
//             const Material::Handle& = WZ_NULL_RESOURCE_HANDLE,
//             const string& name = "unnamed_mesh");
//
//        inline
//        const std::vector<VertexBufferPtr>& GetVBOs() const { return m_VBOs; }
//        inline
//        const VertexBufferPtr& GetMainVBO() const { return m_VBOs[0]; }
//        inline
//        const IndexBufferPtr& GetIBO() const { return m_IBO; }
//        inline
//        const VertexArrayPtr& GetVAO() const { return m_VAO; }
//        inline
//        const Material::Handle& GetMaterialHandle() const { return m_materialHandle; }
//
//        inline
//        const string& GetName() const { return m_name; }
//
//    private:
//        void Init();
//    private:
//        std::vector<Vertex>             m_vertices;
//        std::vector<u32>                m_indices;
//        string                          m_name;
//        std::vector<VertexBufferPtr>    m_VBOs;
//        IndexBufferPtr                  m_IBO;
//        VertexArrayPtr                  m_VAO;
//        Material::Handle                m_materialHandle;
//    };
//
//    class Model : public Resource 
//    {
//    public:
//        __HANDLE_DEF;
//    public:
//        Model(const string& data, const PropertyLibrary& props);
//
//        inline
//        std::vector<Mesh>& GetMeshes() { return m_meshes; }
//
//        virtual
//        string Serialize() const override;
//
//        static
//        Model* Create(const string& data, const PropertyLibrary& props);
//
//        virtual const PropertyLibrary& GetTemplateProps() const override;
//        virtual void Deserialize(const string& data) override;
//
//    private:
//		void AssimpLoad(const string& data, const PropertyLibrary& props);
//        bool VerifyAssimpScene(const aiScene *scene, string *error);
//        bool VerifyAssimpMesh(const aiMesh *mesh, string *error);
//        bool ProcessAssimpNode(aiNode *node, const aiScene *scene);
//        Mesh ProcessAssimpMesh(aiMesh *mesh, const aiScene *scene, bool *success, int32 meshIndex);
//
//    private:
//        std::vector<Mesh> m_meshes;
//    };
//
//}
