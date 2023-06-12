#pragma once

#include "SDL_main.h"
#include "SDL.h"
#include <stdbool.h>

extern SDL_Renderer* gRenderer;
extern SDL_Window* gWindow;

/*!
Initializes the SDL libraries.
\return Its recommended to terminate aplication on failure.
*/
bool InitSDL();

/*!
Destroy renderer, window and deinit all libraries.
*/
void DeinitSDL();

/*!
Creates window with HW accelerated renderer. Rendering is using VSync.
\param title Window title text
\param width Window width
\param height Window height
\return Its recommended to terminate aplication on failure.
*/
bool CreateWindow(const char* title, int width, int height);

/*!
Starts the main game loop
\param update Function pointer to update function with the dt as an param
\param render Function pointer to render function with the interpolation as an param
*/
void StartLoop(void (*update)(float), void (*render)(float));

/*!
Requests the termination of the game.
*/
void ExitGame();

/*!
Returns true, if key with provided scan code is down ATM
*/
bool IsKeyDown(SDL_Scancode scanCode);

/*!
Returns true, if key with provided scan code is up ATM
*/
bool IsKeyUp(SDL_Scancode scanCode);

/*!
Returns true, if key with provided scan code was released
since previous update
*/
bool IsKeyReleased(SDL_Scancode scanCode);

/*!
Returns true, if key with provided scan code was pressed
since previous update
*/
bool IsKeyPressed(SDL_Scancode scanCode);

/*!
Controls visibility of system mouse cursor in the game window.
Cursor shouldbe visible by default.
*/
void SetCursorVisible(bool isVisible);

/*!
Returns if the system cursor is shown in the game window.
*/
bool IsCursorVisible();

/*!
Sets provided pointees to position of the mouse withing the game window.
Both x and y can be NULL.
*/
void GetMousePosition(int* x, int* y);

/*!
Returns the state of given button.
\param button may be SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE or SDL_BUTTON_RIGHT
*/
bool IsMouseButtonPressed(int button);

/*!
Sets pointees to latest mouse wheel delta.
Positive value of X means rifht
Negative value of X means left
Positive value of Y means from user (usually UP)
Negative value of Y means towards user (usually down)
Note: My Thinkpad laptom is returning me the X axis flipped
*/
void GetMouseWheel(int* x, int* y);
