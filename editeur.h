#ifndef EDITEUR_H_INCLUDED
#define EDITEUR_H_INCLUDED

#include <windows.h>
#include <commdlg.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "fichiers.h"
#include "constantes.h"

//Moteur de l'éditeur
int EditorEngine (SDL_Surface* sdlScreen, stSettings stGameSettings);

#endif // EDITEUR_H_INCLUDED
