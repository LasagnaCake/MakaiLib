#define GLEW_STATIC
#include <GLEW/include/GL/glew.h>
#include <GLEW/include/GL/wglew.h>
#include <GL/gl.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

#include "image.hpp"

using namespace Makai::Graph;

using
	Image2D::ImageFileType,
	Image2D::ComponentType,
	Image2D::ImageFormat,
	Image2D::FilterMode,
	Image::ImageTarget
;

constexpr uint convert(Image::ImageTarget const& target) {
	switch (target) {
		case Image::ImageTarget::IT_TEXTURE_2D: return GL_TEXTURE_2D;
	}
}

constexpr ImageFileType fromFileExtension(String const& path) {
	String ext = Helper::toLower(Helper::splitStringAtLast(path, '.').second);
	if (ext == "png")					return ImageFileType::IFT_PNG;
	if (ext == "jpg" || ext == "jpeg")	return ImageFileType::IFT_JPG;
	if (ext == "tga")					return ImageFileType::IFT_TGA;
	if (ext == "bmp")					return ImageFileType::IFT_BMP;
	return ImageFileType::IFT_INVALID;
}

constexpr uint convert(ComponentType const& type) {
	switch (type) {
		case ComponentType::CT_UBYTE:		return GL_UNSIGNED_BYTE;
		case ComponentType::CT_BYTE:		return GL_BYTE;
		case ComponentType::CT_USHORT:		return GL_UNSIGNED_SHORT;
		case ComponentType::CT_SHORT:		return GL_SHORT;
		case ComponentType::CT_HALF_FLOAT:	return GL_HALF_FLOAT;
		case ComponentType::CT_UINT:		return GL_UNSIGNED_INT;
		case ComponentType::CT_INT:			return GL_INT;
		case ComponentType::CT_FLOAT:		return GL_FLOAT;
		case ComponentType::CT_UINT_24_8:	return GL_UNSIGNED_INT_24_8;
	}
}

constexpr uint convert(ImageFormat const& type) {
	switch (type) {
		case ImageFormat::IF_D:		return GL_DEPTH_COMPONENT;
		case ImageFormat::IF_R:		return GL_RED;
		case ImageFormat::IF_DS:	return GL_DEPTH_STENCIL;
		case ImageFormat::IF_RG:	return GL_RG;
		case ImageFormat::IF_RGB:	return GL_RGB;
		case ImageFormat::IF_RGBA:	return GL_RGBA;
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
	ComponentType	type			= ComponentType::CT_UBYTE,
	ImageFormat		format			= ImageFormat::IF_RGBA,
	FilterMode		magFilter		= FilterMode::FM_SMOOTH,
	FilterMode		minFilter		= FilterMode::FM_SMS,
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
	ComponentType	type			= ComponentType::CT_UBYTE,
	ImageFormat		format			= ImageFormat::IF_RGBA,
	FilterMode		magFilter		= FilterMode::FM_SMOOTH,
	FilterMode		minFilter		= FilterMode::FM_SMS,
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
		case ComponentFormat::CT_UBYTE:
		case ComponentFormat::CT_BYTE:			size = 1;	break;
		case ComponentFormat::CT_USHORT:
		case ComponentFormat::CT_SHORT:
		case ComponentFormat::CT_HALF_FLOAT:	size = 2;	break;
		case ComponentFormat::CT_UINT:
		case ComponentFormat::CT_INT:
		case ComponentFormat::CT_FLOAT:			size = 4;	break;
	}
	DEBUG("Image Size: ", size);
	switch (attributes.format) {
		case ImageFormat::IF_D:
		case ImageFormat::IF_R:					break;
		case ImageFormat::IF_DS:
		case ImageFormat::IF_RG:	size *= 2;	break;
		case ImageFormat::IF_RGB:	size *= 3;	break;
		default:
		case ImageFormat::IF_RGBA:	size *= 4;	break;
	}
	DEBUGLN("Image Pixel Width: ", size);
	DEBUGLN("Width: ", attributes.width);
	DEBUGLN("Height: ", attributes.height);
	DEBUGLN("Image Size: ", ((usize)attributes.width) * ((usize)attributes.height) * size);
	ImageData imgdat = (ImageData)attributes;
	DEBUGLN("Reserving buffer...");
	imgdat.data.resize(((usize)attributes.width) * ((usize)attributes.height) * size, 0);
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

Image2D const& Image2D::saveToFile(String const& path, uint8 const& quality, ImageFileType type = ImageFileType::IFT_AUTO_DETECT) const {
	saveImageToFile(path, quality, type);
	return *this;
}

Image2D& Image2D::saveToFile(String const& path, uint8 const& quality, ImageFileType type = ImageFileType::IFT_AUTO_DETECT) {
	saveImageToFile(path, quality, type);
	return *this;
}

Image2D& Image2D::saveToFile(String const& path, ImageFileType type = ImageFileType::IFT_AUTO_DETECT) {
	return saveToFile(path, 50, type);
}

Image2D const& Image2D::saveToFile(String const& path, ImageFileType type = ImageFileType::IFT_AUTO_DETECT) const	{
	return saveToFile(path, 50, type);
}

static Image2D* Image2D::newImage(
	uint			width,
	uint			height,
	ComponentType	type			= ComponentType::CT_UBYTE,
	ImageFormat		format			= ImageFormat::IF_RGBA,
	FilterMode		magFilter		= FilterMode::FM_SMOOTH,
	FilterMode		minFilter		= FilterMode::FM_SMS,
	uchar*			data			= NULL,
	uint			internalFormat	= 0,
	ImageTarget		target			= ImageTarget::IT_TEXTURE_2D
) {
	Image2D* image = new Image2D();
	return Image2D::newImage(image, width, height, type, format, format, minFilter, magFilter, data);
}

static Image2D* Image2D::newImage(
	Image2D*		image,
	uint			width,
	uint			height,
	ComponentType	type			= ComponentType::CT_UBYTE,
	ImageFormat		format			= ImageFormat::IF_RGBA,
	FilterMode		magFilter		= FilterMode::FM_SMOOTH,
	FilterMode		minFilter		= FilterMode::FM_SMS,
	uchar*			data			= NULL,
	uint			internalFormat	= 0,
	ImageTarget		target			= ImageTarget::IT_TEXTURE_2D
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

void Image2D::saveImageToFile(String const& path, uint8 const& quality, ImageFileType type) const {
	if (!exists()) return;
	ImageData imgdat = getData();
	uchar channels = 0;
	switch (imgdat.format) {
		case ImageFormat::IF_D:
		case ImageFormat::IF_R:		channels = 1;	break;
		case ImageFormat::IF_DS:
		case ImageFormat::IF_RG:	channels = 2;	break;
		case ImageFormat::IF_RGB:	channels = 3;	break;
		default:
		case ImageFormat::IF_RGBA:	channels = 4;	break;
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
