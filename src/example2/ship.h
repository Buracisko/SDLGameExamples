#pragma once

struct SDL_Texture;

typedef struct Ship
{
	double positionX, positionY;
	int width, height;
	double speed;
	double friction;
	double acceleration;
	double rotation;
} Ship;

void ShipAccelerate(Ship* ship);

void ShipUpdate(Ship* ship);

void ShipRender(Ship* ship, SDL_Texture* sprite);