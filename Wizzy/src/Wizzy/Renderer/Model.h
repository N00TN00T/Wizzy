#pragma once

#include "Wizzy/Resource/IResource.h"
#include "Wizzy/Renderer/Buffers.h"
#include "Wizzy/Renderer/Material.h"

struct aiMesh;
struct aiScene;

namespace Wizzy {

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
             const string& name = "unnamed_mesh");

        inline
        const std::vector<VertexBufferPtr>& GetVBOs() const { return m_VBOs; }
        inline
        const VertexBufferPtr& GetMainVBO() const { return m_VBOs[0]; }
        inline
        const IndexBufferPtr& GetIBO() const { return m_IBO; }
        inline
        const VertexArrayPtr& GetVAO() const { return m_VAO; }

    private:
        void Init();
    private:
        std::vector<Vertex>             m_vertices;
        std::vector<u32>                m_indices;
        string                          m_name;
        std::vector<VertexBufferPtr>    m_VBOs;
        IndexBufferPtr                  m_IBO;
        VertexArrayPtr                  m_VAO;
    };

    class Model
        : public IResource {
    public:
        Model(const string& file, const ulib::Bitset& flags);

        virtual
        void Load() override;
        virtual
        void Unload() override;
        virtual
        void Reload() override;
        virtual
        void Save() override;
        inline virtual
        bool IsGarbage() const override { return m_isGarbage; }

        inline
        std::vector<Mesh>& GetMeshes() { return m_meshes; }

        static Model* Create(const string& file, ulib::Bitset flags = ulib::Bitset());

    private:
        bool VerifyAssimpScene(const aiScene *scene, string *error);
        bool VerifyAssimpMesh(const aiMesh *mesh, string *error);
        Mesh ProcessMesh(aiMesh *assimpMesh, bool *success);

    private:
        std::vector<Mesh> m_meshes;
        bool m_isGarbage = true;
        ulib::Bitset m_flags;
    };

}
