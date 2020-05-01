#pragma once

#include "Wizzy/Resource/Resource.h"
#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/Renderer/RenderTarget.h"

namespace Wizzy
{

	class Font : public Resource
	{
	private:
		struct CharacterInfo
		{
			vec2 	advance;
			vec2 	size;
			vec2 	bearing;
			float 	xPos;
		};

		struct Cache
		{
			RenderTarget::Handle hTexture;
			vec2 textSize;
			size_t sizeBytes = 0;
		};

		const u32 CACHE_BUDGET = _MB(100);

	public:
		__HANDLE_DEF;
		Font(const ResData& data, const PropertyTable& props);
		~Font();

		// Render the text from the atlas to a texture
		const RenderTarget::Handle& Render(const string& text, const Shader::Handle& shader);

		glm::vec2 MeasureString(const string& str);

		inline Texture::Handle GetAtlasTexture() const { return m_hAtlasTexture; }

		virtual ResData Serialize() const override;

		inline const CharacterInfo& CharInfo(char c) const
		{
			return m_characterInfo.at(c);
		}

		inline static bool IsFileBinary() { return true; }

	private:
		void ValidateCache();

		void InitFT(const ResData& data, const PropertyTable& props);
		void InitWZ(const ResData& data, const PropertyTable& props);

	private:
		Texture::Handle m_hAtlasTexture;
		// TODO: Use something with less memory fragmentation (#OPTIMIZE)
		// Note Render() needs .find()
		std::unordered_map<int32, CharacterInfo> m_characterInfo; 
		std::unordered_map<string, Cache> m_cache;
		std::queue<string> m_cachedStrings;
		size_t m_cachedBytes = 0;
		u32 m_fontSize;
		float m_spaceSize;

		const string TOKEN_HEADER =		"FONT";
		const string TOKEN_ATLAS =		"ATLAS";
		const string TOKEN_INFO =		"INFO";
		const string TOKEN_FONTSIZE = "FONTSIZE";


	public:
		static Resource* Create(const ResData& data, const PropertyTable& props);
		inline static const PropertyTable& GetTemplateProps() 
		{ 
			static PropertyTable pl;
			pl.Set<int32>("Font size", 36);
			return pl;
		}
	};


}