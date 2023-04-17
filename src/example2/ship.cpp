#include "ship.h"
#include "engine.h"
#include "SDL.h"
#include <math.h>

void ShipAccelerate(Ship* ship)
{
	ship->speed += ship->acceleration;
}

void ShipUpdate(Ship* ship)
{
	double dirX = sin((ship->rotation * M_PI) / 180);
	double dirY = -cos((ship->rotation * M_PI) / 180);

	ship->positionY += dirY * ship->speed;
	ship->positionX += dirX * ship->speed;

	ship->speed -= ship->friction;

	if (ship->speed < 0.1)
		ship->speed = 0;
}

void ShipRender(Ship* ship, SDL_Texture* sprite)
{
	const SDL_Rect target = {
		(int)((ship->positionX - ship->width / 2) + 0.5),
		(int)((ship->positionY - ship->height / 2) + 0.5),
		ship->width,
		ship->height
	};
	SDL_RenderCopyEx(gRenderer, sprite, NULL, &target, ship->rotation, NULL, SDL_RendererFlip::SDL_FLIP_VERTICAL);
}
