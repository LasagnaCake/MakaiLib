#define GLEW_STATIC
#include <GLEW/include/GL/glew.h>
#include <GLEW/include/GL/wglew.h>
#include <GL/gl.h>

#include "shader.hpp"

using namespace Makai::Graph;

namespace SLF = Makai::SLF;

using ShaderType = SLF::ShaderType;

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

struct Shader::ShaderProgram {
	ShaderProgram()		{}
	ShaderProgram(bool)	{create();	}
	~ShaderProgram()	{destroy();	}
	void create()	{if (id == 0) id = glCreateProgram();	}
	void destroy()	{if (id != 0) id = glDeleteProgram(id);	}
	GLuint id = 0;
};

/// Similar to create, but internal.
void Shader::attach(String const& code, ShaderType const& shaderType) {
	// Compile shaders
	GLuint shader;
	int success;
	char infoLog[2048];
	const char* shaderCode = code.c_str();
	// Vertex Shader
	shader = glCreateShader(getGLShaderType(shaderType));
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

Shader::Shader() {
	instance.bind(new ShaderProgram());
}

Shader::Shader(String const& vertexCode, String const& fragmentCode) {
	create(vertexCode, fragmentCode);
}

Shader::Shader(SLF::SLFData const& slfData) {
	create(slfData);
}

Shader::Shader(String const& code, ShaderType const& shaderType) {
	create(code, shaderType);
}

Shader::Shader(Shader const& other) {
	instance = other.instance;
}

Shader::Shader(Shader&& other) {
	instance = other.instance;
}

Shader::~Shader() {
	destroy();
}

/// Returns whether this object has a shader associated with it (i.e. "is created").
inline bool Shader::isCreated() const {
	return created;
}

/// Creates a shader and associates it to the object. Returns false if already created.
bool Shader::create(String const& vertexCode, String const& fragmentCode) {
	if (created) return false;
	else created = true;
	// Compile shaders
	GLuint vertex = 0, fragment = 0;
	int success;
	char infoLog[2048];
	if (vertexCode != "") {
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
	if (fragmentCode != "") {
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
bool Shader::create(SLF::SLFData const& slfData) {
	if (created) return false;
	String dir = slfData.folder;
	String shaderPath = "";
	String log = "";
	String code;
	for (SLF::ShaderEntry const& shader: slfData.shaders) {
		shaderPath = FileSystem::concatenatePath(dir, shader.path);
		DEBUGLN(shaderPath);
		code = File::getTextFile(shaderPath);
		try {
			attach(code, shader.type);
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
bool Shader::create(String const& code, ShaderType const& shaderType) {
	if (created) return false;
	else created = true;
	// Compile shaders
	GLuint shader;
	int success;
	char infoLog[2048];
	const char* shaderCode = code.c_str();
	// Vertex Shader
	shader = glCreateShader(getGLShaderType(shaderType));
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

void Shader::make(String const& vertexCode, String const& fragmentCode) {
	destroy();
	create(vertexCode, fragmentCode);
}

void Shader::make(SLF::SLFData const& slfData) {
	destroy();
	create(slfData);
}

void Shader::make(String const& code, ShaderType const& shaderType) {
	destroy();
	create(code, shaderType);
}

/// Destroys the shader associated with this object, if any. Does not delete object.
void Shader::destroy() {
	if (created) {
		instance.unbind();
		created = false;
	}
}

/// Operator overload.
void Shader::operator()() const {
	glUseProgram(instance->id);
}

/// Enables the shader object.
void Shader::enable() const {
	glUseProgram(instance->id);
}

/**
* The way to set uniforms.
* Done like this: SHADER.uniform(UNIFORM_NAME)(UNIFORM_VALUE);
*/
Uniform Shader::uniform(String const& name) const {
	glUseProgram(instance->id);
	Uniform su(name, instance->id);
	return su;
}

/**
* The way to set uniforms.
* Done like this: SHADER[UNIFORM_NAME](UNIFORM_VALUE);
*/
Uniform Shader::operator[](String const& name) const {
	return uniform(name);
}

Shader& Shader::operator=(Shader const& other) {
	instance = other.instance;
	return *this;
}

Shader& Shader::operator=(Shader&& other) {
	instance = other.instance;
	return *this;
}

Shader Shader::DEFAULT;
