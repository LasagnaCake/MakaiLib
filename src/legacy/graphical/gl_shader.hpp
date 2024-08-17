#ifndef MAKAILIB_SHADER_HANDLER_H
#define MAKAILIB_SHADER_HANDLER_H

#include "../anchors.hpp"
#include "gl_camera.hpp"
#include "slf_fileloader.hpp"

#define SHADER_NULL ""

namespace Scene {
	Matrix4x4 world = Matrix4x4::identity();
	Camera::Camera3D camera;
}

namespace Shader {
	namespace {
		using namespace std;
		using namespace FileLoader;
	}

	typedef SLF::ShaderType ShaderType;

	constexpr GLuint getGLShaderType(ShaderType const& type) {
		switch (type) {
			default:
			case ShaderType::ST_INVALID:	return GL_FALSE;
			case ShaderType::ST_FRAGMENT:	return GL_FRAGMENT_SHADER;
			case ShaderType::ST_VERTEX:		return GL_VERTEX_SHADER;
			case ShaderType::ST_COMPUTE:	return GL_COMPUTE_SHADER;
			case ShaderType::ST_GEOMETRY:	return GL_GEOMETRY_SHADER;
			case ShaderType::ST_TESS_CTRL:	return GL_TESS_CONTROL_SHADER;
			case ShaderType::ST_TESS_EVAL:	return GL_TESS_EVALUATION_SHADER;
		}
		return GL_FALSE;
	}

	struct Uniform {
		String const name;

		Uniform(String const& _name, GLuint const& _id):
			name(_name),
			id(_id),
			location(glGetUniformLocation(_id, _name.c_str()))
		{}

		void set(bool const& value, size_t const& offset = 0) const {
			this->offset = 0;
			glUniform1i(getUniform() + offset, (int)value);
			++this->offset;
		}

		void set(int const& value, size_t const& offset = 0) const {
			this->offset = 0;
			glUniform1i(getUniform() + offset, value);
			++this->offset;
		}

		void set(unsigned int const& value, size_t const& offset = 0) const {
			this->offset = 0;
			glUniform1ui(getUniform() + offset, value);
			++this->offset;
		}

		void set(float const& value, size_t const& offset = 0) const {
			this->offset = 0;
			glUniform1f(getUniform() + offset, value);
		}

		void set(Vector2 const& value, size_t const& offset = 0) const {
			this->offset = 0;
			glUniform2f(getUniform() + offset, value.x, value.y);
			++this->offset;
		}

		void set(Vector3 const& value, size_t const& offset = 0) const {
			this->offset = 0;
			glUniform3f(getUniform() + offset, value.x, value.y, value.z);
			++this->offset;
		}

		void set(Vector4 const& value, size_t const& offset = 0) const {
			this->offset = 0;
			glUniform4f(getUniform() + offset, value.x, value.y, value.z, value.w);
			++this->offset;
		}

		void set(Matrix3x3 const& value, size_t const& offset = 0) const {
			this->offset = 0;
			glUniformMatrix3fv(getUniform() + offset, 1, GL_FALSE, value.begin());
			++this->offset;
		}

		void set(Matrix4x4 const& value, size_t const& offset = 0) const {
			this->offset = 0;
			glUniformMatrix4fv(getUniform() + offset, 1, GL_FALSE, value.begin());
		}

		void setArray(int* const& values, size_t const& count, size_t const& offset = 0) const {
			this->offset = 0;
			glUniform1iv(getUniform() + offset, count, values);
			this->offset = count;
		}

		void setArray(unsigned int* const& values, size_t const& count, size_t const& offset = 0) const {
			this->offset = 0;
			glUniform1uiv(getUniform() + offset, count, values);
			this->offset = count;
		}

		void setArray(float* const& values, size_t const& count, size_t const& offset = 0) const {
			this->offset = 0;
			glUniform1fv(getUniform() + offset, count, values);
			this->offset = count;
		}

		void setArray(Vector2* const& values, size_t const& count, size_t const& offset = 0) const {
			this->offset = 0;
			for SRANGE(i, 0, count)
				glUniform2f(getUniformArray(i) + offset, values[i].x, values[i].y);
			this->offset = count;
		}

		void setArray(Vector3* const& values, size_t const& count, size_t const& offset = 0) const {
			this->offset = 0;
			for SRANGE(i, 0, count)
				glUniform3f(getUniformArray(i)+ offset, values[i].x, values[i].y, values[i].z);
			this->offset = count;
		}

