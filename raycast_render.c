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

raycast_world raycast_create_world(int world_width, int world_height, raycast_grid* grids)
{
	raycast_world world;
	world.world_width = world_width;
	world.world_height = world_height;
	world.grids = grids;
	return world;
}

raycast_window raycast_create_window(int screen_width, int screen_height, void * screen_buffer)
{
	raycast_window window;
	window.screen_width = screen_width;
	window.screen_height = screen_height;
	window.screen_buffer = screen_buffer;
	return window;
}

raycast_texture raycast_create_texture(int width, int height, unsigned char * data, unsigned int texture_handle, raycast_world * world)
{
	raycast_texture texture;
	texture.components = 3;
	texture.data = data;
	texture.height = height;
	texture.width = width;
	world->textures[texture_handle] = texture;
	return texture;
}

raycast_texture * raycast_index_texture(unsigned int texture_handle)
{
	return NULL;
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

void raycast_render_world(raycast_window* window, raycast_world* world, raycast_player* player)
{
	for (int i = 0; i < window->screen_width; ++i) {
		double camera_x = 2.0 * (double)i / (double)window->screen_width - 1.0; 
		double ray_dir_x = player->view_x + player->plane_x * camera_x;
		double ray_dir_y = player->view_y + player->plane_y * camera_x;
		double ray_pos_x = player->x;
		double ray_pos_y = player->y;
		double side_dist_x = 0;
		double side_dist_y = 0;
		double delta_x = sqrt(1.0 + (ray_dir_y * ray_dir_y) / (ray_dir_x * ray_dir_x));
		double delta_y = sqrt(1.0 + (ray_dir_x * ray_dir_x) / (ray_dir_y * ray_dir_y));
		double perp_wall_dist = 0;

		int map_x = (int)ray_pos_x;
		int map_y = (int)ray_pos_y;
		int step_x = 0;
		int step_y = 0;

		int hit = 0;
		int side = 0; 

		//ray goes left x wise
		if (ray_dir_x < 0)
		{
			step_x = -1;
			side_dist_x = (ray_pos_x - map_x) * delta_x;
		}

		//ray goes right x wise
		else
		{
			step_x = 1;
			side_dist_x = (map_x + 1.0 - ray_pos_x) * delta_x;
		}

		//ray goes up y wise
		if (ray_dir_y < 0)
		{
			step_y = -1;
			side_dist_y = (ray_pos_y - map_y) * delta_y;
		}

		//ray goes down y wise
		else
		{
			step_y = 1;
			side_dist_y = (map_y + 1.0 - ray_pos_y) * delta_y;
		}

		while (hit == 0)
		{
			//step to the next grid
			if (side_dist_x < side_dist_y)
			{
				side_dist_x += delta_x;
				map_x += step_x;
				side = 0;
			}
			else
			{
				side_dist_y += delta_y;
				map_y += step_y;
				side = 1;
			}

			//see if we hit a wall

			raycast_grid* grid = raycast_index_grid(map_x, map_y, world);

			assert(grid);

			if (grid && grid->type == RAYCAST_WALL) {
				hit = 1;
			}

		}

		if (side == 0) {
			perp_wall_dist = fabs((map_x - ray_pos_x + (1.0 - step_x) / 2.0) / ray_dir_x) + 0.000001;
		}

		else {
			perp_wall_dist = fabs((map_y - ray_pos_y + (1.0 - step_y) / 2.0) / ray_dir_y) + 0.000001;
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

		double wall_x = 0;

		if (side == 0) {
			wall_x = ray_pos_y + perp_wall_dist * ray_dir_y;
		}

		else {
			wall_x = ray_pos_x + perp_wall_dist * ray_dir_x;
		}

		wall_x -= floor(wall_x);


		raycast_texture* texture = &world->textures[grid->texture_handle];

		int tex_x = (int)(wall_x * (double)texture->width);

		if (side == 0 && ray_dir_x > 0) {
			tex_x = texture->width - tex_x - 1;
		}

		if (side == 1 && ray_dir_y < 0) {
			tex_x = texture->width - tex_x - 1;
		}

		if (grid && grid->type == RAYCAST_WALL) {

			for (int j = start; j < end; ++j) {

				int tex_y = (int)((double)(j - start) / (double)projected_height * (double)texture->height);
				int tex_pixel = (texture->width * tex_y + tex_x) * 4;
				unsigned char* color = &texture->data[tex_pixel];

				int pixel = (j*window->screen_width + i) * 4;
				window->screen_buffer[pixel] = color[3];
				window->screen_buffer[pixel + 1] = color[2];
				window->screen_buffer[pixel + 2] = color[1];
				window->screen_buffer[pixel + 3] = color[0];
			}

			//cheasy floor and ceiling

			for (int j = 0; j < start; ++j) {
				int pixel = (j*window->screen_width + i) * 4;
				window->screen_buffer[pixel] = 255;
				window->screen_buffer[pixel + 1] = 100;
				window->screen_buffer[pixel + 2] = 100;
				window->screen_buffer[pixel + 3] = 100;
			}

			for (int j = end; j < window->screen_height; ++j) {
				int pixel = (j*window->screen_width + i) * 4;
				window->screen_buffer[pixel] = 255; 
				window->screen_buffer[pixel + 1] = 160; 
				window->screen_buffer[pixel + 2] = 160; 
				window->screen_buffer[pixel + 3] = 160; 
			}


		}

	}
}
