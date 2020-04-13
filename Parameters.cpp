#include "Parameters.h"

int EditParameters (SDL_Surface* sdlScreen, stSettings* stGameSettings)
{
    SDL_Rect reaMenuItem[3];
    SDL_Surface* suaMenuItem[3];
    TTF_Font *ttfMenuFont = NULL;
    int SelectedMode = 1, iParamValue = 0, iSelectedParam = 0;
    int done = 0;
    char sParameterDisp[100] = "";

    SDL_Color BlackColor = {0, 0, 0};
    SDL_Color WhiteColor = {255, 255, 255};
    SDL_Color GreenColor = {0, 255, 0};

    // Initialisation des positions
    for(int iOffset = 0; iOffset < 3; iOffset++)
    {
        reaMenuItem[iOffset].x = 10;
        reaMenuItem[iOffset].y = 60 + (iOffset * 30);
    }

    // initialize SDL TTF library
    if ( TTF_Init() == -1 )
    {
        fprintf(stderr, "Unable to init SDL TTF library: %s\n", TTF_GetError() );
        return 1;
    }

    // make sure SDL TTF cleans up before exit
    atexit(TTF_Quit);

    // Création du menu
    ttfMenuFont = TTF_OpenFont("lucon.ttf", 22);
    if (ttfMenuFont == NULL)
    {
        fprintf(stderr,"Unable to load Lucida Console font: %s\n", TTF_GetError());
        return 1;
    }

    // Initialisation du paramètre sélectionné
    iSelectedParam = 0;
    iParamValue = (int)stGameSettings->uiSensitivity;
    // Initialisation de l'affichage des paramètres
    sprintf(sParameterDisp, "Sensibilité : %d", stGameSettings->uiSensitivity);
    suaMenuItem[0] = TTF_RenderText_Shaded(ttfMenuFont, sParameterDisp, GreenColor, BlackColor);
    sprintf(sParameterDisp, "Largeur de l'éditeur : %d", stGameSettings->uiEditorMapWidth);
    suaMenuItem[1] = TTF_RenderText_Shaded(ttfMenuFont, sParameterDisp, WhiteColor, BlackColor);
    sprintf(sParameterDisp, "Hauteur de l'éditeur : %d", stGameSettings->uiEditorMapHeight);
    suaMenuItem[2] = TTF_RenderText_Shaded(ttfMenuFont, sParameterDisp, WhiteColor, BlackColor);

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
                    {
                        if(ParametersSelection(event.key.keysym.sym, &iSelectedParam, &iParamValue) != 0)
                        {
                            // Cas où le paramètre sélectionné à changer

                            // Initialisation de l'affichage des paramètres
                            sprintf(sParameterDisp, "Sensibilité : %d", stGameSettings->uiSensitivity);
                            suaMenuItem[0] = TTF_RenderText_Shaded(ttfMenuFont, sParameterDisp, WhiteColor, BlackColor);
                            sprintf(sParameterDisp, "Largeur de l'éditeur : %d", stGameSettings->uiEditorMapWidth);
                            suaMenuItem[1] = TTF_RenderText_Shaded(ttfMenuFont, sParameterDisp, WhiteColor, BlackColor);
                            sprintf(sParameterDisp, "Hauteur de l'éditeur : %d", stGameSettings->uiEditorMapHeight);
                            suaMenuItem[2] = TTF_RenderText_Shaded(ttfMenuFont, sParameterDisp, WhiteColor, BlackColor);

                            // Mise en forme
                            switch(iSelectedParam)
                            {
                            case 0:
                                iParamValue = (int)stGameSettings->uiSensitivity;
                                sprintf(sParameterDisp, "Sensibilité : %d", stGameSettings->uiSensitivity);
                                suaMenuItem[0] = TTF_RenderText_Shaded(ttfMenuFont, sParameterDisp, GreenColor, BlackColor);
                                break;
                            case 1:
                                iParamValue = (int)stGameSettings->uiEditorMapWidth;
                                sprintf(sParameterDisp, "Largeur de l'éditeur : %d", stGameSettings->uiEditorMapWidth);
                                suaMenuItem[1] = TTF_RenderText_Shaded(ttfMenuFont, sParameterDisp, GreenColor, BlackColor);
                                break;
                            case 2:
                                iParamValue = (int)stGameSettings->uiEditorMapHeight;
                                sprintf(sParameterDisp, "Hauteur de l'éditeur : %d", stGameSettings->uiEditorMapHeight);
                                suaMenuItem[2] = TTF_RenderText_Shaded(ttfMenuFont, sParameterDisp, GreenColor, BlackColor);
                                break;
                            }
                        }
                        else
                        {
                            // Cas où la valeur du paramètre sélectionné à changer
                            // Mise en forme
                            switch(iSelectedParam)
                            {
                            case 0:
                                stGameSettings->uiSensitivity = (unsigned int)iParamValue;
                                sprintf(sParameterDisp, "Sensibilité : %d", stGameSettings->uiSensitivity);
                                suaMenuItem[0] = TTF_RenderText_Shaded(ttfMenuFont, sParameterDisp, GreenColor, BlackColor);
                                break;
                            case 1:
                                stGameSettings->uiEditorMapWidth = (unsigned int)iParamValue;
                                sprintf(sParameterDisp, "Largeur de l'éditeur : %d", stGameSettings->uiEditorMapWidth);
                                suaMenuItem[1] = TTF_RenderText_Shaded(ttfMenuFont, sParameterDisp, GreenColor, BlackColor);
                                break;
                            case 2:
                                stGameSettings->uiEditorMapHeight = (unsigned int)iParamValue;
                                sprintf(sParameterDisp, "Hauteur de l'éditeur : %d", stGameSettings->uiEditorMapHeight);
                                suaMenuItem[2] = TTF_RenderText_Shaded(ttfMenuFont, sParameterDisp, GreenColor, BlackColor);
                                break;
                            }
                        }
                    }
                    break;
                }
            } // end switch
        } // end of message processing

        // DRAWING STARTS HERE

        // clear screen
        SDL_FillRect(sdlScreen, 0, SDL_MapRGB(sdlScreen->format, 0, 0, 0));

        // draw bitmap
        for(int iSurfaceToBlit = 0; iSurfaceToBlit < 3; iSurfaceToBlit++)
        {
            SDL_BlitSurface(suaMenuItem[iSurfaceToBlit], 0, sdlScreen, &reaMenuItem[iSurfaceToBlit]);
        }

        // DRAWING ENDS HERE

        // finally, update the screen :)
        SDL_Flip(sdlScreen);
    } // end main loop

    // free loaded font
    TTF_CloseFont(ttfMenuFont);
    TTF_Quit();

    return 0;
}
