#ifndef MAKAILIB_GRAPH_TEXTURE_H
#define MAKAILIB_GRAPH_TEXTURE_H

#include "../ctl/ctl.hpp"

namespace Makai::Graph {
	struct Image {
		enum class ImageTarget {
			IT_TEXTURE_2D
		};

		Image() {}

		~Image() {destroy();}

		inline Image const& bind(ImageTarget const& target = ImageTarget::IT_TEXTURE_2D) const;

		inline Image& destroy();

		inline Image& create();

		Image& make();

		inline static void unbind(ImageTarget const& target = ImageTarget::IT_TEXTURE_2D) const;

		inline Image& use(uchar const& texture = 0);

		inline Image const& use(uchar const& texture = 0) const;

		inline Image&		operator()(unsigned char texture = 0);
		inline Image const&	operator()(unsigned char texture = 0) const;

		inline bool operator==(Image const& other) const;
		inline Helper::PartialOrder operator<=>(Image const& other) const;

		inline operator unsigned int() const;
		inline unsigned int getID() const;

		inline bool exists() const;
		inline operator bool() const;

	private:
		unsigned int id = 0;
	};

	struct Image2D: public Image {
		using Image::create;

		enum class ImageFileType {
			IFT_INVALID = -1,
			IFT_AUTO_DETECT,
			IFT_TGA,
			IFT_PNG,
			IFT_JPG,
			IFT_BMP
		};

		enum class ComponentType {
			CT_UBYTE, CT_BYTE,
			CT_USHORT, CT_SHORT, CT_HALF_FLOAT,
			CT_UINT, CT_INT, CT_FLOAT
		};

		enum class ImageFormat {
			/// Depth
			IF_D,
			/// Red
			IF_R,
			/// Depth-Stencil
			IF_DS,
			/// Red-Green
			IF_RG,
			/// Red-Green-Blue
			IF_RGB,
			/// Red-Green-Blue-Alpha
			IF_RGBA,
		};

		enum class FilterMode {
			FM_NEAREST,
			FM_SMOOTH,
			/// Nearest-Mipmap-Nearest
			FM_NMN,
			/// Nearest-Mipmap-Smooth
			FM_NMS,
			/// Smooth-Mipmap-Nearest
			FM_SMN,
			/// Smooth-Mipmap-Smooth
			FM_SMS,
		};

		struct Attributes {
			uint width, height;
			ComponentType	type;
			ImageFormat		format;
			uint			internalFormat;
			FilterType		minFilter, magFilter;
		};

		struct ImageData: Attributes {
			Data::BinaryData data;
		};

		Image2D& create(
			uint			width,
			uint			height,
			ComponentType	type			= ComponentType::CT_UBYTE,
			ImageFormat		format			= ImageFormat::IF_RGBA,
			FilterMode		magFilter		= FilterMode::FM_SMOOTH,
			FilterMode		minFilter		= FilterMode::FM_SMS,
			uchar*			data			= NULL,
			uint			internalFormat	= 0
		);

		Image2D& make(
			uint			width,
			uint			height,
			ComponentType	type			= ComponentType::CT_UBYTE,
			ImageFormat		format			= ImageFormat::IF_RGBA,
			FilterMode		magFilter		= FilterMode::FM_SMOOTH,
			FilterMode		minFilter		= FilterMode::FM_SMS,
			uchar*			data			= NULL,
			uint			internalFormat	= 0
		);

		inline Attributes getAttributes();

		ImageData getData() const;

		Image2D&		saveToFile(String const& path, uint8 const& quality, ImageFileType type = ImageFileType::IFT_AUTO_DETECT);
		Image2D const&	saveToFile(String const& path, uint8 const& quality, ImageFileType type = ImageFileType::IFT_AUTO_DETECT) const;

		Image2D&		saveToFile(string const& path, ImageFileType type = ImageFileType::IFT_AUTO_DETECT);
		Image2D const&	saveToFile(string const& path, ImageFileType type = ImageFileType::IFT_AUTO_DETECT) const;

		[[nodiscard]]
		static Image2D* newImage(
			uint			width,
			uint			height,
			ComponentType	type			= ComponentType::CT_UBYTE,
			ImageFormat		format			= ImageFormat::IF_RGBA,
			FilterMode		magFilter		= FilterMode::FM_SMOOTH,
			FilterMode		minFilter		= FilterMode::FM_SMS,
			uchar*			data			= NULL,
			uint			internalFormat	= 0,
			ImageTarget		target			= ImageTarget::IT_TEXTURE_2D
		);

		static Image2D* newImage(
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
		);

	private:
		void saveImageToFile(String const& path, uint8 const& quality, ImageFileType type) const;

		friend class Texture2D;

		Attributes attributes;
	};
}

#endif // MAKAILIB_GRAPH_TEXTURE_H
