#ifndef MAKAILIB_SHADER_HANDLER_H
#define MAKAILIB_SHADER_HANDLER_H

#include "../anchors.hpp"
#include "gl_camera.hpp"
#include "slf_fileloader.hpp"

#ifdef GL_ES_VERSION_2_0
#define GLSL_VERSION "#version 100\n\n"
#else
#define GLSL_VERSION "#version 410\n\n"
#endif

#define SHADER_NULL ""

#define shaderTypeId(SHADER_TYPE_NAME) shaderTypes.find(SHADER_TYPE_NAME)->second

namespace Scene {
	glm::mat4 world = glm::mat4(1.0f);
	Camera::Camera3D camera;
}

namespace Shader {
	namespace {
		using namespace std;
		using namespace FileLoader;

		struct _UniSet {
			string name;
			GLuint id;

			_UniSet(string name, GLuint id) {
				this->name	= name;
				this->id	= id;
			}

			void operator()(bool value) const
			{
				glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
			}

			void operator()(int value) const
			{
				glUniform1i(glGetUniformLocation(id, name.c_str()), value);
			}

			void operator()(unsigned int value) const
			{
				glUniform1ui(glGetUniformLocation(id, name.c_str()), value);
			}

			void operator()(float value) const
			{
				glUniform1f(glGetUniformLocation(id, name.c_str()), value);
			}

			void operator()(glm::vec2 value) const
			{
				glUniform2f(glGetUniformLocation(id, name.c_str()), value.x, value.y);
			}

			void operator()(glm::vec3 value) const
			{
				glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
			}

			void operator()(glm::vec4 value) const
			{
				glUniform4f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z, value.w);
			}

