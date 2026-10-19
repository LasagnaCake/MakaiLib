#ifndef MAKAILIB_EX_MENU_SERVER_H
#define MAKAILIB_EX_MENU_SERVER_H

#include <makai/makai.hpp>

/// @brief Menu system extensions.
namespace Makai::Ex::Menu {
	using EventHandler = Makai::Anima::V2::Runtime::Engine;

	constexpr usize const DEFAULT_SERVER_ID = hash("Makai::Ex::Menu::Server");

	template <usize I = DEFAULT_SERVER_ID>
	struct Server: AUpdateable, ANotifiable<I> {
		using CollisionHandler = Collision::C2D::Server<I>;
		using NotificationHandler = ANotifiable<I>;
		EventHandler		events;
		CollisionHandler	collisions;
		Input::Manager		input;

		struct Component: ID::Identifiable<uint64 const>, AUpdateable, NotificationHandler::IMessage {
			using Server = Server<I>;

			virtual ~Component() {}

			String const type;
			String const name;

			Component(String const& type, String const& name):
				type(type),
				name(name),
				server(server) {
				bounds.onCollision = [&] (auto, auto) {NotificationHandler::broadcast("mk/ex/menu/onMouseEvent", this);};
			}

			void onUpdate(float delta, App& app) override {}

			Collision::C2D::Area& area() {return *bounds;}

		private:
			Unique<CollisionHandler::Collider> const bounds = CollisionHandler::createCollider();
		};

		void onMessage(String const& signal, MessageHandleType const& message) override {
			if (signal == "mk/ex/menu/onMouseEvent") {
				auto const component = message.as<AComponent const>();
				String const event = input.isButtonDown(Input::MouseCode::MC_LEFT) ? "onMouseDown" : "onMouseUp";
				events.expose<void(uint64)>(component->type + ":" + component->name + ":" + event).invoke(component->id());
			}
		}

		Server() {}

		~Server() {}

		void onUpdate(float delta, App& app) override {}
	};
}

#endif
