#include "texture.hpp"
#include"error.hpp"
#include <stb_image.h>
#include<cmath>

void Texture::init(int width, int height, const unsigned char* data, ColorSpace colorSpace) {
	if (width <= 0 || height <= 0) {
		throw Error("Attempted creating texture with zero width or height.");
	}
	if (!data) {
		throw Error("Attempted creating texture with nullptr data.");
	}

	glCreateTextures(GL_TEXTURE_2D, 1, &texID);
	int texLevels = 0;
	//Level of detail parameter calculated according to the formula presented in:
	//Lengyel,E. 2012. "Mathematics for 3D Game Programming and Computer Graphics". Ch.7.5.4 Filtering and Mipmaps. p173
	texLevels = static_cast<int>(std::log2f((float)std::fmax(width, height)) + 1);

	if(colorSpace==ColorSpace::SRGB)
		glTextureStorage2D(texID, texLevels, GL_SRGB8_ALPHA8, width, height);
	else
		glTextureStorage2D(texID, texLevels, GL_RGBA8, width, height);
	glTextureSubImage2D(texID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);//We assume 8-bit 4-channel colours in unsigned byte format
	glGenerateTextureMipmap(texID);
	glTextureParameteri(texID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(texID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(texID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(texID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISTROPY_EXT, 16.f);
}

Texture::Texture(int width, int height, const unsigned char* data, ColorSpace colorSpace) {
	init(width, height, data);
}

Texture::~Texture() {
	glDeleteTextures(1, &texID);
}

void Texture::bindTex(int textureUnit) {
	glBindTextureUnit(textureUnit, texID);
}

TexLoader::TexLoader(int numTexturesHint) {
	textures.reserve(numTexturesHint);
}

TexLoader::~TexLoader() {
	for (int i = 0; i < textures.size(); i++) {
		if (textures[i]) {
			delete textures[i];
		}
	}
}

Texture* TexLoader::loadTexture(const char* filename, ColorSpace colorSpace) {
	if (textures.size() == textures.capacity()) textures.reserve(textures.size() * 2);
	stbi_set_flip_vertically_on_load(true);
	int texWidth = 0;
	int texHeight = 0;
	int texChannels = 0;
	unsigned char* texData = stbi_load(filename, &texWidth, &texHeight, &texChannels, 4);//force 4-channel colours
	if (!texData || texWidth <= 0 || texHeight <= 0) return nullptr;
	Texture* newTex = new Texture(texWidth, texHeight, texData, colorSpace);
	textures.push_back(newTex);
	stbi_image_free(texData);
	return newTex;
}