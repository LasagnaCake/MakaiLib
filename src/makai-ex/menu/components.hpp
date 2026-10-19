#ifndef MAKAILIB_EX_MENU_COMPONENT_H
#define MAKAILIB_EX_MENU_COMPONENT_H

#include <makai/makai.hpp>

#include "server.hpp"

/// @brief Menu system extensions.
namespace Makai::Ex::Menu {
	template <class T, usize I = DEFAULT_SERVER_ID>
	struct Component: typename Server<I>::Component {
		using Base = typename Server<I>::Component;

		Component(String const& name): Base(nameOf<T>(), name) {}
	};

	template <class T, usize I = DEFAULT_SERVER_ID>
	struct VisualComponent: Component<T, I> {
		using Base = Component<T, I>;

		Graph::UTF8Label	text;
		Graph::Renderable	body;

		Component(String const& name): Base(nameOf<T>(), name) {}

		void onUpdate(float delta, App& app) override {
			Base::onUpdate(delta, app);
			text.trans = trans;
			body.trans = trans;
		}
	};

	template <usize I = DEFAULT_SERVER_ID>
	struct Button: VisualComponent<Button, I> {

	};
}

#endif
