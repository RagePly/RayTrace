#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream> // StringStream...
#include <iostream>

#include "rtShader.h"


#define readto(filePath, fileStream, sourceString) {	\
	fileStream.open(filePath);							\
	std::stringstream s;								\
	s << fileStream.rdbuf();							\
	fileStream.close();									\
	sourceString = s.str();								\
}



unsigned int RayTrace::compProg(const char** source_code, const unsigned int shader_type) {
	unsigned int shader_handle = glCreateShader(shader_type);
	glShaderSource(shader_handle, 1, source_code, NULL);
	glCompileShader(shader_handle);

	return shader_handle;
}


bool RayTrace::_assertComp(const unsigned int shader_handle, const char* filePath) {
	int success;
	char infoLog[512];

	glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader_handle, 512, NULL, infoLog);
		std::cout << filePath << ":\n" << infoLog << std::endl;
		return false;
	}
	return true;
}


bool RayTrace::_assertLink(const unsigned int program_handle) {
	int success;
	char infoLog[512];

	glGetProgramiv(program_handle, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program_handle, 512, NULL, infoLog);
		std::cout << "Linking error:\n" << infoLog << std::endl;
		return false;
	}
	return true;
}



RayTrace::Shader::Shader() :
	programId(0),
	compiled(false)
{}


RayTrace::Shader::Shader(const char* vPath, const char* fPath) : 
	programId(0),
	compiled(false)
{
	this->compileFromPath(vPath, fPath);
}


RayTrace::Shader::~Shader() {};


void RayTrace::Shader::compileFromPath(const char* vPath, const char* fPath) {
	if (!compiled) {
		std::string vSourceCode, fSourceCode;
		std::ifstream vFile, fFile;

		// Sets what errors cause exception:
		// badbit	irrecoverable stream error
		// failbit	input / output operation failed(formatting or extraction error)
		vFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			readto(vPath, vFile, vSourceCode);
			readto(fPath, fFile, fSourceCode);
		}
		catch (std::ifstream::failure e) {
			std::cout << "Failed to read source files!" << std::endl;
			vFile.close();
			fFile.close();
			return;
		}
		const char* cstrVSource = vSourceCode.c_str();
		const char* cstrFSource = fSourceCode.c_str();

		unsigned int vShader = compProg(&cstrVSource, GL_VERTEX_SHADER);
		unsigned int fShader = compProg(&cstrFSource, GL_FRAGMENT_SHADER);

		if (!(_assertComp(vShader, vPath) && _assertComp(fShader, fPath)))
			return;

		programId = glCreateProgram();
		glAttachShader(programId, vShader);
		glAttachShader(programId, fShader);
		glLinkProgram(programId);

		if (!_assertLink(programId)) {
			programId = 0;
			return;
		}

		glDeleteShader(vShader);
		glDeleteShader(fShader);
		//delete[] cstrVSource, cstrFSource; //these apperently were maintained by fSourceCode and vSourceCode.
		compiled = true;
	}
}


bool RayTrace::Shader::success() const { return compiled; };

void RayTrace::Shader::use() { glUseProgram(programId); };

RayTrace::Shader::operator const unsigned int() const {
	return programId;
}