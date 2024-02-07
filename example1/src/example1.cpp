#include "engine/engine.h"
#include "sprites/sprite.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void Update(float dt);
void RenderFrame(float dt);
bool OnGameStart();
void OnGameEnd();

SDL_Texture* RenderText(const char* str, SDL_Colour colour, TTF_Font* font, SDL_Rect* textRect);

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 360

// Sprites
SDL_Texture* ship1;
SDL_Texture* ship2;
SDL_Texture* ship3;
int shipWidth, shipHeight;

// Text rendering
TTF_Font* mainFont;
SDL_Texture* mess1Texture;
SDL_Texture* mess2Texture;
SDL_Texture* mess3Texture;

SDL_Rect mess1Rect;
SDL_Rect mess2Rect;
SDL_Rect mess3Rect;

int main(int argc, char* argv[])
{
	if (!InitSDL())
	{
		return 1;
	}

	if (!CreateWindow("Example 1 - Basic rendering", WINDOW_WIDTH, WINDOW_HEIGHT))
	{
		return 1;
	}

	if (!OnGameStart())
	{
		return 1;
	}

	// Push functions to the game loop
	StartLoop(Update, RenderFrame);

	OnGameEnd();
	DeinitSDL();
	return 0;
}

bool OnGameStart()
{
	// Load ship sprites
	ship1 = LoadSprite("assets/kenney_piratepack/PNG/Default size/Ships/ship (3).png");
	ship2 = LoadSprite("assets/kenney_piratepack/PNG/Default size/Ships/ship (5).png");
	ship3 = LoadSprite("assets/kenney_piratepack/PNG/Default size/Ships/ship (7).png");
	SDL_QueryTexture(ship1, NULL, NULL, &shipWidth, &shipHeight);

	// Load font
	mainFont = TTF_OpenFont("assets/open_sans/static/OpenSans/OpenSans-Regular.ttf", 16);
	if (!mainFont)
	{
		fprintf(stderr, "TTF_OpenFont error: %s\n", TTF_GetError());
		return false;
	}

	// Render text
	SDL_Colour textCol = { 255, 255, 255, 255 };
	mess1Texture = RenderText("Regular", textCol, mainFont, &mess1Rect);
	mess2Texture = RenderText("Rotated & flipped", textCol, mainFont, &mess2Rect);
	mess3Texture = RenderText("Scaled", textCol, mainFont, &mess3Rect);

	return true;
}

void OnGameEnd()
{
	// Delete sprite textures
	if (ship1) SDL_DestroyTexture(ship1);
	if (ship2) SDL_DestroyTexture(ship2);
	if (ship3) SDL_DestroyTexture(ship3);

	if (mess1Texture) SDL_DestroyTexture(mess1Texture);
	if (mess2Texture) SDL_DestroyTexture(mess2Texture);
	if (mess3Texture) SDL_DestroyTexture(mess3Texture);

	// Free font
	TTF_CloseFont(mainFont);
}

void Update(float dt)
{
	// Change subsystem of project from Windows to Console
	// in order to see the stderr output
	if (IsKeyDown(SDL_SCANCODE_ESCAPE))
		ExitGame();
}

void RenderFrame(float interpolation)
{
	// Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0x3a, 0x2b, 0x3b, 255);
	SDL_RenderClear(gRenderer);
	
	// SDL_Rect consists of: X position, Y position, widht and height
	SDL_Rect shipRect = { 0, 0, shipWidth, shipHeight };
	SDL_Point shipCenter = { shipWidth / 2, shipHeight / 2 };

	// All ships will be rendered vertically centered into columns
	const int numberOfShips = 3;
	const int verticalCenter = WINDOW_HEIGHT / 2 - shipHeight / 2; // Vertical center of ship
	const int colWidht = WINDOW_WIDTH / numberOfShips;

	// Render red ship in the col number 0
	int col = 0;
	SDL_Rect redShipRect = shipRect;
	redShipRect.y = verticalCenter;
	redShipRect.x = col * colWidht + colWidht / 2 - shipWidth / 2;
	SDL_RenderCopy(gRenderer, ship1, NULL, &redShipRect);

	mess1Rect.x = col * colWidht + colWidht / 2 - mess1Rect.w / 2;
	mess1Rect.y = verticalCenter + 128;
	SDL_RenderCopy(gRenderer, mess1Texture, NULL, &mess1Rect);

	// Render blue ship - rotated 45 degrees around the center and flipped vertically
	col = 1;
	SDL_Rect blueShipRect = shipRect;
	blueShipRect.y = verticalCenter;
	blueShipRect.x = col * colWidht + colWidht / 2 - shipWidth / 2;
	SDL_RenderCopyEx(gRenderer, ship2, NULL, &blueShipRect, 45, &shipCenter, SDL_FLIP_VERTICAL);

	mess2Rect.x = col * colWidht + colWidht / 2 - mess2Rect.w / 2;
	mess2Rect.y = verticalCenter + 128;
	SDL_RenderCopy(gRenderer, mess2Texture, NULL, &mess2Rect);

	// Render white ship - half the size
	col = 2;
	SDL_Rect whiteShipRect = { shipRect.x, shipRect.y, shipRect.w / 2, shipRect.h / 2};
	whiteShipRect.y = verticalCenter + whiteShipRect.h / 2;
	whiteShipRect.x = col * colWidht + colWidht / 2 - whiteShipRect.w / 2;
	SDL_RenderCopy(gRenderer, ship3, NULL, &whiteShipRect);

	mess3Rect.x = col * colWidht + colWidht / 2 - mess3Rect.w / 2;
	mess3Rect.y = verticalCenter + 128;
	SDL_RenderCopy(gRenderer, mess3Texture, NULL, &mess3Rect);
}

SDL_Texture* RenderText(const char* str, SDL_Colour colour, TTF_Font* font, SDL_Rect* textRect)
{
	SDL_Surface* textSurface = TTF_RenderText_Blended(font, str, colour);
	if (!textSurface)
	{
		fprintf(stderr, "TTF_RenderText_Solid error: %s\n", TTF_GetError());
		return NULL;
	}

	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
	if (!textTexture)
	{
		fprintf(stderr, "SDL_CreateTextureFromSurface error: %s\n", SDL_GetError());
		return NULL;
	}

	if (textRect)
	{
		textRect->x = textRect->y = 0;
		textRect->w = textSurface->w;
		textRect->h = textSurface->h;
	}

	SDL_FreeSurface(textSurface);
	return textTexture;
}
