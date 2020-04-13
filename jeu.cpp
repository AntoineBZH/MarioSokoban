#include "jeu.h"

int GameEngine (SDL_Surface* sdlScreen, stSettings stGameSettings)
{
    int **iaMapTable = NULL; // Tableau 2D représentant dynamiquement la carte
    int iCodeRet = 0;
    int iEndOfTheGame = 0;
    unsigned int uiWidthOfMap = 0, uiHeightOfMap = 0;
    unsigned int bIsCtrlHold = 0;
    SDL_Rect sdlItemLocation;
    stCharacter stMario;
    char sLevelFilePath[200] = "";

    // Création des variables liées au compteur
    SDL_Rect sdlCounterLoc;
    SDL_Surface *sdlCounterSurf = NULL;
    unsigned int uiTimeOccured = 0;
    unsigned int uiStartingTime = 0;
    TTF_Font *ttfCounterFont = NULL;
    char pcTimeOccured[100] = "";
    SDL_Color WhiteColor = {255, 255, 255};

    // Initialisation des informations du personnage
    stMario.iLastXPos = -1;
    stMario.iLastYPos = -1;
    stMario.uiBodyPosition = BAS;
    stMario.uiStatus = READY;
    stMario.uiUnderMario = VIDE;

    // Création des variables liées au son du jeu
    FMOD_SYSTEM *fmodSystem;
    FMOD_SOUND *fmodPush = NULL;
    FMOD_SOUND *fmodGoal = NULL;
    FMOD_SOUND *fmodWalk = NULL;
    FMOD_SOUND *fmodMusic = NULL;
    int iPlayerAction = 0;

    // Initialisation de la bibliothèque SDL TTF
    if ( TTF_Init() == -1 )
    {
        fprintf(stderr, "Unable to init SDL TTF library: %s\n", TTF_GetError() );
        return -1;
    }

    // Création d'une police pour l'affichage du compteur
    ttfCounterFont = TTF_OpenFont("lucon.ttf", 18);
    if (ttfCounterFont == NULL)
    {
        fprintf(stderr,"Unable to load Lucida Console font: %s\n", TTF_GetError());
        return -2;
    }

    // Initialisation de la bibliothèque FMOD
    FMOD_System_Create(&fmodSystem);
    if (FMOD_OK != FMOD_System_Init(fmodSystem, 32, FMOD_INIT_NORMAL, NULL))
    {
        fprintf(stderr,"Unable to init FMOD library.\n");
        return -3;
    }

    // Chargement des sons
    if (FMOD_OK != FMOD_System_CreateSound(fmodSystem, "Resources\\mario_push.wav", FMOD_CREATESAMPLE, NULL, &fmodPush))
    {
        fprintf(stderr,"Unable to load sound mario_push.wav.\n");
        return -4;
    }
    if (FMOD_OK != FMOD_System_CreateSound(fmodSystem, "Resources\\Footsteps.wav", FMOD_CREATESAMPLE, NULL, &fmodWalk))
    {
        fprintf(stderr,"Unable to load sound Footsteps.wav.\n");
        return -4;
    }
    if (FMOD_OK != FMOD_System_CreateSound(fmodSystem, "Resources\\goal.wav", FMOD_CREATESAMPLE, NULL, &fmodGoal))
    {
        fprintf(stderr,"Unable to load sound goal.wav.\n");
        return -4;
    }
    // Chargement de la musique de fond
    if (FMOD_OK != FMOD_System_CreateSound(fmodSystem, "Resources\\Music.mp3", FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL, NULL, &fmodMusic))
    {
        fprintf(stderr,"Unable to load the music Music.mp3.\n");
        return -4;
    }
    FMOD_Sound_SetLoopCount(fmodMusic, -1);

    LevelChoice("Choisir un niveau", sLevelFilePath);
    iCodeRet = LoadMap(&iaMapTable, sLevelFilePath, &uiWidthOfMap, &uiHeightOfMap);
    if (iCodeRet != 0)
        return iCodeRet;
    sdlScreen = SDL_SetVideoMode(uiWidthOfMap * BLOC_SIZE, uiHeightOfMap * BLOC_SIZE, 32,
                                           SDL_HWSURFACE|SDL_DOUBLEBUF);
    SDL_EnableKeyRepeat(1, 10); // Active la répétition des touches

    // Charge toutes les textures du jeu
    SDL_Surface* sdlCaisse = IMG_Load("Resources\\caisse.jpg");
    SDL_Surface* sdlCaisseOK = IMG_Load("Resources\\caisse_ok.jpg");
    SDL_Surface* sdlVide = SDL_CreateRGBSurface(SDL_HWSURFACE, BLOC_SIZE, BLOC_SIZE, 32, 0, 0, 0, 0);
    SDL_FillRect(sdlVide,NULL,SDL_MapRGB(sdlScreen->format, 0, 0, 0));
    SDL_Surface* sdlMur = IMG_Load("Resources\\mur.jpg");
    SDL_Surface* sdlObjectif = IMG_Load("Resources\\objectif.png");
    SDL_Surface* sdlMarioDown = IMG_Load("Resources\\mario_bas.gif");
    SDL_Surface* sdlMarioUp = IMG_Load("Resources\\mario_haut.gif");
    SDL_Surface* sdlMarioLeft = IMG_Load("Resources\\mario_gauche.gif");
    SDL_Surface* sdlMarioRight = IMG_Load("Resources\\mario_droite.gif");

    SDL_Event event;

    // Initialisation du compteur de la partie
    uiStartingTime = SDL_GetTicks() / 1000;

    // Début de la musique de fond
    FMOD_System_PlaySound(fmodSystem, FMOD_CHANNEL_FREE, fmodMusic, 0, NULL);
    // Refreshing loop for game issues
    while (!iEndOfTheGame)
    {
        // message processing loop
        while (SDL_PollEvent(&event))
        {
            // check for messages
            switch (event.type)
            {
            // exit if the window is closed
            case SDL_QUIT:
                iEndOfTheGame = 1;
                break;

            case SDL_KEYUP:
                if(event.key.keysym.sym == SDLK_LCTRL)
                    bIsCtrlHold = 0;
                break;

            // check for keypresses
            case SDL_KEYDOWN:
                {
                    if(event.key.keysym.sym == SDLK_ESCAPE) // exit if ESCAPE is pressed
                        iEndOfTheGame = 1;

                    if(event.key.keysym.sym == SDLK_LCTRL)
                        bIsCtrlHold = 1;

                    if((event.key.keysym.sym == SDLK_s) && bIsCtrlHold && stMario.uiUnderMario == VIDE) // On sauvegarde la partie
                        SaveMap(iaMapTable, uiWidthOfMap, uiHeightOfMap);

                    // Traitement des déplacements
                    if(stMario.uiStatus == READY) // On attend qu'un déplacement soit fini avant d'en faire un nouveau
                    {
                        iCodeRet = PlayerMove(event.key.keysym.sym, &stMario, &iaMapTable, uiWidthOfMap, uiHeightOfMap, &iPlayerAction);
                        switch(iPlayerAction)
                        {
                        case WALK:
                            FMOD_System_PlaySound(fmodSystem, FMOD_CHANNEL_FREE, fmodWalk, 0, NULL);
                            break;
                        case PUSH:
                            FMOD_System_PlaySound(fmodSystem, FMOD_CHANNEL_FREE, fmodPush, 0, NULL);
                            break;
                        case GOAL:
                            FMOD_System_PlaySound(fmodSystem, FMOD_CHANNEL_FREE, fmodGoal, 0, NULL);
                            break;
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
        if(iEndOfTheGame != 1)
            iEndOfTheGame = 2; // Jeu considéré comme gagné tant qu'aucun objectif n'est détecté sur la carte
        if(OBJECTIF == stMario.uiUnderMario)
            iEndOfTheGame = 0; // Jeu toujours en cours si Mario recouvre un objectif
        for(unsigned int uiXPos = 0; uiXPos < uiWidthOfMap; uiXPos++)
        {
            for(unsigned int uiYPos = 0; uiYPos < uiHeightOfMap; uiYPos++)
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
                    if(iEndOfTheGame != 1)
                        iEndOfTheGame = 0; // Jeu toujours en cours
                    break;
                case MARIO:
                    if(OBJECTIF == stMario.uiUnderMario)
                        SDL_BlitSurface(sdlObjectif, 0, sdlScreen, &sdlItemLocation);

                    if(stMario.iLastXPos == -1) // Initialisation de la position du personnage
                    {
                        stMario.iLastXPos = uiXPos;
                        stMario.iLastYPos = uiYPos;
                        stMario.sdlMarioLocation.x = sdlItemLocation.x;
                        stMario.sdlMarioLocation.y = sdlItemLocation.y;
                    }
                    else if(stMario.iLastXPos < uiXPos)
                    {
                        stMario.sdlMarioLocation.x++;
                    }
                    else if(stMario.iLastXPos > uiXPos)
                    {
                        stMario.sdlMarioLocation.x--;
                    }
                    else if(stMario.iLastYPos < uiYPos)
                    {
                        stMario.sdlMarioLocation.y++;
                    }
                    else if(stMario.iLastYPos > uiYPos)
                    {
                        stMario.sdlMarioLocation.y--;
                    }

                    if((stMario.sdlMarioLocation.x == sdlItemLocation.x)&&(stMario.sdlMarioLocation.y == sdlItemLocation.y)) // Fin du déplacement
                    {
                        stMario.iLastXPos = uiXPos;
                        stMario.iLastYPos = uiYPos;
                        stMario.uiStatus = READY;
                    }
                    else
                    {
                        SDL_Delay(stGameSettings.uiSensitivity);
                    }

                    switch(stMario.uiBodyPosition)
                    {
                    case HAUT:
                        SDL_BlitSurface(sdlMarioUp, 0, sdlScreen, &stMario.sdlMarioLocation);
                        break;
                    case BAS:
                        SDL_BlitSurface(sdlMarioDown, 0, sdlScreen, &stMario.sdlMarioLocation);
                        break;
                    case GAUCHE:
                        SDL_BlitSurface(sdlMarioLeft, 0, sdlScreen, &stMario.sdlMarioLocation);
                        break;
                    case DROITE:
                        SDL_BlitSurface(sdlMarioRight, 0, sdlScreen, &stMario.sdlMarioLocation);
                        break;
                    }
                    break;
                case CAISSE_OK:
                    SDL_BlitSurface(sdlCaisseOK, 0, sdlScreen, &sdlItemLocation);
                    break;
                }
            }
        }
        // Actualisation du compteur
        uiTimeOccured = (SDL_GetTicks() / 1000) - uiStartingTime;
        sprintf(pcTimeOccured, "%d", uiTimeOccured);
        sdlCounterSurf = TTF_RenderText_Blended(ttfCounterFont, pcTimeOccured, WhiteColor);
        sdlCounterLoc.h = 0;
        sdlCounterLoc.x = sdlScreen->w - sdlCounterSurf->w;
        SDL_BlitSurface(sdlCounterSurf, 0, sdlScreen, &sdlCounterLoc);

        // DRAWING ENDS HERE

        // finally, update the screen :)
        SDL_Flip(sdlScreen);
    } // end game loop

    SDL_EnableKeyRepeat(0, 0); // Désactive la répétition des touches

    // free loaded bitmap
    SDL_FreeSurface(sdlCaisse);
    SDL_FreeSurface(sdlCaisseOK);
    SDL_FreeSurface(sdlVide);
    SDL_FreeSurface(sdlMur);
    SDL_FreeSurface(sdlObjectif);
    SDL_FreeSurface(sdlMarioUp);
    SDL_FreeSurface(sdlMarioDown);
    SDL_FreeSurface(sdlMarioLeft);
    SDL_FreeSurface(sdlMarioRight);

    // free loaded font
    TTF_CloseFont(ttfCounterFont);
    TTF_Quit();

    // free loaded sound
    FMOD_Sound_Release(fmodGoal);
    FMOD_Sound_Release(fmodPush);
    FMOD_Sound_Release(fmodWalk);
    FMOD_Sound_Release(fmodMusic);
    FMOD_System_Close(fmodSystem);
    FMOD_System_Release(fmodSystem);

    if (iEndOfTheGame == 2) // Partie gagnée
    {
        iCodeRet = WinningScreen(sdlScreen, uiWidthOfMap, uiHeightOfMap, uiTimeOccured);
    }

    return 0;
}

int PlayerMove(SDLKey SDLK_PressedKey, stCharacter *stMario, int ***iaMapTable, unsigned int uiWidthOfMap, unsigned int uiHeightOfMap, int *iPlayerAction)
{
    // Initialisation à un déplacement impossible
    *iPlayerAction = NOTHING;

    switch (SDLK_PressedKey)
    {
    case SDLK_DOWN:
        if(stMario->iLastYPos+1 < uiHeightOfMap) // Limitation de la carte
        {
            if((*iaMapTable)[stMario->iLastYPos+1][stMario->iLastXPos] == VIDE || (*iaMapTable)[stMario->iLastYPos+1][stMario->iLastXPos] == OBJECTIF) // Déplacement simple
            {
                (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                stMario->uiUnderMario = (*iaMapTable)[stMario->iLastYPos+1][stMario->iLastXPos];
                (*iaMapTable)[stMario->iLastYPos+1][stMario->iLastXPos] = MARIO;
                *iPlayerAction = WALK;
            }
            else if(stMario->iLastYPos+2 < uiHeightOfMap)
            {
                if(((*iaMapTable)[stMario->iLastYPos+1][stMario->iLastXPos] == CAISSE)&&((*iaMapTable)[stMario->iLastYPos+2][stMario->iLastXPos] == VIDE)) // Pousser une caisse
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    stMario->uiUnderMario = VIDE;
                    (*iaMapTable)[stMario->iLastYPos+1][stMario->iLastXPos] = MARIO;
                    (*iaMapTable)[stMario->iLastYPos+2][stMario->iLastXPos] = CAISSE;
                    *iPlayerAction = PUSH;
                }
                else if(((*iaMapTable)[stMario->iLastYPos+1][stMario->iLastXPos] == CAISSE)&&((*iaMapTable)[stMario->iLastYPos+2][stMario->iLastXPos] == OBJECTIF)) // Pousser une caisse sur un objectif
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    stMario->uiUnderMario = VIDE;
                    (*iaMapTable)[stMario->iLastYPos+1][stMario->iLastXPos] = MARIO;
                    (*iaMapTable)[stMario->iLastYPos+2][stMario->iLastXPos] = CAISSE_OK;
                    *iPlayerAction = GOAL;
                }
                else if(((*iaMapTable)[stMario->iLastYPos+1][stMario->iLastXPos] == CAISSE_OK)&&((*iaMapTable)[stMario->iLastYPos+2][stMario->iLastXPos] == VIDE)) // Pousser une caisse
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    (*iaMapTable)[stMario->iLastYPos+1][stMario->iLastXPos] = MARIO;
                    stMario->uiUnderMario = OBJECTIF;
                    (*iaMapTable)[stMario->iLastYPos+2][stMario->iLastXPos] = CAISSE;
                    *iPlayerAction = PUSH;
                }
                else if(((*iaMapTable)[stMario->iLastYPos+1][stMario->iLastXPos] == CAISSE_OK)&&((*iaMapTable)[stMario->iLastYPos+2][stMario->iLastXPos] == OBJECTIF)) // Pousser une caisse sur un objectif
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    (*iaMapTable)[stMario->iLastYPos+1][stMario->iLastXPos] = MARIO;
                    stMario->uiUnderMario = OBJECTIF;
                    (*iaMapTable)[stMario->iLastYPos+2][stMario->iLastXPos] = CAISSE_OK;
                    *iPlayerAction = GOAL;
                }
            }
        }
        stMario->uiBodyPosition = BAS;
        stMario->uiStatus = BUSY;
        break;
    case SDLK_UP:
        if(stMario->iLastYPos-1 >= 0) // Limitation de la carte
        {
            if((*iaMapTable)[stMario->iLastYPos-1][stMario->iLastXPos] == VIDE || (*iaMapTable)[stMario->iLastYPos-1][stMario->iLastXPos] == OBJECTIF) // Déplacement simple
            {
                (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                stMario->uiUnderMario = (*iaMapTable)[stMario->iLastYPos-1][stMario->iLastXPos];
                (*iaMapTable)[stMario->iLastYPos-1][stMario->iLastXPos] = MARIO;
                *iPlayerAction = WALK;
            }
            else if(stMario->iLastYPos-2 >= 0)
            {
                if(((*iaMapTable)[stMario->iLastYPos-1][stMario->iLastXPos] == CAISSE)&&((*iaMapTable)[stMario->iLastYPos-2][stMario->iLastXPos] == VIDE)) // Pousser une caisse
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    stMario->uiUnderMario = VIDE;
                    (*iaMapTable)[stMario->iLastYPos-1][stMario->iLastXPos] = MARIO;
                    (*iaMapTable)[stMario->iLastYPos-2][stMario->iLastXPos] = CAISSE;
                    *iPlayerAction = PUSH;
                }
                else if(((*iaMapTable)[stMario->iLastYPos-1][stMario->iLastXPos] == CAISSE)&&((*iaMapTable)[stMario->iLastYPos-2][stMario->iLastXPos] == OBJECTIF)) // Pousser une caisse sur un objectif
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    stMario->uiUnderMario = VIDE;
                    (*iaMapTable)[stMario->iLastYPos-1][stMario->iLastXPos] = MARIO;
                    (*iaMapTable)[stMario->iLastYPos-2][stMario->iLastXPos] = CAISSE_OK;
                    *iPlayerAction = GOAL;
                }
                else if(((*iaMapTable)[stMario->iLastYPos-1][stMario->iLastXPos] == CAISSE_OK)&&((*iaMapTable)[stMario->iLastYPos-2][stMario->iLastXPos] == VIDE)) // Pousser une caisse
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    (*iaMapTable)[stMario->iLastYPos-1][stMario->iLastXPos] = MARIO;
                    stMario->uiUnderMario = OBJECTIF;
                    (*iaMapTable)[stMario->iLastYPos-2][stMario->iLastXPos] = CAISSE;
                    *iPlayerAction = PUSH;
                }
                else if(((*iaMapTable)[stMario->iLastYPos-1][stMario->iLastXPos] == CAISSE_OK)&&((*iaMapTable)[stMario->iLastYPos-2][stMario->iLastXPos] == OBJECTIF)) // Pousser une caisse sur un objectif
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    (*iaMapTable)[stMario->iLastYPos-1][stMario->iLastXPos] = MARIO;
                    stMario->uiUnderMario = OBJECTIF;
                    (*iaMapTable)[stMario->iLastYPos-2][stMario->iLastXPos] = CAISSE_OK;
                    *iPlayerAction = GOAL;
                }
            }
        }
        stMario->uiBodyPosition = HAUT;
        stMario->uiStatus = BUSY;
        break;
    case SDLK_LEFT:
        if(stMario->iLastXPos-1 >= 0) // Limitation de la carte
        {
            if((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-1] == VIDE || (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-1] == OBJECTIF) // Déplacement simple
            {
                (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                stMario->uiUnderMario = (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-1];
                (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-1] = MARIO;
                *iPlayerAction = WALK;
            }
            else if(stMario->iLastXPos-2 >= 0)
            {
                if(((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-1] == CAISSE)&&((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-2] == VIDE)) // Pousser une caisse
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    stMario->uiUnderMario = VIDE;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-1] = MARIO;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-2] = CAISSE;
                    *iPlayerAction = PUSH;
                }
                else if(((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-1] == CAISSE)&&((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-2] == OBJECTIF)) // Pousser une caisse sur un objectif
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    stMario->uiUnderMario = VIDE;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-1] = MARIO;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-2] = CAISSE_OK;
                    *iPlayerAction = GOAL;
                }
                else if(((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-1] == CAISSE_OK)&&((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-2] == VIDE)) // Pousser une caisse
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-1] = MARIO;
                    stMario->uiUnderMario = OBJECTIF;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-2] = CAISSE;
                    *iPlayerAction = PUSH;
                }
                else if(((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-1] == CAISSE_OK)&&((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-2] == OBJECTIF)) // Pousser une caisse sur un objectif
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-1] = MARIO;
                    stMario->uiUnderMario = OBJECTIF;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos-2] = CAISSE_OK;
                    *iPlayerAction = GOAL;
                }
            }
        }
        stMario->uiBodyPosition = GAUCHE;
        stMario->uiStatus = BUSY;
        break;
    case SDLK_RIGHT:
        if(stMario->iLastXPos+1 < uiWidthOfMap) // Limitation de la carte
        {
            if((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+1] == VIDE || (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+1] == OBJECTIF) // Déplacement simple
            {
                (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                stMario->uiUnderMario = (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+1];
                (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+1] = MARIO;
                *iPlayerAction = WALK;
            }
            else if(stMario->iLastXPos+2 < uiWidthOfMap)
            {
                if(((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+1] == CAISSE)&&((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+2] == VIDE)) // Pousser une caisse
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    stMario->uiUnderMario = VIDE;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+1] = MARIO;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+2] = CAISSE;
                    *iPlayerAction = PUSH;
                }
                else if(((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+1] == CAISSE)&&((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+2] == OBJECTIF)) // Pousser une caisse sur un objectif
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    stMario->uiUnderMario = VIDE;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+1] = MARIO;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+2] = CAISSE_OK;
                    *iPlayerAction = GOAL;
                }
                else if(((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+1] == CAISSE_OK)&&((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+2] == VIDE)) // Pousser une caisse
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+1] = MARIO;
                    stMario->uiUnderMario = OBJECTIF;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+2] = CAISSE;
                    *iPlayerAction = PUSH;
                }
                else if(((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+1] == CAISSE_OK)&&((*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+2] == OBJECTIF)) // Pousser une caisse sur un objectif
                {
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos] = stMario->uiUnderMario;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+1] = MARIO;
                    stMario->uiUnderMario = OBJECTIF;
                    (*iaMapTable)[stMario->iLastYPos][stMario->iLastXPos+2] = CAISSE_OK;
                    *iPlayerAction = GOAL;
                }
            }
        }
        stMario->uiBodyPosition = DROITE;
        stMario->uiStatus = BUSY;
        break;
    }
    return 0;
}

