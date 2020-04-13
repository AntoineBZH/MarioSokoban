#include "MenuManagement.h"

int GameMenuSelection(SDLKey PressedKey, SDL_Rect *sdlCursorLocation, int *SelectedMode)
{
    switch (PressedKey)
    {
    case SDLK_DOWN:
        if(*SelectedMode < 3)
        {
            sdlCursorLocation->y += 33;
            (*SelectedMode)++;
        }
        break;
    case SDLK_UP:
        if(*SelectedMode > 1)
        {
            sdlCursorLocation->y -= 33;
            (*SelectedMode)--;
        }
        break;
    case SDLK_KP1:
        *SelectedMode = 1;
        return 1;
    case SDLK_KP2:
        *SelectedMode = 2;
        return 1;
    case SDLK_KP3:
        *SelectedMode = 3;
        return 1;
    case SDLK_1:
        *SelectedMode = 1;
        return 1;
    case SDLK_2:
        *SelectedMode = 2;
        return 1;
    case SDLK_3:
        *SelectedMode = 3;
        return 1;
    case SDLK_KP_ENTER:
        return 1;
    case SDLK_RETURN:
        return 1;
    default:
        break;
    }
    return 0;
}

int ParametersSelection(SDLKey PressedKey, int *SelectedParam, int *ParamValue)
{
    switch (PressedKey)
    {
    case SDLK_DOWN:
        if(*SelectedParam < 2)
        {
            (*SelectedParam)++;
        }
        return 1;
        break;
    case SDLK_UP:
        if(*SelectedParam > 0)
        {
            (*SelectedParam)--;
        }
        return 1;
        break;
    case SDLK_KP_PLUS:
        (*ParamValue)++;
        break;
    case SDLK_KP_MINUS:
        if(*ParamValue > 1)
        {
            (*ParamValue)--;
        }
        break;
    case SDLK_PLUS:
        (*ParamValue)++;
        break;
    case SDLK_MINUS:
        if(*ParamValue > 1)
        {
            (*ParamValue)--;
        }
        break;
    default:
        break;
    }
    return 0;
}
