#ifndef MAKAILIB_SHADER_HANDLER_H
#define MAKAILIB_SHADER_HANDLER_H

#include "../anchors.hpp"
#include "gl_camera.hpp"
#include "slf_fileloader.hpp"

#define SHADER_NULL ""

#define shaderTypeId(SHADER_TYPE_NAME) shaderTypes.find(SHADER_TYPE_NAME)->second

namespace Scene {
	Matrix4x4 world = Matrix4x4::identity();
	Camera::Camera3D camera;
}

namespace Shader {
	namespace {
		using namespace std;
		using namespace FileLoader;
	}

	const Dictionary<GLuint> shaderTypes = {
		{"frag", GL_FRAGMENT_SHADER},
		{"vert", GL_VERTEX_SHADER},
		{"comp", GL_COMPUTE_SHADER},
		{"geom", GL_GEOMETRY_SHADER},
		{"tsct", GL_TESS_CONTROL_SHADER},
		{"tsev", GL_TESS_EVALUATION_SHADER}
	};

	struct Uniform {
		String const name;

		Uniform(String const& _name, GLuint const& _id):
			name(_name),
			id(_id),
			location(glGetUniformLocation(_id, _name.c_str()))
		{}

		void set(bool const& value) const {
			glUniform1i(getUniform(), (int)value);
		}

		void set(int const& value) const {
			glUniform1i(getUniform(), value);
		}

		void set(unsigned int const& value) const {
			glUniform1ui(getUniform(), value);
		}

		void set(float const& value) const {
			glUniform1f(getUniform(), value);
		}

		void set(Vector2 const& value) const {
			glUniform2f(getUniform(), value.x, value.y);
		}

		void set(Vector3 const& value) const {
			glUniform3f(getUniform(), value.x, value.y, value.z);
		}

		void set(Vector4 const& value) const {
			glUniform4f(getUniform(), value.x, value.y, value.z, value.w);
		}

		void set(Matrix3x3 const& value) const {
			glUniformMatrix3fv(getUniform(), 1, GL_FALSE, value.begin());
		}

		void set(Matrix4x4 const& value) const {
			glUniformMatrix4fv(getUniform(), 1, GL_FALSE, value.begin());
		}

		void setArray(int* const& values, size_t const& count) const {
			glUniform1iv(getUniform(), count, values);
		}

		void setArray(unsigned int* const& values, size_t const& count) const {
			glUniform1uiv(getUniform(), count, values);
		}

		void setArray(float* const& values, size_t const& count) const {
			glUniform1fv(getUniform(), count, values);
		}

		void setArray(Vector2* const& values, size_t const& count) const {
			for SSRANGE(i, 0, count)
				glUniform2f(getUniformArray(i), values[i].x, values[i].y);
		}

		void setArray(Vector3* const& values, size_t const& count) const {
			for SSRANGE(i, 0, count)
				glUniform3f(getUniformArray(i), values[i].x, values[i].y, values[i].z);
		}

		void setArray(Vector4* const& values, size_t const& count) const {
			for SSRANGE(i, 0, count)
				glUniform4f(getUniformArray(i), values[i].x, values[i].y, values[i].z, values[i].w);
		}

		template <typename T>
		void set(List<T> const& values) const {
			setArray((T*)values.data(), values.size());
			glUniform1ui(getUniform("Count"), values.size());
		}

		template <typename T, size_t S>
		void set(Span<T, S> const& values) const {
			setArray((T*)values.data(), values.size());
			glUniform1ui(getUniform("Count"), S);
		}

		template<typename... Args>
		void set(Args const&... args) const
		requires (sizeof...(Args) > 1) {
			(setSpecial(args), ...);
			offset = 0;
		}

		template <typename T>			void operator()(T const& value)				{set(value);	}
		template <typename... Args>		void operator()(Args const&... args)		{set(args...);	}
		template <typename T>			void operator()(List<T> const& values)		{set(values);	}
		template <typename T, size_t S>	void operator()(Span<T, S> const& values)	{set(values);	}

	private:
		constexpr Uniform(String  const& _name, GLuint const& _id, GLuint const& _location): name(_name), id(_id), location(_location) {}

		template<typename T>
		void setSpecial(T const& value) const {
			set(value);
			++offset;
		}

		template <typename T>
		void setSpecial(List<T> const& values) const {
			set(values);
			glUniform1ui(getUniform() + values.size() + 1, values.size());
			offset += values.size() + 1;
		}

		template <typename T, size_t S>
		void setSpecial(Span<T, S> const& values) const {
			set(values);
			glUniform1ui(getUniform() + S + 1, S);
			offset += S + 1;
		}

		GLuint getUniformArray(size_t const& offset) const {
			return location + this->offset + offset;
		}

		inline GLuint getUniform() const {
			return location + offset;
		}

		inline GLuint getUniform(String const& append) const {
			return glGetUniformLocation(id, (name + append).c_str()) + offset;
		}

		GLuint mutable offset = 0;
		GLuint const id;
		intmax const location;
	};

	class Shader {
	private:
		struct ShaderProgram {
			ShaderProgram()		{}
			ShaderProgram(bool)	{id = glCreateProgram();	}
			~ShaderProgram()	{glDeleteProgram(id);		}
			void create() {if (id == 0) id = glCreateProgram();}
			GLuint id = 0;
		};

		typedef StrongPointer<ShaderProgram> ShaderInstance;

		ShaderInstance instance;

		bool created = false;

