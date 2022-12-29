// Engine Library for the Game Engine
#pragma once

#if defined(__WIN32)
#define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>
#include <tools/shader.h>
#include <tools/loadin.h>

#include <engine/camera.h>
#include <engine/object.h>

#include <engine/physics.h>

bool __engine_init = false;

/**
 * @brief The Engine class
 */
class Engine : public Physics
{
private:
    // Main Window
    SDL_Window *window;
    SDL_GLContext glcontext;
    bool shouldClose;

    camera *cam = NULL;

    // 2D Renderer
    gls ui_shader;
    uint VAO;

    // Text Renderer
    font chars;

    // Button States; You can't have more than 128 buttons rendering at the same time
    bool bstate[128];

    // inputs
    const Uint8 *keyboard;
    bool mbutton[3];

public:
    std::map<std::string, texture> texs;
    std::map<std::string, object> objs;

    int width, height;

    // timing
    Uint32 millis, past;
    float deltaTime;

    // inputs
    float sensitivity = 1.0f;
    vec2 mouse, scroll;

    // Basics

    Engine(std::string wname = "Game", bool fullscreen = true, bool vsync = true);
    bool update(float r, float g, float b);
    void clean();
    void kill();

    // Loadings

    void load(std::string name, texture tex);
    void load(std::string name, object obj);

    // Removing

    void destroy(std::string name);

    // Render 2D Shapes

    void rect(vec2 center, vec2 size, vec3 color);
    void rect(vec2 center, vec2 size, std::string tex);
    void text(vec2 pos, float scale, std::string text, vec3 color);

    // Draw Buttons

    bool rect(vec2 center, vec2 size, vec3 color_a, vec3 color_b, int bid);
    bool rect(vec2 center, vec2 size, std::string tex_a, std::string tex_b, int bid);

    // UI

    bool key(std::string k);

    // Camera

    void select(camera *c);

    vec3 wasd(float speed = 10.0f);
};

// Basics

/**
 * @brief Construct a new Engine class & init renderers
 */
Engine::Engine(std::string wname, bool fullscreen, bool vsync)
{
    if (!__engine_init)
    {
        // Init SDL2 & OpenGL
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
            debug::error("graphics::init()", "failed to init SDL2", SDL_GetError());

        window = SDL_CreateWindow(wname.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP * fullscreen); // Init Window with OpenGL and Maximized
        if (window == NULL)
            debug::error("graphics::init()", "failed to create window", SDL_GetError());

        SDL_SetWindowMinimumSize(window, 720, 480);

        SDL_ShowCursor((SDL_bool)!fullscreen);
        SDL_SetRelativeMouseMode((SDL_bool)fullscreen);

        // Request OpenGL 3.3 Core (or higher)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        glcontext = SDL_GL_CreateContext(window);

        if (!gladLoadGLLoader(SDL_GL_GetProcAddress) || glcontext == NULL)
            debug::error("graphics::init()", "failed to init OpenGL");
        debug::glinfo();

        SDL_GetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);

        // Enable VSync (for the editor)
        SDL_GL_SetSwapInterval(vsync);
        SDL_SetWindowIcon(window, IMG_Load("res/oof.jpg"));

        // Init 2D Renderer
        ui_shader = shader::load("2d");
        shader::use(ui_shader);

        // 2D Renderer Init
        uint VBO, EBO;

        float vertices[] = {
            // positions      // texture coords
            1.0f, 1.0f, 1.0f, 1.0f,   // top right
            1.0f, -1.0f, 1.0f, 0.0f,  // bottom right
            -1.0f, -1.0f, 0.0f, 0.0f, // bottom left
            -1.0f, 1.0f, 0.0f, 1.0f   // top left
        };

        unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Init 3D Renderer

        // Init Text Renderer
        // chars = loadin::ttf("ubuntu.ttf");
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
        debug::error("engine::init()", "multiple engine inits are not possible (yet)");

    Physics::init(&objs, false);
}

/**
 * @brief Update the Engine
 *
 * @param r Clear Color [R]
 * @param g Clear Color [G]
 * @param b Clear Color [B]
 * @returns shouldClose?
 */
bool Engine::update(float r, float g, float b)
{
    // Update Window
    SDL_GL_SwapWindow(window);

    // Clear Screen
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update Window's Size
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    if (width != w || height != h)
        glViewport(0, 0, w, h);
    width = w;
    height = h;

    // Update Time
    past = millis;
    millis = SDL_GetTicks();
    deltaTime = (float)(millis - past) / 1000;

    // update inputs
    keyboard = SDL_GetKeyboardState(NULL);

    int m[2];
    if (!SDL_GetRelativeMouseMode())
    {
        SDL_GetMouseState(&m[0], &m[1]);
        mouse.x = map(m[0], 0, width, -1, 1);
        mouse.y = map(m[1], 0, height, 1, -1);
    }
    else
    {
        SDL_GetRelativeMouseState(&m[0], &m[1]);
        mouse.x = m[0] * sensitivity;
        mouse.y = m[1] * sensitivity;
    }

    if (cam != NULL)
        cam->update();

    // Update Objects
    for (auto &elem : objs)
    {
        std::string name = elem.first;
        object *obj = &elem.second;

        obj->update(deltaTime, millis);

        if (cam != NULL && obj->drawable)
            cam->draw(*obj);
    }

    // poll events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            shouldClose = true;

        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
                mbutton[0] = true;
            if (event.button.button == SDL_BUTTON_MIDDLE)
                mbutton[1] = true;
            if (event.button.button == SDL_BUTTON_RIGHT)
                mbutton[2] = true;
        }

        if (event.type == SDL_MOUSEBUTTONUP)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
                mbutton[0] = false;
            if (event.button.button == SDL_BUTTON_MIDDLE)
                mbutton[1] = false;
            if (event.button.button == SDL_BUTTON_RIGHT)
                mbutton[2] = false;
        }
    }
    return shouldClose;
}

