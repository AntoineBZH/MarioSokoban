#ifndef CONSTANTES_H_INCLUDED
#define CONSTANTES_H_INCLUDED

#define BLOC_SIZE           34 // Taille d'un bloc (carré) en pixels
#define CURSOR_WIDTH        300
#define CURSOR_HEIGHT       20

enum {HAUT, BAS, GAUCHE, DROITE};
enum {VIDE, MUR, CAISSE, OBJECTIF, MARIO, CAISSE_OK};
enum {READY, BUSY};
enum {NOTHING,WALK,PUSH,GOAL};

typedef struct {
    unsigned int uiSensitivity;
    unsigned int uiEditorMapWidth;
    unsigned int uiEditorMapHeight;
} stSettings;

#endif // CONSTANTES_H_INCLUDED
