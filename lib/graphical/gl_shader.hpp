#ifndef MAKAILIB_SHADER_HANDLER
#define MAKAILIB_SHADER_HANDLER

#include "../anchors.hpp"

#define GLSL_VERSION "#version 120\n"

namespace Shader {
	namespace {
		using
		std::string,
		std::runtime_error;
	}

	/// Default Vertex Shader code.
	const string DEFAULT_VERTEX_SHADER =
		GLSL_VERSION
		"attribute vec2 coord2d;                  "
		"void main(void) {                        "
		"  gl_Position = vec4(coord2d, 0.0, 1.0); "
		"}";

	/// Default Fragment Shader code.
	const string DEFAULT_FRAGMENT_SHADER =
		GLSL_VERSION  // OpenGL 2.1
		"void main(void) {        "
		"  gl_FragColor[0] = 1.0; "
		"  gl_FragColor[1] = 1.0; "
		"  gl_FragColor[2] = 1.0; "
		"}";

	class Shader {
	private:
		GLuint id;
		bool created = false;
	public:
		Shader() {}

		Shader(string vertexCode, string fragmentCode) {
			create(vertexCode, fragmentCode);
		}

		~Shader() {
			destroy();
		}

		/// Gets the attached shader program's ID.
		inline GLuint getID() {
			return id;
		}

		/// Returns whether this object has a shader associated with it (i.e. "is created").
		inline bool isCreated() {
			return created;
		}

		/// Creates a shader and associates it to the object. Returns false if already created.
		bool create(string vertexCode = DEFAULT_VERTEX_SHADER, string fragmentCode = DEFAULT_FRAGMENT_SHADER) {
			if (created) return false;
			else created = true;
			const char* vShaderCode = vertexCode.c_str();
			const char* fShaderCode = fragmentCode.c_str();
			// Compile shaders
			GLuint vertex, fragment;
			int success;
			char infoLog[512];
			// Vertex Shader
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);
			// Log compile errors if any
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);
				throw runtime_error(string("Could not compile Vertex Shader!\n") + infoLog);
			};

			// similiar for Fragment Shader
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);

			// Log compile errors if any
			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);
				throw runtime_error(string("Could not compile Fragment Shader!\n") + infoLog);
			}
			// Shader Program
			id = glCreateProgram();
			glAttachShader(id, vertex);
			glAttachShader(id, fragment);
			glLinkProgram(id);
			// Log linking errors if any
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(id, 512, NULL, infoLog);
				throw runtime_error(string("Could not link shader programs!\n") + infoLog);
			}
			// Delete shaders
			glDeleteShader(vertex);
			glDeleteShader(fragment);
			return true;
		}

		/// Destroys the shader associated with this object, if any. Does not delete object.
		void destroy() {
			if (created) {
				glDeleteProgram(id);
				created = false;
			}
		}

		/// Enables this object as shader.
		void operator()() {
			glUseProgram(id);
		}

		void operator()(const std::string& name, bool value) const
		{
			glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
		}

		void operator()(const std::string& name, int value) const
		{
			glUniform1i(glGetUniformLocation(id, name.c_str()), value);
		}

		void operator()(const std::string& name, unsigned int value) const
		{
			glUniform1ui(glGetUniformLocation(id, name.c_str()), value);
		}

		void operator()(const std::string& name, float value) const
		{
			glUniform1f(glGetUniformLocation(id, name.c_str()), value);
		}

		void operator()(const std::string& name, glm::vec2 value) const
		{
			glUniform2f(glGetUniformLocation(id, name.c_str()), value.x, value.y);
		}

		void operator()(const std::string& name, glm::vec3 value) const
		{
			glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
		}

		void operator()(const std::string& name, glm::vec4 value) const
		{
			glUniform4f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z, value.w);
		}
		/*
		void operator()(const std::string& name, glm::mat4 value) const
		{
			glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
		}*/
	};

	Shader defaultShader;
}
#endif // MAKAILIB_SHADER_HANDLER
