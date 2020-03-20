#include "wzpch.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Wizzy/Renderer/Font.h"
#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/Resource/ResourceManagement.h"
#include "Wizzy/Renderer/RenderTarget.h"
#include "Wizzy/Renderer/Renderer2D.h"

namespace Wizzy 
{

    Font::Font(const ResData& data, const PropertyLibrary& props)
        : Resource(props)
    {

        if (data.size() <= TOKEN_HEADER.size())
        {
            WZ_THROW(Exception, "Invalid font data");
        }

        string header = "";

        for (int32 i = 0; i < TOKEN_HEADER.size(); i++) header += data[i];


        /*
            Wizzy serialized ttf fonts to a custom file format after first
            load. This needs to be detected by looking for the header to use
            the correct initialization function to process the data properly.
        */
        if (header == TOKEN_HEADER)
        {
            InitWZ(data, props);
        }
        else
        {
            InitFT(data, props);
        }

        WZ_CORE_INFO("Font initialized (fontsize: {0}, characters: {1})", m_fontSize, m_characterInfo.size());
    }

    Font::~Font()
    {
        while (m_cachedStrings.size() > 0)
        {
            auto& str = m_cachedStrings.front();

            ResourceManagement::Delete(m_cache[str].hTexture);

            m_cache.erase(str);

            m_cachedStrings.pop();
        }
    }

    RenderTarget::Handle Font::Render(const string& text, Shader::Handle hShader)
    {
        WZ_CORE_ASSERT(ResourceManagement::IsLoaded(hShader), "Shader not loaded in font render");
        if (m_cache.find(text) != m_cache.end() && ResourceManagement::IsLoaded(m_cache[text].hTexture))
        { // Text has a cached texture, just use that
            WZ_CORE_TRACE("Reusing text rendertarget from cache");
            return m_cache[text].hTexture;
        }
        else
        { // Text has no cached texture, create new texture
            WZ_CORE_TRACE("Rendering text to rendertarget");
            glm::vec2 textureSize = MeasureString(text);
            RenderTarget::Handle hNewTexture = ResourceManagement::AddRuntimeResource<RenderTarget>(RenderTarget::Create(textureSize.x, textureSize.y), RenderTarget::GetTemplateProps());
            WZ_CORE_ASSERT(ResourceManagement::IsLoaded(hNewTexture), "Failed creating runtime rendertarget texture for font render");

            ValidateCache();

            m_cache[text].hTexture = hNewTexture;
            m_cache[text].size = MeasureString(text);
            m_cachedStrings.push(text);

            auto& newTexture = ResourceManagement::Get<RenderTarget>(hNewTexture);
            Renderer2D::Begin(hShader, glm::ortho<float>(0, newTexture.GetWidth(), 0.f, newTexture.GetHeight()), hNewTexture);
            std::vector<RenderTarget::Handle> toDelete;
            glm::vec2 penPos(0, textureSize.y - m_fontSize);
            for (auto c : text)
            {
                if (c == '\n')
                {
                    penPos.y -= m_fontSize;
                    penPos.x = 0;
                    continue;
                }

                if (m_characterInfo.find(c) == m_characterInfo.end()) c = '?';

                auto& info = m_characterInfo[c];

                if (c == ' ')
                {
                    penPos.x += info.advanceX;
                    continue;
                }

                RenderTarget::Handle hCharTexture = ResourceManagement::AddRuntimeResource<RenderTarget>(RenderTarget::Create(info.width, info.height), RenderTarget::GetTemplateProps());
                WZ_CORE_ASSERT(ResourceManagement::IsLoaded(hCharTexture), "Failed creating rendertarget texture for char font rendering");
                toDelete.push_back(hCharTexture);

                auto& charTexture = ResourceManagement::Get<RenderTarget>(hCharTexture);
                // Submit & draw character on char texture
                Renderer2D::Begin(hShader, glm::ortho<float>(0, charTexture.GetWidth(), 0.f, charTexture.GetHeight()), hCharTexture);
                auto& atlasTexture = ResourceManagement::Get<Texture>(m_hAtlasTexture);
                Renderer2D::SubmitImage(m_hAtlasTexture, { -(info.xOffset * atlasTexture.GetWidth()), 0 }, vec2(1.f), 0, Color::white, hCharTexture);
                Renderer2D::End(hCharTexture);

                // Draw the char texture on the final texture
                Renderer2D::SubmitImage(hCharTexture, { penPos.x + info.bitmapLeft, penPos.y - (info.height - info.bitmapTop) }, vec2(1.f), 0, Color::white, hNewTexture);

                penPos.x += info.advanceX;
            }

            Renderer2D::End(hNewTexture);

            for (auto r : toDelete)
            {
                ResourceManagement::Delete(r);
            }

            return hNewTexture;
        }
    }

