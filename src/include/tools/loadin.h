// LoadIn Library for the Game Engine
#pragma once

#include <tools/shader.h>

#include <ft2build.h>
#include FT_FREETYPE_H

/**
 * @brief The LoadIn Library
 */
namespace loadin
{
    bool enable_logs = true;
    /**
     * @brief Load an image from a file (i.e.: .png or .jpg)
     *
     * @param path The path of the image
     * @param filter Select the filtering option (GL_LINEAR, GL_NEAREST), default: GL_LINEAR
     * @return The ID of the texture
     */
    texture image(std::string path, int filter = GL_LINEAR)
    {
        texture out;

        // Load the image into a surface
        SDL_Surface *surface = IMG_Load(("res/" + path).c_str());
        if (surface != NULL)
        {
            // Flip the surface & generate textures
            flip_surface(surface);
            glGenTextures(1, &out);
            glBindTexture(GL_TEXTURE_2D, out);

            // Decide, whether it has an alpha channel or not
            int Mode = GL_RGB;
            if (surface->format->BytesPerPixel == 4)
                Mode = GL_RGBA;

            // Load texture into OpenGL
            glTexImage2D(GL_TEXTURE_2D, 0, Mode, surface->w, surface->h, 0, Mode, GL_UNSIGNED_BYTE, surface->pixels);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

            // Free surface
            SDL_FreeSurface(surface);

            if (enable_logs)
                debug::log("loadin::image()", "loaded texture");
        }
        else
            debug::warning("loadin::image()", "can't load image", path.c_str());

        return out;
    }

