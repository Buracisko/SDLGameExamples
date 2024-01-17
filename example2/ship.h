#pragma once

struct SDL_Texture;

typedef struct Ship
{
	double positionX, positionY;
	int width, height;
	double rotation;
	double thrust;
	double dragCoef;
	double velocity;
	double mass;
} Ship;

void ShipAccelerate(Ship* ship);

void ShipUpdate(Ship* ship, float dt);

void ShipRender(Ship* ship, SDL_Texture* sprite);