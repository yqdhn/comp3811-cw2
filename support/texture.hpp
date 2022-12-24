#pragma once
#include<glad.h>
#include<vector>

enum class ColorSpace { LINEAR, SRGB };

/*
* A texture class acting as an OpenGL texture wrapper.
*/
class Texture {
private:
	GLuint texID;
	void init(int width, int height, const unsigned char* data, ColorSpace colorSpace = ColorSpace::SRGB);
public:
	Texture(int width, int height, const unsigned char* data, ColorSpace colorSpace = ColorSpace::SRGB);
	~Texture();

	//Binds the texture to a given texture unit.
	void bindTex(int textureUnit);
};

/*
* A helper class for loading textures from files. The loader also stores the textures, so they can be automatically deleted when the object goes
* out of scope.
*/
struct TexLoader {
private:
	//A self-expanding array that doubles its size when capacity is full reached.
	std::vector<Texture*> textures;
public:
	//Input:
	// - numTexturesHint: expected number of textures to be loaded, so early allocation can be done.
	TexLoader(int numTexturesHint = 5);

	//Destroy all associated textures.
	~TexLoader();

	//Returns a pointer to the loaded texture on success or nullptr on failure.
	//NOTE: DO NOT call delete on returned pointer. The texture will be automatically deleted when the loader goes out of scope.
	Texture* loadTexture(const char* filename, ColorSpace colorSpace = ColorSpace::SRGB);
};
