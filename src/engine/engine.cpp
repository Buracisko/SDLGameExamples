#include "engine.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

//=============================================================================
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
static bool isRunning = true;
static const Uint8* keyStates = NULL;
static Uint8 lastKeyStates[SDL_NUM_SCANCODES];
static int mouseWheelX = 0;
static int mouseWheelY = 0;

//=============================================================================
bool InitSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return false;
	}

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		fprintf(stderr, "IMG_Init failed: %s\n", IMG_GetError());
		return false;
	}

	if (TTF_Init() < 0) {
		fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
		return false;
	}

	/*
	Texture filtering
	0 or nearest - nearest pixel sampling
	1 or linear - linear filtering (supported by OpenGL and Direct3D)
	2 or best - anisotropic filtering (supported by Direct3D)
	*/
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
	{
		fprintf(stderr,
			"SDL_SetHint of SDL_HINT_RENDER_SCALE_QUALITY failed: %s\n", SDL_GetError()
		);
	}

	return true;
}

//=============================================================================
void DeinitSDL()
{
	// Free renderer and window
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	// Quit all subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

//=============================================================================
bool CreateWindow(const char* title, int width, int height)
{
	gWindow = SDL_CreateWindow(title,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height,
		SDL_WINDOW_SHOWN
	);
	if (!gWindow)
	{
		fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
		return false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (!gRenderer)
	{
		fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
		return false;
	}

	// Set alpha blending
	if (SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND))
	{
		fprintf(stderr, "SDL_SetRenderDrawBlendMode error: %s\n", SDL_GetError());
	}

	// Initialize keyboardState
	keyStates = SDL_GetKeyboardState(NULL);
	memset(lastKeyStates, 0, sizeof(Uint8) * SDL_NUM_SCANCODES);

	return true;
}

//=============================================================================
void StartLoop(void (*update)(float), void (*render)(float))
{
	double time = .0;
	const double dt = 1.0 / 60.0;
	double currentTime = SDL_GetTicks() / 1000.0;
	double accumulator = 0.0;

	while (isRunning)
	{
		// Copy keyStates state to lastKeyStates.
		// It get's auto updated while calling SDL_PollEvent.
		memcpy(lastKeyStates, keyStates, sizeof(Uint8) * SDL_NUM_SCANCODES);

		// Event handling
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				isRunning = false;
				return;
			}

			if (e.type == SDL_MOUSEWHEEL)
			{
				mouseWheelX = e.wheel.x;
				mouseWheelY = e.wheel.y;

				// Flip direction to be consistent
				if (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
				{
					mouseWheelX *= -1;
					mouseWheelY *= -1;
				}
			}
		}

		// Game logic and time handling
		double newTime = SDL_GetTicks() / 1000.0;
		double frameTime = newTime - currentTime;
		if (frameTime > 0.25)
		{
			frameTime = 0.25;
		}
		currentTime = newTime;
		accumulator += frameTime;

		while (accumulator >= dt)
		{
			update((float)dt);
			time += dt;
			accumulator -= dt;

			// Reset mouse events after update was called
			mouseWheelX = mouseWheelY = 0;
		}

		if (!isRunning)
			break;

		render((float)(accumulator/dt));
		SDL_RenderPresent(gRenderer);
	}
}

//=============================================================================
void ExitGame()
{
	isRunning = false;
}

//=============================================================================
bool IsKeyDown(SDL_Scancode scanCode)
{
	return keyStates[scanCode];
}

//=============================================================================
bool IsKeyUp(SDL_Scancode scanCode)
{
	return !keyStates[scanCode];
}

//=============================================================================
bool IsKeyReleased(SDL_Scancode scanCode)
{
	return lastKeyStates[scanCode] && !keyStates[scanCode];
}

//=============================================================================
bool IsKeyPressed(SDL_Scancode scanCode)
{
	return !lastKeyStates[scanCode] && keyStates[scanCode];
}

//=============================================================================
void SetCursorVisible(bool isVisible)
{
	SDL_ShowCursor(isVisible ? SDL_ENABLE : SDL_DISABLE);
}

//=============================================================================
bool IsCursorVisible()
{
	return SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE;
}

//=============================================================================
void GetMousePosition(int* x, int* y)
{
	SDL_GetMouseState(x, y);
}

//=============================================================================
bool IsMouseButtonPressed(int button)
{
	int mouseState = SDL_GetMouseState(NULL, NULL);
	return mouseState & SDL_BUTTON(button);
}

//=============================================================================
void GetMouseWheel(int* x, int* y)
{
	// X positive = right, negative = left
	*x = mouseWheelX;
	// Y positive = from user (up)m negative = towards user (down)
	*y = mouseWheelY;
}
