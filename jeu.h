#ifndef JEU_H_INCLUDED
#define JEU_H_INCLUDED

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "fmod.h"
#include "fichiers.h"
#include "constantes.h"

typedef struct {
    int iLastXPos;
    int iLastYPos;
    unsigned int uiBodyPosition;
    SDL_Rect sdlMarioLocation;
    unsigned int uiStatus;
    unsigned int uiUnderMario;
} stCharacter;

// Moteur du jeu
int GameEngine (SDL_Surface* sdlScreen, stSettings stGameSettings);

// Déplacement du joueur
int PlayerMove(SDLKey SDLK_PressedKey, stCharacter *stMario, int ***piaMapTable, unsigned int uiWidthOfMap, unsigned int uiHeightOfMap, int *iPlayerAction);

// Affichage de l'écran de victoire
int WinningScreen (SDL_Surface* sdlScreen, unsigned int uiWidthOfMap, unsigned int uiHeightOfMap, unsigned int uiPastTime);

#endif // JEU_H_INCLUDED
