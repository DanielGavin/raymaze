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
	int screen_width;
	int screen_height;
	unsigned char* screen_buffer;
} raycast_window;

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char type;
	unsigned int texture_handle;
} raycast_grid;


typedef struct {
	raycast_grid* grids;
	raycast_texture textures[255];
	int world_width;
	int world_height;
} raycast_world;

typedef struct {
	double x;
	double y;
	double view_x;
	double view_y;
	double plane_x;
	double plane_y;
} raycast_player;

typedef struct {
	raycast_grid* grid;
	int ray_length;
} raycast_cast_result;

raycast_player raycast_create_player(double x, double y, double view_x, double view_y, double plane_x, double plane_y);

raycast_world raycast_create_world(int world_width, int world_height, raycast_grid* grids);

void raycast_delete_world(raycast_world* world);

raycast_window raycast_create_window(int screen_width,
									 int screen_height, void* screen_buffer);

raycast_texture raycast_create_texture(int width, int height, unsigned char* data, unsigned int texture_handle, raycast_world* world);

void raycast_render_world(raycast_window* window, raycast_world* world, raycast_player* player);

raycast_grid* raycast_index_grid(int x, int y, raycast_world* world);

raycast_texture* raycast_index_texture(unsigned int texture_handle);


#endif