/**
 * @brief Cleanup
 */
void Engine::clean()
{
    Physics::clean();

    // Update Objects
    for (auto &[name, it] : objs)
        destroy(name);

    SDL_DestroyWindow(window);
    __engine_init = false;
}

/**
 * @brief Close the app at the next update
 */
void Engine::kill()
{
    shouldClose = true;
}

// Loadings

/**
 * @brief Load a texture
 *
 * @param name The name of the texture
 * @param tex The texture data
 */
void Engine::load(std::string name, texture tex)
{
    texs.insert(std::pair<std::string, texture>(name, tex));
}

/**
 * @brief Load an object
 *
 * @param name The name of the object
 * @param obj The object data
 */
void Engine::load(std::string name, object obj)
{
    objs.insert(std::pair<std::string, object>(name, obj));
}

/**
 * @brief Render a Rectangle
 *
 * @param center The center
 * @param size The size
 * @param color The fill color
 */
void Engine::rect(vec2 center, vec2 size, vec3 color)
{
    glDisable(GL_DEPTH_TEST);

    shader::use(ui_shader);
    shader::set(ui_shader, "center", center);
    shader::set(ui_shader, "size", size / 2);
    shader::set(ui_shader, "type", 1);
    shader::set(ui_shader, "color", color);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glEnable(GL_DEPTH_TEST);
}

// Render Text

/**
 * @brief Render Text
 *
 * @param pos The position (bottom-left corner) of the textbox
 * @param scale The scale of the text (multiplyer)
 * @param text The text to draw
 * @param color The text's color
 */
void Engine::text(vec2 pos, float scale, std::string text, vec3 color)
{
    /*glDisable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    shader::use(ui_shader);

    shader::set(ui_shader, "type", 3);
    shader::set(ui_shader, "color", color);

    float ui_vertices[12];
    float ui_texcoords[12];

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        character ch = chars[*c];

        float xpos = pos.x + ch.offset.x * scale / 1000;
        float ypos = pos.y - (ch.height - ch.offset.y) * scale / 1000;

        float w = ch.width * scale / 1000;
        float h = ch.height * scale / 1000;

        // render glyph texture over quad

        glBindTexture(GL_TEXTURE_2D, ch.tex);

        shader::set(ui_shader, "center", (vec2){xpos, ypos});
        shader::set(ui_shader, "size", (vec2){w, h} / 2);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        pos.x += (ch.advance >> 6) * scale / 1000; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);*/
    debug::warning("graphics::text()", "currently unsupported");
}

// Draw 2D Textures

/**
 * @brief Draw a (Textured) Rectangle
 *
 * @param center The center
 * @param size The size
 */
void Engine::rect(vec2 center, vec2 size, std::string tex)
{
    glDisable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_2D, texs[tex]);

    shader::use(ui_shader);
    shader::set(ui_shader, "center", center);
    shader::set(ui_shader, "size", size / 2);
    shader::set(ui_shader, "type", 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glEnable(GL_DEPTH_TEST);
}

// Draw Buttons

/**
 * @brief Draw a button
 *
 * @param center The center
 * @param size The size
 * @param color_a The unpressed button's fill color
 * @param color_b The pressed button's fill color
 * @param bid The button's id
 * @returns is the button pressed?
 */
bool Engine::rect(vec2 center, vec2 size, vec3 color_a, vec3 color_b, int bid)
{
    if (collide(mouse, center - size / 2, size))
    {
        if (bstate[bid] && mbutton[0] == false)
        {
            bstate[bid] = false;
            return true;
        }
        bstate[bid] = mbutton[0];
    }
    else
        bstate[bid] = false;

    if (bstate[bid]) // Pressed
        rect(center, size, color_b);
    else // Released
        rect(center, size, color_a);

    return false;
}

/**
 * @brief Draw a (textured) button
 *
 * @param center The center
 * @param size The size
 * @param tex_a The unpressed button's texture's name
 * @param tex_b The pressed button's texture's name
 * @param bid The button's id
 * @returns is the button pressed
 */
