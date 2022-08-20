#ifndef MAKAILIB_SHADER_HANDLER
#define MAKAILIB_SHADER_HANDLER

#include "../anchors.hpp"

#ifdef GL_ES_VERSION_2_0
#define GLSL_VERSION "#version 100\n\n"
#else
#define GLSL_VERSION "#version 120\n\n"
#endif

#define SHADER_NULL ""

namespace Shader {
	namespace {
		using
		std::string,
		std::vector,
		std::regex,
		std::regex_replace,
		std::to_string,
		std::function,
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
			// Compile shaders
			GLuint vertex, fragment;
			int success;
			char infoLog[512];
			if (vertexCode != SHADER_NULL) {
				const char* vShaderCode = vertexCode.c_str();
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
			}
			// similiar for Fragment Shader
			if (fragmentCode != SHADER_NULL) {
				const char* fShaderCode = fragmentCode.c_str();
				fragment = glCreateShader(GL_FRAGMENT_SHADER);
				glShaderSource(fragment, 1, &fShaderCode, NULL);
				glCompileShader(fragment);

				// Log compile errors if any
				glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
				if (!success) {
					glGetShaderInfoLog(vertex, 512, NULL, infoLog);
					throw runtime_error(string("Could not compile Fragment Shader!\n") + infoLog);
				}
			}
			// Shader Program
			id = glCreateProgram();
			if (vertex)		glAttachShader(id, vertex);
			if (fragment)	glAttachShader(id, fragment);
			glLinkProgram(id);
			// Log linking errors if any
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(id, 512, NULL, infoLog);
				throw runtime_error(string("Could not link shader program(s)!\n") + infoLog);
			}
			// Delete shaders
			if (vertex)		glDeleteShader(vertex);
			if (fragment)	glDeleteShader(fragment);
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

	typedef vector<string> ShaderCodes;

	/// Combines a set of fragment shaders into a single shader.
	string composeShader(ShaderCodes fragShaders) {
		string result = GLSL_VERSION;
		string mainCode = "\n";
		mainCode += "void main(void) {\n";
		mainCode += "    vec4 SHADER_in, SHADER_out;\n";
		mainCode += "    SHADER_in = vec4(1, 1, 1, 1);\n";
		size_t shaderCount = 0;
		for (string frag : fragShaders) {
			string shaderID = "SHADER_fShader" + to_string(shaderCount++);
			string fragCode = regex_replace(
				frag,
				regex("fragment"),
				shaderID
			);
			result += fragCode + "\n";
			mainCode += "    " + shaderID;
			mainCode += "(SHADER_in, SHADER_out);\n";
			mainCode += "    SHADER_in = SHADER_out;\n";
		}
		mainCode +=
			"    gl_FragColor[0] = SHADER_out.x;\n"
			"    gl_FragColor[1] = SHADER_out.y;\n"
			"    gl_FragColor[2] = SHADER_out.z;\n"
			"    gl_FragColor[3] = SHADER_out.w;\n"
		;
		mainCode += "}\n";
		return result + mainCode;
	}

	typedef vector<Shader> ShaderList;

	/// Does a render action with a set of shaders sequentially.
	void multiShaderPass(function<void()> action, ShaderList shaders) {
		for (Shader s : shaders) {
			s();
			action();
		}
	}

	/// Executes a set of shaders sequentially.
	void multiShaderPass(ShaderList shaders) {
		for (Shader s : shaders)
			s();
	}

	Shader defaultShader;
}
#endif // MAKAILIB_SHADER_HANDLER
