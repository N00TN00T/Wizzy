#include <wzpch.h>

#include <stb_image.h>
#include <glad/glad.h>

#include "Wizzy/Resources/Texture2D.h"
#include "Wizzy/platform/OpenGL/GLErrorHandling.h"

namespace Wizzy {

    Texture2D::~Texture2D() {
        if (!this->IsGarbage()) this->Unload();
    }

    void Texture2D::Load() {
        const string& _file = GetSourceFile();

        WZ_CORE_TRACE("Loading Texture2D from file '{0}' ...", _file);

        stbi_set_flip_vertically_on_load(true);

        m_data = stbi_load(_file.c_str(), &m_width, &m_height,
                                        &m_channels, 0);

        WZ_CORE_ASSERT(m_data, "Failed loading Texture2D; invalid file");

        Init();

        WZ_CORE_INFO("Successfully loaded Texture2D and assigned id '{0}'",
                                                                m_textureId);
    }
    void Texture2D::Unload() {
        WZ_CORE_TRACE("Unloading Texture2D with id '{0}'", m_textureId);
        WZ_CORE_ASSERT(!IsGarbage(), "Tried unloading garbage-flagged Texture2D");
        stbi_image_free(m_data);
        GL_CALL(glDeleteTextures(1, &m_textureId));
        m_data = nullptr;
        m_textureId = WZ_TEXTURE_ID_INVALID;
    }
    void Texture2D::Reload() {
        Unload();
        Load();
    }
    void Texture2D::Save() {

    }

    void Texture2D::Bind(const int32& location) const {
        GL_CALL(glActiveTexture(location));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_textureId));
    }

    void Texture2D::Unbind() const {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void Texture2D::Init() {

        GL_CALL(glGenTextures(1, &m_textureId));

        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_textureId));

        GL_CALL(glTexParameteri(GL_TEXTURE_2D,
                                GL_TEXTURE_WRAP_S,
                                GL_CLAMP_TO_EDGE));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D,
                                GL_TEXTURE_WRAP_T,
                                GL_CLAMP_TO_EDGE));

        GL_CALL(glTexParameteri(GL_TEXTURE_2D,
                                GL_TEXTURE_MIN_FILTER,
                                GL_LINEAR_MIPMAP_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D,
                                GL_TEXTURE_MAG_FILTER,
                                GL_LINEAR));

        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height,
                             0, GL_RGB, GL_UNSIGNED_BYTE, m_data));

		GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

		float halfW = m_width / 2.f;
		float halfH = m_height / 2.f;

		float _vertices[] = {
			// positions            // texture coords
			 halfW,  halfH, 0.0f,      1.0f, 1.0f,   // top right
			 halfW, -halfH, 0.0f,      1.0f, 0.0f,   // bottom right
			-halfW, -halfH, 0.0f,      0.0f, 0.0f,   // bottom left
			-halfW,  halfH, 0.0f,      0.0f, 1.0f    // top left
		};

		u32 _indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};

		u32 VBO, VAO, EBO;
		GL_CALL(glGenVertexArrays(1, &VAO));
		GL_CALL(glGenBuffers(1, &VBO));
		GL_CALL(glGenBuffers(1, &EBO));

		GL_CALL(glBindVertexArray(VAO));

		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW));

		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
		GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices), _indices, GL_STATIC_DRAW));

		// position attribute
		GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
		GL_CALL(glEnableVertexAttribArray(0));
		// texture coord attribute
		GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
		GL_CALL(glEnableVertexAttribArray(2));

		m_vao = VAO;
    }

}
