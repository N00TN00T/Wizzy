#include <wzpch.h>

#include <stb_image.h>

#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/platform/OpenGL/GLTexture.h"
#include "Wizzy/Utils.h"
#include "Wizzy/Renderer/Renderer.h"

namespace Wizzy {

    Texture::~Texture() {

    }

    void Texture::Load() {
        const string& _file = GetSourceFile();

        WZ_CORE_TRACE("Loading Texture from file '{0}' ...", ulib::File::to_absolute(_file));

        stbi_set_flip_vertically_on_load(false);

        m_data = stbi_load(_file.c_str(), &m_width, &m_height,
                                        &m_channels, 4);

        WZ_CORE_ASSERT(m_data, "Failed loading Texture; invalid file");

        WZ_CORE_INFO("Succesfully loaded Texture (W: '{0}', H: '{1}', C: {2})", m_width, m_height, m_channels);

        this->Init();
    }
    void Texture::Unload() {
        WZ_CORE_TRACE("Unloading Texture originally loaded from '{0}'", this->GetSourceFile());
        WZ_CORE_ASSERT(!IsGarbage(), "Tried unloading garbage-flagged Texture");
        stbi_image_free(m_data);
        this->Delete();
        m_data = nullptr;
    }
    void Texture::Reload() {
        Unload();
        Load();
    }
    void Texture::Save() {
        WZ_CORE_WARN("Saving texture not yet implemted");
    }

    Texture* Texture::Create(const string& file, const ulib::Bitset& flags) {

        CREATE_BY_API(new GLTexture(file, flags));

        return nullptr;
    }

}
