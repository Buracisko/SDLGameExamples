#include "engine.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// Forward function declarations
void Update(float dt);
void RenderFrame(float dt);

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 360

#define NO_RECTS 15
SDL_Rect rects[NO_RECTS];

void SetupRects()
{
	rects[0] = SDL_Rect {128, 128, 2, 2};
	rects[1] = SDL_Rect {256, 128, 64, 64};

	rects[2] = SDL_Rect {128, 256, 32, 32};

	rects[3] = SDL_Rect {128 + 32, 256, 32, 32};
	rects[4] = SDL_Rect {128 + 64, 256, 32, 32};
	rects[5] = SDL_Rect {128 + 96, 256, 32, 32};

	rects[6] = SDL_Rect {512, 80, 16, 64};
	rects[7] = SDL_Rect {512, 80 + 64, 16, 64};
	rects[8] = SDL_Rect {512, 80 + 128, 16, 64};
	rects[9] = SDL_Rect {512, 80 + 192, 16, 64};

	rects[10] = SDL_Rect {16, 16, 128, 32};
	rects[11] = SDL_Rect {16 + 128, 16, 128, 32};
	rects[12] = SDL_Rect {16 + 256, 16, 128, 32};
	rects[13] = SDL_Rect {16 + 384, 16, 128, 32};

	rects[14] = SDL_Rect {256, 256 + 64, 128, 128};
}

SDL_Rect playerRect = {192, 64, 64, 64};

//=============================================================================
int main(int argc, char* argv[])
{
	if (!InitSDL())
	{
		return 1;
	}

	if (!CreateWindow("Example 3 - AABB collisions", WINDOW_WIDTH, WINDOW_HEIGHT))
	{
		return 1;
	}

	SetupRects();

	// Push functions to the game loop
	StartLoop(Update, RenderFrame);

	DeinitSDL();
	return 0;
}

//=============================================================================

struct Vec2 {
	float x, y;
};

Vec2 AddVectors(const Vec2 v1, const Vec2 v2)
{
	return Vec2 {v1.x + v2.x, v1.y + v2.y};
}

Vec2 SubtractVectors(const Vec2 v1, const Vec2 v2)
{
	return Vec2 {v1.x - v2.x, v1.y - v2.y};
}

Vec2 MultiplyVector(const Vec2 v, float val)
{
	return Vec2 {v.x * val, v.y * val};
}

Vec2 NormalizeVector(const Vec2 v)
{
	if (v.x == 0 && v.y == 0)
		return v;
	const float len = sqrtf(v.x * v.x + v.y * v.y);
	return {v.x / len, v.y / len};
}

//=============================================================================

void swapf(float* a, float* b)
{
	float tmp = *a;
	*a = *b;
	*b = tmp;
}

float maxf(float a, float b)
{
	return a > b ? a : b;
}

float minf(float a, float b)
{
	return a < b ? a : b;
}

bool RayVsRect(const Vec2* rayOrigin, const Vec2* rayDir,
	const SDL_Rect* target, Vec2* contactPoint, Vec2* contactNormal,
	float* contactTime
)
{
	*contactPoint = {};
	*contactNormal = {};
	*contactTime = .0f;

	// Cache division to multiply instead of divide later
	Vec2 invDir = {1.0f / rayDir->x, 1.0f / rayDir->y};

	// Calculate intersections with rectangle bounding axes
	Vec2 tNear = {
		((float)target->x - rayOrigin->x) * invDir.x,
	 	((float)target->y - rayOrigin->y) * invDir.y
	};
	Vec2 tFar = {
		((float)(target->x + target->w) - rayOrigin->x) * invDir.x,
		((float)(target->y + target->h) - rayOrigin->y) * invDir.y
	};

	if (isnan(tFar.x) || isnan(tFar.y)) return false;
	if (isnan(tNear.x)|| isnan(tNear.y)) return false;

	// Sort distances
	if (tNear.x > tFar.x) swapf(&tNear.x, &tFar.x);
	if (tNear.y > tFar.y) swapf(&tNear.y, &tFar.y);

	// Early rejection
	if (tNear.x > tFar.y || tNear.y > tFar.x) return false;

	// Closest "time" will be the first contact
	*contactTime = maxf(tNear.x, tNear.y);

	// Furthest 'time' is contact on opposite side of target
	float tHitFar = minf(tFar.x, tFar.y);

	// Reject if ray direction is pointing away from object
	if (tHitFar < 0)
		return false;

	// Contact point of collision from parametric line equation
	contactPoint->x = rayOrigin->x + *contactTime * rayDir->x;
	contactPoint->y = rayOrigin->y + *contactTime * rayDir->y;

	// Make better normal vectors, maybe using dot product of the collision edge?
	if (tNear.x > tNear.y)
	{
		if (invDir.x < 0)
			*contactNormal = {1, 0};
		else
			*contactNormal = {-1, 0};
	}
	else if (tNear.x < tNear.y)
	{
		if (invDir.y < 0)
			*contactNormal = {0, 1};
		else
			*contactNormal = {0, -1};
	}
	return true;
}

