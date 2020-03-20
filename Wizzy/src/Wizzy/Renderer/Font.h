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
			RenderTarget::Handle hTexture;
			vec2 size;
		};

		const u32 MAX_CACHED_STRINGS = 100;

	public:
		__HANDLE_DEF;
		Font(const ResData& data, const PropertyLibrary& props);
		~Font();

		// Render the text from the atlas to a texture
		RenderTarget::Handle Render(const string& text, Shader::Handle shader);

		glm::vec2 MeasureString(const string& str);

		inline Texture::Handle GetAtlasTexture() const { return m_hAtlasTexture; }

		virtual ResData Serialize() const override;

	private:
		void ValidateCache();

		void InitFT(const ResData& data, const PropertyLibrary& props);
		void InitWZ(const ResData& data, const PropertyLibrary& props);

	private:
		Texture::Handle m_hAtlasTexture;
		std::unordered_map<int32, CharacterInfo> m_characterInfo;
		std::unordered_map<string, Cache> m_cache;
		std::queue<string> m_cachedStrings;
		u32 m_fontSize;
		float m_spaceSize;

		const string TOKEN_HEADER =		"FONT";
		const string TOKEN_ATLAS =		"ATLAS";
		const string TOKEN_INFO =		"INFO";
		const string TOKEN_FONTSIZE =	"FONTSIZE";

	public:
		static Resource* Create(const ResData& data, const PropertyLibrary& props);
		inline static const PropertyLibrary& GetTemplateProps() 
		{ 
			static PropertyLibrary pl;
			pl.SetProperty<int32>("Font size", 100);
			return pl;
		}
	};


}