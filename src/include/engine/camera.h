#pragma once

#include <tools/shader.h>

#include <engine/object.h>

struct camera
{
    bool script = false;

    lua_State *L;

    gls s;
    uint attrib_vertex, attrib_texcoord, attrib_normal;
    int *width, *height;

    float fov = 60.0f;
    float near = 0.01f;
    float far = 10000.0f;

    vec3 position;
    vec3 rotation;

    vec3 lookDir;
    vec3 front, up, right;

    camera(int *w, int *h, gls shader);
    void add(std::string luascript);

    void update();
    void move(vec3 amount, bool moveTowardsYaw = false);
    void rotate(vec3 amount);

    mat4 projection(int w, int h);
    mat4 view();

    void draw(object obj);
};

/**
 * @brief Init The Camera
 *
 * @param fov The Field-Of-View of the camera
 * @param far The far
 */
camera::camera(int *w, int *h, gls shader)
{
    this->width = w;
    this->height = h;

    this->s = shader;

    this->attrib_vertex = glGetAttribLocation(s, "aPos");
    this->attrib_texcoord = glGetAttribLocation(s, "aTexCoord");
    this->attrib_normal = glGetAttribLocation(s, "aNormal");
}

void camera::add(std::string luascript)
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
 * @brief Update The Camera
 */
void camera::update()
{
    vec3 f;
    f.x = cos(radians(this->rotation.y)) * cos(radians(this->rotation.z));
    f.y = sin(radians(this->rotation.z));
    f.z = sin(radians(this->rotation.y)) * cos(radians(this->rotation.z));

    this->lookDir = vector::normalize(f);

    this->front = vector::normalize({f.x, 0.0f, f.z});
    this->right = vector::normalize(vector::crossproduct(this->front, {0.0f, 1.0f, 0.0f}));
    this->up = vector::normalize(vector::crossproduct(this->right, this->front));
}

/**
 * @brief Move The Camera
 *
 * @param amount The amount
 * @param moveTowardsYaw should move up/down?
 */
void camera::move(vec3 amount, bool moveTowardsYaw)
{
    if (moveTowardsYaw)
        this->position += this->lookDir * amount.x;
    else
        this->position += this->front * amount.x;

    this->position += this->right * amount.z;
    this->position += this->up * amount.y;
}

/**
 * @brief Rotate the camera
 *
 * @param amount The amount to rotate
 */
void camera::rotate(vec3 amount)
{
    this->rotation += amount;
}

void camera::draw(object obj)
{
    shader::use(s);

    shader::set(s, "view", matrix::lookAt(this->position, this->position + this->lookDir, this->up));
    shader::set(s, "projection", matrix::perspective(this->fov, (float)*width / (float)*height, this->near, this->far));

    shader::set(s, "model", obj.model());

    shader::set(s, "tmc", obj.tmc);
    shader::set(s, "color", obj.color);

    glBindTexture(GL_TEXTURE_2D, obj.tex);

    // draw mesh
    glBindVertexArray(obj.VAO);
    glDrawElements(GL_TRIANGLES, obj.m.tris * 3, GL_UNSIGNED_INT, 0);
}