#include "ship.h"
#include "engine.h"
#include "SDL.h"
#include <math.h>

void ShipAccelerate(Ship* ship)
{
	ship->thrust = 100;
	ship->mass = 50.0;
	ship->dragCoef = .8;
}

void ShipUpdate(Ship* ship, float dt)
{
	double totalFoce;
	double acceleration;
	double newVelocity;
	double newPosition;

	totalFoce = ship->thrust - ship->dragCoef * ship->velocity * ship->velocity;
	acceleration = totalFoce / ship->mass;

	newVelocity = ship->velocity + acceleration * dt;
	newPosition = ship->positionX + newVelocity * dt;

	ship->velocity = newVelocity;
	ship->positionX = newPosition;
}

void DebugRender(Ship* ship)
{
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 255);
	double dirX = sin((ship->rotation * M_PI) / 180);
	double dirY = -cos((ship->rotation * M_PI) / 180);
	SDL_RenderDrawLineF(gRenderer,
		(float)ship->positionX, (float)ship->positionY,
		(float)(ship->positionX + dirX * ship->height), (float)(ship->positionY + dirY * ship->height)
	);
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

	#if 1
	DebugRender(ship);
	#endif
}
