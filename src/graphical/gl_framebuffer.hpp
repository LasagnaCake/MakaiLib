#ifndef MAKAI_FRAME_BUFFERRING_H
#define MAKAI_FRAME_BUFFERRING_H

namespace Buffer {
	class FrameBuffer {
	public:
		FrameBuffer(
			unsigned int width	= 800,
			unsigned int height	= 600
		) {
			glGenFramebuffers(1, &id);
			glGenTextures(1, &texture.color);
			glBindTexture(GL_TEXTURE_2D, texture.color);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA16F,
				width,
				height,
				0,
				GL_RGBA16F,
				GL_FLOAT,
				NULL
			);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glGenTextures(1, &texture.depth);
			glBindTexture(GL_TEXTURE_2D, texture.depth);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_DEPTH24_STENCIL8,
				width,
				height,
				0,
				GL_UNSIGNED_INT_24_8,
				GL_FLOAT,
				NULL
			);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.color, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture.depth, 0);
		}
		~FrameBuffer() {
			glDeleteFramebuffers(1, &id);
		}

		void operator()() {
			glBindFramebuffer(GL_FRAMEBUFFER, id);
		}

	private:
		unsigned int id;
		struct {
			unsigned int color;
			unsigned int depth;
		} texture;
	};
}

#endif // MAKAI_FRAME_BUFFERRING_H
