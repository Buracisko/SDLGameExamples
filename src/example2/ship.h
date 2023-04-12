#pragma once

typedef struct Ship
{
	double positionX, positionY;
	double speed;
	double friction;
	double acceleration;
	double rotation;
} Ship;

void ShipAccelerate(Ship* ship);

void ShipUpdate(Ship* ship);