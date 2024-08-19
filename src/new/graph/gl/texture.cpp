#define GLEW_STATIC
#include <GLEW/include/GL/glew.h>
#include <GLEW/include/GL/wglew.h>
#include <GL/gl.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

#include "texture.hpp"

using Texture2D::WrapMode, Image2D::FilterMode, Image2D::ImageFileType;

constexpr uint convert(WrapMode const& mode) {
	switch (mode) {
		case WrapMode::WM_REPEAT:		return GL_REPEAT;
		case WrapMode::WM_CLAMP:		return GL_CLAMP_TO_EDGE;
		case WrapMode::WM_MIRROR:		return GL_MIRRORED_REPEAT;
		case WrapMode::WM_MIRROR_CLAMP:	return GL_MIRROR_CLAMP_TO_EDGE;
	}
}

constexpr uint convert(FilterMode const& type) {
	switch (type) {
		case FilterMode::FM_NEAREST:	return GL_NEAREST;
		case FilterMode::FM_SMOOTH:		return GL_LINEAR;
		case FilterMode::FM_NMN:		return GL_NEAREST_MIPMAP_NEAREST;
		case FilterMode::FM_NMS:		return GL_NEAREST_MIPMAP_LINEAR;
		case FilterMode::FM_SMN:		return GL_LINEAR_MIPMAP_NEAREST;
		case FilterMode::FM_SMS:		return GL_LINEAR_MIPMAP_LINEAR;
	}
}

