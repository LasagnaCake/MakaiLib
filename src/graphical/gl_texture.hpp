void setTexture2D(unsigned char index, GLuint texture) {
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, texture);
}

struct ImageData2D {
	unsigned int
		width,
		height,
		type,
		format,
		internalFormat,
		minFilter,
		magFilter
	;
	vector<ubyte> data;
};

struct Image {
	Image() {
		create();
	}

	~Image() {
		destroy();
	}

	inline Image const& bind(unsigned int target = GL_TEXTURE_2D) const {
		glBindTexture(target, id);
		return *this;
	}

	inline Image& destroy() {
		if (!created) return *this;
		glDeleteTextures(1, &id);
		id = 0;
		return *this;
	}

	inline Image& create() {
		if (created) return *this;
		glGenTextures(1, &id);
		return *this;
	}

	inline Image& make() {
		return destroy().make();
	}

	inline static void unbind(unsigned int target = GL_TEXTURE_2D) {
		glBindTexture(target, 0);
	}

	inline operator unsigned int() const {return id;}

	inline unsigned int getID() const {return id;}

private:
	bool created = false;

	unsigned int id = 0;
};

Image* createTexture(
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
	Image* texture = new Image();
	texture->bind(target);
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
	Image::unbind(target);
	// Return texture ID
	return texture;
}

Image* createTexture2D(
	unsigned int width,
	unsigned int height,
	unsigned int type = GL_UNSIGNED_BYTE,
	unsigned int format = GL_RGBA,
	unsigned int internalFormat = GL_RGBA,
	unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR,
	unsigned int magFilter = GL_LINEAR,
	unsigned char* data = NULL
) {
	return createTexture(width, height, type, format, format, minFilter, magFilter, data);
}

namespace {
	unsigned int createCopyBuffer() {
		DEBUGLN("Creating copy buffer...");
		unsigned int id = 0;
		glGenFramebuffers(1, &id);
		return id;
	}
}

