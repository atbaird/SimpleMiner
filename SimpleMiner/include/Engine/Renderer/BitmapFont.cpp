#include "Engine/Renderer/BitmapFont.hpp"
#include <cstdlib>

std::map< std::string, BitmapFont* > BitmapFont::s_fontRegistry;

BitmapFont::BitmapFont()
: m_fontName("")
{

}

BitmapFont::BitmapFont(const std::string& bitmapFontName)
:m_fontName(bitmapFontName),
m_spriteSheet(bitmapFontName, 16, 16)
{
}

BitmapFont* BitmapFont::CreateOrGetFont(const std::string& bitmapFontName) 
{
	std::string lowerCaseName = bitmapFontName;
	auto foundIter = s_fontRegistry.find(lowerCaseName);
	if (foundIter != s_fontRegistry.end())
		return nullptr;//foundIter->seconds;

	//Create and register it
	std::string glyphsTexturePath = Stringf("Data/Fonts/%s.png", bitmapFontName.c_str());
	BitmapFont* glyphsTexture = new BitmapFont(glyphsTexturePath);
	if (glyphsTexture != nullptr)
	{
		s_fontRegistry.insert(std::pair<std::string, BitmapFont* > (bitmapFontName, new BitmapFont(bitmapFontName)));
		return glyphsTexture;
	}


	return nullptr;
}

const Texture* BitmapFont::GetTexture() const
{
	return m_spriteSheet.getSpriteSheetTexture();
}
const AABB2 BitmapFont::GetTexCoordsForGlyph(int glyphUnicode) const
{
	
	return m_spriteSheet.GetTexCoordsForSpriteIndex(glyphUnicode);
}


const std::string BitmapFont::GetAsLowerCase(const std::string& text) const
{
	return text;
}