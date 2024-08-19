#define GLEW_STATIC
#include <GLEW/include/GL/glew.h>
#include <GLEW/include/GL/wglew.h>
#include <GL/gl.h>

#include "image.hpp"

using namespace Makai::Graph;

constexpr uint convert(Image::ImageTarget const& target) {
	switch (target) {
		case Image::ImageTarget::IT_TEXTURE_2D: return GL_TEXTURE_2D;
	}
}

constexpr Image2D::ImageFileType fromFileExtension(String const& path) {
	String ext = Helper::toLower(Helper::splitStringAtLast(path, '.').second);
	if (ext == "png")					return Image2D::ImageFileType::IFT_PNG;
	if (ext == "jpg" || ext == "jpeg")	return Image2D::ImageFileType::IFT_JPG;
	if (ext == "tga")					return Image2D::ImageFileType::IFT_TGA;
	if (ext == "bmp")					return Image2D::ImageFileType::IFT_BMP;
	return Image2D::ImageFileType::IFT_INVALID;
}

constexpr uint convert(Image2D::ComponentType const& type) {
	switch (type) {
		case Image2D::ComponentType::CT_UBYTE:		return GL_UNSIGNED_BYTE;
		case Image2D::ComponentType::CT_BYTE:		return GL_BYTE;
		case Image2D::ComponentType::CT_USHORT:		return GL_UNSIGNED_SHORT;
		case Image2D::ComponentType::CT_SHORT:		return GL_SHORT;
		case Image2D::ComponentType::CT_HALF_FLOAT:	return GL_HALF_FLOAT;
		case Image2D::ComponentType::CT_UINT:		return GL_UNSIGNED_INT;
		case Image2D::ComponentType::CT_INT:		return GL_INT;
		case Image2D::ComponentType::CT_FLOAT:		return GL_FLOAT;
	}
}

constexpr uint convert(Image2D::ImageFormat const& type) {
	switch (type) {
		case Image2D::ImageFormat::IF_D:	return GL_DEPTH_COMPONENT;
		case Image2D::ImageFormat::IF_R:	return GL_RED;
		case Image2D::ImageFormat::IF_DS:	return GL_DEPTH_STENCIL;
		case Image2D::ImageFormat::IF_RG:	return GL_RG;
		case Image2D::ImageFormat::IF_RGB:	return GL_RGB;
		case Image2D::ImageFormat::IF_RGBA:	return GL_RGBA;
	}
}

constexpr uint convert(Image2D::FilterMode const& type) {
	switch (type) {
		case Image2D::FilterMode::FM_NEAREST:	return GL_NEAREST;
		case Image2D::FilterMode::FM_SMOOTH:	return GL_LINEAR;
		case Image2D::FilterMode::FM_NMN:		return GL_NEAREST_MIPMAP_NEAREST;
		case Image2D::FilterMode::FM_NMS:		return GL_NEAREST_MIPMAP_LINEAR;
		case Image2D::FilterMode::FM_SMN:		return GL_LINEAR_MIPMAP_NEAREST;
		case Image2D::FilterMode::FM_SMS:		return GL_LINEAR_MIPMAP_LINEAR;
	}
}

inline Image const& Image::bind(ImageTarget const& target = Image::ImageTarget::IT_TEXTURE_2D) const {
	glBindTexture(convert(target), id);
	return *this;
}

inline Image& Image::destroy() {
	if (!exists()) return *this;
	glDeleteTextures(1, &id);
	id = 0;
	return *this;
}

inline Image& Image::create() {
	if (exists()) return *this;
	glGenTextures(1, &id);
	return *this;
}

Image& Image::make() {
	return destroy().create();
}

inline static void Image::bind(ImageTarget const& target = Image::ImageTarget::IT_TEXTURE_2D) {
	glBindTexture(convert(target), 0);
}

inline Image& Image::use(uchar const& texture = 0) {
	setTexture2D(texture, id);
	return *this;
}

inline Image const& Image::use(uchar const& texture = 0) const {
	setTexture2D(texture, id);
	return *this;
}

inline Image& Image::operator()(uchar const& texture = 0) {
	return use(texture);
}

inline Image const& Image::operator()(uchar const& texture = 0) const {
	return use(texture);
}

inline bool Image::operator==(Image const& other) const {
	return id == other.id;
}

inline Helper::PartialOrder Image::operator<=>(Image const& other) const {
	return id <=> other.id;
}

inline Image::operator unsigned int() const	{
	return id;
}

inline unsigned int Image::getID() const {
	return id;
}

inline bool Image::exists() const {
	return id != 0;
}

inline Image::operator bool() const {
	return exists();
}

Image2D& Image2D::create(
	uint			width,
	uint			height,
	Image2D::ComponentType	type			= Image2D::ComponentType::CT_UBYTE,
	Image2D::ImageFormat	format			= Image2D::ImageFormat::IF_RGBA,
	Image2D::FilterMode		magFilter		= Image2D::FilterMode::FM_SMOOTH,
	Image2D::FilterMode		minFilter		= Image2D::FilterMode::FM_SMS,
	uchar*			data			= NULL,
	uint			internalFormat	= 0
) {
	if (exists()) return *this;
	if (!internalFormat)
		internalFormat = (uint)format;
	newImage(this, width, height, type, format, internalFormat, minFilter, magFilter, data);
	return *this;
}

Image2D& Image2D::make(
	uint			width,
	uint			height,
	Image2D::ComponentType	type			= Image2D::ComponentType::CT_UBYTE,
	Image2D::ImageFormat	format			= Image2D::ImageFormat::IF_RGBA,
	Image2D::FilterMode		magFilter		= Image2D::FilterMode::FM_SMOOTH,
	Image2D::FilterMode		minFilter		= Image2D::FilterMode::FM_SMS,
	uchar*			data			= NULL,
	uint			internalFormat	= 0
) {
	destroy();
	return create(width, height, type, format, magFilter, minFilter, data, internalFormat);
}

ImageData Image2D::getData() const {
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
