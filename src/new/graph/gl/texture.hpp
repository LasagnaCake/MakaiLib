#ifndef MAKAILIB_GRAPH_TEXTURE_H
#define MAKAILIB_GRAPH_TEXTURE_H

#include "../../ctl/ctl.hpp"
#include "../../file/json.hpp"
#include "image.hpp"

namespace Makai::Graph {
	class Texture2D {
	public:
		using Image2DInstance = Instance<Image2D>;

		enum class WrapMode {
			WM_CLAMP,
			WM_REPEAT,
			WM_MIRROR_REPEAT,
			WM_MIRROR_CLAMP
		};

		static Texture2D fromJSON(JSON::JSONData const& img, String const& sourcepath = "");

		Texture2D(): image(new Image2D()) {}

		Texture2D(
			uint			width,
			uint			height,
			Image2D::ComponentType	type		= Image2D::ComponentType::CT_UBYTE,
			Image2D::ImageFormat	format		= Image2D::ImageFormat::IF_RGBA,
			Image2D::FilterMode		magFilter	= Image2D::FilterMode::FM_SMOOTH,
			Image2D::FilterMode		minFilter	= Image2D::FilterMode::FM_SMS,
			uchar*			data				= NULL,
			uint			internalFormat		= 0
		);

		Texture2D(
			String const& path,
			Image2D::FilterMode magFilter = Image2D::FilterMode::FM_SMOOTH,
			Image2D::FilterMode minFilter = Image2D::FilterMode::FM_SMS
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
			Image2D::ComponentType	type		= Image2D::ComponentType::CT_UBYTE,
			Image2D::ImageFormat	format		= Image2D::ImageFormat::IF_RGBA,
			Image2D::FilterMode		magFilter	= Image2D::FilterMode::FM_SMOOTH,
			Image2D::FilterMode		minFilter	= Image2D::FilterMode::FM_SMS,
			uchar*			data				= NULL,
			uint			internalFormat		= 0
		);

		Texture2D& create(
			String path,
			Image2D::FilterMode magFilter = Image2D::FilterMode::FM_SMOOTH,
			Image2D::FilterMode minFilter = Image2D::FilterMode::FM_SMS
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
			Image2D::ComponentType	type		= Image2D::ComponentType::CT_UBYTE,
			Image2D::ImageFormat	format		= Image2D::ImageFormat::IF_RGBA,
			Image2D::FilterMode		magFilter	= Image2D::FilterMode::FM_SMOOTH,
			Image2D::FilterMode		minFilter	= Image2D::FilterMode::FM_SMS,
			uchar*			data				= NULL,
			uint			internalFormat		= 0
		);

		Texture2D& make(
			String path,
			Image2D::FilterMode magFilter = Image2D::FilterMode::FM_SMOOTH,
			Image2D::FilterMode minFilter = Image2D::FilterMode::FM_SMS
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

		bool operator==(Texture2D const& other) const;
		Helper::PartialOrder operator<=>(Texture2D const& other) const;

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

		Texture2D& setWrapMode(
			WrapMode const& horizontal,
			WrapMode const& vertical
		);

		Texture2D& setWrapMode(WrapMode const& mode = WrapMode::WM_REPEAT);

		Texture2D& setFilterMode(
			Image2D::FilterMode magFilter = Image2D::FilterMode::FM_SMOOTH,
			Image2D::FilterMode minFilter = Image2D::FilterMode::FM_SMS
		);

		Image2D::FilterMode minFilter() const;

		Image2D::FilterMode magFilter() const;

		Image2D::Attributes attributes() const;

		Texture2D& enable(uchar const& texture = 0);

		Texture2D const& enable(uchar const& texture = 0) const;

		Texture2D&			operator()(uchar const& texture = 0);
		Texture2D const&	operator()(uchar const& texture = 0) const;

		uint getID() const;

		Image2D::ImageData getData() const;

		Texture2D const& saveToFile(
			String const& path,
			uint8 const& quality,
			Image2D::ImageFileType const& type = Image2D::ImageFileType::IFT_AUTO_DETECT
		) const;

		Texture2D& saveToFile(
			String const& path,
			uint8 const& quality,
			Image2D::ImageFileType const& type = Image2D::ImageFileType::IFT_AUTO_DETECT
		);

		Texture2D const& saveToFile(
			String const& path,
			Image2D::ImageFileType const& type = Image2D::ImageFileType::IFT_AUTO_DETECT
		) const;

		Texture2D& saveToFile(
			String const& path,
			Image2D::ImageFileType const& type = Image2D::ImageFileType::IFT_AUTO_DETECT
		);

		bool exists() const;

		operator bool() const;

	private:
		Image2DInstance image;
	};
}

#endif // MAKAILIB_GRAPH_TEXTURE_H
