#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720


int main(void) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    return 1;
  }
  IMG_Init(IMG_INIT_PNG);

  SDL_Window* window = SDL_CreateWindow(
      "Casteroids",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      WINDOW_WIDTH, WINDOW_HEIGHT,
      SDL_WINDOW_SHOWN
      );
  if (window == NULL) {
    printf("Could not create window! SDL Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(
      window,
      -1,
      SDL_RENDERER_ACCELERATED
  );

  // Create a background Image
  SDL_Surface* background_surface = IMG_Load_RW(
      SDL_RWFromFile("./graphics/background.png", "rb"),
      1
    );
  if (background_surface == NULL) {
    printf("Could not load background image: %s\n", IMG_GetError());
  }
  SDL_Texture* background_texture = SDL_CreateTextureFromSurface(renderer, background_surface);
  SDL_FreeSurface(background_surface);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);




  bool running = true;
  while(running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }
    SDL_SetRenderDrawColor(renderer, 12, 2, 26, 255);
    SDL_RenderClear(renderer);
    SDL_Rect background_rect = {0,0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_SetTextureAlphaMod(background_texture, 180);
    SDL_RenderCopy(renderer, background_texture, NULL, &background_rect);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
