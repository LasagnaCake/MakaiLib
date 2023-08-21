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

			void operator()(bool value) const {
				glUniform1i(getUniform(), (int)value);
			}

			void operator()(int value) const {
				glUniform1i(getUniform(), value);
			}

			void operator()(unsigned int value) const {
				glUniform1ui(getUniform(), value);
			}

			void operator()(float value) const {
				glUniform1f(getUniform(), value);
			}

			void operator()(glm::vec2 value) const {
				glUniform2f(getUniform(), value.x, value.y);
			}

			void operator()(Vector2 value) const {
				glUniform2f(getUniform(), value.x, value.y);
			}

			void operator()(glm::vec3 value) const {
				glUniform3f(getUniform(), value.x, value.y, value.z);
			}

			void operator()(Vector3 value) const {
				glUniform3f(getUniform(), value.x, value.y, value.z);
			}

			void operator()(glm::vec4 value) const {
				glUniform4f(getUniform(), value.x, value.y, value.z, value.w);
			}

			void operator()(Vector4 value) const {
				glUniform4f(getUniform(), value.x, value.y, value.z, value.w);
			}

			void operator()(glm::mat4 value) const {
				glUniformMatrix4fv(getUniform(), 1, GL_FALSE, glm::value_ptr(value));
			}

			void operator()(int* const values, size_t count) {
				glUniform1iv(getUniform(), count, values);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(unsigned int* const values, size_t count) {
				glUniform1uiv(getUniform(), count, values);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(float* const values, size_t count) {
				glUniform1fv(getUniform(), count, values);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(Vector2* const values, size_t count) {
				for $ssrange(i, 0, count)
					glUniform2f(getUniformArray(i), values[i].x, values[i].y);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(glm::vec2* const values, size_t count) {
				for $ssrange(i, 0, count)
					glUniform2f(getUniformArray(i), values[i].x, values[i].y);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(Vector3* const values, size_t count) {
				for $ssrange(i, 0, count)
					glUniform3f(getUniformArray(i), values[i].x, values[i].y, values[i].z);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(glm::vec3* const values, size_t count) {
				for $ssrange(i, 0, count)
					glUniform3f(getUniformArray(i), values[i].x, values[i].y, values[i].z);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(Vector4* const values, size_t count) {
				for $ssrange(i, 0, count)
					glUniform4f(getUniformArray(i), values[i].x, values[i].y, values[i].z, values[i].w);
				glUniform1ui(getUniform("Count"), count);
			}

			void operator()(glm::vec4* const values, size_t count) {
				for $ssrange(i, 0, count)
					glUniform4f(getUniformArray(i), values[i].x, values[i].y, values[i].z, values[i].w);
				glUniform1ui(getUniform("Count"), count);
			}

			template <typename T>
			void operator()(vector<T> const& values) {
				(*this)((T*)values.data(), values.size());
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

<<<<<<< HEAD
	class ShaderModule {
	public:
		ShaderModule() {}

		ShaderModule(string const& code, GLuint shaderType) {
			create(code,shaderType);
		}

		~ShaderModule() {
			destroy();
		}

		void create(string const& code, GLuint shaderType) {
			if (created) return;
			created = true;
			// Get shader code
			const char* shaderCode = code.c_str();
			// Create & compile shader
			id = glCreateShader(shaderType);
			glShaderSource(id, 1, &shaderCode, NULL);
			glCompileShader(id);
			// Log compile errors if any
			int success;
			char infoLog[2048];
			glGetShaderiv(id, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(id, sizeof(infoLog), NULL, infoLog);
				throw Error::FailedAction(
					string("Could not compile shader!\n"),
					__FILE__,
					toString(__LINE__),
					"ShaderModule::create()",
					string(infoLog)
				);;
			};
		}

		void destroy() {
			if (!created) return;
			created = false;
			glDeleteShader(id);
			//detachFromAll();
			id = 0;
		}

		inline size_t getID() const {
			return id;
		}

		void operator()(GLuint program) {
			attachTo(program);
		}

		void attachTo(GLuint program) {
			if (!created) return;
			if ((!enabled) && fallback) fallback->attachTo(program);
			glAttachShader(program, id);
			//programs.push_back(program);
		}

		void detachFrom(GLuint program) {
			if (!created) return;
			glDetachShader(program, id);
			//std::erase_if(programs, [&](auto& e){return e == program;});
		}

		/*
		void detachFromAll() {
			for (auto& p: programs)
				glDetachShader(p, id);
			programs.clear();
		}
		*/

		ShaderModule* fallback = nullptr;

		bool enabled = true;
	private:
		bool created = false;

		GLuint links = 0;

		//List<GLuint> programs;
		GLuint id = 0;

		friend class Shader;
	};

	typedef List<ShaderModule*> ModuleList;

=======
>>>>>>> parent of e42a948d (Shader System Change BEGIN)
	class Shader {
	private:
		GLuint id;
		bool created = false;
<<<<<<< HEAD

		FuzzyHashMap<string, ShaderModule*> modules;
=======
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
				throw Error::FailedAction(string("Could not compile Shader!\n") + infoLog);
			};
			// Shader Program
			if (!created) id = glCreateProgram();
			glAttachShader(id, shader);
			glLinkProgram(id);
			// Log linking errors if any
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(id, 2048, NULL, infoLog);
				throw Error::FailedAction(string("Could not link shader program!\n") + infoLog);
			}
			glDeleteShader(shader);
			created = true;
		}
>>>>>>> parent of e42a948d (Shader System Change BEGIN)
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

<<<<<<< HEAD
		inline FuzzyHashMap<string, ShaderModule*> getModules() {
			return modules;
		}

=======
>>>>>>> parent of e42a948d (Shader System Change BEGIN)
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
					throw Error::FailedAction(string("Could not compile Vertex Shader!\n") + infoLog);
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
					throw Error::FailedAction(string("Could not compile Fragment Shader!\n") + infoLog);
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
				throw Error::FailedAction(string("Could not link shader program!\n") + infoLog + "\n\n\n Program:" + vertexCode + "\n\n\n" + fragmentCode);
			}
			// Delete shaders
			if (vertex)		glDeleteShader(vertex);
			if (fragment)	glDeleteShader(fragment);
			return true;
		}

		/// Creates a shader from an SLF file and associates it to the object. Returns false if already created.
		bool create(CSVData slfData) {
			if (created) return false;
			string dir = slfData[0];
			string log = "";
			string code;
			GLuint type;
<<<<<<< HEAD
			ModuleList newModules;
			if (shaderTypes.find(moduleData[1]) == shaderTypes.end()) {
				for (size_t i = 1; i < moduleData.size(); i += 2) {
					$debug(moduleData[i]);
					code = loadTextFile(dir + moduleData[i]);
					type = shaderTypeId(moduleData[i+1]);
					try {
						newModules.push_back(addModule(moduleData[i], code, type));
=======
			if (shaderTypes.find(slfData[1]) == shaderTypes.end()) {
				for (size_t i = 1; i < slfData.size(); i += 2) {
					code = loadTextFile(dir + slfData[i]);
					type = shaderTypeId(slfData[i+1]);
					try {
						attach(code, type);
>>>>>>> parent of e42a948d (Shader System Change BEGIN)
					} catch (Error::Error err) {
						log += string("\n[[ Error on shader '") + dir + slfData[i] + "' ]]:\n";
						log += err.what();
					}
				}
			} else {
<<<<<<< HEAD
				type = shaderTypeId(moduleData[1]);
				for (size_t i = 2; i < moduleData.size(); i++) {
					$debug(moduleData[i]);
					code = loadTextFile(dir + moduleData[i]);
					try {
						newModules.push_back(addModule(moduleData[i], code, type));
=======
				type = shaderTypeId(slfData[1]);
				for (size_t i = 2; i < slfData.size(); i++) {
					code = loadTextFile(dir + slfData[i]);
					try {
						attach(code, type);
>>>>>>> parent of e42a948d (Shader System Change BEGIN)
					} catch (Error::Error err) {
						log += string("\n[[ Error on shader '") + dir + slfData[i] + "' ]]:\n";
						log += err.what();
					}
				}
			}
			if (log != "") {
				throw Error::FailedAction(log);
			}
<<<<<<< HEAD
			return newModules;
		}

		/// Creates a shader module from a given shader code, and a shader type and associates it to the object.
		ShaderModule* addModule(string const& moduleName, string const& code, GLuint const& shaderType) {
			if (!created) return nullptr;
			ShaderModule* newModule = new ShaderModule(code, shaderType);
			modules[moduleName] = newModule;
			newModule->links++;
			return newModule;
		}

		/// Associates an existing shader module to this object.
		void addModule(string const& moduleName, ShaderModule* const& module) {
			modules[moduleName] = module;
			module->links++;
		}

		/// Removes a given shader module by its pointer value.
		void removeModule(ShaderModule* const& module) {
			std::erase_if(modules, [&](auto& e) {
				auto& [mName, mPtr] = e;
				if (mPtr == module) {
					mPtr->links--;
					if (!mPtr->links)
						delete mPtr;
					return true;
				}
				return false;
			});
		}

		/// Removes a given shader module by its name.
		void removeModule(string const& module) {
			std::erase_if(modules, [&](auto& e) {
				auto& [mName, mPtr] = e;
				if (mName == module) {
					mPtr->links--;
					if (!mPtr->links)
						delete mPtr;
					return true;
				}
				return false;
			});
		}

		void create() {
			if (created) return;
=======
>>>>>>> parent of e42a948d (Shader System Change BEGIN)
			created = true;
			return true;
		}

<<<<<<< HEAD
		ModuleList create(CSVData const& moduleData) {
			if (created) return ModuleList();
			created = true;
			$debug("\n\n<Creating Shader...>\n");
			id = glCreateProgram();
			return addModules(moduleData);
=======
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
				throw Error::FailedAction(string("Could not compile Shader!\n") + infoLog);
			};
			// Shader Program
			if (!created) id = glCreateProgram();
			glAttachShader(id, shader);
			glLinkProgram(id);
			// Log linking errors if any
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(id, 2048, NULL, infoLog);
				throw Error::FailedAction(string("Could not link shader program!\n") + infoLog  + infoLog + "\n\n\n Program:" + code);
			}
			glDeleteShader(shader);
			return true;
>>>>>>> parent of e42a948d (Shader System Change BEGIN)
		}

		/// Destroys the shader associated with this object, if any. Does not delete object.
		void destroy() {
<<<<<<< HEAD
			if (!created) return;
			created = true;
			glDeleteProgram(id);
			for(auto [_, m]: modules) {
				m->links--;
				if (!m->links)
					delete m;
			}
			modules.clear();
=======
			if (created) {
				glDeleteProgram(id);
				created = false;
			}
>>>>>>> parent of e42a948d (Shader System Change BEGIN)
		}

		/// Operator overload.
		void operator()() {
			glUseProgram(id);
		}

		/// Enables the shader object.
		void enable() {
<<<<<<< HEAD
			if (!created) return;
			for(auto [_, m]: modules) {
				m->attachTo(id);
			}
			// Check if linking was a success
			int success;
			char infoLog[2048];
			glLinkProgram(id);
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(id, sizeof(infoLog), NULL, infoLog);
				throw Error::FailedAction(
					string("Could not link shader program!\n"),
					__FILE__,
					toString(__LINE__),
					"Shader::enable()",
					string(infoLog)
				);
			}
			glUseProgram(id);
		}

		/// Disables the shader object.
		void disable() {
			if (!created) return;
			for(auto [_, m]: modules)
				m->detachFrom(id);
			glUseProgram(0);
		}

=======
			glUseProgram(id);
		}

>>>>>>> parent of e42a948d (Shader System Change BEGIN)
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

	// TODO: Pipeline stuff

	Shader defaultShader;
}

#define $shd Shader::

#define $mainshader Shader::defaultShader

#endif // MAKAILIB_SHADER_HANDLER