		void setArray(Vector4* const& values, size_t const& count, size_t const& offset = 0) const {
			this->offset = 0;
			for SRANGE(i, 0, count)
				glUniform4f(getUniformArray(i)+ offset, values[i].x, values[i].y, values[i].z, values[i].w);
			this->offset = count;
		}

		template <typename T>
		void set(List<T> const& values, size_t const& offset = 0) const {
			this->offset = 0;
			setArray((T*)values.data(), values.size(), offset);
			this->offset = values.size();
		}

		template <typename T, size_t S>
		void set(Span<T, S> const& values, size_t const& offset = 0) const {
			this->offset = 0;
			setArray((T*)values.data(), values.size(), offset);
			this->offset = values.size();
		}

		template <typename... Args>
		void set(Args const&... args) const
		requires (sizeof...(Args) > 1) {
			this->offset = 0;
			size_t off = 0;
			(setSpecial(args, off), ...);
			offset = off;
		}

		template <Type::Class T>
		void forEach(List<T> const& values, Functor<void(T&, Uniform const&)> func) const {
			for (T& val: values) {
				func(val, Uniform(name, id, location + offset+1));
			}
		}

		template <typename T>			void operator()(T const& value)				{set(value);	}
		template <typename... Args>		void operator()(Args const&... args)		{set(args...);	}
		template <typename T>			void operator()(List<T> const& values)		{set(values);	}
		template <typename T, size_t S>	void operator()(Span<T, S> const& values)	{set(values);	}

	private:
		constexpr Uniform(
			String  const& _name,
			GLuint const& _id,
			GLuint const& _location
		): name(_name), id(_id), location(_location) {}

		template<typename T>
		void setSpecial(T const& value, size_t& offset) const {
			set(value, offset);
			++offset;
		}

		template <typename T>
		void setSpecial(List<T> const& values, size_t& offset) const {
			set(values, offset);
			offset += this->offset;
		}

		template <typename T, size_t S>
		void setSpecial(Span<T, S> const& values, size_t& offset) const {
			set(values, offset);
			offset += this->offset;
		}

		GLuint getUniformArray(size_t const& offset) const {
			return location + offset + this->offset;
		}

		inline GLuint getUniform() const {
			return location + offset;
		}

		inline GLuint getUniform(String const& append) const {
			return glGetUniformLocation(id, (name + append).c_str());
		}

		GLuint const id;
		intmax const location;

		size_t mutable offset;
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

		typedef Instance<ShaderProgram> ShaderInstance;

		ShaderInstance instance;

		bool created = false;

		/// Similar to create, but internal.
		void attach(String const& code, GLuint const& shaderType) {
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

		Shader(String const& vertexCode, String const& fragmentCode) {
			create(vertexCode, fragmentCode);
		}

		Shader(SLF::SLFData const& slfData) {
			create(slfData);
		}

		Shader(String const& code, GLuint const& shaderType) {
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
		bool create(String const& vertexCode, String const& fragmentCode) {
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
		bool create(SLF::SLFData const& slfData) {
			if (created) return false;
			String dir = slfData.folder;
			String shaderPath = "";
			String log = "";
			String code;
			GLuint type;
			for (SLF::ShaderEntry const& shader: slfData.shaders) {
				shaderPath = FileSystem::concatenatePath(dir, shader.path);
				DEBUGLN(shaderPath);
				code = getTextFile(shaderPath);
				type = getGLShaderType(shader.type);
				try {
					attach(code, type);
				} catch (Error::Error const& err) {
					log += String("\n[[ Error on shader '") + shaderPath + "'! ]]:\n";
					log += err.what();
				}
			}
			if (!log.empty())
				throw Error::FailedAction(log);
			created = true;
			return true;
		}

		/// Creates a shader from a given shader code, and a shader type  and associates it to the object. Returns false if already created.
		bool create(String const& code, GLuint const& shaderType) {
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
				throw Error::FailedAction(String("Could not link shader program!\n") + infoLog + "\n\n\n Program:" + code);
			}
			glDeleteShader(shader);
			return true;
		}

		void make(String const& vertexCode, String const& fragmentCode) {
			destroy();
			create(vertexCode, fragmentCode);
		}

		void make(SLF::SLFData const& slfData) {
			destroy();
			create(slfData);
		}

		void make(String const& code, GLuint const& shaderType) {
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
		Uniform uniform(String const& name) {
			glUseProgram(instance->id);
			Uniform su(name, instance->id);
			return su;
		}

		/**
		* The way to set uniforms.
		* Done like this: SHADER[UNIFORM_NAME](UNIFORM_VALUE);
		*/
		Uniform operator[](String const& name) {
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
