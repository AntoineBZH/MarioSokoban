#include "editeur.h"

int EditorEngine (SDL_Surface* sdlScreen, stSettings stGameSettings)
{
    int **iaMapTable = NULL; // Tableau 2D représentant dynamiquement la carte
    int iCodeRet = 0;
    int iEndOfEdition = 0;
    unsigned int uiWidthOfMap = stGameSettings.uiEditorMapWidth, uiHeightOfMap = stGameSettings.uiEditorMapHeight;
    unsigned int bIsCtrlHold = 0;
    SDL_Rect sdlItemLocation;
    unsigned int uiMarioIsPlaced = 0;
    char sLevelFilePath[200] = "";
    unsigned int uiXPos = 0, uiYPos = 10;
    unsigned int uiSelectedObject = MUR;
    unsigned int uiClickIsActive = 0;
    int iXMousePos = 0, iYMousePos = 0;

    if(MessageBox(NULL, "Voulez-vous choisir un niveau deja existant ?", "Demande de chargement niveau", MB_YESNO)==IDYES)
    {
        LevelChoice("Choisir un niveau", sLevelFilePath);
        iCodeRet = LoadMap(&iaMapTable, sLevelFilePath, &uiWidthOfMap, &uiHeightOfMap);
        if (iCodeRet != 0)
            return iCodeRet;
        sdlScreen = SDL_SetVideoMode(uiWidthOfMap * BLOC_SIZE, uiHeightOfMap * BLOC_SIZE, 32,
                                               SDL_HWSURFACE|SDL_DOUBLEBUF);
    }
    else
    {
        // Allocation du tableau 2D représentant la carte
        iaMapTable = (int**)malloc(uiHeightOfMap * sizeof(int));
        for(unsigned int uiLineOfMapCounter = 0; uiLineOfMapCounter < uiHeightOfMap; uiLineOfMapCounter++)
        {
            iaMapTable[uiLineOfMapCounter] = (int*)malloc(uiWidthOfMap * sizeof(int));
        }

        // Remplissage du tableau 2D pour représenter une carte vide
        for(uiXPos = 0; uiXPos < uiWidthOfMap; uiXPos++)
        {
            for(uiYPos = 0; uiYPos < uiHeightOfMap; uiYPos++)
            {
                iaMapTable[uiYPos][uiXPos]=0;
            }
        }
        sdlScreen = SDL_SetVideoMode(uiWidthOfMap * BLOC_SIZE, uiHeightOfMap * BLOC_SIZE, 32,
                                               SDL_HWSURFACE|SDL_DOUBLEBUF);
    }

    // Charge toutes les textures du jeu
    SDL_Surface* sdlCaisse = IMG_Load("Resources\\caisse.jpg");
    SDL_Surface* sdlCaisseOK = IMG_Load("Resources\\caisse_ok.jpg");
    SDL_Surface* sdlVide = SDL_CreateRGBSurface(SDL_HWSURFACE, BLOC_SIZE, BLOC_SIZE, 32, 0, 0, 0, 0);
    SDL_FillRect(sdlVide,NULL,SDL_MapRGB(sdlScreen->format, 0, 0, 0));
    SDL_Surface* sdlMur = IMG_Load("Resources\\mur.jpg");
    SDL_Surface* sdlObjectif = IMG_Load("Resources\\objectif.png");
    SDL_Surface* sdlMario = IMG_Load("Resources\\mario_bas.gif");

    SDL_Event event;

    // Refreshing loop for game issues
    while (!iEndOfEdition)
    {
        // message processing loop
        while (SDL_PollEvent(&event))
        {
            // check for messages
            switch (event.type)
            {
            // exit if the window is closed
            case SDL_QUIT:
                iEndOfEdition = 1;
                break;

            case SDL_KEYUP:
                if(event.key.keysym.sym == SDLK_LCTRL)
                    bIsCtrlHold = 0;
                break;

            // check for key presses
            case SDL_KEYDOWN:
                {
                    if(event.key.keysym.sym == SDLK_ESCAPE) // exit if ESCAPE is pressed
                        iEndOfEdition = 1;

                    if(event.key.keysym.sym == SDLK_LCTRL)
                        bIsCtrlHold = 1;

                    if((event.key.keysym.sym == SDLK_s) && bIsCtrlHold) // On sauvegarde le niveau édité
                        SaveMap(iaMapTable, uiWidthOfMap, uiHeightOfMap);

                    switch(event.key.keysym.sym)
                    {
                    case SDLK_KP1:
                        uiSelectedObject = MUR;
                        break;
                    case SDLK_KP2:
                        uiSelectedObject = CAISSE;
                        break;
                    case SDLK_KP3:
                        uiSelectedObject = OBJECTIF;
                        break;
                    case SDLK_KP4:
                        uiSelectedObject = CAISSE_OK;
                        break;
                    case SDLK_KP5:
                        if(!uiMarioIsPlaced)
                            uiSelectedObject = MARIO;
                        break;
                    case SDLK_1:
                        uiSelectedObject = MUR;
                        break;
                    case SDLK_2:
                        uiSelectedObject = CAISSE;
                        break;
                    case SDLK_3:
                        uiSelectedObject = OBJECTIF;
                        break;
                    case SDLK_4:
                        uiSelectedObject = CAISSE_OK;
                        break;
                    case SDLK_5:
                        if(!uiMarioIsPlaced)
                            uiSelectedObject = MARIO;
                        break;
                    }
                    break;
                }

            // check for mouse presses
            case SDL_MOUSEBUTTONDOWN:
                {
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        if (uiSelectedObject != MARIO)
                            uiClickIsActive = 1;
                        else
                        {
                            SDL_GetMouseState(&iXMousePos, &iYMousePos); // Récupère la position de la souris
                            // Calcul de la position de l'item
                            uiXPos = iXMousePos / BLOC_SIZE;
                            uiYPos = iYMousePos / BLOC_SIZE;
                            iaMapTable[uiYPos][uiXPos] = uiSelectedObject;
                            uiSelectedObject = MUR;
                        }
                    }
                    else if (event.button.button == SDL_BUTTON_RIGHT)
                    {
                        uiClickIsActive = 2;
                    }
                    break;
                }

            case SDL_MOUSEBUTTONUP:
                uiClickIsActive = 0;
                break;
            } // end switch
        } // end of message processing

        // DRAWING STARTS HERE

        // clear screen
        SDL_FillRect(sdlScreen, 0, SDL_MapRGB(sdlScreen->format, 0, 0, 0));

        uiMarioIsPlaced = 0;
        // draw bitmap
        for(uiXPos = 0; uiXPos < uiWidthOfMap; uiXPos++)
        {
            for(uiYPos = 0; uiYPos < uiHeightOfMap; uiYPos++)
            {
                // Calcul de la position de l'item
                sdlItemLocation.x = uiXPos * BLOC_SIZE;
                sdlItemLocation.y = uiYPos * BLOC_SIZE;

                // Sélection de la texture à placer sur la carte
                switch (iaMapTable[uiYPos][uiXPos])
                {
                case VIDE:
                    SDL_BlitSurface(sdlVide, 0, sdlScreen, &sdlItemLocation);
                    break;
                case MUR:
                    SDL_BlitSurface(sdlMur, 0, sdlScreen, &sdlItemLocation);
                    break;
                case CAISSE:
                    SDL_BlitSurface(sdlCaisse, 0, sdlScreen, &sdlItemLocation);
                    break;
                case OBJECTIF:
                    SDL_BlitSurface(sdlObjectif, 0, sdlScreen, &sdlItemLocation);
                    break;
                case CAISSE_OK:
                    SDL_BlitSurface(sdlCaisseOK, 0, sdlScreen, &sdlItemLocation);
                    break;
                case MARIO:
                    SDL_BlitSurface(sdlMario, 0, sdlScreen, &sdlItemLocation);
                    uiMarioIsPlaced = 1;
                    break;
                }
            }
        }

        // Pré-visualisation placement objet
        if(!uiClickIsActive)
        {
            SDL_GetMouseState(&iXMousePos, &iYMousePos); // Récupère la position de la souris
            sdlItemLocation.x = (iXMousePos / BLOC_SIZE) * BLOC_SIZE;
            sdlItemLocation.y = (iYMousePos / BLOC_SIZE) * BLOC_SIZE;
            // Sélection de la texture à pré-visualiser
            switch (uiSelectedObject)
            {
            case MUR:
                SDL_BlitSurface(sdlMur, 0, sdlScreen, &sdlItemLocation);
                break;
            case CAISSE:
                SDL_BlitSurface(sdlCaisse, 0, sdlScreen, &sdlItemLocation);
                break;
            case OBJECTIF:
                SDL_BlitSurface(sdlObjectif, 0, sdlScreen, &sdlItemLocation);
                break;
            case CAISSE_OK:
                SDL_BlitSurface(sdlCaisseOK, 0, sdlScreen, &sdlItemLocation);
                break;
            case MARIO:
                SDL_BlitSurface(sdlMario, 0, sdlScreen, &sdlItemLocation);
                break;
            }
        }

        // Placer un objet
        if (uiClickIsActive)
        {
            SDL_GetMouseState(&iXMousePos, &iYMousePos); // Récupère la position de la souris
            // Calcul de la position de l'item
            uiXPos = iXMousePos / BLOC_SIZE;
            uiYPos = iYMousePos / BLOC_SIZE;
            if(uiClickIsActive == 1)
                iaMapTable[uiYPos][uiXPos] = uiSelectedObject;
            else if(uiClickIsActive == 2)
                iaMapTable[uiYPos][uiXPos] = VIDE;
        }

        // DRAWING ENDS HERE

        // finally, update the screen :)
        SDL_Flip(sdlScreen);
    } // end editor loop

    // free loaded bitmap
    SDL_FreeSurface(sdlCaisse);
    SDL_FreeSurface(sdlCaisseOK);
    SDL_FreeSurface(sdlVide);
    SDL_FreeSurface(sdlMur);
    SDL_FreeSurface(sdlObjectif);
    SDL_FreeSurface(sdlMario);

    if(MessageBox(NULL, "Voulez-vous sauvegarder le niveau créé ?", "Demande de sauvegarde", MB_YESNO)==IDYES)
    {
        int iRetryAnswer = IDYES;
        do
        {
            if (SaveMap(iaMapTable, uiWidthOfMap, uiHeightOfMap) != 0)
                iRetryAnswer = MessageBox(NULL, "Une erreur s'est produite lors de la sauvegarde. Réessayer ?", "Echec sauvegarde",MB_YESNO);
            else
                iRetryAnswer = IDNO;
        } while(iRetryAnswer == IDYES);

    }

    return 0;
}
