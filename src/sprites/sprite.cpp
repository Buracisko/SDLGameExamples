#include "sprite.h"
#include "engine/engine.h"
#include "SDL.h"
#include "SDL_image.h"
#include <stdio.h>

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