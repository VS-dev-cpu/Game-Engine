// Lighting Library for the Game Engine
#pragma once

#include <tools/shader.h>

typedef enum
{
    NONE = -1,
    DIR_LIGHT,
    POINT_LIGHT,
    SPOT_LIGHT
} LIGHT_TYPE;

/**
 * @brief The Light manager class
 * @details Manages lights and shadows in a shader
 * @warning Only works with shaders made for this class
 */
class Light
{
private:
    uint MAX_LIGHTS = 0;

public:
    gls s;

    void setup(std::string path, uint max_lights = 4);
    void update();

    void set_state(int n, LIGHT_TYPE en);
    void set_pos(int n, vec3 pos);
    void set_dir(int n, vec3 dir);
    void set_strength(int n, float strength);
    // TODO: Use Shadows
};

/**
 * @brief Setup the Light manager & load the shader
 *
 * @param path
 */
void Light::setup(std::string path, uint max_lights)
{
    if (max_lights < 1)
        debug::error("light::setup()", "light num must be an unsigned integer (non-zero)"); // TODO: Think about merging lighting into the shader lib, max_lights=0 disables lighting for shader

    MAX_LIGHTS = max_lights;

    std::string vertex, fragment;

    // vertex
    std::ifstream v(path + ".vs");
    if (v.is_open())
    {
        std::string line;
        while (getline(v, line))
        {
            vertex += line + "\n";
        }
    }
    else
        debug::error("light::setup()", "can't open shader file", (path + ".vs").c_str());

    // fragment
    std::ifstream f(path + ".fs");
    if (f.is_open())
    {
        std::string line;
        while (getline(f, line))
        {
            if (isin(line, "#define MAX_LIGHTS"))
                fragment += "#define MAX_LIGHTS " + itos(MAX_LIGHTS) + "\n";
            else
                fragment += line + "\n";
        }
    }
    else
        debug::error("light::setup()", "can't open shader file", (path + ".fs").c_str());

    s = shader::load_raw(vertex.c_str(), fragment.c_str());

    shader::use(s);
    for (int i = 0; i < MAX_LIGHTS; i++)
        shader::set(s, "light[" + itos(i) + "].enabled", false);
}

/**
 * @brief Update the lights
 */
void Light::update()
{
}

void Light::set_state(int n, LIGHT_TYPE en)
{
    n = clamp(n, 0, MAX_LIGHTS - 1);

    shader::set(s, "light[" + itos(n) + "].type", en);
}

void Light::set_pos(int n, vec3 pos)
{
    n = clamp(n, 0, MAX_LIGHTS - 1);

    shader::set(s, "light[" + itos(n) + "].position", pos);
}

void Light::set_dir(int n, vec3 dir)
{
    n = clamp(n, 0, MAX_LIGHTS - 1);

    shader::set(s, "light[" + itos(n) + "].direction", dir);
}

void Light::set_strength(int n, float strength)
{
    n = clamp(n, 0, MAX_LIGHTS - 1);

    shader::set(s, "light[" + itos(n) + "].strength", strength);
}
