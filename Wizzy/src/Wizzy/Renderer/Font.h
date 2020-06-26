#pragma once

#include "Wizzy/Resource/Resource.h"
#include "Wizzy/Renderer/Shader.h"
#include "Wizzy/Renderer/Texture.h"
#include "Wizzy/Renderer/RenderTarget.h"

#define DEFAULT_FONT_CACHE_BUDGET	_MB(20)

namespace Wizzy
{

	class Font : public Resource
	{
	private:
		struct CharacterInfo
		{
			fvec2 	advance;
			fvec2 	size;
			fvec2 	bearing;
			float 	xPos;
		};

		struct Cache
		{
			RenderTarget::Handle hTexture;
			fvec2 textSize;
			size_t sizeBytes = 0;
		};

	public:
		__HANDLE_DEF;
		Font(const ResData& data, const PropertyTable& props);
		~Font();

		// Render the text from the atlas to a texture
		const RenderTarget::Handle& Render(const string& text, const Shader::Handle& shader);

		fvec2 MeasureString(const string& str);

		inline Texture::Handle GetAtlasTexture() const { return m_hAtlasTexture; }

		inline int32 GetFontSize() const { return m_fontSize; }

		virtual ResData Serialize() const override;

		inline const CharacterInfo& CharInfo(char c) const
		{
			return m_characterInfo.find(c) == m_characterInfo.end() 
							? m_characterInfo.at('?')
							: m_characterInfo.at(c);
		}

		// 0.0 - 1.0
		inline f32 GetBudgetUsage() const { return m_cachedBytes / m_cacheBudget; }

		inline bool IsCached(const string& str) const 
		{
			return m_cache.find(str) != m_cache.end();
		}

		inline u32 GetNumBudgetOverflows() const { return m_budgetOverflows; }

		// Size in bytes
		inline u64 GetCacheBudget() const { return m_cacheBudget; }
		inline void SetCacheBudget(const u64& bytes) { m_cacheBudget = bytes; }

	private:
		void ValidateCache();

		void InitFT(const ResData& data, const PropertyTable& props);
		void InitWZ(const ResData& data, const PropertyTable& props);

	public:
		inline static bool IsFileBinary() { return true; }

	private:
		Texture::Handle m_hAtlasTexture;
		// TODO: Use something with less memory fragmentation (#OPTIMIZE)
		// Note Render() needs .find()
		std::unordered_map<int32, CharacterInfo> m_characterInfo; 
		std::unordered_map<string, Cache> m_cache;
		std::queue<string> m_cachedStrings;
		size_t m_cachedBytes = 0;
		u32 m_fontSize;
		u64 m_cacheBudget;
		u32 m_budgetOverflows = 0;

		const string TOKEN_HEADER =		"FONT";
		const string TOKEN_ATLAS =		"ATLAS";
		const string TOKEN_INFO =		"INFO";
		const string TOKEN_FONTSIZE = 	"FONTSIZE";


	public:
		static Resource* Create(const ResData& data, const PropertyTable& props);
		inline static const PropertyTable& GetTemplateProps() 
		{ 
			static PropertyTable pl;
			pl.Set<int32>("fontSize", 36);
			pl.Set<int32>("budget", DEFAULT_FONT_CACHE_BUDGET);
			return pl;
		}
	};


}