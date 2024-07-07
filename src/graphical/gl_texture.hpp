void setTexture2D(unsigned char index, GLuint texture) {
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, texture);
}

struct Image {
	Image() {}

	~Image() {destroy();}

	inline Image const& bind(unsigned int target = GL_TEXTURE_2D) const {
		glBindTexture(target, id);
		return *this;
	}

	inline Image& destroy() {
		if (!exists()) return *this;
		glDeleteTextures(1, &id);
		id = 0;
		return *this;
	}

	inline Image& create() {
		if (exists()) return *this;
		glGenTextures(1, &id);
		return *this;
	}

	Image& make() {return destroy().create();}

	inline static void unbind(unsigned int target = GL_TEXTURE_2D) {
		glBindTexture(target, 0);
	}

	inline Image& use(unsigned char texture = 0) {
		setTexture2D(texture, id);
		return *this;
	}

	inline Image const& use(unsigned char texture = 0) const {
		setTexture2D(texture, id);
		return *this;
	}

	inline Image&		operator()(unsigned char texture = 0)		{return use(texture);}
	inline Image const&	operator()(unsigned char texture = 0) const	{return use(texture);}

	inline bool operator==(Image const& other) const					{return id == other.id;		}
	inline Helper::PartialOrder operator<=>(Image const& other) const	{return id <=> other.id;	}

	inline operator unsigned int() const	{return id;	}
	inline unsigned int getID() const		{return id;	}

	inline bool exists() const		{return id != 0;	}
	inline operator bool() const	{return exists();	}

private:
	unsigned int id = 0;
};

struct Image2D: public Image {
	using Image::create;

	enum class FileType {
		IFT_INVALID = -1,
		IFT_AUTO_DETECT,
		IFT_TGA,
		IFT_PNG,
		IFT_JPG,
		IFT_BMP
	};

	constexpr static FileType fromFileExtension(string const& path) {
		string ext = Helper::toLower(Helper::splitStringAtLast(path, '.').second);
		if (ext == "png")					return FileType::IFT_PNG;
		if (ext == "jpg" || ext == "jpeg")	return FileType::IFT_JPG;
		if (ext == "tga")					return FileType::IFT_TGA;
		if (ext == "bmp")					return FileType::IFT_BMP;
		return FileType::IFT_INVALID;
	}

	struct Attributes {
		unsigned int
			width,
			height,
			type,
			format,
			internalFormat,
			minFilter,
			magFilter
		;
	};

	struct ImageData: Attributes {
		vector<ubyte> data;
	};

	Image2D& create(
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
		if (!internalFormat)
			internalFormat = format;
		newImage(this, width, height, type, format, internalFormat, minFilter, magFilter, data);
		return *this;
	}

	Image2D& make(
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
		return create(width, height, type, format, minFilter, magFilter, data, internalFormat);
	}

	inline Attributes getAttributes() const {return attributes;	}

