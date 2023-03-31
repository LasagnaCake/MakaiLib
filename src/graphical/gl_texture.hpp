void setTexture2D(unsigned char index, GLuint texture) {
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, texture);
}

unsigned int createTexture(
	unsigned int width,
	unsigned int height,
	unsigned int type = GL_UNSIGNED_BYTE,
	unsigned int format = GL_RGBA,
	unsigned int internalFormat = GL_RGBA32F,
	unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR,
	unsigned int magFilter = GL_LINEAR,
	unsigned char* data = NULL,
	unsigned int target = GL_TEXTURE_2D
) {
	GLuint texture;
	// Create texture
	glGenTextures(1, &texture);
	glBindTexture(target, texture);
	// Bind image data
	glTexImage2D(
		target,
		0,
		internalFormat,
		width,
		height,
		0,
		format,
		type,
		data
	);
	// Set texture wrapping & mipmaps
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(target);
	// Set filtering
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
	// Unbind texture
	glBindTexture(target, 0);
	// Return texture ID
	return texture;
}

unsigned int createMultisampleTexture(
	unsigned int width,
	unsigned int height,
	unsigned int samples,
	unsigned int internalFormat = GL_RGBA32F,
	unsigned int minFilter = GL_LINEAR,
	unsigned int magFilter = GL_LINEAR,
	unsigned int target = GL_TEXTURE_2D_MULTISAMPLE
) {
	GLuint texture;
	// Create texture
	glGenTextures(1, &texture);
	glBindTexture(target, texture);
	// Bind image data
	glTexImage2DMultisample(
		target,
		samples,
		internalFormat,
		width,
		height,
		GL_TRUE
	);

	// Set texture wrapping & mipmaps
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(target);
	// Set filtering
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
	// Unbind texture
	glBindTexture(target, 0);
	// Return texture ID
	return texture;
}

unsigned int createTexture2D(
	unsigned int width,
	unsigned int height,
	unsigned int type = GL_UNSIGNED_BYTE,
	unsigned int format = GL_RGBA,
	unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR,
	unsigned int magFilter = GL_LINEAR,
	unsigned char* data = NULL
) {
	return createTexture(width, height, type, format, format, minFilter, magFilter, data);
}

class Texture2D {
public:
	Texture2D() {}

	Texture2D(
		unsigned int width,
		unsigned int height,
		unsigned int type = GL_UNSIGNED_BYTE,
		unsigned int format = GL_RGBA,
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR,
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

	Texture2D(
		std::string path,
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR
	) {
		create(path, minFilter, magFilter);
	}

	void create(
		unsigned int width,
		unsigned int height,
		unsigned int type = GL_UNSIGNED_BYTE,
		unsigned int format = GL_RGBA,
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR,
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

	void create(
		std::string path,
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR
	) {
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

	inline unsigned int getID() {
		return id;
	}

	bool exists() {
		return created;
	}

private:
	bool created = false;
	unsigned int id = 0;
};

class MultisampleTexture2D {
public:
	MultisampleTexture2D() {}

	MultisampleTexture2D(
		unsigned int width,
		unsigned int height,
		unsigned int samples = 4,
		unsigned int format = GL_RGBA,
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR
	) {
		create(
			width,
			height,
			samples,
			format,
			minFilter,
			magFilter
		);
	}

	void create(
		unsigned int width,
		unsigned int height,
		unsigned int samples = 4,
		unsigned int format = GL_RGBA,
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR
	) {
		if (created) return;
		created = true;
		id = createMultisampleTexture(
			width,
			height,
			samples,
			format,
			minFilter,
			magFilter
		);
	}

	void destroy() {
		if (!created) return;
		glDeleteTextures(1, &id);
		created = false;
	}

	inline unsigned int getID() {
		return id;
	}

	bool exists() {
		return created;
	}

private:
	bool created = false;
	unsigned int id = 0;
};
