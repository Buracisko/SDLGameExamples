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
SDL_Texture* LoadSprite(const char* path);
SDL_Texture* RenderText(const char* str, SDL_Colour colour, TTF_Font* font, SDL_Rect* textRect);

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 360

#define NO_RECTS 2
SDL_Rect rects[NO_RECTS];

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

	rects[0] = SDL_Rect {0, 0, 64, 64};
	rects[1] = SDL_Rect {255, 128, 128, 128};

	// Push functions to the game loop
	StartLoop(Update, RenderFrame);

	DeinitSDL();
	return 0;
}

//=============================================================================

struct Vec2 {
	float x, y;
};

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

bool RayVsRect(const Vec2* rayOrigin, const Vec2* rayDir,
	const SDL_Rect* target, Vec2* contactPoint, Vec2* contactNormal,
	float* tHitNear
)
{
	//TODO: Change SDL_Rect to SDL_FRect. My linux libraries are ancient
	*contactNormal = {0 , 0};
	*contactPoint = {0, 0};

	// Cache division
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

	if (tFar.x == NAN || tFar.y == NAN) return false;
	if (tNear.x == NAN || tNear.y == NAN) return false;

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

	// Make better normal vectors
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

void Update(float dt)
{
	// Change subsystem of project from Windows to Console
	// in order to see the stderr output
	if (IsKeyDown(SDL_SCANCODE_ESCAPE))
		ExitGame();

	const float speed = 100;
	Vec2 displacement = {0, 0};
	Vec2 positionCenter = {
		(float)(rects[0].x + rects[0].w / 2),
		(float)(rects[0].y + rects[0].h / 2)
	};

	if (IsKeyDown(SDL_SCANCODE_RIGHT))
		displacement.x += speed * dt;
	else if (IsKeyDown(SDL_SCANCODE_LEFT))
		displacement.x -= speed * dt;
	
	if (IsKeyDown(SDL_SCANCODE_DOWN))
		displacement.y += speed * dt;
	else if (IsKeyDown(SDL_SCANCODE_UP))
		displacement.y -= speed * dt;

	// Collision information
	bool isColliding = false;
	Vec2 contactPoint = {};
	Vec2 contactNormal = {};
	float contactTime = .0f;

	// Check if rectangle is actually moving - we assume rectangles are NOT in collision to start
	if (displacement.x != 0 || displacement.y != 0)
	{
		// Expand target rectangle by source dimensions
		SDL_Rect expandedTargetRect = rects[1];
		expandedTargetRect.x -= rects[0].w / 2;
		expandedTargetRect.y -= rects[0].h / 2;
		expandedTargetRect.w += rects[0].w;
		expandedTargetRect.h += rects[0].h;

		// Are we colliding with the target rect?
		if (RayVsRect(&positionCenter, &displacement, &expandedTargetRect, &contactPoint, &contactNormal, &contactTime)
			&& (contactTime >= 0.0f && contactTime < 1.0f)
		)
		{
				isColliding = true;
		}
	}

	// If there is a colision, resolve it
	if (isColliding)
	{
		displacement.x += contactNormal.x * fabs(displacement.x) * (1 - contactTime);
		displacement.y += contactNormal.y * fabs(displacement.y) * (1 - contactTime);
	}

	rects[0].x += (int)(displacement.x);
	rects[0].y += (int)(displacement.y);
}

void RenderFrame(float interpolation)
{
	// Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0x3a, 0x2b, 0x3b, 255);
	SDL_RenderClear(gRenderer);

	// Draw rects
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0xff, 0x11, 255);

	for (int i = 0; i < NO_RECTS; ++i)
		SDL_RenderFillRect(gRenderer, &rects[i]);
}