uint createCopyBuffer() {
	DEBUGLN("Creating copy buffer...");
	unsigned int id = 0;
	glGenFramebuffers(1, &id);
	return id;
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
	ImageFilter filter = ImageFilter::IF_NEAREST
) {
	if (!src || !dst) return;
	static uint const fb = createCopyBuffer();
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

static Texture2D Texture2D::fromJSON(JSON::JSONData const& img, String const& sourcepath = "") {
	Texture2D texture;
	if (img["data"].is_object() && img["data"]["path"].is_string() && !img["data"]["path"].get<String>().empty()) {
		texture.create(FileSystem::concatenatePath(sourcepath, img["path"].get<String>()));
		texture.setTextureFilterMode(
			img.value<uint>("minFilter", GL_NEAREST_MIPMAP_NEAREST),
			img.value<uint>("magFilter", GL_NEAREST)
		);
	} else if (img["data"].is_string() && !img["data"].get<String>().empty()) {
		List<ubyte> data = decodeData(img["data"].get<String>(), img["encoding"]);
		int w, h, nc;
		uchar* imgdat = stbi_load_from_memory(
			data.data(),
			data.size(),
			&w,
			&h,
			&nc,
			4
		);
		if (imgdat) {
			texture.create(
				w,
				h,
				GL_UNSIGNED_BYTE,
				GL_RGBA,
				img.value<uint>("minFilter", GL_NEAREST_MIPMAP_NEAREST),
				img.value<uint>("magFilter", GL_NEAREST),
				imgdat
			);
			stbi_image_free(imgdat);
		} else throw Error::FailedAction(
			"Failed at getting image effect!",
			__FILE__,
			toString(__LINE__),
			"loadImageEffect",
			"Could not decode embedded image data!",
			"Please check to see if values are correct!"
		);
	}
	return texture;
}

Texture2D::Texture2D(
	uint			width,
	uint			height,
	ComponentType	type			= ComponentType::CT_UBYTE,
	ImageFormat		format			= ImageFormat::IF_RGBA,
	FilterMode		magFilter		= FilterMode::FM_SMOOTH,
	FilterMode		minFilter		= FilterMode::FM_SMS,
	uchar*			data			= NULL,
	uint			internalFormat	= 0,
): Texture2D::Texture2D() {
	create(
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

Texture2D::Texture2D(
	String const& path,
	FilterMode magFilter = FilterMode::FM_SMOOTH,
	FilterMode minFilter = FilterMode::FM_SMS
): Texture2D::Texture2D() {
	create(path, minFilter, magFilter);
}

Texture2D::Texture2D(
	Image2D::ImageData const& image
): Texture2D::Texture2D() {
	create(image);
}

Texture2D::Texture2D(
	Texture2D const& other
) {
	image = other.image;
}

Texture2D::Texture2D(
	Texture2D* const& other
) {
	if (other)
		image = other->image;
}

Texture2D::Texture2D(
	Texture2D const& other,
	unsigned int startX,
	unsigned int startY,
	unsigned int endX,
	unsigned int endY,
	bool filter = false
): Texture2D::Texture2D() {
	create(other, startX, startY, endX, endY, filter);
}

Texture2D& Texture2D::create(
	uint			width,
	uint			height,
	ComponentType	type			= ComponentType::CT_UBYTE,
	ImageFormat		format			= ImageFormat::IF_RGBA,
	FilterMode		magFilter		= FilterMode::FM_SMOOTH,
	FilterMode		minFilter		= FilterMode::FM_SMS,
	uchar*			data			= NULL,
	uint			internalFormat	= 0,
) {
	if (exists()) return *this;
	if (!image)
		image = Image2D::newImage(
			width,
			height,
			type,
			format,
			internalFormat ? internalFormat : format,
			minFilter,
			magFilter,
			data
		);
	else if (image && !image->exists())
		Image2D::newImage(
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
	setWrapMode();
	return *this;
}

Texture2D& Texture2D::create(
	String const& path,
	FilterMode magFilter = FilterMode::FM_SMOOTH,
	FilterMode minFilter = FilterMode::FM_SMS
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
			ComponentType::CT_UBYTE,
			ImageFormat::IF_RGBA,
			minFilter,
			magFilter,
			data
		);
		stbi_image_free(data);
	} else {
		throw Error::FailedAction(String("Could not load image file '") + path + "'!\n\n" + stbi_failure_reason());
	}
	return *this;
}

Texture2D& Texture2D::create(
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

Texture2D& Texture2D::create(
	Texture2D const& other
) {
	if (exists()) return *this;
	image = other.image;
	return *this;
}

Texture2D& Texture2D::create(
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

Texture2D& Texture2D::destroy() {
	if (!exists()) return *this;
	image.unbind();
	return *this;
}

Texture2D& Texture2D::clear() {
	if (!exists()) return *this;
	image->destroy();
	return *this;
}

Texture2D& Texture2D::make(
	uint			width,
	uint			height,
	ComponentType	type			= ComponentType::CT_UBYTE,
	ImageFormat		format			= ImageFormat::IF_RGBA,
	FilterMode		magFilter		= FilterMode::FM_SMOOTH,
	FilterMode		minFilter		= FilterMode::FM_SMS,
	uchar*			data			= NULL,
	uint			internalFormat	= 0,
) {
	destroy();
	create(width, height, type, format, magFilter, minFilter, data, internalFormat);
	return *this;
}

Texture2D& Texture2D::make(
	String const& path,
	FilterMode magFilter = FilterMode::FM_SMOOTH,
	FilterMode minFilter = FilterMode::FM_SMS
) {
	destroy();
	create(path, minFilter, magFilter);
	return *this;
}

Texture2D& Texture2D::make(
	Image2D::ImageData const& image
) {
	destroy();
	create(image);
	return *this;
}

Texture2D& Texture2D::make(
	Texture2D const& other
) {
	destroy();
	create(other);
	return *this;
}

Texture2D& Texture2D::make(
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

Texture2D& Texture2D::makeUnique(bool filter = false) {
	if (!exists()) return *this;
	Image2D* newImg = Image2D::newImage(
		image->attributes.width,
		image->attributes.height,
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

Texture2D& Texture2D::operator=(Texture2D const& other)	{make(other); return *this;								}
Texture2D& Texture2D::operator=(Texture2D&& other)		{make(other); return *this;								}
Texture2D& Texture2D::operator=(Texture2D* other)		{if (other) make(*other); else destroy(); return *this;	}

inline bool Texture2D::operator==(Texture2D const& other) const						{return *image == *other.image;		}
inline Helper::PartialOrder Texture2D::operator<=>(Texture2D const& other) const	{return *image <=> *other.image;	}

Texture2D& Texture2D::copyFrom(
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

Texture2D& Texture2D::copyFrom(
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

Texture2D& Texture2D::setWrapMode(
	Texture2D::WrapMode const& horizontal,
	Texture2D::WrapMode const& vertical
) {
	if (!exists()) return *this;
	glBindTexture(GL_TEXTURE_2D, image->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, convert(horizontal));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, convert(vertical));
	glBindTexture(GL_TEXTURE_2D, 0);
	return *this;
}

Texture2D& Texture2D::setWrapMode(Texture2D::WrapMode const& mode = Texture2D::WrapMode::WM_REPEAT) {
	setWrapMode(mode, mode);
	return *this;
}

Texture2D& Texture2D::setFilterMode(
	FilterMode magFilter = FilterMode::FM_SMOOTH,
	FilterMode minFilter = FilterMode::FM_SMS
) {
	if (!exists()) return *this;
	glBindTexture(GL_TEXTURE_2D, image->getID());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, convert(minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, convert(magFilter));
	glBindTexture(GL_TEXTURE_2D, 0);
	image->attributes.minFilter = minFilter;
	image->attributes.magFilter = magFilter;
	return *this;
}

inline unsigned int Texture2D::minFilter() const {
	return image->attributes.minFilter;
}

inline unsigned int Texture2D::magFilter() const {
	return image->attributes.magFilter;
}

inline Image2D::Attributes Texture2D::attributes() const {
	return image->attributes;
}

Texture2D& Texture2D::enable(uchar const& texture = 0) {
	image->use(texture);
	return *this;
}

Texture2D const& Texture2D::enable(uchar const& texture = 0) const {
	image->use(texture);
	return *this;
}

Texture2D& Texture2D::operator()(uchar const& texture = 0) {
	return enable(texture);
}

Texture2D const& Texture2D::operator()(uchar const& texture = 0) const {
	return enable(texture);
}

inline unsigned int Texture2D::getID() const {
	if (!exists()) return 0;
	return image->getID();
}

Image2D::ImageData Texture2D::getData() const {
	return image->getData();
}

Texture2D const& Texture2D::saveToFile(
	String const& path,
	uint8 const& quality,
	ImageFileType const& type = ImageFileType::IFT_AUTO_DETECT
) const {
	if (!exists()) return *this;
	image->saveToFile(path, quality, type);
	return *this;
}

Texture2D& Texture2D::saveToFile(
	String const& path,
	uint8 const& quality,
	ImageFileType const& type = ImageFileType::IFT_AUTO_DETECT
) {
	if (!exists()) return *this;
	image->saveToFile(path, quality, type);
	return *this;
}

Texture2D const& Texture2D::saveToFile(
	String const& path,
	ImageFileType const& type = ImageFileType::IFT_AUTO_DETECT
) const {
	return saveToFile(path, 50, type);
}

Texture2D& Texture2D::saveToFile(
	String const& path,
	ImageFileType const& type = ImageFileType::IFT_AUTO_DETECT
) {
	return saveToFile(path, 50, type);
}

bool Texture2D::exists() const {
	return image.exists() && image->exists();
}

inline Texture2D::operator bool() const {
	return exists();
}
