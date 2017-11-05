#include "Texture.h"

GLuint loadTextureFromFile(const std::string& filename)
{
	GLuint textureID;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	SDL_Surface * surface = IMG_Load(filename.c_str());
	if (surface == nullptr)
	{
		printf("Could not load file %s", IMG_GetError());
		return 0;
	}


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
	

	SDL_FreeSurface(surface);

	return textureID;
}