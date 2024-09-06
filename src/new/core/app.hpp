#ifndef MAKAILIB_CORE_APP_H
#define MAKAILIB_CORE_APP_H

#include "../ctl/ctl.hpp"

#include "extern.hpp"
#include "input/manager.hpp"
#include "../graph/graph.hpp"
#include "../audio/audio.hpp"

using namespace Makai;

namespace Makai {
	struct App {
	public:
		/// Initializes the program.
		App (
			unsigned int const& width,
			unsigned int const& height,
			String const& windowTitle,
			bool const& fullscreen = false,
			Audio::Formats const& formats = {Audio::Format::AF_OGG, Audio::Format::AF_MP3}
		);

		virtual ~App();

		/// Returns whether the user pressed the close button.
		static bool shouldClose();

		/// Sets the window's title.
		void setWindowTitle(String const& title);

		/// Sets the program fullscreen.
		void setFullscreen(bool const& state = false);

		/// Loads the program's shaders from SLF files.
		void loadShaders(SLF::SLFData const& main, SLF::SLFData const& buffer);

		/// Runs the program.
		void run();

		/// Closes the program.
		void close();

		/// Returns whether the program is currently running.
		bool running();

		/// Sets the program's window size.
		void setWindowSize(Vector2 const& size);

		/// Gets the current frame.
		usize getCurrentFrame();
		/// Gets the current cycle.
		usize getCurrentCycle();

		/// Gets the current cycle rate.
		usize getCycleRate();
		/// Gets the current cycle rate.
		usize getFrameRate();

		/// Renders the reserved layer.
		[[deprecated]]
		void renderReservedLayer();

		/// Sets an OpenGL flag.
		static void setGLFlag(usize const& flag, bool const& state = true);
		/// Sets an OpenGL value.
		static void setGLValue(usize const& flag, int const& value, bool const& state = true);

		Graph::FrameBuffer& getFrameBuffer();
		Graph::FrameBuffer& getLayerBuffer();

		Vector2 getWindowSize();

		Vector2 getWindowScale();

		/// Gets called when the program is opened.
		virtual void onOpen() {}

		/// Gets called whenever the program is rendering to the screen.

		/// Happens before the screen is rendered, before the frame buffer is cleared.
		virtual void onDrawBegin()		{}
		/// Happens before the screen is rendered, after the frame buffer is cleared.
		virtual void onPostFrameClear()	{}
		/// Gets called when the program begins rendering a layer, before the the layer buffer is cleared.
		virtual void onLayerDrawBegin(usize const& layerID)	{}
		/// Gets called when the program begins rendering a layer, after the the layer buffer is cleared.
		virtual void onPostLayerClear(usize const& layerID)	{}
		/// Gets called when the program ends rendering a layer, before the layer buffer is drawn to the screen.
		virtual void onPreLayerDraw(usize const& layerID)	{pushLayerToFrame();}
		/// Gets called when the program ends rendering a layer, after the layer buffer is drawn to the screen.
		virtual void onLayerDrawEnd(usize const& layerID)	{}
		/// Happens after the screen is rendered, before the frame buffer is drawn to the screen.
		virtual void onPreFrameDraw()	{}
		/// Happens after the screen is rendered, after the frame buffer is drawn to the screen.
		virtual void onDrawEnd()		{}

		/// Reserved Layer only.

		/// Gets called when the program begins rendering the reserved layer, before the the layer buffer is cleared.
		[[deprecated]]
		virtual void onReservedLayerDrawBegin()	{}
		/// Gets called when the program begins rendering the reserved layer, after the the layer buffer is cleared.
		[[deprecated]]
		virtual void onPostReservedLayerClear()	{}
		/// Gets called when the program ends rendering the reserved layer, before the layer buffer is drawn to the screen.
		[[deprecated]]
		virtual void onPreReservedLayerDraw()	{}
		/// Gets called when the program ends rendering the reserved layer, after the layer buffer is drawn to the screen.
		[[deprecated]]
		virtual void onReservedLayerDrawEnd()	{}

		/// Gets called every frame, along all other logic.
		virtual void onLogicFrame(float delta)	{}

		/// Gets called when the program is closing. Happens before Window is terminated.
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

		/// The window's clear color.
		Vector4 color = Graph::Color::BLACK;

		/// The program's input manager.
		Input::Manager input;

		/// The program's maximum framerate.
		float maxFrameRate = 30.0;

		/// The program's maximum "cycles per second" count.
		float maxCycleRate = 60.0;

		/// The program's speed scale.
		float speed = 1.0f;

		/// The program's taskers.
		Tasking::MultiTasker taskers;

		/// The program's notification handler.
		Event::Notifier notifier;

		/// Enables/disables OpenGL debug logs.
		void setGLDebug(bool const& state = false);

	protected:
		Graph::FrameBufferData toFrameBufferData();

	private:
		usize cycleRate = 0, frameRate = 0;

		bool
			skipLayer	= false,
			pushToFrame	= false
		;

		List<Graph::Texture2D> screenQueue;

		/// The program's main framebuffer.
		Graph::FrameBuffer framebuffer;
		Graph::FrameBuffer layerbuffer;

		/// The window's resolution.
		unsigned int width, height;

		/// Properly finishes program execution.
		void finalize();

		/// Draws the window.
		void render();

		void copyScreenToQueued();

		/// Frame counter.
		usize frame = 0;

		/// Cycle counter.
		usize cycle = 0;

		/// Current execution state.
		bool shouldRun = true;

		/// The program's window.
		Extern::Resource window;
	};

	/// Returns the currently-opened app. Returns null if no app is open.
	App* getOpenApp();
}

#endif // MAKAILIB_CORE_APP_H
