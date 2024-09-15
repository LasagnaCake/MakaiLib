#ifndef MAKAILIB_GRAPH_IMAGE_H
#define MAKAILIB_GRAPH_IMAGE_H

#include "../../ctl/ctl.hpp"
#include "core.hpp"

namespace Makai::Graph {
	struct Texture2D;

	struct Image {
		enum class ImageTarget {
			IT_TEXTURE_2D
		};

		Image() {}

		~Image() {destroy();}

		Image&			bind(ImageTarget const& target = ImageTarget::IT_TEXTURE_2D);
		Image const&	bind(ImageTarget const& target = ImageTarget::IT_TEXTURE_2D) const;

		Image& destroy();

		Image& create();

		Image& make();

		static void unbind(ImageTarget const& target = ImageTarget::IT_TEXTURE_2D);

		Image&			use(uchar const& slot = 0, ImageTarget const& target = ImageTarget::IT_TEXTURE_2D);
		Image const&	use(uchar const& slot = 0, ImageTarget const& target = ImageTarget::IT_TEXTURE_2D) const;

		Image&			operator()(uchar const& slot = 0, ImageTarget const& target = ImageTarget::IT_TEXTURE_2D);
		Image const&	operator()(uchar const& slot = 0, ImageTarget const& target = ImageTarget::IT_TEXTURE_2D) const;

		static void set(
			uint const&			image,
			uchar const&		slot,
			ImageTarget const&	target = ImageTarget::IT_TEXTURE_2D
		);

		bool operator==(Image const& other) const;
		Helper::PartialOrder operator<=>(Image const& other) const;

		operator uint() const;
		uint getID() const;

		bool exists() const;
		operator bool() const;

	private:
		uint id = 0;
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
			CT_UINT, CT_INT, CT_FLOAT,
			CT_UINT_24_8
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

		enum class ComponentLayout {
			/// Auto-detect components
			CL_AUTO = -1,
			/// Standard image components
			CL_D,
			CL_R,
			CL_DS,
			CL_RG,
			CL_RGB,
			CL_RGBA,
			/// Specialized components
			CL_RGBA_16F,
			CL_D24_S8
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
			ComponentLayout	layout;
			FilterMode		minFilter, magFilter;
		};

		struct ImageData: Attributes {
			List<ubyte> data;
		};

		Image2D& create(
			uint const&				width,
			uint const&				height,
			ComponentType const&	type		= ComponentType::CT_UBYTE,
			ImageFormat const&		format		= ImageFormat::IF_RGBA,
			FilterMode const&		magFilter	= FilterMode::FM_SMOOTH,
			FilterMode const&		minFilter	= FilterMode::FM_SMS,
			uchar* const&			data		= NULL,
			ComponentLayout const&	layout		= ComponentLayout::CL_AUTO
		);

		Image2D& make(
			uint const&				width,
			uint const&				height,
			ComponentType const&	type		= ComponentType::CT_UBYTE,
			ImageFormat const&		format		= ImageFormat::IF_RGBA,
			FilterMode const&		magFilter	= FilterMode::FM_SMOOTH,
			FilterMode const&		minFilter	= FilterMode::FM_SMS,
			uchar* const&			data		= NULL,
			ComponentLayout const&	layout		= ComponentLayout::CL_AUTO
		);

		Attributes getAttributes();

		ImageData getData() const;

		Image2D&		saveToFile(
			String const&			path,
			uint8 const&			quality,
			ImageFileType const&	type = ImageFileType::IFT_AUTO_DETECT
		);

		Image2D const&	saveToFile(
			String const&			path,
			uint8 const&			quality,
			ImageFileType const&	type = ImageFileType::IFT_AUTO_DETECT
		) const;

		Image2D&		saveToFile(
			String const&			path,
			ImageFileType const&	type = ImageFileType::IFT_AUTO_DETECT
		);

		Image2D const&	saveToFile(
			String const&			path,
			ImageFileType const&	type = ImageFileType::IFT_AUTO_DETECT
		) const;

		[[nodiscard]]
		static Image2D* newImage(
			uint const&				width,
			uint const&				height,
			ComponentType const&	type		= ComponentType::CT_UBYTE,
			ImageFormat const&		format		= ImageFormat::IF_RGBA,
			FilterMode const&		magFilter	= FilterMode::FM_SMOOTH,
			FilterMode const&		minFilter	= FilterMode::FM_SMS,
			uchar* const&			data		= NULL,
			ComponentLayout const&	layout		= ComponentLayout::CL_AUTO,
			ImageTarget const&		target		= ImageTarget::IT_TEXTURE_2D
		);

		static Image2D* newImage(
			Image2D* const&			image,
			uint const&				width,
			uint const&				height,
			ComponentType const&	type		= ComponentType::CT_UBYTE,
			ImageFormat const&		format		= ImageFormat::IF_RGBA,
			FilterMode const&		magFilter	= FilterMode::FM_SMOOTH,
			FilterMode const&		minFilter	= FilterMode::FM_SMS,
			uchar* const&			data		= NULL,
			ComponentLayout const&	layout		= ComponentLayout::CL_AUTO,
			ImageTarget const&		target		= ImageTarget::IT_TEXTURE_2D
		);

	private:
		void saveImageToFile(
			String const&	path,
			uint8 const&	quality,
			ImageFileType	type
		) const;

		friend class Texture2D;

		Attributes attributes;
	};
}

#endif // MAKAILIB_GRAPH_IMAGE_H
