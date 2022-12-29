// Object Library for the Game Engine
#pragma once

#include <tools/loadin.h>
#include <lua/lua.hpp>

#define lualib "res/scripts/libs/class.lua"

/**
 * @brief Object for [bodies], [scripts], [audio sources], [lights], etc.
 */
struct object
{
private:
    float getFloat(std::string table, std::string var)
    {
        lua_getglobal(L, "object");         // push t onto the stack
        lua_getfield(L, -1, table.c_str()); // push t.x onto the stack

        lua_pushstring(L, var.c_str()); // push the key "y" onto the stack
        lua_gettable(L, -2);            // retrieve the value of t.x.y and push it onto the stack

        return lua_tonumber(L, -1);
    }

    vec3 getVec3(std::string table)
    {
        lua_getglobal(L, "object");         // push t onto the stack
        lua_getfield(L, -1, table.c_str()); // push t.x onto the stack

        lua_pushstring(L, "x"); // push the key "y" onto the stack
        lua_gettable(L, -2);    // retrieve the value of t.x.y and push it onto the stack
        lua_pushstring(L, "y"); // push the key "y" onto the stack
        lua_gettable(L, -3);    // retrieve the value of t.x.y and push it onto the stack
        lua_pushstring(L, "z"); // push the key "y" onto the stack
        lua_gettable(L, -4);    // retrieve the value of t.x.y and push it onto the stack

        return (vec3){lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1)};
    }

    void setFloat(std::string var, float val)
    {
        lua_getglobal(L, "object");
        // lua_getfield(L, -1, table.c_str());

        // set the value of t.x.y to 10
        lua_pushnumber(L, val);
        lua_setfield(L, -2, var.c_str());
        lua_pop(L, 2);
    }

    void setVec3(std::string table, vec3 val)
    {
        lua_getglobal(L, "object");

        // set the value of t.x.y to 10
        lua_pushnumber(L, val.x);
        lua_pushnumber(L, val.y);
        lua_pushnumber(L, val.z);

        lua_setfield(L, -2, table.c_str());
        lua_pop(L, 2);
    }

public:
    // Properties
    bool script = false;   // has a script ?
    bool body = false;     // has a body ?
    bool collider = false; // has collider ?

    bool drawable = false; // is drawable ?
    bool textured = false; // is textured ?

    bool physical = false; // is physical ?
    bool gravity = false;  // has gravity ?

    lua_State *L;

    mesh m, c;          // Main and Collider Mesh
    uint VAO, VBO, EBO; // rendering objects

    float tmc = 0.0f; // texture-mix-color

    vec3 color;
    texture tex;

    vec3 lookDir;
    vec3 front, up, right;

    vec3 position, rotation;

    // Functions
    void add(vec3 colour);
    void add(texture t);
    void add(std::string luascript);
    void add(mesh m, bool draw = true);
    void add(mesh c, bool physical, bool gravity);

    void pusharray();

    void update(float deltaTime, int millis);
    void destroy();

    mat4 model();
};

void object::add(vec3 colour)
{
    this->color = colour; // oh, no
}

void object::add(texture t)
{
    this->textured = true;
    this->tex = t;
}

/**
 * @brief Add a Lua script to the object
 *
 * @param luascript The script to add
 */
void object::add(std::string luascript)
{
    this->script = true;

    if (this->script)
    {
        // Init Lua
        L = luaL_newstate();
        luaL_openlibs(L);

        // Start Lua Scripts
        luaL_dofile(L, lualib);
        luaL_dofile(L, ("res/scripts/" + luascript).c_str()); // Load Code

        // call "object.onCreate()" function
        lua_getglobal(L, "object");
        lua_getfield(L, -1, "onCreate");

        if (lua_pcall(L, 0, 0, 0) != LUA_OK)
            debug::warning("object::create()", "script runtime error", lua_tostring(L, -1));
    }
}

/**
 * @brief Add a mesh to the object
 *
 * @param m The mesh to add
 */