int WinningScreen (SDL_Surface* sdlScreen, unsigned int uiWidthOfMap, unsigned int uiHeightOfMap, unsigned int uiPastTime)
{

    // Création des variables liées au son du jeu
    FMOD_SYSTEM *fmodSystem;
    FMOD_SOUND *fmodSuccess = NULL;

    stCharacter stMario;
    int iEndOfTheGame = 0;
    SDL_Rect sdlItemLocation;
    SDL_Event event;
    TTF_Font *ttfMsgFont = NULL;
    char pcWonMsg[100] = "";

    SDL_Color BlackColor = {0, 0, 0};
    SDL_Color GreenColor = {0, 255, 0};

    stMario.uiBodyPosition = BAS;

     // initialize SDL TTF library
    if ( TTF_Init() == -1 )
    {
        fprintf(stderr, "Unable to init SDL TTF library: %s\n", TTF_GetError() );
        return -1;
    }

    // make sure SDL TTF cleans up before exit
    atexit(TTF_Quit);

    // create font for text screening
    ttfMsgFont = TTF_OpenFont("lucon.ttf", 22);
    if (ttfMsgFont == NULL)
    {
        fprintf(stderr,"Unable to load Lucida Console font: %s\n", TTF_GetError());
        return -2;
    }

    // Initialisation de la bibliothèque FMOD
    FMOD_System_Create(&fmodSystem);
    if (FMOD_OK != FMOD_System_Init(fmodSystem, 32, FMOD_INIT_NORMAL, NULL))
    {
        fprintf(stderr,"Unable to init FMOD library.\n");
        return -3;
    }

    // Chargement du son
    if (FMOD_OK != FMOD_System_CreateSound(fmodSystem, "Resources\\success.wav", FMOD_CREATESAMPLE, NULL, &fmodSuccess))
    {
        fprintf(stderr,"Unable to load sound success.wav.\n");
        return -4;
    }

    // Surface informative
    sprintf (pcWonMsg, "You won in %d sec !!", uiPastTime);
    SDL_Surface* sdlCongratsMsg = TTF_RenderText_Shaded(ttfMsgFont, pcWonMsg, GreenColor, BlackColor);
    SDL_Surface* sdlQuitMsg = TTF_RenderText_Shaded(ttfMsgFont, "Press Esc to quit", GreenColor, BlackColor);
    // Surfaces Mario
    SDL_Surface* sdlMarioDown = IMG_Load("Resources\\mario_bas.gif");
    SDL_Surface* sdlMarioUp = IMG_Load("Resources\\mario_haut.gif");
    SDL_Surface* sdlMarioLeft = IMG_Load("Resources\\mario_gauche.gif");
    SDL_Surface* sdlMarioRight = IMG_Load("Resources\\mario_droite.gif");

    // Joue le son
    FMOD_System_PlaySound(fmodSystem, FMOD_CHANNEL_FREE, fmodSuccess, 0, NULL);

    // Winner screen
    while (!iEndOfTheGame)
    {
        // message processing loop
        while (SDL_PollEvent(&event))
        {
            // check for messages
            switch (event.type)
            {

            // exit if the window is closed
            case SDL_QUIT:
                iEndOfTheGame = 1;
                break;

            // exit if the window is closed
            case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_ESCAPE) // exit if ESCAPE is pressed
                    iEndOfTheGame = 1;
                break;
            } // end switch
        } // end of message processing

        // DRAWING STARTS HERE

        // clear screen
        SDL_FillRect(sdlScreen, 0, SDL_MapRGB(sdlScreen->format, 0, 0, 0));

        // Ajout des messages sur l'écran
        sdlItemLocation.x = ((uiWidthOfMap * BLOC_SIZE) / 2) - (sdlCongratsMsg->w / 2);
        sdlItemLocation.y = ((uiHeightOfMap * BLOC_SIZE) / 2) - BLOC_SIZE - 20;
        SDL_BlitSurface(sdlCongratsMsg, 0, sdlScreen, &sdlItemLocation);
        sdlItemLocation.x = ((uiWidthOfMap * BLOC_SIZE) / 2) - (sdlQuitMsg->w / 2);
        sdlItemLocation.y = ((uiHeightOfMap * BLOC_SIZE) / 2) + BLOC_SIZE;
        SDL_BlitSurface(sdlQuitMsg, 0, sdlScreen, &sdlItemLocation);

        // Rotation de mario au milieu de l'écran
        stMario.sdlMarioLocation.x = ((uiWidthOfMap * BLOC_SIZE) / 2) - (BLOC_SIZE / 2);
        stMario.sdlMarioLocation.y = ((uiHeightOfMap * BLOC_SIZE) / 2) - (BLOC_SIZE / 2);

        switch(stMario.uiBodyPosition)
        {
        case HAUT:
            SDL_BlitSurface(sdlMarioUp, 0, sdlScreen, &stMario.sdlMarioLocation);
            stMario.uiBodyPosition = DROITE;
            break;
        case BAS:
            SDL_BlitSurface(sdlMarioDown, 0, sdlScreen, &stMario.sdlMarioLocation);
            stMario.uiBodyPosition = GAUCHE;
            break;
        case GAUCHE:
            SDL_BlitSurface(sdlMarioLeft, 0, sdlScreen, &stMario.sdlMarioLocation);
            stMario.uiBodyPosition = HAUT;
            break;
        case DROITE:
            SDL_BlitSurface(sdlMarioRight, 0, sdlScreen, &stMario.sdlMarioLocation);
            stMario.uiBodyPosition = BAS;
            break;
        }
        // DRAWING ENDS HERE

        SDL_Delay(100);

        // finally, update the screen :)
        SDL_Flip(sdlScreen);
    } // end game loop

    SDL_FreeSurface(sdlCongratsMsg);
    SDL_FreeSurface(sdlQuitMsg);
    SDL_FreeSurface(sdlMarioUp);
    SDL_FreeSurface(sdlMarioDown);
    SDL_FreeSurface(sdlMarioLeft);
    SDL_FreeSurface(sdlMarioRight);

    // free loaded font
    TTF_CloseFont(ttfMsgFont);
    TTF_Quit();

    // free loaded sound
    FMOD_Sound_Release(fmodSuccess);
    FMOD_System_Close(fmodSystem);
    FMOD_System_Release(fmodSystem);

    return 0;
}
