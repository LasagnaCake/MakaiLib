#ifndef MAKAILIB_GRAPH_GLOBAL_H
#define MAKAILIB_GRAPH_GLOBAL_H

#include "../../ctl/ctl.hpp"
#include "camera.hpp"
#include "material/material.hpp"

namespace Makai::Graph {
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpedantic"
	struct Global {
		static Matrix4x4				space;
		static Camera3D					camera;
		static Material::WorldMaterial	world;
	};
	#pragma GCC diagnostic pop

	struct GlobalState {
		/// Capture global state.
		GlobalState(): camera(Global::camera), space(Global::space), world(Global::world) {}

		/// "Capture-and-set" global state.
		GlobalState(
			Camera3D const&					newCamera,
			Matrix4x4 const&				newSpace,
			Material::WorldMaterial const&	newWorld
		): GlobalState() {
			set(newCamera, newSpace, newWorld);
		}

		void set(
			Camera3D const&					newCamera,
			Matrix4x4 const&				newSpace,
			Material::WorldMaterial const&	newWorld
		) {
			Global::camera	= newCamera;
			Global::space	= newSpace;
			Global::world	= newWorld;
		}

		void reset() {
			Global::camera	= camera;
			Global::space	= space;
			Global::world	= world;
		}

		~GlobalState() {reset();}

		Camera3D const					camera;
		Matrix4x4 const					space;
		Material::WorldMaterial const	world;
	};
}

#endif // MAKAILIB_GRAPH_GLOBAL_H
