#ifndef MAKAILIB_GRAPH_MATERIAL_MATERIALS_H
#define MAKAILIB_GRAPH_MATERIAL_DEBUG_H

#include "effect.hpp"
#include "debug.hpp"
#include "../color.hpp"
#include "../shader.hpp"

namespace Makai::Graph::Material {
	struct BaseObjectMaterial {
		Vector4	color = Color::WHITE;
	};

	enum class ObjectCulling {
		OC_NONE,
		OC_FRONT,
		OC_BACK,
	};

	enum class ObjectFill {
		OF_NONE,
		OF_POINT,
		OF_LINE,
		OF_FILL,
	};

	struct ObjectMaterial: BaseObjectMaterial {
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
		ObjectCulling	culling	= ObjectCulling::OC_NONE;
		ObjectFill		fill	= ObjectFill::OF_FILL;
		ObjectDebugView	debug	= ObjectDebugView::ODV_NONE;

		void use(Shader& shader);
	};

	struct BaseBufferMaterial {
		Vector4 background = Color::NONE;
	};

	struct BufferMaterial: BaseBufferMaterial {
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
		Effect::Mask	mask;
		WarpEffect		warp;
		NegativeEffect	negative;
		BlurEffect		blur;
		OutlineEffect	outline;
		WaveEffect		wave;
		WaveEffect		prism;
		PolarWarpEffect	polarWarp;
		GradientEffect	gradient;
		RainbowEffect	rainbow;
		NoiseEffect		noise;
		BufferDebugView	debug	= BufferDebugView::BDV_NONE;

		void use(Shader& shader);
	};

	struct BaseWorldMaterial {};

	struct WorldMaterial: BaseWorldMaterial {
		FogEffect		nearFog;
		FogEffect		farFog;
		AmbientEffect	ambient;

		void use(Shader& shader);
	};

	template<class T, class BASE>
	concept ValidMaterial =
		Type::Subclass<T, BASE>
	&&	requires (Shader& s, T& mat) {
			mat.use(s);
		}
	;

	template<class T> concept ValidObjectMaterial	= ValidMaterial<T, BaseObjectMaterial>;
	template<class T> concept ValidBufferMaterial	= ValidMaterial<T, BaseBufferMaterial>;
	template<class T> concept ValidWorldMaterial	= ValidMaterial<T, BaseWorldMaterial>;
}

#endif // MAKAILIB_GRAPH_MATERIAL_MATERIALS_H
