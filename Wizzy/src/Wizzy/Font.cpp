#include "pch.h"
#include "Font.h"
// NOT USED IN PROJECT
#include "RenderTarget.h"

#include <GL\glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

Font::Font(const string& file, int32 fontSize, Window* targetWindow)
    : m_textRenderer(Renderer(targetWindow))
{
    FT_Library ft;
	FT_Face face;

    auto _error = FT_Init_FreeType(&ft);

    if (_error != FT_Err_Ok)
    {
        std::cout << "Failed FT init\n";
        abort();
    }

    _error = FT_New_Face(ft, file.c_str(), 0, &face);

    if (_error != FT_Err_Ok)
    {
        std::cout << "Failed loading font face\n";
        abort();
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);

    m_fontSize = fontSize;

    int32 rowWidth = 0;
    int32 colHeight = 0;

    int32 atlasWidth = 0;
    int32 atlasHeight = 0;

    int32 charCount = 0;

    for (int32 i = 0; i < 128; i++)
    {
        _error = FT_Load_Char(face, i, FT_LOAD_DEFAULT);
        if (_error != FT_Err_Ok || i == ' ')  continue;

        atlasWidth += face->glyph->bitmap.width + 2;
        atlasHeight = std::max(atlasHeight, (int)face->glyph->bitmap.rows);

        charCount++;
    }

    m_spaceSize = atlasWidth / charCount;

    m_atlasTexture = new Texture(NULL, atlasWidth, atlasHeight, 1);
    int32 textPos = 0;

    for (int32 i = 0; i < 128; i++)
    {
        _error = FT_Load_Char(face, i, FT_LOAD_RENDER);
        if (_error != FT_Err_Ok)  continue;

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
            m_atlasTexture->AddSubTexture(glyph->bitmap.buffer, glyph->bitmap.width, glyph->bitmap.rows, textPos, 0, 1);
        }

        textPos += glyph->bitmap.width + 2;
    }
}

Font::~Font()
{
    while (m_cachedStrings.size() > 0)
    {
        auto& str = m_cachedStrings.front();

        delete m_cache[str].texture;

        m_cache.erase(str);

        m_cachedStrings.pop();
    }
}

Texture* Font::Render(const string& text, Shader* shader)
{
    if (m_cache.find(text) != m_cache.end() && m_cache[text].texture) 
    { // Text has a cached texture, just use that
        return m_cache[text].texture;
    }
    else
    { // Text has no cached texture, create new texture
        glm::vec2 textureSize = MeasureString(text);
        RenderTarget* newTexture = new RenderTarget(textureSize.x, textureSize.y);

        ValidateCache();

        m_cache[text].texture = newTexture;
        m_cache[text].size = MeasureString(text);
        m_cachedStrings.push(text);

        m_textRenderer.Begin(newTexture);
        std::vector<RenderTarget*> toDelete;
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

            RenderTarget* charTexture = new RenderTarget(info.width, info.height);
            toDelete.push_back(charTexture);

            // Submit & draw character on char texture
            m_textRenderer.Begin(charTexture);
            m_textRenderer.SubmitImage(charTexture, m_atlasTexture, shader, { -(info.xOffset * m_atlasTexture->GetWidth()), 0}, { m_atlasTexture->GetWidth(), m_atlasTexture->GetHeight() }, { 1.f, 1.f, 1.f, 1.f });
            m_textRenderer.End(charTexture, glm::ortho<float>(0, charTexture->GetWidth(), charTexture->GetHeight(), 0.f));

            // Draw the char texture on the final texture
            m_textRenderer.SubmitImage(newTexture, charTexture, shader, { penPos.x + info.bitmapLeft, penPos.y - (info.height - info.bitmapTop) }, { charTexture->GetWidth(), charTexture->GetHeight() }, { 1.f, 1.f, 1.f, 1.f });

            penPos.x += info.advanceX;
        }

        m_textRenderer.End(newTexture, glm::ortho<float>(0, newTexture->GetWidth(), 0.f, newTexture->GetHeight()));

        for (auto r : toDelete)
        {
            delete r;
        }

        return newTexture;
    }
}

glm::vec2 Font::MeasureString(const string& str)
{
    if (m_cache.find(str) != m_cache.end())
    {
        return m_cache[str].size;
    }
    else
    {
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

        delete m_cache[top].texture;

        m_cache.erase(top);

        m_cachedStrings.pop();
    }
}
