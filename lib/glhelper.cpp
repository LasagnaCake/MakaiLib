#include <glad/glad.h>
#include "glhelper.hpp"

#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <stdexcept>

unsigned int GLHelper::loadTexture2D(std::string path, unsigned int channels) {
	// Generate and bind OpenGL texture
	unsigned int id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	// Load image
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	// If image loaded, copy to texture
	if (data)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, channels, GL_UNSIGNED_BYTE, data);
	// Else throw error
	else throw std::runtime_error("Failed to load texture");
	// Generate mipmap
	glGenerateMipmap(GL_TEXTURE_2D);
	// Unload image
	stbi_image_free(data);
	// Return image ID
	return id;
}

unsigned int GLHelper::genBuffer() {
	unsigned int id;
	glGenBuffers(1, &id);
	return id;
}

unsigned int GLHelper::genVertexArray() {
	unsigned int id;
	glGenVertexArrays(1, &id);
	return id;
}
