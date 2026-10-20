#ifndef MAKAILIB_EX_MENU_SERVER_H
#define MAKAILIB_EX_MENU_SERVER_H

#include <makai/makai.hpp>

/// @brief Menu system extensions.
namespace Makai::Ex::Menu {
	using EventHandler = Makai::Anima::V2::Runtime::Engine;

	constexpr usize const DEFAULT_SERVER_ID = hash("Makai::Ex::Menu::Server");

	template <usize I = DEFAULT_SERVER_ID>
	struct Server: AUpdateable, ANotifiable<I>, EventHandler {
		using CollisionHandler = Collision::C2D::Server<I>;
		using NotificationHandler = ANotifiable<I>;
		CollisionHandler	collisions;
		Input::Manager		input;

		struct EventMessage: NotificationHandler::IMessage {
			Server&	server;
			Data::Value value;
		};

		struct Component: ID::Identifiable<uint64 const>, AUpdateable, NotificationHandler, NotificationHandler::IMessage {
			using Server = Server<I>;

			virtual ~Component() {}

			String const type;
			String const name;

			Transform3D trans;

			Component(String const& type, String const& name):
				type(type),
				name(name),
				server(server) {
				mouseClickBounds->onCollision = [&] (auto, auto) {
					if (updating) NotificationHandler::broadcast("mk/ex/menu/events/mouse", this);
				};
				mouseHoverBounds->onCollision = [&] (auto, auto) {
					if (updating) NotificationHandler::broadcast("mk/ex/menu/events/mouse/hover", this);
				};
				auto const base = "mk/ex/menu/v1/events/component/" + toString(id());
				subscribeTo(
					StringList::(
						eventPath("/transform/position"),
						eventPath("/transform/rotation"),
						eventPath("/transform/scale")
					)
				);
				broadcast("mk/ex/menu/v1/events/initComponent");
			}

			void onUpdate(float delta, App& app) override {}

			Collision::C2D::Area& area() {return *bounds;}

			bool enable()	{updating = false; bounds->canCollide = false;}
			bool disable()	{updating = false; bounds->canCollide = false;}

			void onMessage(String const& message, MessageHandleType const& message) {
				EventMessage& m = *message.morph<EventMessage::SetTransform const>();
				if (message == eventPath("/transform/position"))		trans.position	= tc.value.getVector();
				else if (message == eventPath("/transform/rotation"))	trans.rotation	= tc.value.getVector();
				else if (message == eventPath("/transform/scale"))		trans.scale		= tc.value.getVector();
				else if (message == eventPath("/enable"))	enable();
				else if (message == eventPath("/disable"))	disable();
			}

			String eventPath(String const& name) {
				return "mk/ex/menu/v1/events/component/" + toString(id()) + name;
			}

		private:
			Unique<CollisionHandler::Collider> const mouseClickBounds = CollisionHandler::createCollider(1);
			Unique<CollisionHandler::Collider> const mouseHoverBounds = CollisionHandler::createCollider(2);
		};

		struct Callbacks {
			using Callback = EventHandler::ExposedCall<void(uint64)>;
			Callback const onMouseDown;
			Callback const onMouseUp;
			Callback const onFocus;
			Callback const onUnfocus;
			Callback const onInit;

			Events(EventHandler& events, Component& component):
				onMouseDown(events.template expose<void(uint64)>(component.type + ":" + component.name + ":onMouseDown")),
				onMouseUp(events.template expose<void(uint64)>(component.type + ":" + component.name + ":onMouseUp")),
				onFocus(events.template expose<void(uint64)>(component.type + ":" + component.name + ":onFocus")),
				onUnfocus(events.template expose<void(uint64)>(component.type + ":" + component.name + ":onUnfocus")),
				onInit(events.template expose<void(uint64)>(component.type + ":" + component.name + ":onInit"))
		 	{}
		}

		void onMessage(String const& signal, MessageHandleType const& message) override {
			if (!message) return;
			auto const& component = *message.template morph<AComponent const>();
			Callbacks callbacks{*this, *component};
			if (signal == "mk/ex/menu/v1/events/mouse") {
				if (input.isButtonDown(Input::MouseCode::MC_LEFT))
					callbacks.onMouseDown(component.id());
				else callbacks.onMouseUp(component.id());
			} else if (signal == "mk/ex/menu/v1/events/focus")
				callbacks.onFocus(component.id());
			else if (signal == "mk/ex/menu/v1/events/unfocus")
				callbacks.onUnfocus(component.id());
			else if (signal == "mk/ex/menu/v1/events/initComponent")
				callbacks.onInit(component.id());
		}

		void onUpdate(float delta, App& app) override {
			mouse->position = input.getWindowMousePosition();
			if (
				input.isButtonJustPressed(Input::MouseCode::MC_LEFT)
			or	input.isButtonJustReleased(Input::MouseCode::MC_LEFT)
			) collisions.process();
			while (EventHandler::process()) {}
		}

		~Server() {}

		Server(Server const&)	= delete;
		Server(Server&&)		= delete;

		Server& operator=(Server const&)	= delete;
		Server& operator=(Server&&)			= delete;

		static Server& instance() {
			static Server server;
			return server;
		}

	private:
		auto const mouse			= Instance<Collision::C2D::Point>::create();

		auto const mouseCollider	= CollisionHandler::createCollider(0, Collision::C2D::Area{mousePos});

		static broadcastToComponent(u64 const id, String const& event, Data::Value value) {
			Component::EventMessage message{this};
			message.value = value;
			broadcast("mk/ex/menu/v1/component/" + toString(id) + "/" + event, &value);
		}

		static broadcastGlobally(String const& event, Data::Value value) {
			static Component::EventMessage message;
			message.value = value;
			broadcast(event, &value);
		}

		Server(): callbacks(events) {
			CollisionHandler::layers[0].affects		= 0b110;
			CollisionHandler::layers[1].affectedBy	= 0b001;
			CollisionHandler::layers[2].affectedBy	= 0b001;
			subscribeTo(
				StringList::from(
					"mk/ex/menu/v1/events/mouse",
					"mk/ex/menu/v1/events/focus",
					"mk/ex/menu/v1/events/unfocus",
					"mk/ex/menu/v1/events/initComponent"
				)
			);
			context.addNativeCall("mk/ex/menu/v1/broadcastToComponent", broadcastToComponent);
			context.addNativeCall("mk/ex/menu/v1/broadcastGlobally", broadcastGlobally);
		}
	};
}

#endif
