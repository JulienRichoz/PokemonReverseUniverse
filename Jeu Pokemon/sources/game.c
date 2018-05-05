//game.c
//Auteur: Julien Richoz
//Date: 04.03.2016
//base du jeu, permettant d'afficher le décor, se déplacer avec le personnage, gestion de collisions etc.
//Version 1.0

#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>


#include "constant.h"
#include "game.h"
#include "files.h"
int niveauActuel=1;

void play(SDL_Surface* ecran){
    //Réserver les adresses mémoires pour les éléments graphiques (décor/personnage)
    SDL_Surface *chemin=NULL, *rocher=NULL, *arbre=NULL, *objectif=NULL, *porte=NULL, *porteOuverte=NULL, *pikachuActuel=NULL, *victoire=NULL;
    SDL_Surface *pikachu[4]={NULL}; //
    SDL_Rect positionDecor, positionPikachu, positionInitiale;
    SDL_Event event; //gérer les évenements
    int posx=0, posy=0;
    int pause=1, ligne=0, colonne=0, generation=0; //Ligne et colonne permettront de parcouir la map
    int map[NB_CASE_LARGEUR][NB_CASE_HAUTEUR] = {0}; //Tableaux deux dimensions pour générer la carte
    int obstacle=0, objectifRestant=0;
    int posLigne=0, posColonne=0;

    //CHARGEMENT DES IMAGES
    victoire = SDL_LoadBMP("data/victoire.bmp");
    chemin = SDL_LoadBMP("data/chemin.bmp");
    rocher = SDL_LoadBMP("data/rocher.bmp");
    arbre = SDL_LoadBMP("data/arbre.bmp");
    objectif = SDL_LoadBMP("data/pokeball.bmp");
    porte = SDL_LoadBMP("data/porte.bmp");
    porteOuverte = SDL_LoadBMP("data/porteOuverte.bmp");
    pikachu[UP] = SDL_LoadBMP("data/pikachu_back_simple.bmp");
    pikachu[DOWN] = SDL_LoadBMP("data/pikachu_front_simple.bmp");
    pikachu[LEFT] = SDL_LoadBMP("data/pikachu_left_simple.bmp");
    pikachu[RIGHT] = SDL_LoadBMP("data/pikachu_right_simple.bmp");
    pikachuActuel = pikachu[DOWN]; //Rendre le positionnement par défaut qui regarde en bas

    //Rendre les images au fond transparent
    SDL_SetColorKey(chemin, SDL_SRCCOLORKEY, SDL_MapRGB(chemin->format, 0, 255, 0));
    SDL_SetColorKey(rocher, SDL_SRCCOLORKEY, SDL_MapRGB(rocher->format, 0, 0, 255));
    SDL_SetColorKey(arbre, SDL_SRCCOLORKEY, SDL_MapRGB(arbre->format, 0, 0, 255));
    SDL_SetColorKey(objectif, SDL_SRCCOLORKEY, SDL_MapRGB(objectif->format, 0, 255, 0));
    SDL_SetColorKey(porte, SDL_SRCCOLORKEY, SDL_MapRGB(porte->format, 0, 255, 0));
    SDL_SetColorKey(porteOuverte, SDL_SRCCOLORKEY, SDL_MapRGB(porteOuverte->format, 0, 255, 0));
    SDL_SetColorKey(pikachu[UP], SDL_SRCCOLORKEY, SDL_MapRGB(pikachu[UP]->format, 0, 255, 0));
    SDL_SetColorKey(pikachu[DOWN], SDL_SRCCOLORKEY, SDL_MapRGB(pikachu[DOWN]->format, 0, 255, 0));
    SDL_SetColorKey(pikachu[LEFT], SDL_SRCCOLORKEY, SDL_MapRGB(pikachu[LEFT]->format, 0, 255, 0));
    SDL_SetColorKey(pikachu[RIGHT], SDL_SRCCOLORKEY, SDL_MapRGB(pikachu[RIGHT]->format, 0, 255, 0));

    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 55, 225, 22)); //afficher fond d'ecran meme couleur que chemin
    objectifRestant=chargerNiveau(niveauActuel, map);

    //Placement des objets
    for(ligne=0; ligne<NB_CASE_LARGEUR; ligne++){
        for(colonne=0; colonne<NB_CASE_HAUTEUR; colonne++){
            positionDecor.x = colonne * DIMENSION_CASE;
            positionDecor.y = ligne * DIMENSION_CASE;
            switch(map[colonne][ligne]){
                    case 0: //chemin
                        SDL_BlitSurface(chemin, NULL, ecran, &positionDecor);
                    break;
                    case 1: //rocher
                        SDL_BlitSurface(rocher, NULL, ecran, &positionDecor);
                    break;
                    case 2: //arbre
                        SDL_BlitSurface(arbre, NULL, ecran, &positionDecor);
                    break;
                    case 3: //objectif
                        SDL_BlitSurface(objectif, NULL, ecran, &positionDecor);
                    break;
                    case 4: //porte
                        SDL_BlitSurface(porte, NULL, ecran, &positionDecor);
                    break;
                    case 5: //porteOuverte
                        SDL_BlitSurface(porteOuverte, NULL, ecran, &positionDecor);
                    break;
                    case 6: //Pikachu
                        SDL_BlitSurface(pikachu[DOWN], NULL, ecran, &positionDecor);
                        positionPikachu=positionDecor; //On attribue directement la position de pikachu d'ou il sera mis
                        positionInitiale=positionPikachu;
                        posLigne=positionPikachu.y/DIMENSION_CASE;
                        posColonne=positionPikachu.x/DIMENSION_CASE;
                    break;
            }
        }
    }

    //On met à jour l'ecran
    SDL_Flip(ecran);

    //Simplifier calcul via la map de pikachu pour calculer en fonction des points de la carte et non des pixels
    posx=positionPikachu.x/DIMENSION_CASE;
    posy=positionPikachu.y/DIMENSION_CASE;

