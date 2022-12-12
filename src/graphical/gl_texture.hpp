void setTexture2D(unsigned char index, GLuint texture) {
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, texture);
}

unsigned int createTexture2D(
	unsigned int width,
	unsigned int height,
	unsigned int type = GL_UNSIGNED_BYTE,
	unsigned int format = GL_RGBA,
	unsigned int minFilter = GL_LINEAR,
	unsigned int magFilter = GL_LINEAR,
	unsigned char* data = NULL
) {
	GLuint texture;
	// Create texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Bind image data
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		format,
		width,
		height,
		0,
		format,
		type,
		data
	);
	// Set texture wrapping & mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);
	// Set filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	// Return texture ID
	return texture;
}

class Texture2D {
public:
	Texture2D() {}

	Texture2D(
		unsigned int width,
		unsigned int height,
		unsigned int type = GL_UNSIGNED_BYTE,
		unsigned int format = GL_RGBA,
		unsigned int minFilter = GL_LINEAR,
		unsigned int magFilter = GL_LINEAR,
		unsigned char* data = NULL
	) {
		create(
			width,
			height,
			type,
			format,
			minFilter,
			magFilter,
			data
		);
	}

	Texture2D(std::string path, unsigned int minFilter = GL_LINEAR, unsigned int magFilter = GL_LINEAR) {
		create(path, minFilter, magFilter);
	}

	void create(
		unsigned int width,
		unsigned int height,
		unsigned int type = GL_UNSIGNED_BYTE,
		unsigned int format = GL_RGBA,
		unsigned int minFilter = GL_LINEAR,
		unsigned int magFilter = GL_LINEAR,
		unsigned char* data = NULL
	) {
		if (created) return;
		created = true;
		id = createTexture2D(
			width,
			height,
			type,
			format,
			minFilter,
			magFilter,
			data
		);
	}

	void create(std::string path, unsigned int minFilter = GL_LINEAR, unsigned int magFilter = GL_LINEAR) {
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			create(
				width,
				height,
				GL_UNSIGNED_BYTE,
				GL_RGBA,
				minFilter,
				magFilter,
				data
			);
			stbi_image_free(data);
		} else {
			throw runtime_error(string("Could not load image file '") + path + "'!");
		}
	}

	void destroy() {
		if (!created) return;
		glDeleteTextures(1, &id);
		created = false;
	}

	void operator()(unsigned char texture = 0) {
		enable(texture);
	}

	void enable(unsigned char texture = 0) {
		setTexture2D(texture, id);
	}

private:
	bool created = false;
	unsigned int id;
};
