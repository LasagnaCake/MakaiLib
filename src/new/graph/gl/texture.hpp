#ifndef MAKAILIB_GRAPH_TEXTURE_H
#define MAKAILIB_GRAPH_TEXTURE_H

#include "../../compat/ctl.hpp"
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

		static Texture2D fromJSON(JSON::JSONData img, String const& sourcepath = "");

		Texture2D(): image(new Image2D()) {}

		Texture2D(decltype(nullptr)): Texture2D() {}

		Texture2D(
			uint const						width,
			uint const						height,
			Image2D::ComponentType const&	type		= Image2D::ComponentType::CT_UBYTE,
			Image2D::ImageFormat const&		format		= Image2D::ImageFormat::IF_RGBA,
			Image2D::FilterMode const&		magFilter	= Image2D::FilterMode::FM_SMOOTH,
			Image2D::FilterMode const&		minFilter	= Image2D::FilterMode::FM_SMS,
			uchar* const					data		= NULL,
			Image2D::ComponentLayout const&	layout		= Image2D::ComponentLayout::CL_AUTO
		);

		Texture2D(
			String const& path,
			Image2D::FilterMode const& magFilter = Image2D::FilterMode::FM_SMOOTH,
			Image2D::FilterMode const& minFilter = Image2D::FilterMode::FM_SMS
		);

		Texture2D(
			Image2D::ImageData const& image
		);

		Texture2D(
			Texture2D const& other
		);

		Texture2D(
			Texture2D const& other,
			uint const startX,
			uint const startY,
			uint const endX,
			uint const endY,
			bool const filter = false
		);

		Texture2D& create(
			uint const						width,
			uint const						height,
			Image2D::ComponentType const&	type		= Image2D::ComponentType::CT_UBYTE,
			Image2D::ImageFormat const&		format		= Image2D::ImageFormat::IF_RGBA,
			Image2D::FilterMode const&		magFilter	= Image2D::FilterMode::FM_SMOOTH,
			Image2D::FilterMode const&		minFilter	= Image2D::FilterMode::FM_SMS,
			uchar* const					data		= NULL,
			Image2D::ComponentLayout const&	layout		= Image2D::ComponentLayout::CL_AUTO
		);

		Texture2D& create(
			String const& path,
			Image2D::FilterMode const& magFilter = Image2D::FilterMode::FM_SMOOTH,
			Image2D::FilterMode const& minFilter = Image2D::FilterMode::FM_SMS
		);

		Texture2D& create(
			Image2D::ImageData const& image
		);

		Texture2D& create(
			Texture2D const& other
		);

		Texture2D& create(
			Texture2D const& other,
			uint const startX,
			uint const startY,
			uint const endX,
			uint const endY,
			bool const filter = false
		);

		Texture2D& destroy();

		Texture2D& clear();

		Texture2D& make(
			uint const						width,
			uint const						height,
			Image2D::ComponentType const&	type		= Image2D::ComponentType::CT_UBYTE,
			Image2D::ImageFormat const&		format		= Image2D::ImageFormat::IF_RGBA,
			Image2D::FilterMode const&		magFilter	= Image2D::FilterMode::FM_SMOOTH,
			Image2D::FilterMode const&		minFilter	= Image2D::FilterMode::FM_SMS,
			uchar* const					data		= NULL,
			Image2D::ComponentLayout const&	layout		= Image2D::ComponentLayout::CL_AUTO
		);

		Texture2D& make(
			String const& path,
			Image2D::FilterMode const& magFilter = Image2D::FilterMode::FM_SMOOTH,
			Image2D::FilterMode const& minFilter = Image2D::FilterMode::FM_SMS
		);

		Texture2D& make(
			Image2D::ImageData const& image
		);

		Texture2D& make(
			Texture2D const& other
		);

		Texture2D& make(
			Texture2D const& other,
			uint const startX,
			uint const startY,
			uint const endX,
			uint const endY,
			bool const filter = false
		);

		Texture2D& makeUnique(bool const filter = false);

		Texture2D&			operator=(Texture2D const& other);
		Texture2D&			operator=(Texture2D&& other);
		inline Texture2D&	operator=(decltype(nullptr)) {destroy(); return *this;}

		bool operator==(Texture2D const& other) const;
		ValueOrder operator<=>(Texture2D const& other) const;

		Texture2D& copyFrom(
			Texture2D const& other,
			uint const startX,
			uint const startY,
			uint const endX,
			uint const endY,
			bool const filter = false
		);

		Texture2D& copyFrom(
			Texture2D const& other,
			bool const filter = false
		);

		Texture2D& setWrapMode(
			WrapMode const& horizontal,
			WrapMode const& vertical
		);

		Texture2D& setWrapMode(WrapMode const& mode = WrapMode::WM_REPEAT);

		Texture2D& setFilterMode(
			Image2D::FilterMode const& magFilter = Image2D::FilterMode::FM_SMOOTH,
			Image2D::FilterMode const& minFilter = Image2D::FilterMode::FM_SMS
		);

		Image2D::FilterMode minFilter() const;

		Image2D::FilterMode magFilter() const;

		Image2D::Attributes attributes() const;

		Texture2D& enable(uchar const slot = 0);

		Texture2D const& enable(uchar const slot = 0) const;

		Texture2D&			operator()(uchar const slot = 0);
		Texture2D const&	operator()(uchar const slot = 0) const;

		uint getID() const;

		Image2D::ImageData getData() const;

		Texture2D const& saveToFile(
			String const& path,
			uint8 const quality,
			Image2D::ImageFileType const& type = Image2D::ImageFileType::IFT_AUTO_DETECT
		) const;

		Texture2D& saveToFile(
			String const& path,
			uint8 const quality,
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
