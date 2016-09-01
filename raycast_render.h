#ifndef RAY_RENDER_H
#define RAY_RENDER_H

#define RAYCAST_WALL 0
#define RAYCAST_EMPTY 0

typedef struct {
	unsigned char* data;
	unsigned int width;
	unsigned int height;
	unsigned int components;
} raycast_texture;

typedef struct {
	unsigned int screen_fov;
	unsigned int screen_width;
	unsigned int screen_height;
	char* screen_buffer;
} raycast_window;

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char type;
} raycast_grid;


typedef struct {
	raycast_grid* grids;
	raycast_texture** textures;
	unsigned int texture_handle_count;
	unsigned int grid_width;
	unsigned int grid_height;
	unsigned int world_width;
	unsigned int world_height;
} raycast_world;

typedef struct {
	int x;
	int y;
	int height;
	double viewing_direction;
} raycast_player;

raycast_player raycast_create_player(int x, int y, int height, double viewing_direction);

raycast_world raycast_create_world(unsigned int grid_width, unsigned int grid_height, unsigned int world_width,
	unsigned int world_height, char* worldMap, unsigned int texture_handle_count);

void raycast_delete_world(raycast_world* world);

raycast_window raycast_create_window(unsigned int screen_fov, unsigned int screen_width,
	unsigned int screen_height, void* screen_buffer);

raycast_texture raycast_create_texture(unsigned int width, unsigned int height, unsigned char* data, unsigned texture_handle);

void raycast_render_world(raycast_window* window, raycast_world* world, raycast_player* player);


#endif