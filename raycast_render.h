#ifndef RAY_RENDER_H
#define RAY_RENDER_H

#define RAYCAST_WALL 0
#define RAYCAST_EMPTY 1

typedef struct {
	unsigned char* data;
	int width;
	int height;
	int components;
} raycast_texture;

typedef struct {
	int screen_fov;
	int screen_width;
	int screen_height;
	int center_x;
	int center_y;
	int projection_distance;
	double ray_angle;
	unsigned char* screen_buffer;
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
	int texture_handle_count;
	int grid_size;
	int world_width;
	int world_height;
} raycast_world;

typedef struct {
	int x;
	int y;
	double viewing_direction;
} raycast_player;

typedef struct {
	raycast_grid* grid;
	int ray_length;
} raycast_cast_result;

raycast_player raycast_create_player(int x, int y, double viewing_direction);

raycast_world raycast_create_world(int grid_size, int world_width,
	int world_height, char* worldMap, int texture_handle_count);

void raycast_delete_world(raycast_world* world);

raycast_window raycast_create_window(int screen_fov, int screen_width,
									 int screen_height, void* screen_buffer);

raycast_texture raycast_create_texture(int width, int height, unsigned char* data, int texture_handle);

void raycast_render_world(raycast_window* window, raycast_world* world, raycast_player* player);


#endif