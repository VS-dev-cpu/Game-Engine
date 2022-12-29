// GLSL Shader Library for the Game Engine
#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <tools/debug.h>

/**
 * @brief OpenGL shader
 */
typedef uint gls;

namespace shader
{

    /**
     * @brief Load shader from variable(s)
     *
     * @param vShaderCode The Vertex Shader's variable
     * @param fShaderCode The Fragment Shader's variable
     * @param gShaderCode The Geometry Shader's variable (NULL if not used)
     * @return The shader program's id
     */
    gls load_raw(const char *vShaderCode, const char *fShaderCode, const char *gShaderCode = NULL)
    {
        // Define Variables
        bool gshader = gShaderCode != NULL;

        gls s;
        gls vs, fs, gs;

        int success;
        char infoLog[1024];

        // Compile Vertex Shader
        vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vShaderCode, NULL);
        glCompileShader(vs);

        // Check if compilation was successfull
        glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vs, 1024, NULL, infoLog);
            debug::error("shader::load_raw()", "can't compile vertex code", infoLog);
        }

        // Compile Fragment Shader
        fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fShaderCode, NULL);
        glCompileShader(fs);

        // Check if compilation was successful
        glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fs, 1024, NULL, infoLog);
            debug::error("shader::load_raw()", "can't compile fragment code", infoLog);
        }

        // Compile Geometry Shader
        if (gshader)
        {
            gs = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(gs, 1, &gShaderCode, NULL);
            glCompileShader(gs);

            // Check if compilation was successful
            glGetShaderiv(gs, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(gs, 1024, NULL, infoLog);
                debug::error("shader::load_raw()", "can't compile geometry code", infoLog);
            }
        }

        // Create Shader Program
        s = glCreateProgram();
        glAttachShader(s, vs);
        glAttachShader(s, fs);
        if (gshader)
            glAttachShader(s, gs);

        glLinkProgram(s);

        // Check if compilation was successful
        glGetProgramiv(s, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(s, 1024, NULL, infoLog);
            debug::error("shader::load_raw()", "can't link", infoLog);
        }

        // Delete unnecessary shaders
        glDeleteShader(vs);
        glDeleteShader(fs);
        if (gshader)
            glDeleteShader(gs);

        return s;
    }

    /**
     * @brief Load a shader from files
     *
     * @param vertexPath The Vertex Shader's path
     * @param fragmentPath The Fragment Shader's path
     * @param geometryPath The Geometry Shader's path ("" if not used)
     * @return The shader program's id
     */
    gls load(std::string vertexPath, std::string fragmentPath, std::string geometryPath = "")
    {
        // Define variables
        bool gshader = geometryPath != "";

        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;

        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;

        // Open Files
        vShaderFile.open("res/shaders/" + vertexPath);
        fShaderFile.open("res/shaders/" + fragmentPath);
        if (gshader)
            gShaderFile.open("res/shaders/" + geometryPath);

        if (!vShaderFile.is_open() || !vShaderFile.is_open())
            debug::error("shader::load", "can't open file(s)", (vertexPath + "; " + fragmentPath).c_str());
        if (gshader && !gShaderFile.is_open())
            gshader = false;

        // Read Lines
        std::stringstream vShaderStream, fShaderStream, gShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        if (gshader)
            gShaderStream << gShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();
        if (gshader)
            gShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        if (gshader)
            geometryCode = gShaderStream.str();

        return load_raw(vertexCode.c_str(), fragmentCode.c_str(), (gshader ? geometryCode.c_str() : NULL));
    }

    /**
     * @brief Load a shader from one path + name (ex. "light" -> loads light.vs, light.fs (maybe light.gs))
     *
     * @param path The shaders' path
     * @return The shader program's id
     */
    gls load(std::string path)
    {
        return load(path + ".vs", path + ".fs", path + ".gs");
    }

    /**
     * @brief Selects a shader program to use
     *
     * @param s The shader program to select
     */
    void use(gls s)
    {
        glUseProgram(s);
    }

    /**
     * @brief Set Shader's Boolean uniform
     *
     * @param s The Shader Program
     * @param n The name of the variable
     * @param value The value to set
     */
    void set(gls s, std::string n, bool value)
    {
        glUniform1i(glGetUniformLocation(s, n.c_str()), (int)value);
    }

    /**
     * @brief Set Shader's Integer uniform
     *
     * @param s The Shader Program
     * @param n The name of the variable
     * @param value The value to set
     */
    void set(gls s, std::string n, int value)
    {
        glUniform1i(glGetUniformLocation(s, n.c_str()), value);
    }

    /**
     * @brief Set Shader's Float uniform
     *
     * @param s The Shader Program
     * @param n The name of the variable
     * @param value The value to set
     */
    void set(gls s, std::string n, float value)
    {
        glUniform1f(glGetUniformLocation(s, n.c_str()), value);
    }

    /**
     * @brief Set Shader's 2D Vector uniform
     *
     * @param s The Shader Program
     * @param n The name of the variable
     * @param value The value to set
     */
    void set(gls s, std::string n, vec2 vec)
    {
        glUniform2f(glGetUniformLocation(s, n.c_str()), vec.x, vec.y);
    }

    /**
     * @brief Set Shader's 3D Vector uniform
     *
     * @param s The Shader Program
     * @param n The name of the variable
     * @param value The value to set
     */
    void set(gls s, std::string n, vec3 vec)
    {
        glUniform3f(glGetUniformLocation(s, n.c_str()), vec.x, vec.y, vec.z);
    }

    /**
     * @brief Set Shader's 4 by 4 Matrix uniform
     *
     * @param s The Shader Program
     * @param n The name of the variable
     * @param value The value to set
     */
    void set(gls s, std::string n, mat4 mat)
    {
        glUniformMatrix4fv(glGetUniformLocation(s, n.c_str()), 1, GL_FALSE, &mat.m[0][0]);
    }
};