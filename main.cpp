#include <stdlib.h>
#include <stdio.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "MenuManagement.h"
#include "jeu.h"
#include "editeur.h"
#include "constantes.h"
#include "Parameters.h"

int main ( int argc, char** argv )
{
    SDL_Rect sdlOrigin, sdlCursorLocation;
    SDL_Rect reaMenuItem[3];
    SDL_Surface* suaMenuItem[3];
    TTF_Font *ttfMenuFont = NULL;
    int SelectedMode = 1;
    int done = 0;
    stSettings stAppSettings;

    SDL_Color BlackColor = {0, 0, 0};
    SDL_Color WhiteColor = {255, 255, 255};

    // Initialisation des paramètres du jeu
    stAppSettings.uiSensitivity = 3;
    stAppSettings.uiEditorMapHeight = 12;
    stAppSettings.uiEditorMapWidth = 12;

    // Initialisation des positions
    sdlOrigin.x = 0;
    sdlOrigin.y = 0;
    sdlCursorLocation.x = 60;
    sdlCursorLocation.y = 197;

    for(int iOffset = 0; iOffset < 3; iOffset++)
    {
        reaMenuItem[iOffset].x = 60;
        reaMenuItem[iOffset].y = 197 + (iOffset * 33);
    }

    // initialize SDL video
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    // initialize SDL TTF library
    if ( TTF_Init() == -1 )
    {
        fprintf(stderr, "Unable to init SDL TTF library: %s\n", TTF_GetError() );
        return 1;
    }

    // make sure SDL TTF cleans up before exit
    atexit(TTF_Quit);

    // create a new window
    SDL_WM_SetIcon(IMG_Load("Resources\\caisse.jpg"), NULL);
    SDL_Surface* screen = SDL_SetVideoMode(408, 408, 32,
                                           SDL_HWSURFACE|SDL_DOUBLEBUF);
    SDL_WM_SetCaption("Mario Sokoban", NULL);
    if ( !screen )
    {
        fprintf(stderr,"Unable to set 408x408 video: %s\n", SDL_GetError());
        return 1;
    }

    // load an image
    SDL_Surface* MenuScreen = IMG_Load("Resources\\menu.jpg");
    if (!MenuScreen)
    {
        fprintf(stderr,"Unable to load menu: %s\n", SDL_GetError());
        return 1;
    }

    // Création du menu
    ttfMenuFont = TTF_OpenFont("lucon.ttf", 22);
    if (ttfMenuFont == NULL)
    {
        fprintf(stderr,"Unable to load Lucida Console font: %s\n", TTF_GetError());
        return 1;
    }
    suaMenuItem[0] = TTF_RenderText_Shaded(ttfMenuFont, "1 : jouer", WhiteColor, BlackColor);
    suaMenuItem[1] = TTF_RenderText_Shaded(ttfMenuFont, "2 : éditeur de niveaux", WhiteColor, BlackColor);
    suaMenuItem[2] = TTF_RenderText_Shaded(ttfMenuFont, "3 : paramètres", WhiteColor, BlackColor);

    SDL_Surface* SelectSurface = SDL_CreateRGBSurface(SDL_HWSURFACE, CURSOR_WIDTH, CURSOR_HEIGHT, 32, 0, 0, 0, 0);
    if (!SelectSurface)
    {
        fprintf(stderr,"Unable to load cursor: %s\n", SDL_GetError());
        return 1;
    }
    SDL_FillRect(SelectSurface,NULL,SDL_MapRGB(screen->format, 0, 255, 0));
    SDL_SetAlpha(SelectSurface, SDL_SRCALPHA, 100); // Transparence Alpha moyenne (128)

    // program main loop
    while (!done)
    {
        // message processing loop
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // check for messages
            switch (event.type)
            {
                // exit if the window is closed
            case SDL_QUIT:
                done = 1;
                break;

                // check for keypresses
            case SDL_KEYDOWN:
                {
                    // exit if ESCAPE is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        done = 1;
                    // menu navigation
                    else
                        if(GameMenuSelection(event.key.keysym.sym, &sdlCursorLocation, &SelectedMode))
                        {
                            switch (SelectedMode)
                            {
                            case 1:
                                done = GameEngine(screen, stAppSettings);
                                SelectedMode = 1;
                                screen = SDL_SetVideoMode(408, 408, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
                                sdlCursorLocation.x = 60;
                                sdlCursorLocation.y = 197;
                                break;
                            case 2:
                                done = EditorEngine(screen, stAppSettings);
                                SelectedMode = 1;
                                screen = SDL_SetVideoMode(408, 408, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
                                sdlCursorLocation.x = 60;
                                sdlCursorLocation.y = 197;
                                break;
                            case 3:
                                done = EditParameters(screen, &stAppSettings);
                                SelectedMode = 1;
                                screen = SDL_SetVideoMode(408, 408, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);
                                sdlCursorLocation.x = 60;
                                sdlCursorLocation.y = 197;
                                break;
                            default:
                                fprintf(stderr,"Unexpected Mode %d\n",SelectedMode);
                                return 1;
                            }
                        }
                    break;
                }
            } // end switch
        } // end of message processing

        // DRAWING STARTS HERE

        // clear screen
        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

        // draw bitmap
        SDL_BlitSurface(MenuScreen, 0, screen, &sdlOrigin);
        for(int iSurfaceToBlit = 0; iSurfaceToBlit < 3; iSurfaceToBlit++)
        {
            SDL_BlitSurface(suaMenuItem[iSurfaceToBlit], 0, screen, &reaMenuItem[iSurfaceToBlit]);
        }
        SDL_BlitSurface(SelectSurface, 0, screen, &sdlCursorLocation);

        // DRAWING ENDS HERE

        // finally, update the screen :)
        SDL_Flip(screen);
    } // end main loop

    // free loaded bitmap
    SDL_FreeSurface(SelectSurface);
    SDL_FreeSurface(MenuScreen);

    // free loaded font
    TTF_CloseFont(ttfMenuFont);

    // all is well ;)
    fprintf(stderr,"Exited cleanly\n");
    return 0;
}
