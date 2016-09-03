#include <stdio.h>
#include <SDL.h> 
#include <math.h>

#include "raycast_render.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WORLD_WIDTH 8
#define WORLD_HEIGHT 10
#define PI 3.14159265


//test map
char maze_map[WORLD_WIDTH*WORLD_HEIGHT] = {'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w',
										   'w', 'e', 'w', 'e', 'e', 'e', 'e', 'w',
										   'w', 'e', 'w', 'e', 'e', 'e', 'e', 'w',
				                           'w', 'e', 'w', 'e', 'e', 'e', 'e', 'w',
				                           'w', 'e', 'e', 'e', 'e', 'e', 'e', 'w',
		                                   'w', 'e', 'e', 'e', 'e', 'e', 'e', 'w',
		                                   'w', 'e', 'e', 'e', 'e', 'e', 'e', 'w',
			                               'w', 'e', 'e', 'e', 'e', 'e', 'e', 'w',
			                               'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'};

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200


int main(int argc, char *argv[])
{

	//test code
	time_t t;
	srand((unsigned)time(&t));


	raycast_grid* grids = malloc(sizeof(raycast_grid)*WORLD_WIDTH*WORLD_HEIGHT);


	for (int i = 0; i < WORLD_WIDTH*WORLD_HEIGHT; ++i) {

		if (maze_map[i] == 'e') {
			grids[i].type = RAYCAST_EMPTY;
			grids[i].r = 0;
			grids[i].g = 0;
			grids[i].b = 0;
		}

		else if (maze_map[i] == 'w') {
			grids[i].type = RAYCAST_WALL;
			grids[i].r = rand() % 200;
			grids[i].g = rand() % 200;
			grids[i].b = rand() % 200;
			grids[i].texture_handle = 0;
		}

	}



	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	SDL_Window* sdlWindow;
	SDL_Renderer *sdlRenderer;
	SDL_CreateWindowAndRenderer(1600, 900, SDL_RENDERER_ACCELERATED, &sdlWindow, &sdlRenderer);


	if (!sdlWindow || !sdlRenderer) {
		printf("SDL could not create a window! SDL_Error: %s\n", SDL_GetError());
	}


	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
	SDL_RenderClear(sdlRenderer);
	SDL_RenderPresent(sdlRenderer);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(sdlRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);


	SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 320, 200);

	int exit = 0;

	unsigned char* buffer = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4);

	raycast_window window = raycast_create_window(SCREEN_WIDTH, SCREEN_HEIGHT, buffer);
	raycast_player player = raycast_create_player(5, 5, -1, 0, 0, 0.66);
	raycast_world world = raycast_create_world(8, 9, grids);

	int width, height, components;
	unsigned char* image_data = stbi_load("paint1.png", &width, &height, &components, 0);

	raycast_create_texture(width, height, image_data, 0, &world);

	while (!exit) {

		raycast_grid* currentPos = raycast_index_grid(player.x, player.y, &world);


		

		SDL_Event e;
		while (SDL_PollEvent(&e)) {

			if (e.type == SDL_WINDOWEVENT) {
				if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
					exit = 1;
				}
			}
			
			switch (e.type) {
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
				case SDLK_LEFT:
					{
					double old_view_x = player.view_x;
					double old_plane_x = player.plane_x;
					player.view_x = player.view_x * cos(30 * PI / 180.0) - player.view_y * sin(30 * PI / 180.0);
					player.view_y = player.view_y * cos(30 * PI / 180.0) + old_view_x * sin(30 * PI / 180.0);
					player.plane_x = player.plane_x * cos(30 * PI / 180.0) - player.plane_y * sin(30 * PI / 180.0);
					player.plane_y = player.plane_y * cos(30 * PI / 180.0) + old_plane_x * sin(30 * PI / 180.0);
					break;
					}
				case SDLK_RIGHT:
					{
					double old_view_x = player.view_x;
					double old_plane_x = player.plane_x;
					player.view_x = player.view_x * cos(-30 * PI / 180.0) - player.view_y * sin(-30 * PI / 180.0);
					player.view_y = player.view_y * cos(-30 * PI / 180.0) + old_view_x * sin(-30 * PI / 180.0);
					player.plane_x = player.plane_x * cos(-30 * PI / 180.0) - player.plane_y * sin(-30 * PI / 180.0);
					player.plane_y = player.plane_y * cos(-30 * PI / 180.0) + old_plane_x * sin(-30 * PI / 180.0);
					break;
					}
				case SDLK_UP:
					{
					double new_x = player.x + player.view_x;
					double new_y = player.y + player.view_y;

					raycast_grid* forward = raycast_index_grid(floor(new_x), floor(new_y), &world);

					if (forward && forward->type != RAYCAST_WALL) {
						player.x += player.view_x;
						player.y += player.view_y;
					}
					break;
					}
				case SDLK_DOWN:
					break;
				case SDLK_ESCAPE:
					exit = 1;
					break;
				default:
					break;
				}
			}
		}

		memset(buffer, 255, SCREEN_WIDTH * SCREEN_HEIGHT * 4);

		raycast_render_world(&window, &world, &player);

		SDL_UpdateTexture(sdlTexture, NULL, buffer, SCREEN_WIDTH * 4);

		SDL_RenderClear(sdlRenderer);
		SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
		SDL_RenderPresent(sdlRenderer);

		SDL_Delay(100);

	}
	return 0;
	
}