#include "fichiers.h"

int LoadMap(int ***piaMapTable, const char* sFilePath, unsigned int *puiMapWidth, unsigned int *puiMapHeight)
{
    char pcExtractedLine[SIZE_TABLE_MAX] = "";
    char *pcSplitedLine = NULL;
    unsigned int uiCheckMapWidthCounter = 0, uiLineOfMapCounter = 0, uiColumOfMapCounter = 0;
    int iCheckedMapWidth = 0;
    (*puiMapWidth) = 0;
    (*puiMapHeight) = 0;

    FILE *fileLevel = fopen(sFilePath, "r");
    // Premier passage pour calculer la taille de la carte largeur x hauteur
    if (fileLevel != NULL)
    {
        while ( fgets(pcExtractedLine,SIZE_TABLE_MAX,fileLevel) != NULL )
        {
            (*puiMapHeight)++;
            uiCheckMapWidthCounter = 0;
            pcSplitedLine = strtok(pcExtractedLine,",;");
            while (pcSplitedLine != NULL)
            {
                if (!iCheckedMapWidth) // Au premier passage, on compte le nombre de colonnes
                {
                    (*puiMapWidth)++;
                    pcSplitedLine = strtok(NULL,",;");
                }
                else // Aux passages suivants, on vérifie si le nombre de colonnes est identique
                {
                    uiCheckMapWidthCounter++;
                    pcSplitedLine = strtok(NULL,",;");
                }
            }
            if((uiCheckMapWidthCounter != (*puiMapWidth)) && (iCheckedMapWidth))
            {
                fprintf(stderr, "Width of the map is not always the same\n");
                return -2;
            }
            iCheckedMapWidth = 1;
        }
    }
    else
    {
        fprintf(stderr, "Unable to open the level file\n");
        return -1;
    }

    // Allocation du tableau 2D représentant la carte
    (*piaMapTable) = (int**)malloc(sizeof(int*) * (*puiMapHeight));
    for(uiLineOfMapCounter = 0; uiLineOfMapCounter < (*puiMapHeight); uiLineOfMapCounter++)
    {
        (*piaMapTable)[uiLineOfMapCounter] = (int*)malloc(sizeof(int*) * (*puiMapWidth));
    }

    // Remplissage du tableau 2D
    rewind(fileLevel);
    uiLineOfMapCounter = 0;
    while ( fgets(pcExtractedLine,SIZE_TABLE_MAX,fileLevel) != NULL )
    {
        uiColumOfMapCounter = 0;
        pcSplitedLine = strtok(pcExtractedLine,",;");
        while (pcSplitedLine != NULL)
        {
            (*piaMapTable)[uiLineOfMapCounter][uiColumOfMapCounter]=(int)atol(pcSplitedLine);
            pcSplitedLine = strtok(NULL,",;");
            uiColumOfMapCounter++;
        }
        uiLineOfMapCounter++;
    }
    fclose(fileLevel);
    return 0;
}

int SaveMap(int **piaMapTable, unsigned int uiMapWidth, unsigned int uiMapHeight)
{
    unsigned int uiFileLine, uiFileColumn;
    char sFilePath[200] = "";

    LevelChoice("Sauvegarder le fichier niveau",sFilePath);
    FILE *fileLevel = fopen(sFilePath, "w"); // Ouverture d'un fichier texte en écriture avec écrasement
    if (fileLevel != NULL)
    {
        for(uiFileLine = 0; uiFileLine < uiMapHeight; uiFileLine++)
        {
            for(uiFileColumn = 0 ; uiFileColumn < uiMapWidth; uiFileColumn++)
            {
                // Ajoute un point virgule sauf pour la dernière colonne
                if(uiFileColumn < (uiMapWidth - 1))
                    fprintf(fileLevel,"%d;",piaMapTable[uiFileLine][uiFileColumn]);
                else
                    fprintf(fileLevel,"%d",piaMapTable[uiFileLine][uiFileColumn]);
            }
            // Ajoute un saut de ligne sauf pour la dernière ligne
            if(uiFileLine < (uiMapHeight - 1))
                fprintf(fileLevel,"%s","\n");
        }
    }
    else
    {
        fprintf(stderr, "Unable to open the level file in writing mode\n");
        return -1;
    }

    fclose(fileLevel);
    return 0;
}

int LevelChoice(const char *sWindowName,char *psFilePath)
{
    char sFilePath[200] = "";
    char szFile[200];
    OPENFILENAME oExplorer;

    // Permet de sauvegarder le répertoire courant qui est nécessaire pour le bon déroulement de l'application
    char lpBuffer[556];
    GetCurrentDirectory(sizeof(lpBuffer),lpBuffer);

    ZeroMemory( &oExplorer , sizeof( oExplorer));
	oExplorer.lStructSize = sizeof ( oExplorer );
	oExplorer.hwndOwner = NULL  ;
	oExplorer.lpstrFile = szFile ;
	oExplorer.lpstrFile[0] = '\0';
	oExplorer.nMaxFile = sizeof( szFile );
	oExplorer.lpstrFilter = "Level\0*.lvl\0All\0*.*\0";
	oExplorer.nFilterIndex =1;
	oExplorer.lpstrFileTitle = NULL ;
	oExplorer.nMaxFileTitle = 0 ;
	oExplorer.lpstrInitialDir = NULL ;
	oExplorer.Flags = OFN_PATHMUSTEXIST ;
	oExplorer.lpstrTitle = sWindowName;

	GetOpenFileName(&oExplorer);
	strcpy(psFilePath, oExplorer.lpstrFile);
	SetCurrentDirectory(lpBuffer);
	return 0;
}
