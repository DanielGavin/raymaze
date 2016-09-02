#include <stdio.h>
#include <SDL.h> 
#include <math.h>

#include "raycast_render.h"

//map of the maze



char maze_map[] = {'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w',
				   'w', 'e', 'e', 'e', 'e', 'e', 'e', 'w',
				   'w', 'e', 'e', 'e', 'e', 'e', 'e', 'w',
				   'w', 'e', 'e', 'e', 'e', 'e', 'e', 'w',
				   'w', 'e', 'e', 'e', 'e', 'e', 'e', 'w',
		           'w', 'e', 'e', 'e', 'e', 'e', 'e', 'w',
		           'w', 'e', 'e', 'e', 'e', 'e', 'e', 'w',
			       'w', 'e', 'e', 'e', 'e', 'e', 'e', 'w',
			       'w', 'w', 'w', 'w', 'w', 'w', 'w', 'w'};

int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	SDL_Window* sdlWindow;
	SDL_Renderer *sdlRenderer;
	SDL_CreateWindowAndRenderer(900, 600, SDL_RENDERER_ACCELERATED, &sdlWindow, &sdlRenderer);


	if (!sdlWindow || !sdlRenderer) {
		printf("SDL could not create a window! SDL_Error: %s\n", SDL_GetError());
	}


	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
	SDL_RenderClear(sdlRenderer);
	SDL_RenderPresent(sdlRenderer);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(sdlRenderer, 320, 200);


	SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 320, 200);

	int exit = 0;

	unsigned char* buffer = malloc(320 * 200 * 4);

	raycast_window window = raycast_create_window(60, 320, 200, buffer);
	raycast_player player = raycast_create_player(4 * 64+2, 3 * 64+2, 20);
	raycast_world world = raycast_create_world(64, 8, 9, maze_map, 1);

	while (!exit) {

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
					player.viewing_direction -= 10;
					break;
				case SDLK_RIGHT:
					player.viewing_direction += 10;
					break;
				case SDLK_UP:
					player.x += 20 * cos(player.viewing_direction * 3.14159265 / 180.0);
					player.y += 20 * (-sin(player.viewing_direction * 3.14159265 / 180.0));
					break;
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

		memset(buffer, 255, 320 * 200 * 4);

		raycast_render_world(&window, &world, &player);

		SDL_UpdateTexture(sdlTexture, NULL, buffer, 320 * 4);

		SDL_RenderClear(sdlRenderer);
		SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
		SDL_RenderPresent(sdlRenderer);

		SDL_Delay(100);

	}
	return 0;
	
}