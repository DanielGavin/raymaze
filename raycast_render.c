#include "raycast_render.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define PI 3.14159265

double degree_tan(double x)
{
	return tan(x * PI / 180.0);
}

raycast_player raycast_create_player(int x, int y, double viewing_direction)
{
	raycast_player player;
	player.x = x;
	player.y = y;
	player.viewing_direction = viewing_direction;
	return player;
}

raycast_world raycast_create_world(int grid_size, int world_width, 
						  int world_height, char * worldMap, int texture_handle_count)
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
			world.grids[i].r = 0;
			world.grids[i].g = 0;
			world.grids[i].b = 0;
		}

		else {
			world.grids[i].type = RAYCAST_WALL;
			world.grids[i].r = rand() % 200;
			world.grids[i].g = rand() % 200;
			world.grids[i].b = rand() % 200;
		}

	}

	return world;
}

raycast_window raycast_create_window(int screen_fov, int screen_width, int screen_height, void * screen_buffer)
{
	raycast_window window;
	window.screen_fov = screen_fov;
	window.screen_width = screen_width;
	window.screen_height = screen_height;
	window.screen_buffer = screen_buffer;
	window.center_x = screen_width / 2;
	window.center_y = screen_height / 2;
	window.ray_angle = (double)screen_fov / (double)screen_width;
	window.projection_distance = (int)abs(((double)screen_width / 2.0) / degree_tan((double)screen_fov / 2.0));
	return window;
}

raycast_grid* raycast_index_grid(int x, int y, raycast_world* world)
{
	if (y*world->world_width + x >= world->world_height * world->world_width) {
		return NULL;
	}

	if (x < 0 || y < 0) {
		return NULL;
	}

	return &world->grids[y*world->world_width + x];
}


double degree_clamp(double x)
{
	double deg = fmod(x, 360.0);

	if (deg < 0) {
		deg += 360.0;
	}

	return deg;
}

raycast_cast_result raycast_find_closest_grid(raycast_world* world, raycast_player* player, double raycast_direction)
{
	//Finding horizontal intersection
	int x_vert, y_vert;
	int x_hori, y_hori;
	double distance_vert, distance_hori;
	double y_slope, x_slope;
	raycast_cast_result result;
	result.grid = NULL;

	//x_slope = abs((double)world->grid_size / degree_tan(raycast_direction));
	x_slope = ((double)world->grid_size / degree_tan(raycast_direction));

	if (180.0 > raycast_direction && raycast_direction > 0.0) {
		y_hori = (int)floor((double)player->y / (double)world->grid_size) * world->grid_size - 1;
		y_slope = -world->grid_size;
	}

	else {
		y_hori = (int)floor((double)player->y / (double)world->grid_size) * world->grid_size + world->grid_size;
		y_slope = world->grid_size;
	}

	/*
	if (180.0 > raycast_direction && raycast_direction > 90.0) {
		x_slope = -x_slope;
	}
	*/

	x_hori = player->x + (player->y - y_hori) / degree_tan(raycast_direction);

	raycast_grid* grid_hori = NULL;

	printf("x_hori:%d y_hori: %d \n", x_hori, y_hori);

	while (2200 > x_hori && x_hori >= 0 && 2200 > y_hori && y_hori >= 0) {

		grid_hori = raycast_index_grid(x_hori / world->grid_size, y_hori / world->grid_size, world);

		if (grid_hori && grid_hori->type == RAYCAST_WALL) {
			break;
		}

		x_hori = x_hori + x_slope;
		y_hori = y_hori + y_slope;
	}

	//finding vertical interestion
	//y_slope = abs((double)world->grid_size * degree_tan(raycast_direction));
	y_slope = ((double)world->grid_size * degree_tan(raycast_direction));

	if (180.0 > raycast_direction && raycast_direction > 90.0) {
		x_vert = (int)floor((double)player->x / (double)world->grid_size) * world->grid_size - 1;
		x_slope = -world->grid_size;
	}

	else {
		x_vert = (int)floor((double)player->x / (double)world->grid_size) * world->grid_size + world->grid_size;
		x_slope = world->grid_size;
	}

	/*
	if (180.0 > raycast_direction && raycast_direction > 0.0) {
		y_slope = -y_slope;
	}
	*/

	y_vert = (int)((double)player->y + (double)(player->x - x_vert) * degree_tan(raycast_direction));

	raycast_grid* grid_vert = NULL;

	printf("x_vert:%d y_vert: %d \n", x_vert, y_vert);

	while (2200 > x_vert && x_vert >= 0 && 2200 > y_vert && y_vert >= 0) {

		grid_vert = raycast_index_grid(x_vert / world->grid_size, y_vert / world->grid_size, world);

		if (grid_vert && grid_vert->type == RAYCAST_WALL) {
			break;
		}

		x_vert = x_vert + x_slope;
		y_vert = y_vert + y_slope;
	}


	//calculate the distances of the closest intersections
	double distorted = cos(degree_clamp(raycast_direction - player->viewing_direction) * PI / 180.0);

	assert(distorted > 0.0);

	distance_hori = sqrt((player->x - x_hori)*(player->x - x_hori) + (player->y - y_hori)*(player->y - y_hori));
	distance_vert = sqrt((player->x - x_vert)*(player->x - x_vert) + (player->y - y_vert)*(player->y - y_vert));

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

	else if(grid_hori) {
		result.grid = grid_hori;
		result.ray_length = distance_hori;
	}

	//compare between both intersections
	return result;
}

void raycast_render_world(raycast_window* window, raycast_world* world, raycast_player* player)
{
	double begin_ray = player->viewing_direction - ((double)window->screen_fov / 2.0);

	for (int i = 0; i < window->screen_width; ++i) {
		double ray_angle = begin_ray + window->ray_angle * (double)i;
		raycast_cast_result result = raycast_find_closest_grid(world, player, floor(ray_angle));

		int projected_height = (int)(((double)world->grid_size / (double)result.ray_length) * (double)window->projection_distance);

		if (projected_height > window->screen_height) {
			projected_height = window->screen_height;
		}

		int start = (int)ceil(window->center_y - ((double)projected_height / 2.0));


		if (result.grid && result.grid->type == RAYCAST_WALL) {

			for (int j = start; j < start + projected_height; ++j) {
				int pixel = (j*window->screen_width + i) * 4;
				window->screen_buffer[pixel] = 255;
				window->screen_buffer[pixel + 1] = result.grid->r;
				window->screen_buffer[pixel + 2] = result.grid->g;
				window->screen_buffer[pixel + 3] = result.grid->b;
			}
			
		}

	}

}