bool RectVsRectCollision(const SDL_Rect* movingRect, const SDL_Rect* staticRect, Vec2* displacement, Vec2* contactPoint, Vec2* contactNormal, float* contactTime)
{
	// Check if rectangle is actually moving - we assume rectangles are NOT in collision to start
	if (displacement->x == 0 && displacement->y == 0)
	{
		return false;
	}

	// Expand target rectangle by source dimensions
	SDL_Rect expandedTargetRect = *staticRect;
	expandedTargetRect.x -= playerRect.w / 2;
	expandedTargetRect.y -= playerRect.h / 2;
	expandedTargetRect.w += playerRect.w;
	expandedTargetRect.h += playerRect.h;

	Vec2 positionCenter = {
		(float)(movingRect->x + movingRect->w / 2),
		(float)(movingRect->y + movingRect->h / 2)
	};

	// Are we colliding with the target rect?
	if (RayVsRect(&positionCenter, displacement, &expandedTargetRect, contactPoint, contactNormal, contactTime))
	{
		if (*contactTime >= 0 && *contactTime < 1.0f)
		{
			return true;
		}
	}

	return false;
}

void Update(float dt)
{
	// Change subsystem of project from Windows to Console
	// in order to see the stderr output
	if (IsKeyDown(SDL_SCANCODE_ESCAPE))
		ExitGame();

	const float speed = 150;
	Vec2 displacement = {0, 0};

	if (IsKeyDown(SDL_SCANCODE_RIGHT))
		displacement.x = 1;
	else if (IsKeyDown(SDL_SCANCODE_LEFT))
		displacement.x = -1;
	
	if (IsKeyDown(SDL_SCANCODE_DOWN))
		displacement.y = 1;
	else if (IsKeyDown(SDL_SCANCODE_UP))
		displacement.y = -1;

	// Normalize
	displacement = MultiplyVector(NormalizeVector(displacement), speed * dt);
	
	// Holds information about particular collision
	struct CollisionInfo
	{
		float Time;
		Vec2 Normal;
		Vec2 Point;
		int rectIdx;
	};

	// Find all possible collisions
	#define MAX_COLLISIONS NO_RECTS
	CollisionInfo collisions [MAX_COLLISIONS] = {};
	int colIdx = 0;

	for (int i = 0; i < NO_RECTS; ++i)
	{

		if (RectVsRectCollision(&playerRect, &rects[i], &displacement,
			&collisions[colIdx].Point,
			&collisions[colIdx].Normal,
			&collisions[colIdx].Time))
		{
			collisions[colIdx++].rectIdx = i;
		}
	}

	// Resolution of nearest collision
	for (int i = colIdx - 1; i >= 0; --i)
	{
		// Find nearest
		int nearest = i;
		for (int j = i - 1; j >= 0; --j)
		{
			if (collisions[j].Time < collisions[nearest].Time)
			{
				nearest = j;
			}
		}

		// Fast erase: store the nearest and overwrite it with the last one
		CollisionInfo nearestCollision = collisions[nearest];
		collisions[nearest] = collisions[i];

		// Collide and resolve (have to collide because of subsequent collision)
		
		if (RectVsRectCollision(&playerRect, &rects[nearestCollision.rectIdx], &displacement,
			&nearestCollision.Point,
			&nearestCollision.Normal,
			&nearestCollision.Time))
		{
			// If we have zero normal and its last collision - stop
			if (nearestCollision.Normal.x == 0 && nearestCollision.Normal.y == 0)
			{
				continue;
			}
			else
			{
				displacement.x += nearestCollision.Normal.x * fabsf(displacement.x) * (1 - nearestCollision.Time);
				displacement.y += nearestCollision.Normal.y * fabsf(displacement.y) * (1 - nearestCollision.Time);
			}
		}
	}

	playerRect.x += (int)round(displacement.x);
	playerRect.y += (int)round(displacement.y);
}

void RenderBox(const SDL_Rect* rect)
{
	SDL_RenderDrawRect(gRenderer, rect);
	SDL_RenderDrawLine(gRenderer, rect->x, rect->y, rect->x + rect->w - 1, rect->y + rect->h - 1);
	SDL_RenderDrawLine(gRenderer, rect->x + rect->w - 1, rect->y, rect->x, rect->y + rect->h - 1);
}

void RenderFrame(float interpolation)
{
	// Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0x3a, 0x2b, 0x3b, 255);
	SDL_RenderClear(gRenderer);

	// Draw playerRect
	SDL_SetRenderDrawColor(gRenderer, 0x11, 0x11, 0xff, 255);
	RenderBox(&playerRect);
	// Draw rects
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0xff, 0x11, 255);
	for (int i = 0; i < NO_RECTS; ++i)
		RenderBox(&rects[i]);
}
