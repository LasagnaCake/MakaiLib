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
		GL_RGBA,
		width,
		height,
		0,
		GL_RGBA,
		type,
		data
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	return texture;
}

class Texture2D {
public:
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
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			width,
			height,
			0,
			GL_RGBA,
			type,
			data
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	}
	void destroy() {
		if (!created) return;
		glDeleteTextures(1, &id);
		created = false;
	}
	void operator()(unsigned char texture) {
		setTexture2D(texture, id);
	}
private:
	bool created = false;
	unsigned int id;
};
