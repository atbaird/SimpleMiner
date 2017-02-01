#pragma once
#include <string>
#include <map>
#include "Engine/Math/IntVector2.hpp"

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"

#define STATIC // Do-nothing indicator that method/member is static in class definition

class Texture;

extern STATIC std::map< std::string, Texture* > s_textureRegistry;

class Texture
{
public:
	//constructors
	Texture(); //DO NOT USE THIS FOR CREATING NEW TEXTURES!!! SPECIFICALLY FOR running internal methods.
	Texture(const std::string& imageFilePath);

	//getters
	static Texture* GetTextureByName(const std::string& imageFilePath);
	static Texture* CreateOrGetTexture(const std::string& imageFilePath);

	//variables
	int m_openglTextureID;
	IntVector2 m_texelSize;

	// (0,0) at top left, (1,1) at bottom right.
};