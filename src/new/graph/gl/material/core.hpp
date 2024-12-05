#ifndef MAKAILIB_GRAPH_MATERIAL_MATERIALS_H
#define MAKAILIB_GRAPH_MATERIAL_MATERIALS_H

#include "../../../file/json.hpp"
#include "../../../compat/ctl.hpp"
#include "effect.hpp"
#include "debug.hpp"
#include "../color.hpp"
#include "../shader.hpp"
#include "../renderer/mode.hpp"

namespace Makai::Graph::Material {

	struct IMaterial {
		virtual void use(Shader const& shader) const = 0;
	};

	struct IObjectMaterial: IMaterial {
		Vector4	color = Color::WHITE;

		//virtual void use(Shader& shader) const = 0;
	};

	struct ObjectMaterial final: IObjectMaterial {
		bool shaded			= false;
		bool illuminated	= false;
		float			hue			= 0;
		float			saturation	= 1;
		float			luminosity	= 1;
		float			brightness	= 0;
		float			contrast	= 1;
		Vector2				uvShift;
		Effect::Texture		texture;
		Effect::NormalMap	normalMap;
		Effect::Emission	emission;
		Effect::Warp		warp;
		Effect::Negative	negative;
		Effect::Gradient	gradient;
		List<Vector3>	instances = {Vec3(0, 0, 0)};
		CullMode	culling		= CullMode::OCM_NONE;
		FillMode	fill		= FillMode::OFM_FILL;
		ObjectDebugView	debug	= ObjectDebugView::ODV_NONE;

		void use(Shader const& shader) const override final;
	};

	struct IBufferMaterial: IMaterial {
		Vector4 background = Color::NONE;

		//virtual void use(Shader& shader) const = 0;
	};

	struct BufferMaterial final: IBufferMaterial {
		Vector4
			color	= Color::WHITE,
			accent	= Color::NONE
		;
		float			hue			= 0;
		float			saturation	= 1;
		float			luminosity	= 1;
		float			brightness	= 0;
		float			contrast	= 1;
		Vector2			uvShift;
		Effect::Mask		mask;
		Effect::Warp		warp;
		Effect::Negative	negative;
		Effect::Blur		blur;
		Effect::Outline		outline;
		Effect::Wave		wave;
		Effect::Wave		prism;
		Effect::PolarWarp	polarWarp;
		Effect::Gradient	gradient;
		Effect::Rainbow		rainbow;
		Effect::Noise		noise;
		BufferDebugView	debug	= BufferDebugView::BDV_NONE;

		void use(Shader const& shader) const override final;
	};

	struct IWorldMaterial: IMaterial {
		//virtual void use(Shader& shader) const = 0;
	};

	struct WorldMaterial final: IWorldMaterial {
		Effect::Fog		nearFog;
		Effect::Fog		farFog;
		Effect::Ambient	ambient;

		void use(Shader const& shader) const override final;
	};

	template<class T, class BASE>
	concept ValidMaterial =
		Makai::Type::Subclass<T, BASE>
	;

	template<class T> concept ValidObjectMaterial	= ValidMaterial<T, IObjectMaterial>;
	template<class T> concept ValidBufferMaterial	= ValidMaterial<T, IBufferMaterial>;
	template<class T> concept ValidWorldMaterial	= ValidMaterial<T, IWorldMaterial>;
}

#endif // MAKAILIB_GRAPH_MATERIAL_MATERIALS_H
