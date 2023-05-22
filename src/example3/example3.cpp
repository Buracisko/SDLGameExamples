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
	rects[0] = SDL_Rect {128, 128, 64, 64};
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

struct Vector2 {
	float x, y;
};

Vector2 AddVector2(const Vector2 v1, const Vector2 v2)
{
	return Vector2 {v1.x + v2.x, v1.y + v2.y};
}

Vector2 SubVector2(const Vector2 v1, const Vector2 v2)
{
	return Vector2 {v1.x - v2.x, v1.y - v2.y};
}

Vector2 MultVector2(const Vector2 v, float val)
{
	return Vector2 {v.x * val, v.y * val};
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

// Source: https://github.com/OneLoneCoder/Javidx9/blob/master/PixelGameEngine/SmallerProjects/OneLoneCoder_PGE_Rectangles.cpp

bool RayVsRect(const Vector2* rayOrigin, const Vector2* rayDir,
	const SDL_Rect* target, Vector2* contactPoint, Vector2* contactNormal,
	float* tHitNear
)
{
	// Cache division to multiply instead of divide later
	Vector2 invDir = {1.0f / rayDir->x, 1.0f / rayDir->y};

	// Calculate intersections with rectangle bounding axes
	Vector2 tNear = {
		((float)target->x - rayOrigin->x) * invDir.x,
	 	((float)target->y - rayOrigin->y) * invDir.y
	};
	Vector2 tFar = {
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
	*tHitNear = maxf(tNear.x, tNear.y);

	// Furthest 'time' is contact on opposite side of target
	float tHitFar = minf(tFar.x, tFar.y);

	// Reject if ray direction is pointing away from object
	if (tHitFar < 0)
		return false;

	// Contact point of collision from parametric line equation
	contactPoint->x = rayOrigin->x + *tHitNear * rayDir->x;
	contactPoint->y = rayOrigin->y + *tHitNear * rayDir->y;

	// Make better normal vectors using dot product
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

	// Note if tNear == tFar, collision is principly in a diagonal
	// so pointless to resolve. By returning a CN={0,0} even though its
	// considered a hit, the resolver wont change anything.
	return true;
}

bool RectVsRectCollision(const SDL_Rect* movingRect, const SDL_Rect* staticRect, Vector2* displacement, Vector2* contactPoint, Vector2* contactNormal, float* contactTime)
{
	// Check if rectangle is actually moving - we assume rectangles are NOT in collision to start
	if (displacement->x == 0 && displacement->y == 0)
	{
		return false;
	}

	*contactPoint = {};
	*contactNormal = {};
	*contactTime = .0f;

	// Expand target rectangle by source dimensions
	SDL_Rect expandedTargetRect = *staticRect;
	expandedTargetRect.x -= playerRect.w / 2;
	expandedTargetRect.y -= playerRect.h / 2;
	expandedTargetRect.w += playerRect.w;
	expandedTargetRect.h += playerRect.h;

	Vector2 positionCenter = {
		(float)(movingRect->x + movingRect->w / 2),
		(float)(movingRect->y + movingRect->h / 2)
	};

	// Are we colliding with the target rect?
	if (RayVsRect(&positionCenter, displacement, &expandedTargetRect, contactPoint, contactNormal, contactTime))
	{
		if (*contactTime >= 0 && *contactTime <= 1.0f)
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
	Vector2 displacement = {0, 0};

	if (IsKeyDown(SDL_SCANCODE_RIGHT))
		displacement.x += speed * dt;
	else if (IsKeyDown(SDL_SCANCODE_LEFT))
		displacement.x -= speed * dt;
	
	if (IsKeyDown(SDL_SCANCODE_DOWN))
		displacement.y += speed * dt;
	else if (IsKeyDown(SDL_SCANCODE_UP))
		displacement.y -= speed * dt;

	for (int i = 0; i < NO_RECTS; ++i)
	{
		// Collision information
		Vector2 contactPoint = {};
		Vector2 contactNormal = {};
		float contactTime = .0f;

		if (RectVsRectCollision(&playerRect, &rects[i], &displacement, &contactPoint, &contactNormal, &contactTime))
		{
			// If diagonal colllision (normal == {0, 0}), just stop
			if (contactNormal.x == 0 && contactNormal.y == 0)
			{
				displacement.x = displacement.y = 0;
			}
			else
			{
				displacement.x += contactNormal.x * fabsf(displacement.x) * (1 - contactTime);
				displacement.y += contactNormal.y * fabsf(displacement.y) * (1 - contactTime);
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
