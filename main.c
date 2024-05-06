#include <stdio.h>
#include <stdbool.h>

#ifdef _WIN32 // Defined by MSVC and MinGW, for both 32-bit and 64-bit environments
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#else // This can be for Linux and potentially for macOS as well
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define FRAME_RATE 120
#define FRAME_DELAY (1000 / FRAME_RATE)
#define SDL_USEREVENT_CREATE_ASTEROID (SDL_USEREVENT + 1)

#define MAX_LASERS_NUMBER 5
#define LASER_SPEED 1

#define MAX_ASTEROIDS_NUMBER 20
#define ASTEROID_COOLDOWN_TIME 500
#define ASTEROID_SPEED 1

int displayScore(SDL_Renderer *renderer, TTF_Font *font, int kills) {
  if (font == NULL) {
    printf("Font not loaded! %s\n", TTF_GetError());
    return 1; // Exit if font is not loaded
  }

  SDL_Color color = {255, 255, 255};
  char scoreText[100];
  sprintf(scoreText, "Score: %d", kills);

  SDL_Surface *surface = TTF_RenderText_Blended(font, scoreText, color);
  if (surface == NULL) {
    printf("Failed to create surface: %s\n", TTF_GetError());
    return 1;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == NULL) {
    printf("Failed to create texture: %s\n", SDL_GetError());
    SDL_FreeSurface(surface); // Free surface before returning
    return 1;
  }

  int text_width, text_height;
  TTF_SizeText(font, scoreText, &text_width, &text_height);
  SDL_Rect rect = {WINDOW_WIDTH / 2 - text_width / 2, WINDOW_HEIGHT - 80, text_width, text_height};

  // Draw a border around the text
  int borderThickness = 20;
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Set the color to white
  for (int i = 10; i < borderThickness; i++) {
    SDL_Rect borderRect = {rect.x - i, rect.y - i, rect.w + 2 * i, rect.h + 2 * i};
    SDL_RenderDrawRect(renderer, &borderRect); // Draw the border
  }

  SDL_RenderCopy(renderer, texture, NULL, &rect);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);

  return 0;
}

// Update Laser
void updateLaser(SDL_Rect laserRects[]) {
  for (int i = 0; i < MAX_LASERS_NUMBER; i++) {
    if (laserRects[i].y < -80) {
      // "Destroy" Laser
      laserRects[i].y = -80;
    }
    laserRects[i].y -= LASER_SPEED;
  }
}

// Create Asteroid Event
Uint32 createAsteroidEvent(Uint32 interval, void *param) {
  SDL_Event event;
  event.type = SDL_USEREVENT_CREATE_ASTEROID;
  event.user.code = SDL_USEREVENT_CREATE_ASTEROID;
  event.user.data1 = 0;
  event.user.data2 = 0;
  SDL_PushEvent(&event);
  return interval;
}