		/// Similar to create, but internal.
		void attach(String code, GLuint shaderType) {
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
				throw Error::FailedAction(String("Could not compile Shader!\n") + infoLog);
			};
			// Shader Program
			instance->create();
			glAttachShader(instance->id, shader);
			glLinkProgram(instance->id);
			// Log linking errors if any
			glGetProgramiv(instance->id, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(instance->id, 2048, NULL, infoLog);
				throw Error::FailedAction(String("Could not link shader program!\n") + infoLog);
			}
			glDeleteShader(shader);
			created = true;
		}
	public:
		Shader() {
			instance.bind(new ShaderProgram());
		}

		Shader(String vertexCode, String fragmentCode) {
			create(vertexCode, fragmentCode);
		}

		Shader(CSVData slfData) {
			create(slfData);
		}

		Shader(String code, GLuint shaderType) {
			create(code, shaderType);
		}

		Shader(Shader const& other) {
			instance = other.instance;
		}

		Shader(Shader&& other) {
			instance = other.instance;
		}

		~Shader() {
			destroy();
		}

		/// Returns whether this object has a shader associated with it (i.e. "is created").
		inline bool isCreated() {
			return created;
		}

		/// Creates a shader and associates it to the object. Returns false if already created.
		bool create(String vertexCode, String fragmentCode) {
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
					throw Error::FailedAction(String("Could not compile Vertex Shader!\n") + infoLog);
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
					throw Error::FailedAction(String("Could not compile Fragment Shader!\n") + infoLog);
				}
			}
			// Shader Program
			instance->create();
			if (vertex)		glAttachShader(instance->id, vertex);
			if (fragment)	glAttachShader(instance->id, fragment);
			glLinkProgram(instance->id);
			// Log linking errors if any
			glGetProgramiv(instance->id, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(instance->id, 2048, NULL, infoLog);
				throw Error::FailedAction(String("Could not link shader program!\n") + infoLog + "\n\n\n Program:" + vertexCode + "\n\n\n" + fragmentCode);
			}
			// Delete shaders
			if (vertex)		glDeleteShader(vertex);
			if (fragment)	glDeleteShader(fragment);
			return true;
		}

		/// Creates a shader from an SLF file and associates it to the object. Returns false if already created.
		bool create(CSVData slfData) {
			if (created) return false;
			String dir = slfData[0];
			String log = "";
			String code;
			GLuint type;
			if (shaderTypes.find(slfData[1]) == shaderTypes.end()) {
				for (size_t i = 1; i < slfData.size(); i += 2) {
					code = getTextFile(dir + slfData[i]);
					type = shaderTypeId(slfData[i+1]);
					try {
						attach(code, type);
					} catch (Error::Error const& err) {
						log += String("\n[[ Error on shader '") + dir + slfData[i] + "' ]]:\n";
						log += err.what();
					}
				}
			} else {
				type = shaderTypeId(slfData[1]);
				for (size_t i = 2; i < slfData.size(); i++) {
					code = getTextFile(dir + slfData[i]);
					try {
						attach(code, type);
					} catch (Error::Error const& err) {
						log += String("\n[[ Error on shader '") + dir + slfData[i] + "' ]]:\n";
						log += err.what();
					}
				}
			}
			if (log != "") {
				throw Error::FailedAction(log);
			}
			created = true;
			return true;
		}

		/// Creates a shader from a given shader code, and a shader type  and associates it to the object. Returns false if already created.
		bool create(String code, GLuint shaderType) {
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
				throw Error::FailedAction(String("Could not compile Shader!\n") + infoLog);
			};
			// Shader Program
			instance->create();
			glAttachShader(instance->id, shader);
			glLinkProgram(instance->id);
			// Log linking errors if any
			glGetProgramiv(instance->id, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(instance->id, 2048, NULL, infoLog);
				throw Error::FailedAction(String("Could not link shader program!\n") + infoLog  + infoLog + "\n\n\n Program:" + code);
			}
			glDeleteShader(shader);
			return true;
		}

		void make(String vertexCode, String fragmentCode) {
			destroy();
			create(vertexCode, fragmentCode);
		}

		void make(CSVData slfData) {
			destroy();
			create(slfData);
		}

		void make(String code, GLuint shaderType) {
			destroy();
			create(code, shaderType);
		}

		/// Destroys the shader associated with this object, if any. Does not delete object.
		void destroy() {
			if (created) {
				instance.unbind();
				created = false;
			}
		}

		/// Operator overload.
		void operator()() {
			glUseProgram(instance->id);
		}

		/// Enables the shader object.
		void enable() {
			glUseProgram(instance->id);
		}

		/**
		* The way to set uniforms.
		* Done like this: SHADER.uniform(UNIFORM_NAME)(UNIFORM_VALUE);
		*/
		Uniform uniform(const String& name) {
			glUseProgram(instance->id);
			Uniform su(name, instance->id);
			return su;
		}

		/**
		* The way to set uniforms.
		* Done like this: SHADER[UNIFORM_NAME](UNIFORM_VALUE);
		*/
		Uniform operator[](const String& name) {
			return uniform(name);
		}

		Shader& operator=(Shader const& other) {
			instance = other.instance;
			return *this;
		}

		Shader& operator=(Shader&& other) {
			instance = other.instance;
			return *this;
		}
	};

	// TODO: Pipeline stuff

	Shader defaultShader;
}

#define MAIN_SHADER Shader::defaultShader

#endif // MAKAILIB_SHADER_HANDLER