void object::add(mesh m, bool draw)
{
    this->m = m;
    this->body = true;

    // Setup for rendering, if drawable
    if (draw)
    {
        if (this->body)
        {
            std::vector<float> vertices;
            std::vector<uint> indices;

            for (int i = 0; i < this->m.tris * 3; i++)
            {
                vertices.push_back(this->m.vertices[i * 3]);
                vertices.push_back(this->m.vertices[i * 3 + 1]);
                vertices.push_back(this->m.vertices[i * 3 + 2]);

                vertices.push_back(this->m.texcoords[i * 2]);
                vertices.push_back(this->m.texcoords[i * 2 + 1]);

                vertices.push_back(this->m.normals[i * 3]);
                vertices.push_back(this->m.normals[i * 3 + 1]);
                vertices.push_back(this->m.normals[i * 3 + 2]);

                indices.push_back(i);
            }

            // Lock Mesh
            glGenVertexArrays(1, &this->VAO);
            glGenBuffers(1, &this->VBO);
            glGenBuffers(1, &this->EBO);

            glBindVertexArray(this->VAO);

            glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

            // vertex positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(0 * sizeof(float)));
            // vertex texture coords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(3 * sizeof(float)));
            // vertex normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(5 * sizeof(float)));

            this->drawable = draw;
        }
        else
            debug::warning("object::create()", "can't make drawable", "has no body");
    }
}

/**
 * @brief Add a collider to the object
 *
 * @param m The collider (mesh) to add
 */
void object::add(mesh c, bool physical, bool gravity)
{
    this->collider = true;
    this->physical = physical;
    this->gravity = gravity;

    this->c = c;
}

/**
 * @brief Update the GPU's array with the mesh data (Fairly slow -> shouldn't be used frequently)
 */
void object::pusharray()
{
    std::vector<float> vertices;
    std::vector<uint> indices;

    for (int i = 0; i < this->m.tris * 3; i++)
    {
        vertices.push_back(this->m.vertices[i * 3]);
        vertices.push_back(this->m.vertices[i * 3 + 1]);
        vertices.push_back(this->m.vertices[i * 3 + 2]);

        vertices.push_back(this->m.texcoords[i * 2]);
        vertices.push_back(this->m.texcoords[i * 2 + 1]);

        vertices.push_back(this->m.normals[i * 3]);
        vertices.push_back(this->m.normals[i * 3 + 1]);
        vertices.push_back(this->m.normals[i * 3 + 2]);

        indices.push_back(i);
    }

    // Update Arrays
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
}

/**
 * @brief Update the object and it's variables
 */
void object::update(float deltaTime, int millis)
{
    // Update object's positions
    vec3 f;
    f.x = cos(radians(this->rotation.y)) * cos(radians(this->rotation.z));
    f.y = sin(radians(this->rotation.z));
    f.z = sin(radians(this->rotation.y)) * cos(radians(this->rotation.z));

    this->lookDir = vector::normalize(f);

    this->front = vector::normalize({f.x, 0, f.z});
    this->right = vector::normalize(vector::crossproduct(this->front, {0.0f, 1.0f, 0.0f}));
    this->up = vector::normalize(vector::crossproduct(this->right, this->front));

    if (this->script)
    {
        // call "object.onUpdate()" function
        lua_getglobal(L, "object");
        lua_getfield(L, -1, "onUpdate");

        lua_pushnumber(L, deltaTime);
        lua_pushinteger(L, millis);

        if (lua_pcall(L, 2, 0, 0) != LUA_OK)
            debug::warning("object::update()", "script runtime error", lua_tostring(L, -1));

        // Update Data
        this->position = getVec3("position");
        this->rotation = getVec3("rotation");
    }
}

/**
 * @brief Destroy the object
 */
void object::destroy()
{
    if (this->script)
    {
        // call "object.onDestroy()" function
        lua_getglobal(L, "object");
        lua_getfield(L, -1, "onDestroy");

        if (lua_pcall(L, 0, 0, 0) != LUA_OK)
            debug::warning("object::destroy()", "script runtime error", lua_tostring(L, -1));
    }
}

/**
 * @brief Get the object's model
 *
 * @return The model
 */
mat4 object::model()
{
    return matrix::rotate(this->rotation * M_RAD) * matrix::translate(this->position);
}