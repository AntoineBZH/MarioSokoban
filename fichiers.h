#ifndef FICHIERS_H_INCLUDED
#define FICHIERS_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <commdlg.h>

#define SIZE_TABLE_MAX 100

// Permet de charger un fichier de niveau dans un tableau 2D
int LoadMap(int ***piaMapTable, const char* sFilePath, unsigned int *puiMapWidth, unsigned int *puiMapHeight);

// Permet de sauvegarder un niveau dans un fichier
int SaveMap(int **piaMapTable, unsigned int uiMapWidth, unsigned int uiMapHeight);

// Permet d'ouvrir une fenêtre de sélection de fichier
int LevelChoice(const char *sWindowName,char *psFilePath);

#endif // FICHIERS_H_INCLUDED