	ImageData getData() const {
		DEBUGLN("Getting image data...");
		if (!exists()) return ImageData{0,0,0,0,0,0};
		size_t size = 0;
		switch (attributes.type) {
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
		switch (attributes.format) {
			case GL_DEPTH_COMPONENT:
			case GL_RED:							break;
			case GL_DEPTH_STENCIL:
			case GL_RG:					size *= 2;	break;
			case GL_RGB:				size *= 3;	break;
			default:
			case GL_RGBA:				size *= 4;	break;
		}
		DEBUGLN("Image Pixel Width: ", size);
		DEBUGLN("Width: ", attributes.width);
		DEBUGLN("Height: ", attributes.height);
		DEBUGLN("Image Size: ", ((size_t)attributes.width) * ((size_t)attributes.height) * size);
		ImageData imgdat = (ImageData)attributes;
		DEBUGLN("Reserving buffer...");
		imgdat.data.resize(((size_t)attributes.width) * ((size_t)attributes.height) * size, 0);
		DEBUGLN("Reserved: ", imgdat.data.size());
		if (imgdat.data.empty()) throw Error::FailedAction("Somehow, the image data is empty.");
		DEBUGLN("Extracting pixels...");
		glBindTexture(GL_TEXTURE_2D, getID());
		glGetTexImage(GL_TEXTURE_2D, 0, attributes.format, attributes.type, imgdat.data.data());
		glBindTexture(GL_TEXTURE_2D, 0);
		DEBUGLN("Done!");
		DEBUGLN("Reserved: ", imgdat.data.size());
		return imgdat;
	}

	Image2D const& saveToFile(string const& path, uint8 const& quality, FileType type = FileType::IFT_AUTO_DETECT) const {
		saveImageToFile(path, quality, type);
		return *this;
	}

	Image2D& saveToFile(string const& path, uint8 const& quality, FileType type = FileType::IFT_AUTO_DETECT) {
		saveImageToFile(path, quality, type);
		return *this;
	}

	Image2D&		saveToFile(string const& path, FileType type = FileType::IFT_AUTO_DETECT)		{return saveToFile(path, 50, type);}
	Image2D const&	saveToFile(string const& path, FileType type = FileType::IFT_AUTO_DETECT) const	{return saveToFile(path, 50, type);}

	static Image2D* newImage(
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
		Image2D* image = new Image2D();
		return newImage(image, width, height, type, format, format, minFilter, magFilter, data);
	}

	static Image2D* newImage(
		Image2D* image,
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
		image->create();
		image->bind(target);
		image->attributes = {
			width,
			height,
			type,
			format,
			internalFormat,
			minFilter,
			magFilter
		};
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
		// Set image wrapping & mipmaps
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(target);
		// Set filtering
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
		// Unbind image
		Image::unbind(target);
		// Return image
		return image;
	}

private:
	void saveImageToFile(string const& path, uint8 const& quality, FileType type) const {
		if (!exists()) return;
		ImageData imgdat = getData();
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
		int result;
		if (type == FileType::IFT_AUTO_DETECT) type = fromFileExtension(path);
		if (type == FileType::IFT_INVALID)
			throw Error::InvalidValue(
				"Invalid file type of '." + Helper::splitStringAtLast(path, '.').second + "'!"
				__FILE__,
				toString(__LINE__),
				"Image2D::saveToFile"
			);
		#define IMAGE2D_STBIWRITE_PARAMS path.c_str(), imgdat.width, imgdat.height, channels, imgdat.data.data()
		switch (type) {
			default:
			case FileType::IFT_TGA:	result = stbi_write_tga(IMAGE2D_STBIWRITE_PARAMS);			break;
			case FileType::IFT_PNG:	result = stbi_write_png(IMAGE2D_STBIWRITE_PARAMS, 0);		break;
			case FileType::IFT_JPG:	result = stbi_write_jpg(IMAGE2D_STBIWRITE_PARAMS, quality);	break;
			case FileType::IFT_BMP:	result = stbi_write_bmp(IMAGE2D_STBIWRITE_PARAMS);			break;
		}
		#undef IMAGE2D_STBIWRITE_PARAMS
		if (!result) {
			throw FileLoader::FileSaveError(
				"Could not save image file!",
				__FILE__,
				toString(__LINE__),
				"Image2D::saveToFile"
			);
		}
		return;
	}

	friend class Texture2D;

	Attributes attributes;
};

Image2D* createTexture2D(
	Image2D* texture,
	unsigned int width,
	unsigned int height,
	unsigned int type = GL_UNSIGNED_BYTE,
	unsigned int format = GL_RGBA,
	unsigned int internalFormat = GL_RGBA,
	unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR,
	unsigned int magFilter = GL_LINEAR,
	unsigned char* data = NULL
) {
	return Image2D::newImage(texture, width, height, type, format, format, minFilter, magFilter, data);
}

Image2D* createTexture2D(
	unsigned int width,
	unsigned int height,
	unsigned int type = GL_UNSIGNED_BYTE,
	unsigned int format = GL_RGBA,
	unsigned int internalFormat = GL_RGBA,
	unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR,
	unsigned int magFilter = GL_LINEAR,
	unsigned char* data = NULL
) {
	return Image2D::newImage(width, height, type, format, format, minFilter, magFilter, data);
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
	Image2D* src,
	Image2D* dst,
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
	typedef Instance<Image2D> Image2DInstance;

	typedef Image2D::FileType ImageFileType;

	Texture2D() {image = new Image2D();}

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
		Image2D::ImageData const& image
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
		if (!image) return *this;
		else if (image && !image->exists())
			createTexture2D(
				(Image2D*)image,
				width,
				height,
				type,
				format,
				internalFormat ? internalFormat : format,
				minFilter,
				magFilter,
				data
			);
		else
			image = createTexture2D(
				width,
				height,
				type,
				format,
				internalFormat ? internalFormat : format,
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
		Image2D::ImageData const& image
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
		image = other.image;
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
			other.image->attributes.type,
			other.image->attributes.format,
			other.image->attributes.minFilter,
			other.image->attributes.magFilter,
			NULL,
			other.image->attributes.internalFormat
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
		Image2D::ImageData const& image
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
		Image2D* newImg = createTexture2D(
			image->attributes.width, image->attributes.height,
			image->attributes.type,
			image->attributes.format,
			image->attributes.internalFormat,
			image->attributes.minFilter,
			image->attributes.magFilter
		);
		copyTexture(
			(Image2D*)image, (Image2D*)newImg,
			0, 0, image->attributes.width, image->attributes.height,
			0, 0, image->attributes.width, image->attributes.height,
			filter ? GL_LINEAR : GL_NEAREST
		);
		image = newImg;
		return *this;
	}

	Texture2D& operator=(Texture2D const& other)	{make(other); return *this;								}
	Texture2D& operator=(Texture2D&& other)			{make(other); return *this;								}
	Texture2D& operator=(Texture2D* other)			{if (other) make(*other); else destroy(); return *this;	}

	inline bool operator==(Texture2D const& other) const					{return *image == *other.image;		}
	inline Helper::PartialOrder operator<=>(Texture2D const& other) const	{return *image <=> *other.image;	}

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
			(Image2D*)other.image, (Image2D*)image,
			startX, startY, endX, endY,
			0, 0, image->attributes.width, image->attributes.height,
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
			(Image2D*)other.image, (Image2D*)image,
			0, 0, other.image->attributes.width, other.image->attributes.height,
			0, 0, image->attributes.width, image->attributes.height,
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
		image->attributes.minFilter = minFilter;
		image->attributes.magFilter = magFilter;
		return *this;
	}

