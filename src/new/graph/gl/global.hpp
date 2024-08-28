#ifndef MAKAILIB_GRAPH_GLOBAL_H
#define MAKAILIB_GRAPH_GLOBAL_H

#include "../../ctl/ctl.hpp"
#include "camera.hpp"
#include "material/material.hpp"

#ifndef MAKAILIB_GRAPH_MATERIAL_MATERIALS_H
#error "HOW."
#endif // MAKAILIB_GRAPH_MATERIAL_MATERIALS_H

namespace Makai::Graph {
	namespace Global {
		Matrix4x4				space = Matrix4x4::identity();
		Camera3D				camera;
		Material::WorldMaterial	world;
	}

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
			Global::world	= space;
			Global::space	= world;
		}

		~GlobalState() {reset();}

		Camera3D const					camera;
		Matrix4x4 const					space;
		Material::WorldMaterial const	world;
	};
}

#endif // MAKAILIB_GRAPH_GLOBAL_H
