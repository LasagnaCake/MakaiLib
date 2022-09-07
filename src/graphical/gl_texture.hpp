void setTexture2D(unsigned char index, GLuint texture) {
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, texture);
}

unsigned int createTexture2D(
	unsigned int width,
	unsigned int height,
	unsigned int type = GL_UNSIGNED_BYTE,
	unsigned int format = GL_RGBA,
	unsigned int filter = GL_LINEAR,
	unsigned char* data = NULL
) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
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
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
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
		unsigned int filter = GL_LINEAR,
		unsigned char* data = NULL
	) {
		create(
			width,
			height,
			type,
			format,
			filter,
			data
		);
	}

	Texture2D(std::string path) {
		create(path);
	}

	void create(
		unsigned int width,
		unsigned int height,
		unsigned int type = GL_UNSIGNED_BYTE,
		unsigned int format = GL_RGBA,
		unsigned int filter = GL_LINEAR,
		unsigned char* data = NULL
	) {
		if (created) return;
		created = true;
		id = createTexture2D(
			width,
			height,
			type,
			format,
			filter,
			data
		);
	}

	void create(std::string path) {
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			create(
				width,
				height,
				GL_UNSIGNED_BYTE,
				GL_RGBA,
				GL_LINEAR,
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
		setTexture2D(texture, id);
	}
private:
	bool created = false;
	unsigned int id;
};
