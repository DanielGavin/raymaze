#include "raycast_render.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>


raycast_player raycast_create_player(double x, double y, double view_x, double view_y, double plane_x, double plane_y)
{
	raycast_player player;
	player.x = x;
	player.y = y;
	player.view_x = view_x + 0.0000001;
	player.view_y = view_y + 0.0000001;
	player.plane_x = plane_x + 0.0000001;
	player.plane_y = plane_y + 0.0000001;
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




raycast_cast_result raycast_find_closest_grid(raycast_world* world, raycast_player* player, double raycast_direction)
{
	
}

void raycast_render_world(raycast_window* window, raycast_world* world, raycast_player* player)
{
	for (int i = 0; i < window->screen_width; ++i) {
		//calculate ray position and direction
		double camera_x = 2.0 * (double)i / (double)window->screen_width - 1.0; 
		double ray_dir_x = player->view_x + player->plane_x * camera_x;
		double ray_dir_y = player->view_y + player->plane_y * camera_x;
		double ray_pos_x = player->x;
		double ray_pos_y = player->y;
		double side_x = 0;
		double side_y = 0;
		double delta_x = sqrt(1.0 + (ray_dir_y * ray_dir_y) / (ray_dir_x * ray_dir_x));
		double delta_y = sqrt(1.0 + (ray_dir_x * ray_dir_x) / (ray_dir_y * ray_dir_y));
		double perp_wall_dist = 0;

		int map_x = (int)ray_pos_x;
		int map_y = (int)ray_pos_y;
		int step_x = 0;
		int step_y = 0;

		int hit = 0;
		int side = 0; 

		//calculate step and initial sideDisto

		//ray goes left x wise
		if (ray_dir_x < 0)
		{
			step_x = -1;
			side_x = (ray_pos_x - map_x) * delta_x;
		}

		//ray goes right x wise
		else
		{
			step_x = 1;
			side_x = (map_x + 1.0 - ray_pos_x) * delta_x;
		}

		//ray goes up y wise
		if (ray_dir_y < 0)
		{
			step_y = -1;
			side_y = (ray_pos_y - map_y) * delta_y;
		}

		//ray goes down y wise
		else
		{
			step_y = 1;
			side_y = (map_y + 1.0 - ray_pos_y) * delta_y;
		}

		while (hit == 0)
		{
			//step to the next grid
			if (side_x < side_y)
			{
				side_x += delta_x;
				map_x += step_x;
				side = 0;
			}
			else
			{
				side_y += delta_y;
				map_y += step_y;
				side = 1;
			}

			//see if we hit a wall

			raycast_grid* grid = raycast_index_grid(map_x, map_y, world);

			assert(grid);

			if (grid && grid->type == RAYCAST_WALL) {
				hit = 1;
			}

			//else if (!(2000 > map_x && map_x >= 0 && 2000 > map_y && map_y >= 0)) {
			//	hit = 1;
			//}
		}

		if (side == 0) {
			perp_wall_dist = (map_x - ray_pos_x + (1.0 - step_x) / 2.0) / ray_dir_x;
		}

		else {
			perp_wall_dist = (map_y - ray_pos_y + (1.0 - step_y) / 2.0) / ray_dir_y;
		}

		int projected_height = (int)(window->screen_height / perp_wall_dist);

		int start = -projected_height / 2 + window->screen_height / 2;

		if (start < 0) {
			start = 0;
		}

		int end = projected_height / 2 + window->screen_height / 2;

		if (end >= window->screen_height) {
			end = window->screen_height - 1;
		}

		raycast_grid* grid = raycast_index_grid(map_x, map_y, world);

		if (grid && grid->type == RAYCAST_WALL) {

			for (int j = start; j < end; ++j) {
				int pixel = (j*window->screen_width + i) * 4;
				window->screen_buffer[pixel] = 255;
				window->screen_buffer[pixel + 1] = grid->r;
				window->screen_buffer[pixel + 2] = grid->g;
				window->screen_buffer[pixel + 3] = grid->b;
			}

		}

	}
}
