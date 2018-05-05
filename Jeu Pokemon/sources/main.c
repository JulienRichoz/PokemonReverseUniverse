//main.c
//Auteur: Julien Richoz
//Date: 04.03.2016
//Affiche le menu + chargement des éléments de base SDL (fenêtre)
//Version 1.0

#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>

#include "constant.h"
#include "game.h"
#include "files.h"

void pause();

int main(int argc, char *argv[])
{

    SDL_Surface *ecran=NULL, *accueil=NULL, *construction=NULL, *regles=NULL; //Pointeur qui va stocker la surface des images
    SDL_Rect positionAccueil; //Structure possédant des coordonées pour définir la position de l'image accueil
    SDL_Event event; //La variable contenant l'événement
    int pause=1; //"Mettre le jeu en pause", permettant d'afficher le contenu au lieu qu'il s'affiche quelques ms et disparaisse

    SDL_Init(SDL_INIT_VIDEO);

    ecran=SDL_SetVideoMode(LARGEUR_FENETRE, HAUTEUR_FENETRE, 32, SDL_HWSURFACE | SDL_DOUBLEBUF); //On tente d'ouvrir une fenetre, le doublebuf permet d'éviter des scintillements de l'écran
    if(ecran==NULL){ //Si l'ouverture echoue, on le note et on arrête
        fprintf(stderr, "Impossible de charger le mode video : &s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_WM_SetCaption("Pokemon Reverse Universe", NULL); //Donner un titre a la fenetre
    accueil=SDL_LoadBMP("data/ImageAccueil.bmp"); //Charger l'image d'accueil
    construction=SDL_LoadBMP("data/editeurNonDisponible.bmp");//Charger image editeur
    regles=SDL_LoadBMP("data/regles.bmp"); //Charger image regle

    //définir les cordonnées de l'image d'accueil
    positionAccueil.x = 0;
    positionAccueil.y = 0;

    SDL_BlitSurface(accueil, NULL, ecran, &positionAccueil);//On colle l'image d'accueil à l'ecran
    SDL_Flip(ecran); //On met à jour l'ecran

    //Gestion événements
    while (pause){
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            //Si on clique sur la croix on ferme le programme
            case SDL_QUIT:
                pause = 0;
            break;

            //Gérer évenement clavier
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    //Jouer au jeu
                    case SDLK_KP1: // Si on appuie sur 1
                        play(ecran);
                    break;

                    //Editeur de niveau
                    case SDLK_KP2: //Si on appuie sur 2
                        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 55, 225, 22));
                        SDL_BlitSurface(construction, NULL, ecran, &positionAccueil);
                        SDL_Flip(ecran);
                    break;

                    //Afficher les règles
                    case SDLK_KP3: //Si on appuie sur 3
                        SDL_BlitSurface(regles, NULL, ecran, &positionAccueil);
                        SDL_Flip(ecran);
                    break;

                    default:
                    break;
                }
            break;
        }
    SDL_Flip(ecran);
    }
    // On efface l'écran
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));
    SDL_BlitSurface(accueil, NULL, ecran, &positionAccueil);
    SDL_Flip(ecran);
    //On vide la mémoire
    SDL_FreeSurface(accueil);
    SDL_FreeSurface(regles);
    SDL_FreeSurface(construction);
    SDL_QUIT;

}