    glm::vec2 Font::MeasureString(const string& str)
    {
        if (m_cache.find(str) != m_cache.end())
        {
            WZ_CORE_TRACE("Reusing text size from cache for '{0}'", str);
            return m_cache[str].size;
        }
        else
        {
            WZ_CORE_TRACE("Measing text size for '{0}'", str);
            glm::vec2 size(0, m_fontSize);

            int32 posX = 0;
            int32 yBonus = 0;
            for (int i = 0; i < str.size(); i++)
            {
                auto c = str[i];
                if (m_characterInfo.find(c) == m_characterInfo.end()) c = '?';
                auto& info = m_characterInfo[c];

                if (c == '\n')
                {
                    size.x = std::max((int32)size.x, posX);
                    posX = 0;
                    size.y += m_fontSize;
                    yBonus = 0;
                    continue;
                }
                if (c == ' ')
                {
                    posX += info.advanceX;
                    continue;
                }

                posX += info.advanceX;
                yBonus = std::max(yBonus, (int32)(info.height - info.bitmapTop));
            }

            size.x = std::max((int32)size.x, posX);
            size.y += yBonus;

            ValidateCache();

            m_cache[str].size = size;

            return size;
        }
    }

    void Font::ValidateCache()
    {
        while (m_cachedStrings.size() >= MAX_CACHED_STRINGS)
        {
            auto& top = m_cachedStrings.front();

            ResourceManagement::Delete(m_cache[top].hTexture);

            m_cache.erase(top);

            m_cachedStrings.pop();
        }
    }
    void Font::InitFT(const ResData& data, const PropertyLibrary& props)
    {
        WZ_CORE_TRACE("Iniitializing font with freetype");
        FT_Library ft;
        FT_Face face;

        auto _error = FT_Init_FreeType(&ft);

        if (_error != FT_Err_Ok)
        {
            std::cout << "Failed FT init\n";
            abort();
        }

        _error = FT_New_Memory_Face(ft, data.data(), data.size(), 0, &face);


        if (_error != FT_Err_Ok)
        {
            std::cout << "Failed loading font face\n";
            abort();
        }
        m_fontSize = props.IsProperty<int32>("Font size") ? props.GetProperty<int32>("Font size") : GetTemplateProps().GetProperty<int32>("Font size");
        FT_Set_Pixel_Sizes(face, 0, m_fontSize);

        int32 rowWidth = 0;
        int32 colHeight = 0;

        int32 atlasWidth = 0;
        int32 atlasHeight = 0;

        int32 charCount = 0;

        WZ_CORE_TRACE("Counting atlas texture size");
        for (int32 i = 0; i < 128; i++) // TODO: Unicode
        {
            _error = FT_Load_Char(face, i, FT_LOAD_DEFAULT);
            if (_error != FT_Err_Ok || i == ' ')  continue;

            WZ_CORE_TRACE("Found char value {0}", i);

            atlasWidth += face->glyph->bitmap.width + 2;
            atlasHeight = std::max(atlasHeight, (int)face->glyph->bitmap.rows);

            charCount++;
        }

        m_spaceSize = atlasWidth / charCount;

        WZ_CORE_TRACE("Creating atlas texture");
        PropertyLibrary atlasProps = Texture::GetTemplateProps();
        atlasProps.SetProperty<int32>("MinFilterMode", (int32)WZ_MIN_FILTER_MODE_LINEAR);
        atlasProps.SetProperty<int32>("MagFilterMode", (int32)WZ_MAG_FILTER_LINEAR);
        m_hAtlasTexture = (Texture::Handle)ResourceManagement::AddRuntimeResource<Texture>(Texture::Create(NULL, atlasWidth, atlasHeight, 1, atlasProps), Texture::GetTemplateProps());// new Texture(NULL, atlasWidth, atlasHeight, 1);
        WZ_CORE_ASSERT(ResourceManagement::IsLoaded(m_hAtlasTexture), "Failed creating atlas texture for font");
        int32 textPos = 0;

        WZ_CORE_TRACE("Drawing characters to atlas texture");
        for (int32 i = 0; i < 128; i++)
        {
            _error = FT_Load_Char(face, i, FT_LOAD_RENDER);
            if (_error != FT_Err_Ok)  continue;

            WZ_CORE_TRACE("Found char value {0}", i);

            auto glyph = face->glyph;

            m_characterInfo[i].advanceX = glyph->advance.x >> 6;
            m_characterInfo[i].advanceY = glyph->advance.y >> 6;

            m_characterInfo[i].width = glyph->bitmap.width;
            m_characterInfo[i].height = glyph->bitmap.rows;

            m_characterInfo[i].bitmapLeft = glyph->bitmap_left;
            m_characterInfo[i].bitmapTop = glyph->bitmap_top;

            m_characterInfo[i].xOffset = (float)textPos / (float)atlasWidth;

            if (glyph->bitmap.buffer)
            {
                auto& atlasTexture = ResourceManagement::Get<Texture>(m_hAtlasTexture);
                atlasTexture.AddSubTexture(glyph->bitmap.buffer, glyph->bitmap.width, glyph->bitmap.rows, textPos, 0, 1);
            }

            textPos += glyph->bitmap.width + 2;
        }
    }
    void Font::InitWZ(const ResData& data, const PropertyLibrary& props)
    {
        WZ_CORE_TRACE("Initializing font with wizzy");

        string dataString = "";
        dataString.resize(data.size());
        memcpy(&dataString[0], data.data(), data.size());

        auto fontSizeIndex = dataString.find(TOKEN_ATLAS) + TOKEN_ATLAS.size();
        auto atlasSizeIndex = fontSizeIndex + sizeof(u32);
        auto atlasIndex = atlasSizeIndex + sizeof(u32);
        auto infoCountIndex = dataString.find(TOKEN_INFO) + TOKEN_INFO.size();
        auto infoIndex = infoCountIndex + sizeof(u32);

        string atlasSizeString = dataString.substr(atlasSizeIndex, sizeof(u32));

        u32 atlasSize = 0;
        memcpy(&atlasSize, atlasSizeString.data(), sizeof(u32));

        string fontSizeString = dataString.substr(fontSizeIndex, sizeof(u32));
        string atlasString = dataString.substr(atlasIndex, atlasSize);
        string infoCountString = dataString.substr(infoCountIndex, sizeof(u32));
        string infoString = dataString.substr(infoIndex);

        memcpy(&m_fontSize, fontSizeString.data(), sizeof(u32));

        ResData atlasData;
        atlasData.resize(atlasString.size());
        memcpy(&atlasData[0], atlasString.data(), atlasString.size());

        PropertyLibrary atlasProps = Texture::GetTemplateProps();
        atlasProps.SetProperty<int32>("MinFilterMode", (int32)WZ_MIN_FILTER_MODE_LINEAR);
        m_hAtlasTexture = ResourceManagement::AddRuntimeResource((Texture*)Texture::Create(atlasData, atlasProps), Texture::GetTemplateProps());
        WZ_CORE_ASSERT(ResourceManagement::IsLoaded(m_hAtlasTexture), "Failed creating atlas texture for font");

        u32 infoCount = 0;
        memcpy(&infoCount, infoCountString.data(), infoCountString.size());

        ResData infoData;
        infoData.resize(infoString.size());
        memcpy(&infoData[0], infoString.data(), infoString.size());

        u32 offset = 0;

        for (int32 i = 0; i < infoCount; i++)
        {
            u32 c = 0;
            memcpy(&c, &infoData[offset], sizeof(u32));
            offset += sizeof(u32);

            CharacterInfo info;
            memcpy(&info, &infoData[offset], sizeof(CharacterInfo));
            offset += sizeof(CharacterInfo);

            m_characterInfo[c] = info;
        }
    }
    ResData Font::Serialize() const
    {
        ResData data;

        u32 offset = 0;

        // Set token header
        data.resize(TOKEN_HEADER.size());
        memcpy(&data[offset], TOKEN_HEADER.data(), TOKEN_HEADER.size());
        offset += TOKEN_HEADER.size();

        // Set token atlas
        data.resize(data.size() + TOKEN_ATLAS.size());
        memcpy(&data[offset], TOKEN_ATLAS.data(), TOKEN_ATLAS.size());
        offset += TOKEN_ATLAS.size();

        // Set font size
        data.resize(data.size() + sizeof(u32));
        memcpy(&data[offset], &m_fontSize, sizeof(u32));
        offset += sizeof(u32);

        auto atlasData = ResourceManagement::Get<Texture>(m_hAtlasTexture).Serialize();
        u32 sz = atlasData.size();

        // Set atlas size
        data.resize(data.size() + sizeof(u32));
        memcpy(&data[offset], &sz, sizeof(u32));
        offset += sizeof(u32);

        // Set atlas data
        data.resize(data.size() + atlasData.size());
        memcpy(&data[offset], atlasData.data(), atlasData.size());
        offset += atlasData.size();

        // Set token info
        data.resize(data.size() + TOKEN_INFO.size());
        memcpy(&data[offset], TOKEN_INFO.data(), TOKEN_INFO.size());
        offset += TOKEN_INFO.size();

        // Set info count
        u32 infoCount = m_characterInfo.size();
        data.resize(data.size() + sizeof(u32));
        memcpy(&data[offset], &infoCount, sizeof(u32));
        offset += sizeof(u32);

        for (auto [c, info] : m_characterInfo)
        {
            // Set info value
            data.resize(data.size() + sizeof(int32));
            memcpy(&data[offset], &c, sizeof(int32));
            offset += sizeof(int32);

            // Set character info  data
            data.resize(data.size() + sizeof(CharacterInfo));
            memcpy(&data[offset], &info, sizeof(CharacterInfo));
            offset += sizeof(CharacterInfo);
        }

        return data;
    }

    Resource* Font::Create(const ResData& data, const PropertyLibrary& props)
    {
        return new Font(data, props);
    }
}


