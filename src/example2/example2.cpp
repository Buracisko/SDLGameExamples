#include "engine.h"
#include "ship.h"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Forward function declarations
void Update(float dt);
void RenderFrame(float dt);
void OnGameLaunch();
SDL_Texture* LoadSprite(const char* path);

#define WINDOW_WIDTH 896
#define WINDOW_HEIGHT 504

// Testing sprite
SDL_Texture* sprite;
int spriteWidth, spriteHeight;

Ship playerShip;

//=============================================================================
int main(int argc, char* argv[])
{
	if (!InitSDL())
	{
		return 1;
	}

	if (!CreateWindow("Example 2 - Ship sailing", WINDOW_WIDTH, WINDOW_HEIGHT))
	{
		return 1;
	}
	
	// Load sprite
	sprite = LoadSprite("assets/kenney_piratepack/PNG/Default size/Ships/ship (5).png");
	SDL_QueryTexture(sprite, NULL, NULL, &spriteWidth, &spriteHeight);

	OnGameLaunch();

	// Push functions to the game loop
	StartLoop(Update, RenderFrame);

	// Delete texture
	if (sprite)
		SDL_DestroyTexture(sprite);

	DeinitSDL();
	return 0;
}

//=============================================================================

void OnGameLaunch()
{
	playerShip.positionX = WINDOW_WIDTH / 2;
	playerShip.positionY = WINDOW_HEIGHT - 100;
	playerShip.speed = 0;
	playerShip.rotation = 0;
	playerShip.acceleration = 0.2;
	playerShip.friction = 0.1;
}

void Update(float dt)
{
	// Change subsystem of project from Windows to Console
	// in order to see the stderr output
	if (IsKeyDown(SDL_SCANCODE_ESCAPE))
		ExitGame();

	if (IsKeyDown(SDL_SCANCODE_UP))
	{
		ShipAccelerate(&playerShip);
	}

	if (IsKeyDown(SDL_SCANCODE_RIGHT))
	{
		playerShip.rotation += 1;
	}
	else if (IsKeyDown(SDL_SCANCODE_LEFT))
	{
		playerShip.rotation -= 1;
	}

	ShipUpdate(&playerShip);
}

void RenderFrame(float interpolation)
{
	// Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 255);
	SDL_RenderClear(gRenderer);

	// Draw ship
	const SDL_Rect shipRect = {
		(int)((playerShip.positionX - spriteWidth / 2) + 0.5),
		(int)((playerShip.positionY - spriteHeight / 2) + 0.5),
		spriteWidth,
		spriteHeight
	};
	SDL_RenderCopyEx(gRenderer, sprite, NULL, &shipRect, playerShip.rotation, NULL, SDL_RendererFlip::SDL_FLIP_VERTICAL);
}

SDL_Texture* LoadSprite(const char* path)
{

	SDL_Texture* texture = IMG_LoadTexture(gRenderer, path);
	if (texture == NULL)
	{
		fprintf(stderr, "IMG_LoadTexture error: %s\n", IMG_GetError());
		return NULL;
	}

	return texture;
}
