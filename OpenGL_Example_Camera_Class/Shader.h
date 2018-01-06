#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

///GLEW
#define GLEW_STATIC
#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

///GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
    private:

        ///The ID of this Shader Program
        unsigned int ID;

    public:

        ///Constructor
        Shader(const GLchar* , const GLchar*);

        ///Getters
        int getID();

        ///Setters - *GLSL uniforms*
        void setBool(const string &name, bool value);
        void setInt(const string &name, int value);
        void setFloat(const string &name, float value);

        ///--4x4 Matrix (Floats)
        void setMatrix4fv(const string &name, glm::mat4);

        ///Use this function to set 'this' shader as the current
        ///OpenGL shader
        void use();

};


///This function receives the path of all the shaders, compiles them and links
///them into 'this' shader program
Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
    /// 1. retrieve the vertex/fragment source code from filePath
    string vertexCode;
    string fragmentCode;

    ifstream vShaderFile;
    ifstream fShaderFile;

    /// ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    fShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    try
    {
        /// open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        stringstream vShaderStream, fShaderStream;

        /// read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        /// close file handlers
        vShaderFile.close();
        fShaderFile.close();
        /// convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch(ifstream::failure e)
    {
        cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    /// 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    ///Create a vertex Shader Object
    vertex = glCreateShader(GL_VERTEX_SHADER);

    ///Populate the object with the shader code
    glShaderSource(vertex, 1, &vShaderCode, NULL);

    ///Compile the shader
    glCompileShader(vertex);

    /// print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(success)
    {
        cout << "Vertex Shader compiled successfully" << endl;
    }
    else
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog
        << endl;
    }

    ///Same process for fragment shader.
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

    if(success)
    {
        cout << "Fragment Shader compiled successfully" << endl;
    }
    else
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog
        << endl;
    }

    ///Once the shaders are compiled link them into a shader program

    ///Create a shader program and link them to 'this' ID
    ID = glCreateProgram();

    ///Link the shaders, to the shader program
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    /// print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(success)
    {
        cout << "Shader Program of ID " << ID << " was linked successfully"
        << endl;
    }
    else
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }

    ///Once the linking is done, delete the shader objects, we don't need them
    ///anymore
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

///Use this function to set 'this' shader as the current
///OpenGL shader
void Shader::use()
{
    glUseProgram(ID);
}

///This function returns the ID if 'this' shader object
int Shader::getID()
{
    return ID;
}

///\////////////////Sets - Uniforms (GLSL)//////////////////////////////////////

void Shader::setBool(const string &name, bool value)
{
    int uniformLocation = glGetUniformLocation(ID, name.c_str());

    if(uniformLocation != -1)
    {
        glUniform1i(uniformLocation, (int)value);
    }
    else
    {
        cout << "Couldn't find uniform " << name << " in Shader program " << ID
        << endl;
    }

}

void Shader::setInt(const string &name, int value)
{
    int uniformLocation = glGetUniformLocation(ID, name.c_str());

    if(uniformLocation != -1)
    {
        glUniform1i(uniformLocation, value);
    }
    else
    {
        cout << "Couldn't find uniform " << name << " in Shader program " << ID
        << endl;
    }

}

void Shader::setFloat(const string &name, float value)
{
    int uniformLocation = glGetUniformLocation(ID, name.c_str());

    if(uniformLocation != -1)
    {
        glUniform1i(uniformLocation, value);
    }
    else
    {
        cout << "Couldn't find uniform " << name << " in Shader program " << ID
        << endl;
    }

}

void Shader::setMatrix4fv(const string &name, glm::mat4 mat)
{
    int uniformLocation = glGetUniformLocation(ID, name.c_str());

    if(uniformLocation != -1)
    {
        glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(mat));
    }
    else
    {
        cout << "Couldn't find uniform " << name << " in Shader program " << ID
        << endl;
    }
}

///\////////////////////////////////////////////////////////////////////////////

#endif // SHADER_H_INCLUDED