			void operator()(glm::mat4 value) const
			{
				glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
			}
		};
	}

	const map<string, GLuint> shaderTypes = {
		{"frag", GL_FRAGMENT_SHADER},
		{"vert", GL_VERTEX_SHADER},
		{"comp", GL_COMPUTE_SHADER},
		{"geom", GL_GEOMETRY_SHADER},
		{"tsct", GL_TESS_CONTROL_SHADER},
		{"tsev", GL_TESS_EVALUATION_SHADER}
	};

	/// Default Vertex Shader code.
	const string DEFAULT_VERTEX_SHADER =
		GLSL_VERSION
		"attribute vec2 coord2d;                  "
		"void main(void) {                        "
		"  gl_Position = vec4(coord2d, 0.0, 1.0); "
		"}";

	/// Default Fragment Shader code.
	const string DEFAULT_FRAGMENT_SHADER =
		GLSL_VERSION
		"void main(void) {        "
		"  gl_FragColor[0] = 1.0; "
		"  gl_FragColor[1] = 1.0; "
		"  gl_FragColor[2] = 1.0; "
		"}";

	class Shader {
	private:
		GLuint id;
		bool created = false;
		/// Similar to create, but internal.
		void attach(string code, GLuint shaderType) {
			// Compile shaders
			GLuint shader;
			int success;
			char infoLog[2048];
			const char* shaderCode = code.c_str();
			// Vertex Shader
			shader = glCreateShader(shaderType);
			glShaderSource(shader, 1, &shaderCode, NULL);
			glCompileShader(shader);
			// Log compile errors if any
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 2048, NULL, infoLog);
				throw runtime_error(string("Could not compile Shader!\n") + infoLog);
			};
			// Shader Program
			if (!created) id = glCreateProgram();
			glAttachShader(id, shader);
			glLinkProgram(id);
			// Log linking errors if any
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(id, 2048, NULL, infoLog);
				throw runtime_error(string("Could not link shader program!\n") + infoLog);
			}
			glDeleteShader(shader);
			created = true;
		}
	public:
		Shader() {}

		Shader(string vertexCode, string fragmentCode) {
			create(vertexCode, fragmentCode);
		}

		Shader(CSVData slfData) {
			create(slfData);
		}

		Shader(string code, GLuint shaderType) {
			create(code, shaderType);
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
			char infoLog[2048];
			if (vertexCode != SHADER_NULL) {
				const char* vShaderCode = vertexCode.c_str();
				// Vertex Shader
				vertex = glCreateShader(GL_VERTEX_SHADER);
				glShaderSource(vertex, 1, &vShaderCode, NULL);
				glCompileShader(vertex);
				// Log compile errors if any
				glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
				if (!success) {
					glGetShaderInfoLog(vertex, 2048, NULL, infoLog);
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
					glGetShaderInfoLog(fragment, 2048, NULL, infoLog);
					throw runtime_error(string("Could not compile Fragment Shader!\n") + infoLog);
				}
			}
			// Shader Program
			if (!created) id = glCreateProgram();
			if (vertex)		glAttachShader(id, vertex);
			if (fragment)	glAttachShader(id, fragment);
			glLinkProgram(id);
			// Log linking errors if any
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(id, 2048, NULL, infoLog);
				throw runtime_error(string("Could not link shader program!\n") + infoLog);
			}
			// Delete shaders
			if (vertex)		glDeleteShader(vertex);
			if (fragment)	glDeleteShader(fragment);
			return true;
		}

		/// Creates a shader from an SLF file and associates it to the object. Returns false if already created.
		bool create(CSVData slfData) {
			string dir = slfData[0];
			string log = "";
			string code;
			GLuint type;
			if (shaderTypes.find(slfData[1]) == shaderTypes.end()) {
				for (size_t i = 1; i < slfData.size(); i += 2) {
					code = loadTextFile(dir + slfData[i]);
					type = shaderTypeId(slfData[i+1]);
					try {
						attach(code, type);
					} catch (runtime_error err) {
						log += string("\n[[ Error on shader '") + dir + slfData[i] + "' ]]:\n";
						log += err.what();
					}
				}
			} else {
				type = shaderTypeId(slfData[1]);
				for (size_t i = 2; i < slfData.size(); i++) {
					code = loadTextFile(dir + slfData[i]);
					try {
						attach(code, type);
					} catch (runtime_error err) {
						log += string("\n[[ Error on shader '") + dir + slfData[i] + "' ]]:\n";
						log += err.what();
					}
				}
			}
			if (log != "") {
				throw runtime_error(log);
			}
			return true;
		}

		/// Creates a shader from a given shader code, and a shader type  and associates it to the object. Returns false if already created.
		bool create(string code, GLuint shaderType) {
			if (created) return false;
			else created = true;
			// Compile shaders
			GLuint shader;
			int success;
			char infoLog[2048];
			const char* shaderCode = code.c_str();
			// Vertex Shader
			shader = glCreateShader(shaderType);
			glShaderSource(shader, 1, &shaderCode, NULL);
			glCompileShader(shader);
			// Log compile errors if any
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 2048, NULL, infoLog);
				throw runtime_error(string("Could not compile Shader!\n") + infoLog);
			};
			// Shader Program
			if (!created) id = glCreateProgram();
			glAttachShader(id, shader);
			glLinkProgram(id);
			// Log linking errors if any
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(id, 2048, NULL, infoLog);
				throw runtime_error(string("Could not link shader program!\n") + infoLog);
			}
			glDeleteShader(shader);
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
			for (unsigned char i = 0; i < 32; i++)
			if (textures[i]) {
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, textures[i]);
			}
			glUseProgram(id);
		}

		/**
		* The way to set uniforms.
		* Done like this: SHADER[UNIFORM_NAME](UNIFORM_VALUE);
		*/
		_UniSet operator[](const string& name) {
			_UniSet su(name, id);
			return su;
		}

		GLuint textures[32];
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

	typedef vector<Shader*> ShaderList;

	/// Converts an SLF file to a list of shader programs.
	ShaderList getShaderList(SLF::SLFData slfData) {
		ShaderList shaders;
		string dir = slfData[0];
		string log = "";
		string code;
		GLuint type;
		if (shaderTypes.find(slfData[1]) == shaderTypes.end()) {
			for (size_t i = 1; i < slfData.size(); i += 2) {
				code = loadTextFile(dir + slfData[i]);
				type = shaderTypeId(slfData[i+1]);
				try {
					shaders.push_back(new Shader(code, type));
				} catch (runtime_error err) {
					log += string("\n[[ Error on shader '") + dir + slfData[i] + "' ]]:\n";
					log += err.what();
				}
			}
		} else {
			for (size_t i = 2; i < slfData.size(); i++) {
				code = loadTextFile(dir + slfData[i]);
				type = shaderTypeId(slfData[1]);
				try {
					shaders.push_back(new Shader(code, type));
				} catch (runtime_error err) {
					log += string("\n[[ Error on shader '") + dir + slfData[i] + "' ]]:\n";
					log += err.what();
				}
			}
		}
		if (log != "") {
			throw runtime_error(log);
		}
		return shaders;
	}

	/// Executes a set of shaders sequentially.
	void multiShaderPass(ShaderList shaders, function<void()> action =[](){}) {
		for (Shader* s : shaders) {
			(*s)();
			action();
		}
	}

	Shader defaultShader;
}
#endif // MAKAILIB_SHADER_HANDLER
