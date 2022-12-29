#pragma once

#include <pthread.h>
#include <string>
#include <map>

#include <engine/object.h>

/**
 * @brief The Physics class' struct to return collision data from 2nd thread
 */
struct __physics_collision_t
{
    std::map<std::string, object> *objs;

    std::vector<std::string> body_a_name;
    std::vector<std::string> body_b_name;
    std::vector<vec3> point;

    bool write = false;
    bool read = false;

    bool online = true;
};

class Physics
{
private:
    // ----- THREADING -----
    bool threading = false;

    pthread_t process;
    __physics_collision_t collisions;

    void *shared[2];

    static void *collision_thread(void *pointer);

    static bool c_line_to_tri(vec3 a[2], vec3 b[3]);
    static bool c_tri_to_tri(vec3 a[3], vec3 b[3]);

    static bool c_mesh_to_mesh(object a, object b);

public:
    void init(std::map<std::string, object> *objs, bool threading = true);
    void clean();

    void add(object o, std::string name);

    bool collide(std::string a, std::string b);

    bool collide(vec2 point, vec2 start, vec2 size);
};

// Private Functions

void *Physics::collision_thread(void *pointer)
{
    __physics_collision_t local;
    __physics_collision_t *shared = (__physics_collision_t *)pointer;
    while (shared->online)
    {
        local.body_a_name.clear();
        local.body_b_name.clear();
        local.point.clear();

        shared->body_a_name.clear();
        shared->body_b_name.clear();
        shared->point.clear();

        // Calculate new collisions
        for (auto &[name, it] : *(shared->objs))
        {
        }

        // Wait while Main is reading
        while (shared->read)
            ;

        shared->write = true;

        // Update "shared" variable
        *shared = local;

        shared->write = false;
    }
}

// End

// Default Collision

bool Physics::c_line_to_tri(vec3 a[2], vec3 b[3])
{
    bool lower_x = false;
    bool higher_x = false;

    bool lower_y = false;
    bool higher_y = false;

    bool lower_z = false;
    bool higher_z = false;

    vec3 point;

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (a[i].x <= b[j].x)
                lower_x = true;
            if (a[i].x >= b[j].x)
                higher_x = true;

            if (a[i].y <= b[j].y)
                lower_y = true;
            if (a[i].y >= b[j].y)
                higher_y = true;

            if (a[i].z <= b[j].z)
                lower_z = true;
            if (a[i].z >= b[j].z)
                higher_z = true;
        }
    }
    if (lower_x and higher_x and lower_y and higher_y and lower_z and higher_z)
        return true;
    return false;
}

bool Physics::c_tri_to_tri(vec3 a[3], vec3 b[3])
{
    bool lower_x = false;
    bool higher_x = false;

    bool lower_y = false;
    bool higher_y = false;

    bool lower_z = false;
    bool higher_z = false;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (a[i].x <= b[j].x)
                lower_x = true;
            if (a[i].x >= b[j].x)
                higher_x = true;

            if (a[i].y <= b[j].y)
                lower_y = true;
            if (a[i].y >= b[j].y)
                higher_y = true;

            if (a[i].z <= b[j].z)
                lower_z = true;
            if (a[i].z >= b[j].z)
                higher_z = true;
        }
    }

    if (lower_x and higher_x and lower_y and higher_y and lower_z and higher_z)
        return true;
    return false;
}

bool Physics::c_mesh_to_mesh(object a, object b)
{
    for (int i = 0; i < (int)a.c.vertices.size() / 9; i++)
    {
        for (int j = 0; j < (int)b.c.vertices.size() / 9; j++)
        {
            vec3 a_tri[3];

            a_tri[0] = {a.c.vertices[i * 9] + a.position.x,
                        a.c.vertices[i * 9 + 1] + a.position.y,
                        a.c.vertices[i * 9 + 2] + a.position.z};

            a_tri[1] = {a.c.vertices[i * 9 + 3] + a.position.x,
                        a.c.vertices[i * 9 + 3 + 1] + a.position.y,
                        a.c.vertices[i * 9 + 3 + 2] + a.position.z};

            a_tri[2] = {a.c.vertices[i * 9 + 6] + a.position.x,
                        a.c.vertices[i * 9 + 6 + 1] + a.position.y,
                        a.c.vertices[i * 9 + 6 + 2] + a.position.z};

            vec3 b_tri[3];

            b_tri[0] = {b.c.vertices[j * 9] + b.position.x,
                        b.c.vertices[j * 9 + 1] + b.position.y,
                        b.c.vertices[j * 9 + 2] + b.position.z};

            b_tri[1] = {b.c.vertices[j * 9 + 3] + b.position.x,
                        b.c.vertices[j * 9 + 3 + 1] + b.position.y,
                        b.c.vertices[j * 9 + 3 + 2] + b.position.z};

            b_tri[2] = {b.c.vertices[j * 9 + 6] + b.position.x,
                        b.c.vertices[j * 9 + 6 + 1] + b.position.y,
                        b.c.vertices[j * 9 + 6 + 2] + b.position.z};

            if (c_tri_to_tri(a_tri, b_tri))
                return true;
        }
    }

    return false;
}

// End

void Physics::init(std::map<std::string, object> *objs, bool threading)
{
    this->collisions.objs = objs;
    this->threading = threading;
    if (this->threading)
    {
        // Launch Collision-Detection Thread
        pthread_create(&this->process, NULL, this->collision_thread, (void *)&this->collisions);
    }
}

void Physics::clean()
{
    this->collisions.online = false;
}

bool Physics::collide(std::string a, std::string b)
{
    if (threading)
    {
        // Wait for writing to end
        while (collisions.write)
            ;
        collisions.read = true;

        for (int i = 0; i < (int)collisions.body_a_name.size(); i++)
        {
            for (int j = 0; j < (int)collisions.body_b_name.size(); j++)
            {
                std::string name[2] = {collisions.body_a_name[i], collisions.body_b_name[j]};

                if ((name[0] == a && name[1] == b) || (name[0] == b && name[1] == a))
                {
                    collisions.read = false;
                    return true;
                }
            }
        }

        collisions.read = false;
    }
    else
    {
        return c_mesh_to_mesh((*this->collisions.objs)[a], (*this->collisions.objs)[b]); // Does it on this thread -> very slow
    }
    return false;
}

/**
 * @brief Check if a 2D Point collides with a 2D rect
 *
 * @param point The point
 * @param start The start (top-left) point of the rect
 * @param size The size of the rect
 * @returns collision
 */
bool Physics::collide(vec2 point, vec2 start, vec2 size)
{
    if (start.y + size.y > point.y and point.y > start.y)
    {
        if (start.x + size.x > point.x and point.x > start.x)
        {
            return true;
        }
    }
    return false;
}