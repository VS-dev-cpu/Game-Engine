#pragma once

#include <vector>
#include <string>
#include <math.h>
#include <map>

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef M_RAD
#define M_RAD 0.01745329251994329576923690768489f
#endif

#ifndef M_DEG
#define M_DEG 57.295779513082320876798154814105f
#endif

/**
 * @brief 2D Vector
 */
struct vec2
{
    float x = 0.0f;
    float y = 0.0f;

    float w = 1.0f;

    vec2 operator+(const vec2 v)
    {
        vec2 ret;
        ret.x = this->x + v.x;
        ret.y = this->y + v.y;
        return ret;
    }

    vec2 operator-(const vec2 v)
    {
        vec2 ret;
        ret.x = this->x - v.x;
        ret.y = this->y - v.y;
        return ret;
    }

    vec2 operator*(const float v)
    {
        vec2 ret;
        ret.x = this->x * v;
        ret.y = this->y * v;
        return ret;
    }

    vec2 operator/(const float v)
    {
        vec2 ret;
        ret.x = this->x / v;
        ret.y = this->y / v;
        return ret;
    }

    void operator+=(const vec2 v)
    {
        this->x += v.x;
        this->y += v.y;
    }

    void operator-=(const vec2 v)
    {
        this->x -= v.x;
        this->y -= v.y;
    }

    void operator=(const vec2 v)
    {
        this->x = v.x;
        this->y = v.y;

        this->w = v.w;
    }

    bool operator==(const vec2 v)
    {
        return this->x == v.x && this->y == v.y;
    }

    bool operator!=(const vec2 v)
    {
        return this->x != v.x || this->y != v.y;
    }
};

/**
 * @brief 3D Vector
 */
struct vec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    float w = 1.0f;

    vec3 operator+(const vec3 v)
    {
        vec3 ret;
        ret.x = this->x + v.x;
        ret.y = this->y + v.y;
        ret.z = this->z + v.z;
        return ret;
    }

    vec3 operator-(const vec3 v)
    {
        vec3 ret;
        ret.x = this->x - v.x;
        ret.y = this->y - v.y;
        ret.z = this->z - v.z;
        return ret;
    }

    vec3 operator*(const float v)
    {
        vec3 ret;
        ret.x = this->x * v;
        ret.y = this->y * v;
        ret.z = this->z * v;
        return ret;
    }

    vec3 operator/(const float v)
    {
        vec3 ret;
        ret.x = this->x / v;
        ret.y = this->y / v;
        ret.z = this->z / v;
        return ret;
    }

    void operator+=(const vec3 v)
    {
        this->x += v.x;
        this->y += v.y;
        this->z += v.z;
    }

    void operator-=(const vec3 v)
    {
        this->x -= v.x;
        this->y -= v.y;
        this->z -= v.z;
    }

    void operator=(const vec3 v)
    {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;

        this->w = v.w;
    }

    bool operator==(const vec3 v)
    {
        return this->x == v.x && this->y == v.y && this->z == v.z;
    }

    bool operator!=(const vec3 v)
    {
        return this->x != v.x || this->y != v.y || this->z != v.z;
    }
};

namespace vector
{
    vec3 avg(vec3 v1, vec3 v2)
    {
        vec3 ret;
        ret.x = (v1.x + v2.x) / 2;
        ret.y = (v1.y + v2.y) / 2;
        ret.z = (v1.z + v2.z) / 2;
        return ret;
    }

    vec3 avg(vec3 v1, vec3 v2, vec3 v3)
    {
        vec3 ret;
        ret.x = (v1.x + v2.x + v3.x) / 3;
        ret.y = (v1.y + v2.y + v3.y) / 3;
        ret.z = (v1.z + v2.z + v3.z) / 3;
        return ret;
    }

    vec3 avg(vec3 v1, vec3 v2, vec3 v3, vec3 v4)
    {
        vec3 ret;
        ret.x = (v1.x + v2.x + v3.x + v4.x) / 4;
        ret.y = (v1.y + v2.y + v3.y + v4.y) / 4;
        ret.z = (v1.z + v2.z + v3.z + v4.z) / 4;
        return ret;
    }

