#ifndef OPENGL_HELPING_HAND_H
#define OPENGL_HELPING_HAND_H

#include <string>

namespace GLHelper {
	/// Loads an image as a texture.
	unsigned int loadTexture2D (std::string path, unsigned int channels);
	/// Generates a buffer.
	unsigned int genBuffer();
	/// Generates a vertex array.
	unsigned int genVertexArray();
}

#endif // OPENGL_HELPING_HAND_H
