#ifndef SHADER_H
#define SHADER_H

#include "glm/ext.hpp"
#include "glm/mat4x4.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath,
        const char* tessControlPath = nullptr, const char* tessEvalPath = nullptr)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string tessControlCode;
        std::string tessEvalCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream tcShaderFile;
        std::ifstream teShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            if (tessControlPath && tessEvalPath) {
                tcShaderFile.open(tessControlPath);
                std::stringstream tcShaderStream;
                tcShaderStream << tcShaderFile.rdbuf();
                tcShaderFile.close();
                tessControlCode = tcShaderStream.str();
                teShaderFile.open(tessEvalPath);
                std::stringstream teShaderStream;
                teShaderStream << teShaderFile.rdbuf();
                teShaderFile.close();
                tessEvalCode = teShaderStream.str();
            }
        }
        catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        const char* tcShaderCode = tessControlPath ? tessControlCode.c_str() : nullptr;
        const char* teShaderCode = tessEvalPath ? tessEvalCode.c_str() : nullptr;
        // 2. compile shaders
        unsigned int vertex, fragment, tessControl, tessEval;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // tess control shader
        if (tessControlPath) {
            tessControl = glCreateShader(GL_TESS_CONTROL_SHADER);
            glShaderSource(tessControl, 1, &tcShaderCode, NULL);
            glCompileShader(tessControl);
            checkCompileErrors(tessControl, "TESS_CONTROL");
        }
        // tess eval shader
        if (tessEvalPath) {
            tessEval = glCreateShader(GL_TESS_EVALUATION_SHADER);
            glShaderSource(tessEval, 1, &teShaderCode, NULL);
            glCompileShader(tessEval);
            checkCompileErrors(tessEval, "TESS_EVALUATION");
        }
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (tessControlPath) glAttachShader(ID, tessControl);
        if (tessEvalPath) glAttachShader(ID, tessEval);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (tessControlPath) glDeleteShader(tessControl);
        if (tessEvalPath) glDeleteShader(tessEval);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }
    unsigned int getID() const
    {
        return ID;
    }
    // utility uniform functions
    void setBool(const std::string name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.data()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.data()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.data()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.data()), 1, &value[0]);
    }
    void setVec2(const std::string name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.data()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.data()), 1, &value[0]);
    }
    void setVec3(const std::string name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.data()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.data()), 1, &value[0]);
    }
    void setVec4(const std::string name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.data()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};

#endif