    float dotproduct(vec3 v1, vec3 v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    float length(vec3 v)
    {
        return sqrtf(dotproduct(v, v));
    }

    float distance(vec3 v1, vec3 v2)
    {
        vec3 dist = v1 - v2;

        float a = dist.x * dist.x + dist.y * dist.y;
        return sqrt(a + dist.z * dist.z);
    }

    vec3 normalize(vec3 v)
    {
        float l = length(v);
        vec3 vv;
        vv.x = v.x / l;
        vv.y = v.y / l;
        vv.z = v.z / l;
        return vv;
    }

    vec3 crossproduct(vec3 v1, vec3 v2)
    {
        vec3 v;
        v.x = v1.y * v2.z - v1.z * v2.y;
        v.y = v1.z * v2.x - v1.x * v2.z;
        v.z = v1.x * v2.y - v1.y * v2.x;
        return v;
    }
};

#undef near
#undef far

/**
 * @brief 4 by 4 matrix
 */
struct mat4
{
    float m[4][4] = {0};

    mat4 operator*(const mat4 m)
    {
        mat4 matrix;
        for (int c = 0; c < 4; c++)
            for (int r = 0; r < 4; r++)
                matrix.m[r][c] = this->m[r][0] * m.m[0][c] + this->m[r][1] * m.m[1][c] + this->m[r][2] * m.m[2][c] + this->m[r][3] * m.m[3][c];
        return matrix;
    }

    void operator=(const mat4 m)
    {
        for (int c = 0; c < 4; c++)
            for (int r = 0; r < 4; r++)
                this->m[r][c] = m.m[r][c];
    }
};

namespace matrix
{
    mat4 identity()
    {
        mat4 matrix;

        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;

        return matrix;
    }

