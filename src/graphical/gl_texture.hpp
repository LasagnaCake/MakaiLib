void setTexture2D(unsigned char index, GLuint texture) {
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, texture);
}

struct ImageData2D {
	unsigned int	width, height, type, format;
	vector<char>	data;
};

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

	Texture2D(
		ImageData2D image,
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR
	) {
		create(image, minFilter, magFilter);
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
		this->width		= width;
		this->height	= height;
		this->format	= format;
		this->type		= type;
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
		int imgWidth, imgHeight;
		int nrChannels;
		unsigned char* data = stbi_load(path.c_str(), &imgWidth, &imgHeight, &nrChannels, 4);
		if (data) {
			create(
				imgWidth,
				imgHeight,
				GL_UNSIGNED_BYTE,
				GL_RGBA,
				minFilter,
				magFilter,
				data
			);
			stbi_image_free(data);
		} else {
			throw Error::FailedAction(string("Could not load image file '") + path + "'!\n\n" + stbi_failure_reason());
		}
	}

	void create(
		ImageData2D image,
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR
	) {
		if (image.data.empty()) return;
		create(
			image.width,
			image.height,
			image.type,
			image.format,
			minFilter,
			magFilter,
			image.data.data()
		)
	}

	void destroy() {
		if (!created) return;
		glDeleteTextures(1, &id);
		created = false;
	}

	/*
	GL_CLAMP_TO_EDGE
	GL_REPEAT
	GL_MIRRORED_REPEAT
	GL_MIRROR_CLAMP_TO_EDGE
	*/
	void setTextureWrapMode(
		unsigned int horizontal,
		unsigned int vertical
	) {
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizontal);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, vertical);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	/*
	GL_CLAMP_TO_EDGE
	GL_REPEAT
	GL_MIRRORED_REPEAT
	GL_MIRROR_CLAMP_TO_EDGE
	*/
	void setTextureWrapMode(unsigned int mode = GL_REPEAT) {
		setTextureWrapMode(mode, mode);
	}

	/*
	MagFilter:
		GL_NEAREST
		GL_LINEAR

	MinFilter:
		GL_NEAREST
		GL_LINEAR
		GL_NEAREST_MIPMAP_NEAREST
		GL_LINEAR_MIPMAP_NEAREST
		GL_NEAREST_MIPMAP_LINEAR
		GL_LINEAR_MIPMAP_LINEAR
	*/
	void setTextureFilterMode(
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR
	) {
		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		glBindTexture(GL_TEXTURE_2D, 0);
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

	ImageData2D getData() {
		if (!created) return ImageData2D{0,0,0,0};
		size_t size = 0;
		switch (type) {
			default:
			case GL_UNSIGNED_BYTE:
			case GL_BYTE:			size = 1;	break;
			case GL_UNSIGNED_SHORT:
			case GL_SHORT:
			case GL_HALF_FLOAT:		size = 2;	break;
			case GL_UNSIGNED_INT:
			case GL_INT:
			case GL_FLOAT:			size = 4;	break;
		}
		switch (format) {
			case GL_DEPTH_COMPONENT:
			case GL_RED:							break;
			case GL_DEPTH_STENCIL:
			case GL_RG:					size *= 2;	break;
			case GL_RGB:				size *= 3;	break;
			default:
			case GL_RGBA:				size *= 4;	break;
		}
		ImageData2D imgdat = {width, height, type, format};
		imgdat.data.reserve(width * height * size);
		enable();
		glBindTexture(GL_TEXTURE_2D, id);
		glGetTexImage(GL_TEXTURE_2D, 0, format, type, imgdat.data.data());
		glBindTexture(GL_TEXTURE_2D, 0);
		return imgdat;
	}

	bool exists() {
		return created;
	}

private:
	bool created = false;
	unsigned int id = 0;
	unsigned int width, height, format, type;
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

	/*
	GL_CLAMP_TO_EDGE
	GL_REPEAT
	GL_MIRRORED_REPEAT
	GL_MIRROR_CLAMP_TO_EDGE
	*/
	void setTextureWrapMode(
		unsigned int horizontal,
		unsigned int vertical
	) {
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, horizontal);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, vertical);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}

	/*
	GL_CLAMP_TO_EDGE
	GL_REPEAT
	GL_MIRRORED_REPEAT
	GL_MIRROR_CLAMP_TO_EDGE
	*/
	void setTextureWrapMode(unsigned int mode = GL_REPEAT) {
		setTextureWrapMode(mode, mode);
	}

	/*
	MagFilter:
		GL_NEAREST
		GL_LINEAR

	MinFilter:
		GL_NEAREST
		GL_LINEAR
		GL_NEAREST_MIPMAP_NEAREST
		GL_LINEAR_MIPMAP_NEAREST
		GL_NEAREST_MIPMAP_LINEAR
		GL_LINEAR_MIPMAP_LINEAR
	*/
	void setTextureFilterMode(
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR
	) {
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, magFilter);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
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