// Update Asteroid
void updateAsteroid(SDL_Rect asteroidRects[]) {
  for (int i = 0; i < MAX_ASTEROIDS_NUMBER; i++) {
    if (asteroidRects[i].y > WINDOW_HEIGHT + 84) {
      asteroidRects[i].y = -84;
    }
    asteroidRects[i].y += ASTEROID_SPEED;
  }
}

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    return 1;
  }
  IMG_Init(IMG_INIT_PNG);

  if (TTF_Init() < 0) {
    printf("TTF could not initialize! TTF Error: %s\n", TTF_GetError());
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow(
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
  if (renderer == NULL) {
    printf("Could not create renderer! SDL Error: %s\n", SDL_GetError());
    return 1;
  }

  // Create the Score Font
  TTF_Font *font = TTF_OpenFont("./resources/fonts/subatomic.ttf", 50);
  if (font == NULL) {
    printf("Could not load font: %s\n", TTF_GetError());
  }

  // Hide Mouse Visibility
  SDL_ShowCursor(SDL_DISABLE);

  // Create a background Image
  SDL_Surface *backgroundSurface = IMG_Load_RW(
      SDL_RWFromFile("./resources/graphics/background.png", "rb"),
      1
  );
  if (backgroundSurface == NULL) {
    printf("Could not load background image: %s\n", IMG_GetError());
  }
  SDL_Texture *backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
  SDL_FreeSurface(backgroundSurface);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  // __________________________________________________________________________

  // Create the Ship Image
  SDL_Surface *shipSurface = IMG_Load_RW(
      SDL_RWFromFile("./resources/graphics/ship.png", "rb"),
      1
  );
  if (shipSurface == NULL) {
    printf("Could not load ship image: %s\n", IMG_GetError());
  }
  SDL_Texture *shipTexture = SDL_CreateTextureFromSurface(renderer, shipSurface);
  SDL_FreeSurface(shipSurface);
  SDL_Rect shipRect = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 64, 64};
  // __________________________________________________________________________

  // Create the Laser Image
  SDL_Surface *laserSurface = IMG_Load_RW(
      SDL_RWFromFile("./resources/graphics/laser.png", "rb"),
      1
  );
  if (laserSurface == NULL) {
    printf("Could not load laser image: %s\n", IMG_GetError());

  }
  SDL_Texture *laserTexture = SDL_CreateTextureFromSurface(renderer, laserSurface);
  SDL_FreeSurface(laserSurface);
  int currentLaser = 0;
  SDL_Rect laserRects[MAX_LASERS_NUMBER];
  // __________________________________________________________________________

  // Create the Asteroid Image
  SDL_Surface *asteroidSurface = IMG_Load_RW(SDL_RWFromFile("./resources/graphics/meteor.png", "rb"), 1);
  if (asteroidSurface == NULL) {
    printf("Could not load asteroid image: %s\n", IMG_GetError());
  }
  SDL_Texture *asteroidTexture = SDL_CreateTextureFromSurface(renderer, asteroidSurface);
  SDL_FreeSurface(asteroidSurface);
  int currentAsteroid = 0;
  SDL_Rect asteroidRects[MAX_ASTEROIDS_NUMBER];

  // TODO: Create a timer to generate new asteroids
  // __________________________________________________________________________

  // Game Loop
  Uint32 frameStart;
  Uint32 frameTime;
  SDL_TimerID asteroidTimer = SDL_AddTimer(ASTEROID_COOLDOWN_TIME, createAsteroidEvent, NULL);

  bool running = true;
  while (running) {
    frameStart = SDL_GetTicks();
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
      if (event.type == SDL_USEREVENT_CREATE_ASTEROID) {
        if (asteroidRects[currentAsteroid].y < 1) {
          int randomX = rand() % WINDOW_WIDTH;
          SDL_Rect asteroidRect = {randomX, -(2 * 84), 101, 84};
          SDL_SetTextureAlphaMod(asteroidTexture, 255);
          asteroidRects[currentAsteroid] = asteroidRect;
        }
        currentAsteroid++;
        if (currentAsteroid > 9) {
          currentAsteroid = 0;
        }
      }
      if (event.type == SDL_MOUSEBUTTONDOWN) {
        SDL_Rect laserRect = {0, 0, 8, 64};
        int shipCenterX = shipRect.x + shipRect.w / 2;
        int lx, ly;
        lx = shipCenterX - laserRect.w / 2;
        ly = shipRect.y + (shipRect.h * -1);
        laserRect.x = lx;
        laserRect.y = ly;
        SDL_SetTextureAlphaMod(laserTexture, 255);
        laserRects[currentLaser] = laserRect;
        currentLaser++;
        if (currentLaser > 4) {
          currentLaser = 0;
        }
      }
    }
    frameTime = SDL_GetTicks() - frameStart;
    if (frameTime < FRAME_DELAY) {
      SDL_Delay(FRAME_DELAY - frameTime);
    }
    SDL_SetRenderDrawColor(renderer, 12, 2, 26, 255);
    SDL_RenderClear(renderer);

    // Draw the Background
    SDL_Rect backgroundRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_SetTextureAlphaMod(backgroundTexture, 200);
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);

    // Draw the Ship
    int x, y;
    SDL_GetMouseState(&x, &y); // Get the mouse position
    shipRect.x = x - shipRect.w / 2;
    shipRect.y = y - shipRect.h / 2;
    SDL_SetTextureAlphaMod(shipTexture, 255);
    SDL_RenderCopy(renderer, shipTexture, NULL, &shipRect);

    // Draw the Laser
    updateLaser(laserRects);
    for (int i = 0; i < MAX_LASERS_NUMBER; i++) {
      SDL_RenderCopy(renderer, laserTexture, NULL, &laserRects[i]);
    }

    // Draw the asteroid
    updateAsteroid(asteroidRects);
    for (int i = 0; i < MAX_ASTEROIDS_NUMBER; i++) {
      SDL_RenderCopy(renderer, asteroidTexture, NULL, &asteroidRects[i]);
    }

    // Display the Score
    displayScore(renderer, font, 100);
    SDL_RenderPresent(renderer);
  }
  // __________________________________________________________________________

  // Cleanup
  SDL_RemoveTimer(asteroidTimer);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}