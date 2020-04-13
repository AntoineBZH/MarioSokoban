#ifndef MENUMANAGEMENT_H_INCLUDED
#define MENUMANAGEMENT_H_INCLUDED

#include <SDL/SDL.h>

// Gestion du Menu de selection du mode
int GameMenuSelection(SDLKey PressedKey, SDL_Rect *sdlCursorLocation, int *SelectedMode);

// Gestion du Menu de paramètres
int ParametersSelection(SDLKey PressedKey, int *SelectedParam, int *ParamValue);

#endif // MENUMANAGEMENT_H_INCLUDED
