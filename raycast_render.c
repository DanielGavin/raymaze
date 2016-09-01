#include "raycast_render.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

raycast_player raycast_create_player(int x, int y, int height, double viewing_direction)
{
	raycast_player player;
	player.x = x;
	player.y = y;
	player.height = height;
	player.viewing_direction = viewing_direction;
	return player;
}

raycast_world raycast_create_world(unsigned int grid_width, unsigned int grid_height, unsigned int world_width, 
						  unsigned int world_height, char * worldMap, unsigned int texture_handle_count)
{
	raycast_world world;
	world.grid_width = grid_width;
	world.grid_height = grid_height;
	world.world_width = world_width;
	world.world_height = world_height;
	world.texture_handle_count = texture_handle_count;
	world.textures = malloc(sizeof(raycast_texture*)*texture_handle_count);
	world.grids = malloc(sizeof(raycast_grid)*world_height*world_width);
	time_t t;

	srand((unsigned)time(&t));

	for (unsigned int i = 0; i < world_width*world_height; ++i) {
		
		if (worldMap[i] == 'e') {
			world.grids[i].type = RAYCAST_EMPTY;
		}

		else {
			world.grids[i].type = RAYCAST_WALL;
		}

		world.grids[i].r = rand() % 200;
		world.grids[i].g = rand() % 200;
		world.grids[i].b = rand() % 200;
	}

	return world;
}

raycast_window raycast_create_window(unsigned int screen_fov, unsigned int screen_width, unsigned int screen_height, void * screen_buffer)
{
	raycast_window window;
	window.screen_fov = screen_fov;
	window.screen_width = screen_width;
	window.screen_height = screen_height;
	window.screen_buffer = screen_buffer;
	return window;
}

raycast_grid* raycast_index_grid(unsigned int x, unsigned int y, raycast_world* world)
{
	return &world->grids[y*world->world_width + x];
}

void raycast_find_closest_grid(raycast_world* world, raycast_player* player, double raycast_direction)
{
	//Finding horizontal intersection
	unsigned int first_x, first_y;

	if (180 > raycast_direction > 0) {
		first_y = floor(player->y / 64) * 64 - 1;
	}

	else {
		first_y = floor(player->y / 64) * 64 + 64;
	}

	first_x = player->x + (player->y - first_y) / tan(raycast_direction);

	raycast_grid* grid = raycast_index_grid(first_x, first_y, world);

	if (grid->type == RAYCAST_WALL) {

	}
}

void raycast_render_world(raycast_window* window, raycast_world* world, raycast_player* player)
{
}
