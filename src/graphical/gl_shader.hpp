#ifndef MAKAILIB_SHADER_HANDLER_H
#define MAKAILIB_SHADER_HANDLER_H

#include "../anchors.hpp"
#include "gl_camera.hpp"
#include "slf_fileloader.hpp"

#define SHADER_NULL ""

#define shaderTypeId(SHADER_TYPE_NAME) shaderTypes.find(SHADER_TYPE_NAME)->second

namespace Scene {
	glm::mat4 world = glm::mat4(1.0f);
	Camera::Camera3D camera;
}

#define $scn	Scene::

namespace Shader {
	namespace {
		using namespace std;
		using namespace FileLoader;
		using namespace Vector;

		struct _UniSet {
			string name;
			GLuint id;

			_UniSet(string name, GLuint id) {
				this->name	= name;
				this->id	= id;
			}

			void operator()(bool value) const
			{
				glUniform1i(getUniform(), (int)value);
			}

			void operator()(int value) const
			{
				glUniform1i(getUniform(), value);
			}

			void operator()(unsigned int value) const
			{
				glUniform1ui(getUniform(), value);
			}

			void operator()(float value) const
			{
				glUniform1f(getUniform(), value);
			}

			void operator()(glm::vec2 value) const
			{
				glUniform2f(getUniform(), value.x, value.y);
			}

			void operator()(Vector2 value) const
			{
				glUniform2f(getUniform(), value.x, value.y);
			}

			void operator()(glm::vec3 value) const
			{
				glUniform3f(getUniform(), value.x, value.y, value.z);
			}

			void operator()(Vector3 value) const
			{
				glUniform3f(getUniform(), value.x, value.y, value.z);
			}

			void operator()(glm::vec4 value) const
			{
				glUniform4f(getUniform(), value.x, value.y, value.z, value.w);
			}

			void operator()(Vector4 value) const
			{
				glUniform4f(getUniform(), value.x, value.y, value.z, value.w);
			}

			void operator()(glm::mat4 value) const
			{
				glUniformMatrix4fv(getUniform(), 1, GL_FALSE, glm::value_ptr(value));
			}

			void operator()(int* values, size_t count) {
				glUniform1iv(getUniform(), count, values);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(unsigned int* values, size_t count) {
				glUniform1uiv(getUniform(), count, values);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(float* values, size_t count) {
				glUniform1fv(getUniform(), count, values);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(Vector2* values, size_t count) {
				for $ssrange(i, 0, count)
					glUniform2f(getUniformArray(i), values[i].x, values[i].y);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(glm::vec2* values, size_t count) {
				for $ssrange(i, 0, count)
					glUniform2f(getUniformArray(i), values[i].x, values[i].y);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(Vector3* values, size_t count) {
				for $ssrange(i, 0, count)
					glUniform3f(getUniformArray(i), values[i].x, values[i].y, values[i].z);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(glm::vec3* values, size_t count) {
				for $ssrange(i, 0, count)
					glUniform3f(getUniformArray(i), values[i].x, values[i].y, values[i].z);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(Vector4* values, size_t count) {
				for $ssrange(i, 0, count)
					glUniform4f(getUniformArray(i), values[i].x, values[i].y, values[i].z, values[i].w);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(glm::vec4* values, size_t count) {
				for $ssrange(i, 0, count)
					glUniform4f(getUniformArray(i), values[i].x, values[i].y, values[i].z, values[i].w);
				glUniform1ui(getUniform("Count"), count);
			}

			template <typename T>
			void operator()(vector<T>& values) {
				(*this)(values.data(), values.size());
			}

		private:
			GLuint getUniformArray(size_t index) {
				auto cname = (name + "[" + std::to_string(index) + "]").c_str();
				return glGetUniformLocation(id, cname);
			}

			inline GLuint getUniform(string append = "") const {
				return glGetUniformLocation(id, (name + append).c_str());
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
		bool create(string vertexCode, string fragmentCode) {
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
				throw runtime_error(string("Could not link shader program!\n") + infoLog + "\n\n\n Program:" + vertexCode + "\n\n\n" + fragmentCode);
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
				throw runtime_error(string("Could not link shader program!\n") + infoLog  + infoLog + "\n\n\n Program:" + code);
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

		/// Operator overload.
		void operator()() {
			glUseProgram(id);
		}

		/// Enables the shader object.
		void enable() {
			glUseProgram(id);
		}

		/**
		* The way to set uniforms.
		* Done like this: SHADER.uniform(UNIFORM_NAME)(UNIFORM_VALUE);
		*/
		_UniSet uniform(const string& name) {
			glUseProgram(id);
			_UniSet su(name, id);
			return su;
		}

		/**
		* The way to set uniforms.
		* Done like this: SHADER[UNIFORM_NAME](UNIFORM_VALUE);
		*/
		_UniSet operator[](const string& name) {
			return uniform(name);
		}
	};

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

#define $shd Shader::

#define $mainshader Shader::defaultShader

#endif // MAKAILIB_SHADER_HANDLER