bool Engine::rect(vec2 center, vec2 size, std::string tex_a, std::string tex_b, int bid)
{
    if (collide(mouse, center - size / 2, size))
    {
        if (bstate[bid] && mbutton[0] == false)
        {
            bstate[bid] = false;
            return true;
        }
        bstate[bid] = mbutton[0];
    }
    else
        bstate[bid] = false;

    if (bstate[bid]) // Pressed
        rect(center, size, tex_a);
    else // Released
        rect(center, size, tex_b);

    return false;
}

/**
 * @brief Get key inputs
 *
 * @param k The key's name
 * @return is the key pressed?
 */
bool Engine::key(std::string k)
{
    // TODO: lower string
    // TODO: add more keys

    k = lowercase(k);
    int v = 0;

    // Letters
    if (k == "a")
        v = 4;
    else if (k == "b")
        v = 5;
    else if (k == "c")
        v = 6;
    else if (k == "d")
        v = 7;
    else if (k == "e")
        v = 8;
    else if (k == "f")
        v = 9;
    else if (k == "g")
        v = 10;
    else if (k == "h")
        v = 11;
    else if (k == "i")
        v = 12;
    else if (k == "j")
        v = 13;
    else if (k == "k")
        v = 14;
    else if (k == "l")
        v = 15;
    else if (k == "m")
        v = 16;
    else if (k == "n")
        v = 17;
    else if (k == "o")
        v = 18;
    else if (k == "p")
        v = 19;
    else if (k == "q")
        v = 20;
    else if (k == "r")
        v = 21;
    else if (k == "s")
        v = 22;
    else if (k == "t")
        v = 23;
    else if (k == "u")
        v = 24;
    else if (k == "v")
        v = 25;
    else if (k == "w")
        v = 26;
    else if (k == "x")
        v = 27;
    else if (k == "y")
        v = 28;
    else if (k == "z")
        v = 29;

    // Numbers
    else if (k == "1")
        v = 30;
    else if (k == "2")
        v = 31;
    else if (k == "3")
        v = 32;
    else if (k == "4")
        v = 33;
    else if (k == "5")
        v = 34;
    else if (k == "6")
        v = 35;
    else if (k == "7")
        v = 36;
    else if (k == "8")
        v = 37;
    else if (k == "9")
        v = 38;
    else if (k == "0")
        v = 39;

    // Control Keys
    else if (k == "return" || k == "enter" || k == "\n")
        v = 40;
    else if (k == "esc")
        v = 41;
    else if (k == "backspace" || k == "\b")
        v = 42;
    else if (k == "tab" || k == "\t")
        v = 43;
    else if (k == "space" || k == " ")
        v = 44;

    else if (k == "lctrl") // left control
        v = 224;
    else if (k == "lshift") // left shift
        v = 225;
    else if (k == "lalt") // left alt
        v = 226;
    else if (k == "lgui") // left GUI (windows logo?)
        v == 227;

    else if (k == "rctrl") // right control
        v = 228;
    else if (k == "rshift") // right shift
        v = 229;
    else if (k == "ralt") // right alt (alt gr)
        v = 230;
    else if (k == "rgui") // right GUI (windows logo?)
        v == 231;

    // Function Keys
    else if (k == "f1")
        v = 58;
    else if (k == "f2")
        v = 59;
    else if (k == "f3")
        v = 60;
    else if (k == "f4")
        v = 61;
    else if (k == "f5")
        v = 62;
    else if (k == "f6")
        v = 63;
    else if (k == "f7")
        v = 64;
    else if (k == "f8")
        v = 65;
    else if (k == "f9")
        v = 66;
    else if (k == "f10")
        v = 67;
    else if (k == "f11")
        v = 68;
    else if (k == "f12")
        v = 69;

    // Mouse Buttons
    else if (k == "ml" || k == "lmouse" || k == "left") // mouse button left
        return mbutton[0];
    else if (k == "mm" || k == "mmouse" || k == "middle") // mouse button middle/scroll
        return mbutton[1];
    else if (k == "mr" || k == "rmouse" || k == "right") // mouse button right
        return mbutton[2];

    else // Unknown Key
    {
        debug::warning("graphics::key()", "unknown key", k.c_str());
        return false;
    }

    return keyboard[v];
}

/**
 * @brief Select a camera to render
 *
 * @param cam The camera
 */
void Engine::select(camera *c)
{
    cam = c;
}

void Engine::destroy(std::string name)
{
    objs[name].destroy();
    // objs.erase(name);
}

vec3 Engine::wasd(float speed)
{
    vec3 out;
    if (cam != NULL)
    {
        if (key("w"))
            out += cam->front * speed * deltaTime;
        if (key("s"))
            out -= cam->front * speed * deltaTime;
        if (key("a"))
            out -= cam->right * speed * deltaTime;
        if (key("d"))
            out += cam->right * speed * deltaTime;

        cam->position += out;
    }
    else
    {
        debug::warning("graphics::wasd()", "no camera selected", "returning vectors");

        if (key("w"))
            out.x += speed * deltaTime;
        if (key("s"))
            out.x -= speed * deltaTime;
        if (key("a"))
            out.z -= speed * deltaTime;
        if (key("d"))
            out.z += speed * deltaTime;
    }

    return out;
}