#ifndef MAKAILIB_EX_MENU_COMPONENT_H
#define MAKAILIB_EX_MENU_COMPONENT_H

#include <makai/makai.hpp>

/// @brief Menu system extensions.
namespace Makai::Ex::Menu {
	using EventHandler = Makai::Anima::V2::Runtime::Engine;

	struct AComponent: ID::Identifiable<ID::VLUID const> {
		virtual ~AComponent() {}

		String const type;
		String const name;

		AComponent(String const& type, String const& name): type(type), name(name) {}
	};

	template <class T>
	struct ATypedComponent: AComponent {
		ATypedComponent(String const& name): AComponent(nameOf<T>(), name) {}
	}
}

#endif