    /**
     * @brief Load a font from a file
     *
     * @param path The path of the font
     */
    font ttf(std::string path, uint resolution = 256)
    {
        font out;

        FT_Library ft;
        FT_Face face;

        if (FT_Init_FreeType(&ft) || FT_New_Face(ft, ("res/fonts/" + path).c_str(), 0, &face))
        {
            debug::error("loadin::ttf()", "failed to open font", path.c_str());
        }
        else
        {
            FT_Set_Pixel_Sizes(face, 0, resolution); // set size to load glyphs as

            // disable byte-alignment restriction
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // load first 128 characters of ASCII set
            for (unsigned char c = 0; c < 128; c++)
            {
                // Load character glyph
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    debug::warning("loadin::ttf()", "failed to load glyph");
                    continue;
                }

                // generate texture
                unsigned int texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer);
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                // now store character for later use

                character newchar;
                newchar.tex = texture;
                newchar.width = face->glyph->bitmap.width;
                newchar.height = face->glyph->bitmap.rows;
                newchar.offset = {(float)face->glyph->bitmap_left, (float)face->glyph->bitmap_top};
                newchar.advance = face->glyph->advance.x;

                out.insert(std::pair<char, character>(c, newchar));
            }
            if (enable_logs)
                debug::log("loadin::ttf()", "loaded font");
        }
        // destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        return out;
    }

    /**
     * @brief Load a Material Template Library (.MTL) File
     *
     * @param path The path of the file
     * @return The loaded material (a new material on fail)
     */
    material mtl(std::string path)
    {
        material out;

        std::ifstream f("res/" + path);
        if (f.is_open())
        {
            std::string line;
            while (getline(f, line))
            {
                std::string index = despace(split(line, " ", 0));
                if (line[0] == '#' || index == "" || index == " " || index == "\n")
                {
                    // Line is a comment
                }
            }

            f.close();

            if (enable_logs)
                debug::log("loadin::mtl()", "loaded material");
        }

        return out;
    }

    /**
     * @brief Load a Wavefront .OBJ file
     * @details See wikipedia for reference:
     * https://en.wikipedia.org/wiki/Wavefront_.obj_file
     *
     * @param path The path of the file
     * @return The loaded object (a new object on fail)
     */
    mesh obj(std::string path)
    {
        mesh out;

        std::ifstream f("res/" + path);
        if (f.is_open())
        {
            std::vector<vec3> vertices;
            std::vector<vec2> texcoords;
            std::vector<vec3> normals;

            std::string line;
            while (getline(f, line))
            {
                std::string index = despace(split(line, " ", 0));

                if (line[0] == '#' || index == "" || index == " " || index == "\n")
                {
                    // Line is a comment
                }
                else if (index == "o")
                {
                    // New Object Starts Here (?)
                    // TODO
                    out.name = split(line, " ", 1);
                }
                else if (index == "v")
                {
                    // Vertex Coordinate(s)
                    if (count(line, ' ') == 3 || count(line, ' ') == 4)
                    {
                        // TODO
                        vertices.push_back({fsplit(line, " ", 1), fsplit(line, " ", 2), fsplit(line, " ", 3)});
                    }
                    else
                    {
                        debug::warning("loadin::obj()", "argument mismatch", "vertex");
                    }
                }
                else if (index == "vt")
                {
                    // Vertex Texture Coordinate(s)
                    if (count(line, ' ') == 2 || count(line, ' ') == 3)
                    {
                        // has u, v (and w sometimes, but we don't use it)
                        // TODO
                        texcoords.push_back({fsplit(line, " ", 1), fsplit(line, " ", 2)});
                    }
                    else
                    {
                        debug::warning("loadin::obj()", "argument mismatch", "texcoord");
                    }
                }
                else if (index == "vn")
                {
                    // Vertex Normal Coordinate(s)
                    if (count(line, ' ') == 3)
                    {
                        // has x, y, z
                        normals.push_back({fsplit(line, " ", 1), fsplit(line, " ", 2), fsplit(line, " ", 3)});
                    }
                    else
                    {
                        debug::warning("loadin::obj()", "argument mismatch", "normal");
                    }
                }
                else if (index == "vp")
                {
                    // Parameter space vertices (for curves and stuff)
                    // TODO
                }
                else if (index == "f")
                {
                    // Face Element
                    int mode = count(line, '/');

                    if (mode % 3 == 0)
                    {
                        std::string part[3];
                        part[0] = split(line, " ", 1);
                        part[1] = split(line, " ", 2);
                        part[2] = split(line, " ", 3);

                        if (mode == 0)
                        {
                            for (int i = 0; i < 3; i++)
                            {
                                out.vertices.push_back(vertices[stoi(part[i]) - 1].x);
                                out.vertices.push_back(vertices[stoi(part[1]) - 1].y);
                                out.vertices.push_back(vertices[stoi(part[2]) - 1].z);
                            }
                        }
                        else if (mode == 3 || mode == 6)
                        {
                            for (int i = 0; i < 3; i++)
                            {
                                out.vertices.push_back(vertices[isplit(part[i], "/", 0) - 1].x);
                                out.vertices.push_back(vertices[isplit(part[i], "/", 0) - 1].y);
                                out.vertices.push_back(vertices[isplit(part[i], "/", 0) - 1].z);
                            }
                        }

                        if (mode == 3 || mode == 6)
                        {
                            // has texcoords
                            for (int i = 0; i < 3; i++)
                            {
                                out.texcoords.push_back(texcoords[isplit(part[i], "/", 1) - 1].x);
                                out.texcoords.push_back(texcoords[isplit(part[i], "/", 1) - 1].y);
                            }
                        }

                        if (mode == 6)
                        {
                            // has normals
                            for (int i = 0; i < 3; i++)
                            {
                                out.normals.push_back(normals[isplit(part[i], "/", 2) - 1].x);
                                out.normals.push_back(normals[isplit(part[i], "/", 2) - 1].y);
                                out.normals.push_back(normals[isplit(part[i], "/", 2) - 1].z);
                            }
                        }

                        out.tris++;
                    }
                    else
                        debug::warning("loadin::obj()", "corrupted or invalid file");
                }
                else if (index == "l")
                {
                    // Line Element
                    // TODO
                }
                else if (index == "mtllib")
                {
                    // Load a material template library
                    out.mtl = mtl(split(line, " ", 1));
                }
                else if (index == "usemtl")
                {
                    // Use a material
                    // TODO
                }
                else if (index == "g")
                {
                    // Object belongs to a Group
                    // TODO
                }
                else if (index == "s")
                {
                    // Smooth shading
                    // TODO
                }
                else
                {
                    debug::warning("loadin::obj()", "unrecognized element(s)", index.c_str());
                }
            }

            f.close();

            if (enable_logs)
                debug::log("loadin::obj()", "loaded mesh");
        }
        else
            debug::warning("loadin::obj()", "can't open file", path.c_str());

        return out;
    }
}