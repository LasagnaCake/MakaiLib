#ifndef MAKAILIB_CORE_APP_H
#define MAKAILIB_CORE_APP_H

#include "../ctl/ctl.hpp"

#include "program.hpp"
#include "input/manager.hpp"

namespace Makai {
	struct Program {
	public:
		/// Initializes the program.
		Program (
			unsigned int width,
			unsigned int height,
			String windowTitle,
			bool fullscreen = false,
			bool useMIDI = false,
			String bufferShaderPath = "shaders/framebuffer/compose.slf",
			String mainShaderPath = "shaders/base/base.slf"
		);

		virtual ~Program() {}

		/// Sets the window's title.
		inline void setWindowTitle(String windowTitle);

		/// Runs the program.
		void run();

		/// Sets the program fullscreen.
		void setFullscreen(bool state = false);

		/// Closes the program.
		void close();

		/// Returns whether the program is currently running.
		inline bool	running();

		/// Sets the program's window size.
		void setWindowSize(Vector2 size);

		/// Gets the current frame.
		size_t getCurrentFrame();
		/// Gets the current cycle.
		size_t getCurrentCycle();

		/// Gets the current cycle rate.
		size_t getCycleRate();
		/// Gets the current cycle rate.
		size_t getFrameRate();

		/// Renders the reserved layer.
		inline void renderReservedLayer();

		inline void setFlag(usize const& flag, bool const& state = true);
		inline void setValue(usize const& flag, int const& value, bool const& state = true);

		inline void enableMainBuffer();

		constexpr inline Drawer::FrameBuffer& getFrameBuffer();
		constexpr inline Drawer::FrameBuffer& getLayerBuffer();

		inline Vector2 getWindowSize();

		inline Vector2 getWindowScale();

		/// Gets called when the program is opened.
		virtual void onOpen() {}

		/// Gets called whenever the program is rendering to the screen.

		/// Happens before the screen is rendered, before the frame buffer is cleared.
		virtual void onDrawBegin()		{}
		/// Happens before the screen is rendered, after the frame buffer is cleared.
		virtual void onPostFrameClear()	{}
		/// Gets called when the program begins rendering a layer, before the the layer buffer is cleared.
		virtual void onLayerDrawBegin(size_t layerID)	{}
		/// Gets called when the program begins rendering a layer, after the the layer buffer is cleared.
		virtual void onPostLayerClear(size_t layerID)	{}
		/// Gets called when the program ends rendering a layer, before the layer buffer is drawn to the screen.
		virtual void onPreLayerDraw(size_t layerID)		{pushLayerToFrame();}
		/// Gets called when the program ends rendering a layer, after the layer buffer is drawn to the screen.
		virtual void onLayerDrawEnd(size_t layerID)		{}
		/// Happens after the screen is rendered, before the frame buffer is drawn to the screen.
		virtual void onPreFrameDraw()	{}
		/// Happens after the screen is rendered, after the frame buffer is drawn to the screen.
		virtual void onDrawEnd()		{}

		/// Reserved Layer only.

		/// Gets called when the program begins rendering the reserved layer, before the the layer buffer is cleared.
		virtual void onReservedLayerDrawBegin()	{}
		/// Gets called when the program begins rendering the reserved layer, after the the layer buffer is cleared.
		virtual void onPostReservedLayerClear()	{}
		/// Gets called when the program ends rendering the reserved layer, before the layer buffer is drawn to the screen.
		virtual void onPreReservedLayerDraw()	{}
		/// Gets called when the program ends rendering the reserved layer, after the layer buffer is drawn to the screen.
		virtual void onReservedLayerDrawEnd()	{}

		/// Gets called every frame, along all other logic.
		virtual void onLogicFrame(float delta)	{}

		/// Gets called when the program is closing. Happens before Window is terminated.
		virtual void onClose()	{}

		/// Queues a texture to recieve a copy of the screen.
		void queueScreenCopy(Drawer::Texture2D target);

		/// Removes a texture from the screen copy queue.
		void unqueueScreenCopy(Drawer::Texture2D target);

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
		Vector4 color = Color::BLACK;

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

	protected:
		Drawer::FrameBufferData toFrameBufferData();

	private:
		size_t cycleRate = 0, frameRate = 0;

		bool
			skipLayer	= false,
			pushToFrame	= false
		;

		List<Drawer::Texture2D> screenQueue;

		/// The program's main framebuffer.
		Drawer::FrameBuffer framebuffer;
		Drawer::FrameBuffer layerbuffer;

		/// The window's resolution.
		unsigned int width, height;

		/// Properly finishes program execution.
		void terminate();

		/// Draws the window.
		void render();

		void copyScreenToQueued();

		/// Frame counter.
		size_t frame = 0;

		/// Cycle counter.
		size_t cycle = 0;

		/// Current execution state.
		bool shouldRun = true;

		/// The program's window.
		void* window;
	};
}

#endif // MAKAILIB_CORE_APP_H
