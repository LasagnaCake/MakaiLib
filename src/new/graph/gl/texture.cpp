#define GLEW_STATIC
#include <GLEW/include/GL/glew.h>
#include <GLEW/include/GL/wglew.h>
#include <GL/gl.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

#include "texture.hpp"

uint createCopyBuffer() {
	DEBUGLN("Creating copy buffer...");
	unsigned int id = 0;
	glGenFramebuffers(1, &id);
	return id;
}

void copyTexture(
	Image2D* src,
	Image2D* dst,
	unsigned int srcStartX,
	unsigned int srcStartY,
	unsigned int srcEndX,
	unsigned int srcEndY,
	unsigned int dstStartX,
	unsigned int dstStartY,
	unsigned int dstEndX,
	unsigned int dstEndY,
	ImageFilter filter = ImageFilter::IF_NEAREST
) {
	if (!src || !dst) return;
	static uint const fb = createCopyBuffer();
	DEBUGLN("Binding copy buffer...");
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	DEBUGLN("Binding source...");
	glFramebufferTexture2D(
		GL_READ_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		src->getID(),
		0
	);
	DEBUGLN("Binding destination...");
	glFramebufferTexture2D(
		GL_DRAW_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D,
		dst->getID(),
		0
	);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	DEBUGLN((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE ? "OK" : "ERR"));
	DEBUGLN("Copying textures...");
	glBlitFramebuffer(
		srcStartX, srcStartY, srcEndX, srcEndY,
		dstStartX, dstStartY, dstEndX, dstEndY,
		GL_COLOR_BUFFER_BIT, filter
	);
	DEBUGLN("Finalizing...");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
