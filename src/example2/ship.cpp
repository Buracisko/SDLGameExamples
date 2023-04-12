#include "ship.h"

#define _USE_MATH_DEFINES
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
