#include <engine/engine.h>

int main()
{
	// Init Engine & Camera
	Engine e("Game Engine");
	camera cam(&e.width, &e.height, shader::load("3d"));

	cam.position.z = -25;
	cam.position.y = 20;
	cam.rotation.y = 90;
	cam.rotation.z = -45;

	e.select(&cam);

	// Load in Object

	e.objs["thing"].add(loadin::obj("thing.obj"));
	e.objs["thing"].add("objects.lua");
	e.objs["thing"].add(loadin::image("a.jpg"));
	e.objs["thing"].add(e.objs["thing"].m, true, false);

	e.objs["groundcheck"].add(loadin::obj("cube.obj"), true, false);

	vec3 player = {0.0f, 3.0f, 0.0f};
	vec3 speed;
	// Main Loop
	while (!e.update(0.15, 0.15, 0.15))
	{
		e.objs["groundcheck"].position = player - (vec3){0.0f, 1.3f, 0.0f};
		cam.position = player + (vec3){0.0f, 2.0f, 0.0f};

		bool isGrounded = e.collide("thing", "groundcheck");

		// Player Controls

		// Move Player
		if (e.key("w"))
			player += cam.front * 10 * e.deltaTime;
		if (e.key("s"))
			player -= cam.front * 10.0f * e.deltaTime;
		if (e.key("d"))
			player += cam.right * 10.0f * e.deltaTime;
		if (e.key("a"))
			player -= cam.right * 10.0f * e.deltaTime;

		if (!isGrounded)
			speed.y -= 0.7f * e.deltaTime;
		else
			speed.y = 0;

		if (e.key("space") && isGrounded)
			speed.y = 0.5f;

		player += speed;

		// Rotate Camera

		cam.rotation += {0.0f, e.mouse.x, -e.mouse.y};

		if (cam.rotation.z > 89.9f)
			cam.rotation.z = 89.9f;
		if (cam.rotation.z < -89.9f)
			cam.rotation.z = -89.9f;

		// Exit on [ESC]
		if (e.key("esc"))
			e.kill();
	}

	e.clean();
	return 0;
}