//----------------------DEPLACER JOUEUR/gestion niveau-------------------------------------
    SDL_EnableKeyRepeat(150,150); //Repetition de touche avec la durée de l'appui de la touche et le délai entre chaque génération d'un événement
    while(pause){

        //Si fini, affiche une page de victoire
        if(niveauActuel>MAX_LEVEL){
            positionDecor.x=0;
            positionDecor.y=0;
            SDL_BlitSurface(victoire, NULL, ecran, &positionDecor);
            SDL_Flip(ecran);
            //On attend 3 secondes et on quitte le programme
            SDL_Delay(3000);
            break;
        }
        SDL_WaitEvent(&event);
        switch(event.type){
            //Si on clique sur la croix on ferme le programme
            case SDL_QUIT:
                pause = 0;
            break;

            //Gérer évenement clavier
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){

                    //----------------------------FLECHE HAUT-------------------------------
                    case SDLK_UP:
                        //Si un rocher est devant pikachu et qu'il n'y rien devant le rocher, alors il peut pousser le rocher
                        if(map[posx][posy-1]==ROCHER && map[posx][posy-2]==CHEMIN){
                            map[posx][posy-2]=ROCHER;
                            map[posx][posy-1]=PIKACHU;
                            positionDecor.x=positionPikachu.x;
                            positionDecor.y=positionPikachu.y-2*DIMENSION_CASE;
                            SDL_BlitSurface(rocher, NULL, ecran, &positionDecor);
                        }
                        //Suite test obstacle rocher
                        if(map[posx][posy-1]==ROCHER && map[posx][posy-2]!=CHEMIN){
                            break;
                        }

                        //S'il rencontre un bord d'écran, arbre ou une porte, le personnage ne peut plus avancer
                        if(map[posx][posy-1]==ARBRE || posy-1<0 || map[posx][posy-1]==PORTE){
                        }

                        //Sinon on le fait aller dans la direction souhaitée
                        else{
                            //Recupération pokeball
                            if(map[posx][posy-1]==OBJECTIF){
                                objectifRestant--;
                                map[posx][posy-1]=CHEMIN;
                            }

                            //S'il rencontre une porte ouverte, il passe au niveau suivant (le même pour l'instant)
                            if(map[posx][posy-1]==PORTEOUVERTE){
                                niveauActuel++;
                                play(ecran);
                            }
                            SDL_BlitSurface(chemin, NULL, ecran, &positionPikachu);
                            map[posx][posy]=CHEMIN;
                            positionPikachu.y-=DIMENSION_CASE;
                            SDL_BlitSurface(chemin, NULL, ecran, &positionPikachu);
                            SDL_BlitSurface(pikachu[UP], NULL, ecran, &positionPikachu);
                        }
                        break;

                    //-------------------------------FLECHE BAS-----------------------------------
                    case SDLK_DOWN:

                        //Si un rocher est devant pikachu et qu'il n'y rien devant le rocher, alors il peut pousser le rocher
                        if(map[posx][posy+1]==ROCHER && map[posx][posy+2]==CHEMIN){
                            map[posx][posy+2]=ROCHER;
                            map[posx][posy+1]=PIKACHU;
                            positionDecor.x=positionPikachu.x;
                            positionDecor.y=positionPikachu.y+2*DIMENSION_CASE;
                            SDL_BlitSurface(rocher, NULL, ecran, &positionDecor);
                        }
                        //Test obstacle-ne rien faire si on ne peut pas avancer
                        if(map[posx][posy+1]==ROCHER && map[posx][posy+2]!=CHEMIN){
                            break;
                        }

                        //S'il rencontre un bord d'écran, arbre ou une porte, le personnage ne peut plus avancer
                        if(map[posx][posy+1]==ARBRE || posy+1>=10 || map[posx][posy+1]==PORTE){
                        }

                        //Sinon on le fait aller dans la direction souhaitée
                        else{
                            //Recupération pokeball
                            if(map[posx][posy+1]==OBJECTIF){
                                objectifRestant--;
                                map[posx][posy+1]=CHEMIN;
                            }

                            //S'il rencontre une porte ouverte, il passe au niveau suivant (le même pour l'instant)
                            if(map[posx][posy+1]==PORTEOUVERTE){
                                niveauActuel++;
                                play(ecran);
                            }
                            SDL_BlitSurface(chemin, NULL, ecran, &positionPikachu);
                            map[posx][posy]=CHEMIN;
                            positionPikachu.y+=DIMENSION_CASE;
                            SDL_BlitSurface(chemin, NULL, ecran, &positionPikachu);
                            SDL_BlitSurface(pikachu[DOWN], NULL, ecran, &positionPikachu);
                        }
                        break;

                    //------------------------------------------FLECHE DROITE----------------------------------------------------
                    case SDLK_RIGHT:

                        //Si un rocher est devant pikachu et qu'il n'y rien devant le rocher, alors il peut pousser le rocher
                        if(map[posx+1][posy]==ROCHER && map[posx+2][posy]==CHEMIN){
                            map[posx+2][posy]=ROCHER;
                            map[posx+1][posy]=PIKACHU;
                            positionDecor.x=positionPikachu.x+2*DIMENSION_CASE;
                            positionDecor.y=positionPikachu.y;
                            SDL_BlitSurface(rocher, NULL, ecran, &positionDecor);
                        }
                        //Test obstacle-ne rien faire si on ne peut pas avancer
                        if(map[posx+1][posy]==ROCHER && map[posx+2][posy]!= CHEMIN){
                            break;
                        }
                        //S'il rencontre un bord d'écran, arbre ou une porte, le personnage ne peut plus avancer
                        if(map[posx+1][posy]==ARBRE || posx+1>=10 || map[posx+1][posy]==PORTE){
                        }

                        //Sinon on le fait aller dans la direction souhaitée
                        else{
                            //Recupération pokeball
                            if(map[posx+1][posy]==OBJECTIF){
                                objectifRestant--;
                                map[posx+1][posy]=CHEMIN;
                            }

                            //S'il rencontre une porte ouverte, il passe au niveau suivant (le même pour l'instant)
                            if(map[posx+1][posy]==PORTEOUVERTE){
                                niveauActuel++;
                                play(ecran);

                            }
                            SDL_BlitSurface(chemin, NULL, ecran, &positionPikachu);
                            map[posx][posy]=CHEMIN;
                            positionPikachu.x+=DIMENSION_CASE;
                            SDL_BlitSurface(chemin, NULL, ecran, &positionPikachu);
                            SDL_BlitSurface(pikachu[RIGHT], NULL, ecran, &positionPikachu);
                        }
                    break;

                    //----------------------FLECHE GAUCHE----------------------------------------
                    case SDLK_LEFT:
                        //Si un rocher est devant pikachu et qu'il n'y rien devant le rocher, alors il peut pousser le rocher
                        if(map[posx-1][posy]==ROCHER && map[posx-2][posy]==CHEMIN){
                            map[posx-2][posy]=ROCHER;
                            map[posx-1][posy]=PIKACHU;
                            positionDecor.x=positionPikachu.x-2*DIMENSION_CASE;
                            positionDecor.y=positionPikachu.y;
                            SDL_BlitSurface(rocher, NULL, ecran, &positionDecor);
                        }
                        //Test obstacle-ne rien faire si on ne peut pas avancer
                        if(map[posx-1][posy]==ROCHER && map[posx-2][posy]!= CHEMIN){
                            break;
                        }
                        //S'il rencontre un bord d'écran, arbre ou une porte, le personnage ne peut plus avancer
                        if(map[posx-1][posy]==ARBRE || posx-1>=10 || map[posx-1][posy]==PORTE){
                        }

                        //Sinon on le fait aller dans la direction souhaitée
                        else{
                            //Recupération pokeball
                            if(map[posx-1][posy]==OBJECTIF){
                                objectifRestant--;
                                map[posx-1][posy]=CHEMIN;
                            }

                            //S'il rencontre une porte ouverte, il passe au niveau suivant (le même pour l'instant)
                            if(map[posx-1][posy]==PORTEOUVERTE){
                                niveauActuel++;
                                play(ecran);
                            }
                            SDL_BlitSurface(chemin, NULL, ecran, &positionPikachu);
                            map[posx][posy]=CHEMIN;
                            positionPikachu.x-=DIMENSION_CASE;
                            SDL_BlitSurface(chemin, NULL, ecran, &positionPikachu);
                            SDL_BlitSurface(pikachu[LEFT], NULL, ecran, &positionPikachu);
                        }
                    break;

                    //Reset le niveau
                    case SDLK_r:
                        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 55, 225, 22)); //afficher ecran blanc
                        objectifRestant=chargerNiveau(niveauActuel, map);
                        SDL_WM_SetCaption("RESET", NULL);
                        //Placement des objets
                        SDL_BlitSurface(chemin, NULL, ecran, &positionPikachu);
                        for(colonne=0; colonne<NB_CASE_LARGEUR; colonne++){
                            for(ligne=0; ligne<NB_CASE_HAUTEUR; ligne++){
                                positionDecor.x = colonne * DIMENSION_CASE;
                                positionDecor.y = ligne * DIMENSION_CASE;
                                switch(map[colonne][ligne]){
                                    case 0: //chemin
                                        SDL_BlitSurface(chemin, NULL, ecran, &positionDecor);
                                    break;
                                    case 1: //rocher
                                        SDL_BlitSurface(rocher, NULL, ecran, &positionDecor);
                                    break;
                                    case 2: //arbre
                                        SDL_BlitSurface(arbre, NULL, ecran, &positionDecor);
                                    break;
                                    case 3: //objectif
                                        SDL_BlitSurface(objectif, NULL, ecran, &positionDecor);
                                    break;
                                    case 4: //porte
                                        SDL_BlitSurface(porte, NULL, ecran, &positionDecor);
                                    break;
                                    case 5:
                                        SDL_BlitSurface(porteOuverte, NULL, ecran, &positionDecor);
                                    break;
                                    case 6:
                                        SDL_BlitSurface(pikachu[DOWN], NULL, ecran, &positionDecor);
                                        positionPikachu=positionInitiale;
                                    break;
                                } //fin switch(map[colonne][ligne])
                            } //fin boucle for ligne
                        } //fin boucle for colonne
                    break; //break SDLK_r
                }//fin switch(event.key.keysym.sym)

                //On met a jour la posx/y
                posx=positionPikachu.x/DIMENSION_CASE;
                posy=positionPikachu.y/DIMENSION_CASE;

                //On ouvre la porte si on a recupérer tous les objectifs
                if(objectifRestant==0){
                    //Parcours la map pour trouver la porte fermée et l'ouvrir
                    for(colonne=0; colonne<NB_CASE_LARGEUR; colonne++){
                        for(ligne=0; ligne<NB_CASE_HAUTEUR; ligne++){
                                if(map[colonne][ligne]==PORTE){
                                    positionDecor.x = colonne * DIMENSION_CASE;
                                    positionDecor.y = ligne * DIMENSION_CASE;
                                    map[colonne][ligne]=PORTEOUVERTE;
                                    SDL_BlitSurface(porteOuverte, NULL, ecran, &positionDecor);
                                }
                        }
                    }
                }


            break; //break CASE SDL_KEYDOWN
        } //fin switch(event.type)

    SDL_Flip(ecran);

    }

    //Libérer la mémoire
    SDL_EnableKeyRepeat(0,0);
    SDL_FreeSurface(pikachu[DOWN]);
    SDL_FreeSurface(pikachu[UP]);
    SDL_FreeSurface(pikachu[LEFT]);
    SDL_FreeSurface(pikachu[RIGHT]);
    SDL_FreeSurface(chemin);
    SDL_FreeSurface(rocher);
    SDL_FreeSurface(arbre);
    SDL_FreeSurface(objectif);
    SDL_FreeSurface(porte);
    SDL_FreeSurface(porteOuverte);
    SDL_FreeSurface(pikachuActuel);
    SDL_FreeSurface(PIKACHU);
    SDL_Quit();

    return EXIT_SUCCESS;
}



