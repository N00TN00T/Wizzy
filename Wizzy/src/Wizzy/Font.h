#pragma once
// NOT USED IN PROJECT
#include "Renderer.h"
#include "Texture.h"
#include "Window.h"

class Font
{
private:
	struct CharacterInfo
	{
		float advanceX;
		float advanceY;

		float width;
		float height;

		float bitmapLeft;
		float bitmapTop;

		float xOffset;
	};

	struct Cache
	{
		Texture* texture;
		glm::vec2 size;
	};

	const u32 MAX_CACHED_STRINGS = 100;

public:
	Font(const string& file, int32 fontSize, Window* targetWindow);
	~Font();

	// Render the text from the atlas to a texture
	Texture* Render(const string& text, Shader* shader);

	glm::vec2 MeasureString(const string& str);

	inline Texture* GetAtlasTexture() const { return m_atlasTexture; }

private:
	void ValidateCache();

private:
	Texture* m_atlasTexture;
	std::unordered_map<int32, CharacterInfo> m_characterInfo;
	std::unordered_map<string, Cache> m_cache;
	std::queue<string> m_cachedStrings;
	u32 m_fontSize;
	Renderer m_textRenderer;
	float m_spaceSize;
};

