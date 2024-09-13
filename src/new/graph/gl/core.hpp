#ifndef MAKAILIB_GRAPH_CORE_H
#define MAKAILIB_GRAPH_CORE_H

#include "../../ctl/ctl.hpp"

namespace Makai::Graph::API {
	namespace Debug {
		struct Context {
			Context(String const& name);
			~Context();

			void append(String const& message);

			String getMessage();

			String report();
		private:
			String message;
			String const name;
			String const indent;
		};

		void addEntry(String const& type);
		void addEntry(String const& type, String const& value);
	}

	void open();

	bool hasRequiredVersion();

	String name();

	enum class Facility {
		GAF_DEPTH_TEST,
		GAF_BLEND,
		GAF_DEBUG

	};

	void toggle(Facility const& fac, bool const& state);

	template<typename... Args>
	inline void toggle(bool const& state, Args const&... facs)
	requires (... && Type::Equal<Args, Facility>) {
		(..., toggle(facs, state));
	}

	enum class Buffer {
		GAB_COLOR,
		GAB_DEPTH,
		GAB_STENCIL
	};

	void clear(Buffer const& buffer);

	template<typename... Args>
	inline void clear(Args const&... buffers)
	requires (... && Type::Equal<Args, Buffer>) {
		(..., clear(buffers));
	}

	void beginRender();
	void endRender();

	void setClearColor(Vector4 const& color);

	void setFrontFace(bool const& clockwise = true);
}

#endif // MAKAILIB_GRAPH_CORE_H
