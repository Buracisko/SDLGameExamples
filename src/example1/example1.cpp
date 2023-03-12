#include "engine.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Forward function declarations
void Update(float dt);
void RenderFrame(float dt);
SDL_Texture* LoadSprite(const char* path);
SDL_Texture* RenderText(const char* str, SDL_Colour colour, TTF_Font* font, SDL_Rect* textRect);

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 360

// Sprites
SDL_Texture* ship1;
SDL_Texture* ship2;
SDL_Texture* ship3;
SDL_Texture* ship4;
SDL_Texture* ship5;
int shipWidth, shipHeight;

// Text rendering
TTF_Font* mainFont;
SDL_Texture* mess1Texture;
SDL_Texture* mess2Texture;
SDL_Texture* mess3Texture;
SDL_Texture* mess4Texture;
SDL_Texture* mess5Texture;

SDL_Rect mess1Rect;
SDL_Rect mess2Rect;
SDL_Rect mess3Rect;
SDL_Rect mess4Rect;
SDL_Rect mess5Rect;

// Ship rotation
double shipRotation;

//=============================================================================
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
	
	// Load ship sprites
	ship1 = LoadSprite("assets/kenney_piratepack/PNG/Default size/Ships/ship (3).png");
	ship2 = LoadSprite("assets/kenney_piratepack/PNG/Default size/Ships/ship (4).png");
	ship3 = LoadSprite("assets/kenney_piratepack/PNG/Default size/Ships/ship (5).png");
	ship4 = LoadSprite("assets/kenney_piratepack/PNG/Default size/Ships/ship (6).png");
	ship5 = LoadSprite("assets/kenney_piratepack/PNG/Default size/Ships/ship (7).png");
	SDL_QueryTexture(ship1, NULL, NULL, &shipWidth, &shipHeight);

	// Load font
	mainFont = TTF_OpenFont("assets/open_sans/static/OpenSans/OpenSans-Regular.ttf", 16);
	if (!mainFont)
	{
		fprintf(stderr, "TTF_OpenFont error: %s\n", TTF_GetError());
		return NULL;
	}

	// Render text
	SDL_Colour textCol = { 255, 255, 255, 255 };
	mess1Texture = RenderText("Regular", textCol, mainFont, &mess1Rect);
	mess2Texture = RenderText("Flipped", textCol, mainFont, &mess2Rect);
	mess3Texture = RenderText("Rotated & flipped", textCol, mainFont, &mess3Rect);
	mess4Texture = RenderText("Rotating", textCol, mainFont, &mess4Rect);
	mess5Texture = RenderText("Scaled", textCol, mainFont, &mess5Rect);

	// Push functions to the game loop
	StartLoop(Update, RenderFrame);

	// Delete sprite textures
	if (ship1) SDL_DestroyTexture(ship1);
	if (ship2) SDL_DestroyTexture(ship2);
	if (ship3) SDL_DestroyTexture(ship3);
	if (ship4) SDL_DestroyTexture(ship4);
	if (ship5) SDL_DestroyTexture(ship5);

	if (mess1Texture) SDL_DestroyTexture(mess1Texture);
	if (mess2Texture) SDL_DestroyTexture(mess2Texture);
	if (mess3Texture) SDL_DestroyTexture(mess3Texture);
	if (mess4Texture) SDL_DestroyTexture(mess4Texture);
	if (mess5Texture) SDL_DestroyTexture(mess5Texture);

	// Free font
	TTF_CloseFont(mainFont);

	DeinitSDL();
	return 0;
}

//=============================================================================

void Update(float dt)
{
	// Change subsystem of project from Windows to Console
	// in order to see the stderr output
	if (IsKeyDown(SDL_SCANCODE_ESCAPE))
		ExitGame();

	const double revolutionsPerSecond = .5;
	shipRotation += dt * revolutionsPerSecond * 360;
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
	const int numberOfShips = 5;
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

	// Render green ship - vertically flipped
	col = 1;
	SDL_Rect greenShipRect = shipRect;
	greenShipRect.y = verticalCenter;
	greenShipRect.x = col * colWidht + colWidht / 2 - shipWidth / 2;
	SDL_RenderCopyEx(gRenderer, ship2, NULL, &greenShipRect, 0, NULL, SDL_FLIP_VERTICAL);

	mess2Rect.x = col * colWidht + colWidht / 2 - mess2Rect.w / 2;
	mess2Rect.y = verticalCenter + 128;
	SDL_RenderCopy(gRenderer, mess2Texture, NULL, &mess2Rect);

	// Render blue ship - rotated 45 degrees around the center and flipped vertically
	col = 2;
	SDL_Rect blueShipRect = shipRect;
	blueShipRect.y = verticalCenter;
	blueShipRect.x = col * colWidht + colWidht / 2 - shipWidth / 2;
	SDL_RenderCopyEx(gRenderer, ship3, NULL, &blueShipRect, 45, &shipCenter, SDL_FLIP_VERTICAL);

	mess3Rect.x = col * colWidht + colWidht / 2 - mess3Rect.w / 2;
	mess3Rect.y = verticalCenter + 128;
	SDL_RenderCopy(gRenderer, mess3Texture, NULL, &mess3Rect);

	// Render yellow ship - rotating constantly
	col = 3;
	SDL_Rect yellowShipRect = shipRect;
	yellowShipRect.y = verticalCenter;
	yellowShipRect.x = col * colWidht + colWidht / 2 - shipWidth / 2;
	SDL_RenderCopyEx(gRenderer, ship4, NULL, &yellowShipRect, shipRotation, &shipCenter, SDL_FLIP_NONE);

	mess4Rect.x = col * colWidht + colWidht / 2 - mess4Rect.w / 2;
	mess4Rect.y = verticalCenter + 128;
	SDL_RenderCopy(gRenderer, mess4Texture, NULL, &mess4Rect);

	// Render white ship - half the size
	col = 4;
	SDL_Rect whiteShipRect = { shipRect.x, shipRect.y, shipRect.w / 2, shipRect.h / 2};
	whiteShipRect.y = verticalCenter + whiteShipRect.h / 2;
	whiteShipRect.x = col * colWidht + colWidht / 2 - whiteShipRect.w / 2;
	SDL_RenderCopy(gRenderer, ship5, NULL, &whiteShipRect);

	mess5Rect.x = col * colWidht + colWidht / 2 - mess5Rect.w / 2;
	mess5Rect.y = verticalCenter + 128;
	SDL_RenderCopy(gRenderer, mess5Texture, NULL, &mess5Rect);
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
