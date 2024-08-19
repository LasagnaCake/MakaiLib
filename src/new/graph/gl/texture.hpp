#ifndef MAKAILIB_GRAPH_TEXTURE_H
#define MAKAILIB_GRAPH_TEXTURE_H

#include "image.hpp"

namespace Makai::Graph {
	class Texture2D {
	public:
		using Image2DInstance = Instance<Image2D>;

		using Image2D::ImageFileType, Image2D::ComponentType, Image2D::ImageFormat, Image2D::FilterMode, Image::ImageTarget;

		enum class TextureWrap {
			TW_CLAMP,
			TW_REPEAT,
			TW_MIRROR_REPEAT,
			TW_MIRROR_CLAMP
		};

		static Texture2D fromJSON(JSON::JSONData const& img, String const& sourcepath = "");

		Texture2D(): image(new Image2D()) {}

		Texture2D(
			uint			width,
			uint			height,
			ComponentType	type			= ComponentType::CT_UBYTE,
			ImageFormat		format			= ImageFormat::IF_RGBA,
			FilterMode		magFilter		= FilterMode::FM_SMOOTH,
			FilterMode		minFilter		= FilterMode::FM_SMS,
			uchar*			data			= NULL,
			uint			internalFormat	= 0,
		);

		Texture2D(
			String const& path,
			FilterMode magFilter = FilterMode::FM_SMOOTH,
			FilterMode minFilter = FilterMode::FM_SMS
		);

		Texture2D(
			Image2D::ImageData const& image
		);

		Texture2D(
			Texture2D const& other
		);

		Texture2D(
			Texture2D* const& other
		);

		Texture2D(
			Texture2D const& other,
			uint startX,
			uint startY,
			uint endX,
			uint endY,
			bool filter = false
		);

		Texture2D& create(
			uint			width,
			uint			height,
			ComponentType	type			= ComponentType::CT_UBYTE,
			ImageFormat		format			= ImageFormat::IF_RGBA,
			FilterMode		magFilter		= FilterMode::FM_SMOOTH,
			FilterMode		minFilter		= FilterMode::FM_SMS,
			uchar*			data			= NULL,
			uint			internalFormat	= 0,
		);

		Texture2D& create(
			String path,
			FilterMode magFilter = FilterMode::FM_SMOOTH,
			FilterMode minFilter = FilterMode::FM_SMS
		);

		Texture2D& create(
			Image2D::ImageData const& image
		);

		Texture2D& create(
			Texture2D const& other
		);

		Texture2D& create(
			Texture2D const& other,
			uint startX,
			uint startY,
			uint endX,
			uint endY,
			bool filter = false
		);

		Texture2D& destroy();

		Texture2D& clear();

		Texture2D& make(
			uint			width,
			uint			height,
			ComponentType	type			= ComponentType::CT_UBYTE,
			ImageFormat		format			= ImageFormat::IF_RGBA,
			FilterMode		magFilter		= FilterMode::FM_SMOOTH,
			FilterMode		minFilter		= FilterMode::FM_SMS,
			uchar*			data			= NULL,
			uint			internalFormat	= 0,
		);

		Texture2D& make(
			String path,
			FilterMode magFilter = FilterMode::FM_SMOOTH,
			FilterMode minFilter = FilterMode::FM_SMS
		);

		Texture2D& make(
			Image2D::ImageData const& image
		);

		Texture2D& make(
			Texture2D const& other
		);

		Texture2D& make(
			Texture2D const& other,
			uint startX,
			uint startY,
			uint endX,
			uint endY,
			bool filter = false
		);

		Texture2D& makeUnique(bool filter = false);

		Texture2D& operator=(Texture2D const& other);
		Texture2D& operator=(Texture2D&& other);

		inline bool operator==(Texture2D const& other) const;
		inline Helper::PartialOrder operator<=>(Texture2D const& other) const;

		Texture2D& copyFrom(
			Texture2D const& other,
			uint startX,
			uint startY,
			uint endX,
			uint endY,
			bool filter = false
		);

		Texture2D& copyFrom(
			Texture2D const& other,
			bool filter = false
		);

		Texture2D& setTextureWrapMode(
			TextureWrap const& horizontal,
			TextureWrap const& vertical
		);

		Texture2D& setTextureWrapMode(TextureWrap const& mode = TextureWrap::TW_REPEAT);

		Texture2D& setFilterMode(
			FilterMode magFilter = FilterMode::FM_SMOOTH,
			FilterMode minFilter = FilterMode::FM_SMS
		);

		inline Image2D::FilterMode minFilter() const;

		inline Image2D::FilterMode magFilter() const;

		inline Image2D::Attributes getAttributes() const;

		Texture2D& enable(unsigned char texture = 0);

		Texture2D const& enable(unsigned char texture = 0) const;

		Texture2D&			operator()(unsigned char texture = 0);
		Texture2D const&	operator()(unsigned char texture = 0) const;

		inline uint getID() const;

		Image2D::ImageData getData() const;

		Texture2D const& saveToFile(
			string const& path,
			uint8 const& quality,
			ImageFileType const& type = ImageFileType::IFT_AUTO_DETECT
		) const;

		Texture2D& saveToFile(
			String const& path,
			uint8 const& quality,
			ImageFileType const& type = ImageFileType::IFT_AUTO_DETECT
		);

		Texture2D const& saveToFile(
			String const& path,
			ImageFileType const& type = ImageFileType::IFT_AUTO_DETECT
		) const;

		Texture2D& saveToFile(
			String const& path,
			ImageFileType const& type = ImageFileType::IFT_AUTO_DETECT
		);

		bool exists() const;

		inline operator bool() const;

	private:
		Image2DInstance image;
	};
}

#endif // MAKAILIB_GRAPH_TEXTURE_H
