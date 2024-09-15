#ifndef MAKAILIB_CORE_APP_H
#define MAKAILIB_CORE_APP_H

#include "../ctl/ctl.hpp"

#include "extern.hpp"
#include "input/manager.hpp"
#include "../graph/graph.hpp"
#include "../audio/audio.hpp"

using namespace Makai;

namespace Makai {
	struct AudioConfig {
		Audio::Formats const& formats = {
			Audio::Format::AF_OGG,
			Audio::Format::AF_MP3
		};
		uint
			channels = 2,
			tracks = 16
		;
	};

	struct App {
	public:
		/// Initializes the application.
		App (
			unsigned int const&	width,
			unsigned int const&	height,
			String const&		windowTitle,
			bool const&			fullscreen	= false,
			AudioConfig const&	audio		= {}
		);

		enum AppState {
			AS_INVALID = -1,
			AS_CLOSED,
			AS_OPENING,
			AS_RUNNING,
			AS_CLOSING,
		};

		virtual ~App();

		/// Returns whether the user pressed the close button.
		static bool closeButtonPressed();

		/// Sets the window's title.
		void setWindowTitle(String const& title);

		/// Sets the application fullscreen.
		void setFullscreen(bool const& state = false);

		/// Loads the application's shaders from SLF files.
		void loadShaders(SLF::SLFData const& main, SLF::SLFData const& buffer);

		/// Runs the application.
		void run();

		/// Closes the application.
		void close();

		/// Returns whether the application is currently running.
		bool running();

		/// Returns the application's current state.
		AppState getState();

		/// Sets the application's window size.
		void setWindowSize(Vector2 const& size);

		/// Set the application window's opacity.
		void setWindowOpacity(float const& opacity = 1);

		void enableTransparentWindowBackground();

		/// Gets the current frame.
		usize getCurrentFrame();
		/// Gets the current cycle.
		usize getCurrentCycle();

		/// Gets the current cycle rate.
		usize getCycleRate();
		/// Gets the current cycle rate.
		usize getFrameRate();

		Graph::FrameBuffer& getFrameBuffer();
		Graph::FrameBuffer& getLayerBuffer();

		Vector2 getWindowSize();

		Vector2 getWindowScale();

		/// Gets called when the application is opened.
		virtual void onOpen() {}

		/// Gets called whenever the application is rendering to the screen.

		/// Happens before the screen is rendered, before the frame buffer is cleared.
		virtual void onDrawBegin()		{}
		/// Happens before the screen is rendered, after the frame buffer is cleared.
		virtual void onPostFrameClear()	{}
		/// Gets called when the application begins rendering a layer, before the the layer buffer is cleared.
		virtual void onLayerDrawBegin(usize const& layerID)	{}
		/// Gets called when the application begins rendering a layer, after the the layer buffer is cleared.
		virtual void onPostLayerClear(usize const& layerID)	{}
		/// Gets called when the application ends rendering a layer, before the layer buffer is drawn to the screen.
		virtual void onPreLayerDraw(usize const& layerID)	{pushLayerToFrame();}
		/// Gets called when the application ends rendering a layer, after the layer buffer is drawn to the screen.
		virtual void onLayerDrawEnd(usize const& layerID)	{}
		/// Happens after the screen is rendered, before the frame buffer is drawn to the screen.
		virtual void onPreFrameDraw()	{}
		/// Happens after the screen is rendered, after the frame buffer is drawn to the screen.
		virtual void onDrawEnd()		{}

		/// Gets called every frame, along all other logic.
		virtual void onLogicFrame(float delta)	{}

		/// Gets called when the application is closing. Happens before Window is terminated.
		virtual void onClose()	{}

		/// Queues a texture to recieve a copy of the screen.
		void queueScreenCopy(Graph::Texture2D target);

		/// Removes a texture from the screen copy queue.
		void unqueueScreenCopy(Graph::Texture2D target);

		/**
		Skips the drawing process of the current layer being drawn.
		Can only be used during onLayerDrawBegin().
		*/
		void skipDrawingThisLayer();

		/**
		Queues the current data in the layerbuffer to be pushed to the framebuffer.
		Can only be used during onPreLayerDraw().
		*/
		void pushLayerToFrame();

		/// The window's background color.
		Vector4 background = Graph::Color::BLACK;

		/// The application's input manager.
		Input::Manager input;

		/// The application's maximum framerate.
		float maxFrameRate = 30.0;

		/// The application's maximum "cycles per second" count.
		float maxCycleRate = 60.0;

		/// The application's speed scale.
		float speed = 1.0f;

		/// The application's taskers.
		Tasking::MultiTasker taskers;

		/// The application's notification handler.
		Event::Notifier notifier;

	protected:
		Graph::FrameBufferData toFrameBufferData();

	private:
		usize cycleRate = 0, frameRate = 0;

		bool
			skipLayer	= false,
			pushToFrame	= false
		;

		List<Graph::Texture2D> screenQueue;

		/// The application's main framebuffer.
		Graph::FrameBuffer framebuffer;
		Graph::FrameBuffer layerbuffer;

		/// The window's resolution.
		unsigned int width, height;

		/// Properly finishes application execution.
		void finalize();

		/// Draws the window.
		void render();

		void copyScreenToQueued();

		/// Frame counter.
		usize frame = 0;

		/// Cycle counter.
		usize cycle = 0;

		/// Current execution state.
		AppState state = AppState::AS_CLOSED;

		/// The application's window.
		Extern::Resource window;
	};

	/// Returns the currently-opened app. Returns null if no app is open.
	App* getOpenApp();
}

#endif // MAKAILIB_CORE_APP_H
