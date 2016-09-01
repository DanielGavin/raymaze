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

raycast_world raycast_create_world(int grid_size, int world_width, 
						  int world_height, char * worldMap, unsigned int texture_handle_count)
{
	raycast_world world;
	world.grid_size = grid_size;
	world.world_width = world_width;
	world.world_height = world_height;
	world.texture_handle_count = texture_handle_count;
	world.textures = malloc(sizeof(raycast_texture*)*texture_handle_count);
	world.grids = malloc(sizeof(raycast_grid)*world_height*world_width);

	time_t t;
	srand((unsigned)time(&t));

	for (int i = 0; i < world_width*world_height; ++i) {
		
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

raycast_cast_result raycast_find_closest_grid(raycast_world* world, raycast_player* player, double raycast_direction)
{
	//Finding horizontal intersection
	int x_vert, y_vert, distance_vert;
	int x_hori, y_hori, distance_hori;
	int y_slope, x_slope;
	raycast_cast_result result;

	if (180 > raycast_direction > 0) {
		y_hori = (int)floor((double)player->y / (double)world->grid_size) * world->grid_size - 1;
		y_slope = -world->grid_size;
	}

	else {
		y_hori = (int)floor((double)player->y / (double)world->grid_size) * world->grid_size + 64;
		y_slope = world->grid_size;
	}

	x_slope = (int)floor((double)world->grid_size / tan(raycast_direction));

	x_hori = player->x + (int)floor(((double)(player->y - y_hori) / tan(raycast_direction)));

	raycast_grid* grid_hori = raycast_index_grid(x_hori / world->grid_size, y_hori / world->grid_size, world);

	while (grid_hori->type != RAYCAST_WALL) {
		x_hori = x_hori + x_slope;
		y_hori = y_hori + y_slope;
		grid_hori = raycast_index_grid(x_hori / world->grid_size, y_hori / world->grid_size, world);
	}

	//finding vertical interestion

	if (180 > raycast_direction > 90) {
		x_vert = (int)floor(player->x / world->grid_size) - 1;
		x_slope = -world->grid_size;
	}

	else {
		x_vert = (int)floor(player->x / world->grid_size) + 64;
		x_slope = world->grid_size;
	}

	y_vert = (int)floor((double)player->y + (double)(player->x - x_vert) * tan(raycast_direction));
	y_slope = (int)floor((double)world->grid_size * tan(raycast_direction));

	raycast_grid* grid_vert = raycast_index_grid(x_vert / world->grid_size, y_vert / world->grid_size, world);

	while (grid_vert->type != RAYCAST_WALL) {
		x_vert = x_vert + x_slope;
		y_vert = y_vert + y_slope;
		grid_vert = raycast_index_grid(x_vert / world->grid_size, y_vert / world->grid_size, world);
	}


	//calculate the distances of the closest intersections
	distance_hori = (int)floor(abs(player->x - x_hori) / cos(raycast_direction));
	distance_vert = (int)floor(abs(player->x - x_vert) / cos(raycast_direction));

	if (grid_vert && grid_hori) {
		if (distance_hori > distance_vert) {
			result.grid = grid_vert;
			result.ray_length = distance_vert;
		}

		else {
			result.grid = grid_hori;
			result.ray_length = distance_hori;
		}
	}

	else if (grid_vert) {
		result.grid = grid_vert;
		result.ray_length = distance_vert;
	}

	else {
		result.grid = grid_hori;
		result.ray_length = distance_hori;
	}

	//compare between both intersections
	return result;
}

void raycast_render_world(raycast_window* window, raycast_world* world, raycast_player* player)
{
}