	inline unsigned int getTextureMinFilter() const {
		return image->attributes.minFilter;
	}

	inline unsigned int getTextureMagFilter() const {
		return image->attributes.magFilter;
	}

	inline Image2D::Attributes getAttributes() const {
		return image->attributes;
	}

	Texture2D& enable(unsigned char texture = 0) {
		image->use(texture);
		return *this;
	}

	Texture2D const& enable(unsigned char texture = 0) const {
		image->use(texture);
		return *this;
	}

	Texture2D&			operator()(unsigned char texture = 0)		{return enable(texture);	}
	Texture2D const&	operator()(unsigned char texture = 0) const	{return enable(texture);	}

	inline unsigned int getID() const {
		if (!exists()) return 0;
		return image->getID();
	}

	Image2D::ImageData getData() const {
		return image->getData();
	}

	Texture2D const& saveToFile(
		string const& path,
		uint8 const& quality,
		ImageFileType const& type = ImageFileType::IFT_AUTO_DETECT
	) const {
		if (!exists()) return *this;
		image->saveToFile(path, quality, type);
		return *this;
	}

	Texture2D& saveToFile(
		string const& path,
		uint8 const& quality,
		ImageFileType const& type = ImageFileType::IFT_AUTO_DETECT
	) {
		if (!exists()) return *this;
		image->saveToFile(path, quality, type);
		return *this;
	}

	Texture2D const& saveToFile(
		string const& path,
		ImageFileType const& type = ImageFileType::IFT_AUTO_DETECT
	) const {
		return saveToFile(path, 50, type);
	}

	Texture2D& saveToFile(
		string const& path,
		ImageFileType const& type = ImageFileType::IFT_AUTO_DETECT
	) {
		return saveToFile(path, 50, type);
	}

	bool exists() const {
		return image.exists() && image->exists();
	}

	inline Texture2D* operator->()	{return this;	}
	inline Texture2D& operator*()	{return *this;	}

	inline operator bool() const {return exists();}

private:
	Image2DInstance image;
};
