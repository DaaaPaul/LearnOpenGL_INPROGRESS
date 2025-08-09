#ifndef SHADER
#define SHADER

#include <glad/glad.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

class Shader {
	public:
		unsigned programID;

		Shader(const char* vShader, const char* fShader);
		void use() const;
		void kill() const;
		void setUniform(const char* name, const bool newValue) const;
		void setUniform(const char* name, const int newValue) const;
		void setUniform(const char* name, const float newValue) const;

	private:
		static bool checkShaderCompilationSuccess(unsigned shaderID, const char* shaderType, bool isShaderProgram);
};

Shader::Shader(const char* vShader, const char* fShader) {
	std::ifstream vShaderFile(vShader);
	std::ifstream fShaderFile(fShader);
	std::stringstream streamIn;
	std::string vShaderCodeString;
	std::string fShaderCodeString;
	const char* vShaderCode;
	const char* fShaderCode;

	unsigned vShaderID;
	vShaderID = glCreateShader(GL_VERTEX_SHADER);
	streamIn << vShaderFile.rdbuf();
	vShaderCodeString = streamIn.str();
	vShaderCode = vShaderCodeString.c_str();
	glShaderSource(vShaderID, 1, &vShaderCode, NULL);
	glCompileShader(vShaderID);
	checkShaderCompilationSuccess(vShaderID, "Vertex", false);

	unsigned fShaderID;
	fShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	streamIn.str("");
	streamIn << fShaderFile.rdbuf();
	fShaderCodeString = streamIn.str();
	fShaderCode = fShaderCodeString.c_str();
	glShaderSource(fShaderID, 1, &fShaderCode, NULL);
	glCompileShader(fShaderID);
	checkShaderCompilationSuccess(fShaderID, "Fragment", false);

	vShaderFile.close();
	fShaderFile.close();

	programID = glCreateProgram();
	glAttachShader(programID, vShaderID);
	glAttachShader(programID, fShaderID);
	glLinkProgram(programID);
	checkShaderCompilationSuccess(programID, "Shader program", true);

	glDeleteShader(vShaderID);
	glDeleteShader(fShaderID);
}

void Shader::use() const {
	glUseProgram(programID);
}

void Shader::kill() const {
	glDeleteProgram(programID);
}

void Shader::setUniform(const char* name, const bool newValue) const {
	glUniform1i(glGetUniformLocation(programID, name), static_cast<int>(newValue));
}

void Shader::setUniform(const char* name, const int newValue) const {
	glUniform1i(glGetUniformLocation(programID, name), newValue);
}

void Shader::setUniform(const char* name, const float newValue) const {
	glUniform1f(glGetUniformLocation(programID, name), newValue);
}

bool Shader::checkShaderCompilationSuccess(unsigned shaderID, const char* shaderType, bool isShaderProgram) {
	int success = 1;
	char errorMessage[512];
	if (isShaderProgram) glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
	else glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

	if (!success) {
		if (isShaderProgram) glGetProgramInfoLog(shaderID, 512, NULL, errorMessage);
		else glGetShaderInfoLog(shaderID, 512, NULL, errorMessage);
		std::cout << shaderType << " shader compilation failed. Error:" << errorMessage << std::endl;
		return false;
	}
	return true;
}

#endif