    mat4 inverse(mat4 m)
    { // Only for Rotation/Translation Matrices
        mat4 matrix;
        matrix.m[0][0] = m.m[0][0];
        matrix.m[0][1] = m.m[1][0];
        matrix.m[0][2] = m.m[2][0];
        matrix.m[0][3] = 0.0f;
        matrix.m[1][0] = m.m[0][1];
        matrix.m[1][1] = m.m[1][1];
        matrix.m[1][2] = m.m[2][1];
        matrix.m[1][3] = 0.0f;
        matrix.m[2][0] = m.m[0][2];
        matrix.m[2][1] = m.m[1][2];
        matrix.m[2][2] = m.m[2][2];
        matrix.m[2][3] = 0.0f;
        matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
        matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
        matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    mat4 perspective(float fovDegrees, float aspect, float near, float far)
    {
        float fov = fovDegrees * M_RAD;
        float tanHalfFov = tan(fov / 2.0f);

        mat4 result;
        result.m[0][0] = 1.0f / (aspect * tanHalfFov);
        result.m[1][1] = 1.0f / (tanHalfFov);
        result.m[2][2] = -(far + near) / (far - near);
        result.m[2][3] = -1.0f;
        result.m[3][2] = -(2.0f * far * near) / (far - near);
        return result;
    }

    mat4 rotationX(float rad)
    {
        mat4 matrix;
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = cosf(rad);
        matrix.m[1][2] = sinf(rad);
        matrix.m[2][1] = -sinf(rad);
        matrix.m[2][2] = cosf(rad);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    mat4 rotationOffsetX(float rad, float x)
    {
        mat4 matrix;
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = cosf(rad * x);
        matrix.m[1][2] = sinf(rad * x);
        matrix.m[2][1] = -sinf(rad * x);
        matrix.m[2][2] = cosf(rad * x);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    mat4 rotationY(float rad)
    {
        mat4 matrix;
        matrix.m[0][0] = cosf(rad);
        matrix.m[0][2] = sinf(rad);
        matrix.m[2][0] = -sinf(rad);
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = cosf(rad);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    mat4 rotationOffsetY(float rad, float y)
    {
        mat4 matrix;
        matrix.m[0][0] = cosf(rad * y);
        matrix.m[0][2] = sinf(rad * y);
        matrix.m[2][0] = -sinf(rad * y);
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = cosf(rad * y);
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    mat4 rotationZ(float rad)
    {
        mat4 matrix;
        matrix.m[0][0] = cosf(rad);
        matrix.m[0][1] = sinf(rad);
        matrix.m[1][0] = -sinf(rad);
        matrix.m[1][1] = cosf(rad);
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    mat4 rotationOffsetZ(float rad, float z)
    {
        mat4 matrix;
        matrix.m[0][0] = cosf(rad * z);
        matrix.m[0][1] = sinf(rad * z);
        matrix.m[1][0] = -sinf(rad * z);
        matrix.m[1][1] = cosf(rad * z);
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        return matrix;
    }

    // Matrix And Vector

    mat4 translate(vec3 v)
    {
        mat4 matrix;
        matrix.m[0][0] = 1.0f;
        matrix.m[1][1] = 1.0f;
        matrix.m[2][2] = 1.0f;
        matrix.m[3][3] = 1.0f;
        matrix.m[3][0] = v.x;
        matrix.m[3][1] = v.y;
        matrix.m[3][2] = v.z;
        return matrix;
    }

    mat4 rotate(vec3 rad)
    {
        mat4 matrix, x, y, z;

        x = rotationX(rad.x);
        y = rotationY(rad.y);
        z = rotationZ(rad.z);

        matrix = identity();
        matrix = x * y;
        matrix = matrix * z;

        return matrix;
    }

    mat4 rotateOffset(vec3 rad, vec3 point)
    {
        mat4 matrix, x, y, z;

        x = rotationOffsetX(rad.x, point.x);
        y = rotationOffsetY(rad.y, point.y);
        z = rotationOffsetZ(rad.z, point.z);

        matrix = identity();
        matrix = x * y;
        matrix = matrix * z;

        return matrix;
    }

    mat4 lookAt(vec3 pos, vec3 target, vec3 up)
    {
        vec3 f = vector::normalize(target - pos);
        vec3 s = vector::normalize(vector::crossproduct(f, up));
        vec3 u = vector::crossproduct(s, f);

        mat4 result = identity();
        result.m[0][0] = s.x;
        result.m[1][0] = s.y;
        result.m[2][0] = s.z;
        result.m[0][1] = u.x;
        result.m[1][1] = u.y;
        result.m[2][1] = u.z;
        result.m[0][2] = -f.x;
        result.m[1][2] = -f.y;
        result.m[2][2] = -f.z;
        result.m[3][0] = -vector::dotproduct(s, pos);
        result.m[3][1] = -vector::dotproduct(u, pos);
        result.m[3][2] = vector::dotproduct(f, pos);
        return result;
    }

    vec3 multiplyvec(mat4 m, vec3 i)
    {
        vec3 v;
        v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
        v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
        v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
        v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
        return v;
    }
};

/**
 * @brief OpenGL's textures
 */
typedef uint texture;

/**
 * @brief One Renderable Character
 */
struct character
{
    texture tex;        // ID handle of the glyph texture
    uint width, height; // Size of glyph
    vec2 offset;        // Offset from baseline to left/top of glyph
    uint advance;       // Horizontal offset to advance to next glyph
};

/**
 * @brief Fonts
 */
typedef std::map<char, character> font;

/**
 * @brief The descriptor of an object's look
 */
struct material
{
    bool lit;
    bool textured;

    float d;

    texture diffuse;
    texture specular;

    float shininess;
};

/**
 * @brief 3D Data
 */
struct mesh
{
    // The name
    std::string name;

    // The material
    material mtl;

    // The Mesh Data
    std::vector<float> vertices; // {x, y, z} * indices * 3 = tri
    std::vector<float> texcoords;
    std::vector<float> normals;
    int tris = 0;

    // The Scale of the Mesh
    float scale;
};