void copyTexture(
	Image* src,
	Image* dst,
	unsigned int srcStartX,
	unsigned int srcStartY,
	unsigned int srcEndX,
	unsigned int srcEndY,
	unsigned int dstStartX,
	unsigned int dstStartY,
	unsigned int dstEndX,
	unsigned int dstEndY,
	unsigned int filter = GL_NEAREST
) {
	if (!src || !dst) return;
	static unsigned int const fb = createCopyBuffer();
	DEBUGLN("Binding copy buffer...");
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	DEBUGLN("Binding source...");
	glFramebufferTexture2D(
		GL_READ_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		src->getID(),
		0
	);
	DEBUGLN("Binding destination...");
	glFramebufferTexture2D(
		GL_DRAW_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D,
		dst->getID(),
		0
	);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	DEBUGLN((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE ? "OK" : "ERR"));
	DEBUGLN("Copying textures...");
	glBlitFramebuffer(
		srcStartX, srcStartY, srcEndX, srcEndY,
		dstStartX, dstStartY, dstEndX, dstEndY,
		GL_COLOR_BUFFER_BIT, filter
	);
	DEBUGLN("Finalizing...");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

class Texture2D {
public:
	typedef SmartPointer::StrongPointer<Image> ImageInstance;

	Texture2D() {}

	Texture2D(
		unsigned int width,
		unsigned int height,
		unsigned int type = GL_UNSIGNED_BYTE,
		unsigned int format = GL_RGBA,
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR,
		unsigned char* data = NULL,
		unsigned int internalFormat = 0
	) {
		make(
			width,
			height,
			type,
			format,
			minFilter,
			magFilter,
			data,
			internalFormat
		);
	}

	Texture2D(
		std::string const& path,
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR
	) {
		make(path, minFilter, magFilter);
	}

	Texture2D(
		ImageData2D const& image
	) {
		make(image);
	}

	Texture2D(
		Texture2D const& other
	) {
		image = other.image;
	}

	Texture2D(
		Texture2D* const& other
	) {
		if (other)
			image = other->image;
	}

	Texture2D(
		Texture2D const& other,
		unsigned int startX,
		unsigned int startY,
		unsigned int endX,
		unsigned int endY,
		bool filter = false
	) {
		make(other, startX, startY, endX, endY, filter);
	}

	Texture2D& create(
		unsigned int width,
		unsigned int height,
		unsigned int type			= GL_UNSIGNED_BYTE,
		unsigned int format			= GL_RGBA,
		unsigned int magFilter		= GL_LINEAR,
		unsigned int minFilter		= GL_LINEAR_MIPMAP_LINEAR,
		unsigned char* data			= NULL,
		unsigned int internalFormat	= 0
	) {
		if (exists()) return *this;
		this->width				= width;
		this->height			= height;
		this->format			= format;
		this->type				= type;
		this->minFilter			= minFilter;
		this->magFilter			= magFilter;
		this->internalFormat	= internalFormat ? internalFormat : format;
		image = createTexture2D(
			width,
			height,
			type,
			format,
			this->internalFormat,
			minFilter,
			magFilter,
			data
		);
		setTextureWrapMode();
		return *this;
	}

	Texture2D& create(
		std::string path,
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR
	) {
		if (exists()) return *this;
		int imgWidth, imgHeight;
		int nrChannels;
		FileLoader::BinaryData imgdat = FileLoader::getBinaryFile(path);
		unsigned char* data = stbi_load_from_memory(imgdat.data(), imgdat.size(), &imgWidth, &imgHeight, &nrChannels, 4);
		imgdat.clear();
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
		return *this;
	}

	Texture2D& create(
		ImageData2D const& image
	) {
		if (exists()) return *this;
		DEBUGLN("copying textures...");
		if (image.data.empty()) return *this;
		DEBUGLN("Texture data exists!");
		create(
			image.width,
			image.height,
			image.type,
			image.format,
			image.minFilter,
			image.magFilter,
			(unsigned char*)image.data.data(),
			image.internalFormat
		);
		return *this;
	}

	Texture2D& create(
		Texture2D const& other
	) {
		if (exists()) return *this;
		image			= other.image;
		width			= other.width;
		height			= other.height;
		type			= other.type;
		format			= other.format;
		minFilter		= other.minFilter;
		magFilter		= other.magFilter;
		internalFormat	= other.internalFormat;
		return *this;
	}

	Texture2D& create(
		Texture2D const& other,
		unsigned int startX,
		unsigned int startY,
		unsigned int endX,
		unsigned int endY,
		bool filter = false
	) {
		if (exists()) return *this;
		unsigned int w, h;
		w = Math::max(startX, endX) - Math::min(startX, endX);
		h = Math::max(startY, endY) - Math::min(startY, endY);
		create(
			w, h,
			other.type,
			other.format,
			other.minFilter,
			other.magFilter,
			NULL,
			other.internalFormat
		);
		copyFrom(other, startX, startY, endX, endY, filter);
		return *this;
	}

	Texture2D& destroy() {
		if (!exists()) return *this;
		image.unbind();
		return *this;
	}

	Texture2D& clear() {
		if (!exists()) return *this;
		image->destroy();
		return *this;
	}

	Texture2D& make(
		unsigned int width,
		unsigned int height,
		unsigned int type			= GL_UNSIGNED_BYTE,
		unsigned int format			= GL_RGBA,
		unsigned int magFilter		= GL_LINEAR,
		unsigned int minFilter		= GL_LINEAR_MIPMAP_LINEAR,
		unsigned char* data			= NULL,
		unsigned int internalFormat	= 0
	) {
		destroy();
		create(width, height, type, format, magFilter, minFilter, data, internalFormat);
		return *this;
	}

	Texture2D& make(
		std::string path,
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR
	) {
		destroy();
		create(path, minFilter, magFilter);
		return *this;
	}

	Texture2D& make(
		ImageData2D image
	) {
		destroy();
		create(image);
		return *this;
	}

	Texture2D& make(
		Texture2D const& other
	) {
		destroy();
		create(other);
		return *this;
	}

	Texture2D& make(
		Texture2D const& other,
		unsigned int startX,
		unsigned int startY,
		unsigned int endX,
		unsigned int endY,
		bool filter = false
	) {
		destroy();
		create(other, startX, startY, endX, endY, filter);
		return *this;
	}

	Texture2D& makeUnique(bool filter = false) {
		if (!exists()) return *this;
		Image* newImg = createTexture2D(
			width, height,
			type,
			format,
			internalFormat,
			minFilter,
			magFilter
		);
		copyTexture(
			(Image*)image, (Image*)newImg,
			0, 0, width, height,
			0, 0, width, height,
			filter ? GL_LINEAR : GL_NEAREST
		);
		image = newImg;
		return *this;
	}

	Texture2D& operator=(Texture2D const& other)	{make(other); return *this;								}
	Texture2D& operator=(Texture2D&& other)			{make(other); return *this;								}
	Texture2D& operator=(Texture2D* other)			{if (other) make(*other); else destroy(); return *this;	}

	Texture2D& copyFrom(
		Texture2D const& other,
		unsigned int startX,
		unsigned int startY,
		unsigned int endX,
		unsigned int endY,
		bool filter = false
	) {
		if (!exists()) return *this;
		// Copy data
		copyTexture(
			(Image*)other.image, (Image*)image,
			startX, startY, endX, endY,
			0, 0, width, height,
			filter ? GL_LINEAR : GL_NEAREST
		);
		// Regenerate mipmaps
		glBindTexture(GL_TEXTURE_2D, image->getID());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		return *this;
	}

	Texture2D& copyFrom(
		Texture2D const& other,
		bool filter = false
	) {
		if (!exists()) return *this;
		// Copy data
		copyTexture(
			(Image*)other.image, (Image*)image,
			0, 0, other.width, other.height,
			0, 0, width, height,
			filter ? GL_LINEAR : GL_NEAREST
		);
		// Regenerate mipmaps
		glBindTexture(GL_TEXTURE_2D, image->getID());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		return *this;
	}

	/*
	GL_CLAMP_TO_EDGE
	GL_REPEAT
	GL_MIRRORED_REPEAT
	GL_MIRROR_CLAMP_TO_EDGE
	*/
	Texture2D& setTextureWrapMode(
		unsigned int horizontal,
		unsigned int vertical
	) {
		if (!exists()) return *this;
		glBindTexture(GL_TEXTURE_2D, image->getID());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizontal);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, vertical);
		glBindTexture(GL_TEXTURE_2D, 0);
		return *this;
	}

	/*
	GL_CLAMP_TO_EDGE
	GL_REPEAT
	GL_MIRRORED_REPEAT
	GL_MIRROR_CLAMP_TO_EDGE
	*/
	Texture2D& setTextureWrapMode(unsigned int mode = GL_REPEAT) {
		setTextureWrapMode(mode, mode);
		return *this;
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
	Texture2D& setTextureFilterMode(
		unsigned int magFilter = GL_LINEAR,
		unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR
	) {
		if (!exists()) return *this;
		glBindTexture(GL_TEXTURE_2D, image->getID());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		glBindTexture(GL_TEXTURE_2D, 0);
		this->minFilter = minFilter;
		this->magFilter = magFilter;
		return *this;
	}

	inline unsigned int getTextureMinFilter() {
		return minFilter;
	}

	inline unsigned int getTextureMagFilter() {
		return magFilter;
	}

	Texture2D& operator()(unsigned char texture = 0) {
		enable(texture);
		return *this;
	}

	Texture2D& enable(unsigned char texture = 0) {
		setTexture2D(texture, image->getID());
		return *this;
	}

	inline unsigned int getID() const {
		if (!exists()) return 0;
		return image->getID();
	}

	ImageData2D getData() const {
		DEBUGLN("Getting image data...");
		if (!exists()) return ImageData2D{0,0,0,0,0,0};
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
		DEBUG("Image Size: ", size);
		switch (format) {
			case GL_DEPTH_COMPONENT:
			case GL_RED:							break;
			case GL_DEPTH_STENCIL:
			case GL_RG:					size *= 2;	break;
			case GL_RGB:				size *= 3;	break;
			default:
			case GL_RGBA:				size *= 4;	break;
		}
		DEBUGLN("Image Pixel Width: ", size);
		DEBUGLN("Width: ", width);
		DEBUGLN("Height: ", height);
		DEBUGLN("Image Size: ", ((size_t)width) * ((size_t)height) * size);
		ImageData2D imgdat = {width, height, type, format, internalFormat, minFilter, magFilter};
		DEBUGLN("Reserving buffer...");
		imgdat.data.resize(((size_t)width) * ((size_t)height) * size, 0);
		DEBUGLN("Reserved: ", imgdat.data.size());
		if (imgdat.data.empty()) throw Error::FailedAction("Somehow, the image data is empty.");
		DEBUGLN("Extracting pixels...");
		glBindTexture(GL_TEXTURE_2D, image->getID());
		glGetTexImage(GL_TEXTURE_2D, 0, format, type, imgdat.data.data());
		glBindTexture(GL_TEXTURE_2D, 0);
		DEBUGLN("Done!");
		DEBUGLN("Reserved: ", imgdat.data.size());
		return imgdat;
	}

	Texture2D const& saveToFile(string const& path) const {
		if (!exists()) return *this;
		ImageData2D imgdat = getData();
		uchar channels = 0;
		switch (imgdat.format) {
			case GL_DEPTH_COMPONENT:
			case GL_RED:				channels = 1;	break;
			case GL_DEPTH_STENCIL:
			case GL_RG:					channels = 2;	break;
			case GL_RGB:				channels = 3;	break;
			default:
			case GL_RGBA:				channels = 4;	break;
		}
		int result = stbi_write_tga(
			path.c_str(),
			imgdat.width,
			imgdat.height,
			channels,
			imgdat.data.data()
		);
		if (!result) {
			throw FileLoader::FileSaveError(
				"Could not save image file!",
				__FILE__,
				toString(__LINE__),
				"Texture2D::saveToFile"
			);
		}
		return *this;
	}

	bool exists() const {
		return image.exists() && image->getID();
	}

	inline Texture2D* operator->() {return this;}

	inline operator bool() const {return exists();}

private:
	ImageInstance image;

	unsigned int
		width,
		height,
		format,
		internalFormat,
		type,
		minFilter,
		magFilter
	;
};
