//constant.h
//Auteur: Julien Richoz
//Date: 04.03.2016
//Fichier qui détermine des constantes pour le programme
//Version 1.0

#ifndef DEF_CONSTANT //éviter les inclusions infinies
#define DEF_CONSTANT
    #define DIMENSION_CASE  100 //Une case fera 100 pixel par 100 pixel

    //Le jeu contiendra 14 * 14 cases
    #define NB_CASE_HAUTEUR 10
    #define NB_CASE_LARGEUR 10

    //Déterminer la dimension de la fenêtre du jeu
    #define HAUTEUR_FENETRE DIMENSION_CASE * NB_CASE_HAUTEUR
    #define LARGEUR_FENETRE DIMENSION_CASE * NB_CASE_LARGEUR

    //Nombre de niveau
    #define MAX_LEVEL 3
    //Les enumérations permettent de différencier les variables en leur attribuant des valeurs différentes
    enum{UP,  DOWN, LEFT, RIGHT};
    enum{CHEMIN, ROCHER, ARBRE, OBJECTIF, PORTE, PORTEOUVERTE, PIKACHU}; //CHEMIN vaut 0, ROCHER 1, MUR 2, etc.

#endif // DEF_CONSTANT
