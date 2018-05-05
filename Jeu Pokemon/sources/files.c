//files.c
//Auteur: Julien Richoz
//Date: 04.03.2016
//Gère les différents niveaux (lecture de fichiers, comptage d'objectifs)
//Version 1.0

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL/SDL.h>

#include "constant.h"
#include "files.h"
#include "game.h"

int chargerNiveau(int niveau, int level[][NB_CASE_HAUTEUR])
{
    FILE* fichier = NULL;
    char ligneFichier[NB_CASE_LARGEUR * NB_CASE_HAUTEUR + 1] = {0}; //On créé un tableau pour stocker les valeurs du fichier
    int i = 0, j = 0, objectifRestant=0;
    char t;
    char buf[0x100];

    //Pris sur internet; permet d'ouvrir un fichier avec le nom d'une variable dedans
    snprintf(buf, sizeof(buf), "level%d.lvl", niveau);
    fichier = fopen(buf, "r");
    if (fichier == NULL) return 0;

    //On lit la ligne que l'on stock dans lignefichier
    fgets(ligneFichier, NB_CASE_LARGEUR * NB_CASE_HAUTEUR + 1, fichier);
    //On analyse le contenu de ligneFichier (les 12 premiers caractères représentent la première ligne, puis la 2eme etc.)
    for (i = 0 ; i < NB_CASE_LARGEUR ; i++)
    {
        for (j = 0 ; j < NB_CASE_HAUTEUR ; j++)
        {
            char t=ligneFichier[i * NB_CASE_LARGEUR + j];
            level[j][i]=t-'0';
            if(level[j][i]==3) objectifRestant++;

        }
    }
    fclose(fichier);

    return objectifRestant;
}

