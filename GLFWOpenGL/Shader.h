#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader
{
public: 
	GLuint program;

	Shader(const GLchar *vertexShaderPath, const GLchar *fragmentShaderPath)
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vertShaderFile;
		std::ifstream fragShaderFile;

		vertShaderFile.exceptions(std::ifstream::badbit);
		fragShaderFile.exceptions(std::ifstream::badbit);

		try 
		{
			vertShaderFile.open(vertexShaderPath);
			fragShaderFile.open(fragmentShaderPath);

			std::stringstream vertShaderStream, fragShaderStream;

			vertShaderStream << vertShaderFile.rdbuf();
			fragShaderStream << fragShaderFile.rdbuf();

			vertShaderFile.close();
			fragShaderFile.close();

			vertexCode = vertShaderStream.str();
			fragmentCode = fragShaderStream.str();
		}
		catch (std::ifstream::failure exception) 
		{
			std::cout << "Shader file not successfully read:\n" << std::endl;
		}
		const GLchar *vertexShaderCode = vertexCode.c_str();
		const GLchar *fragmentShaderCode = fragmentCode.c_str();

		GLuint vertexShader, fragmentShader;
		GLint success;
		GLchar infoLog[512];

		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
		glCompileShader(vertexShader);

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "Vertex Shader Failed to Compile:\n" << infoLog << std::endl;
		}

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "Fragment Shader Failed to Compile:\n" << infoLog << std::endl;
		}

		this->program = glCreateProgram();
		glAttachShader(this->program, vertexShader);
		glAttachShader(this->program, fragmentShader);
		glLinkProgram(this->program);

		glGetProgramiv(this->program, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(this->program, 512, NULL, infoLog);
			std::cout << "Failed to link shaders to program:\n" << infoLog << std::endl;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void useProgram() 
	{
		glUseProgram(this->program);
	}
};

#endif