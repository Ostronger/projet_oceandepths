#ifndef PLONGEUR_H
#define PLONGEUR_H

#include "creatures.h"

typedef struct {
 int points_de_vie;
 int points_de_vie_max;
 int niveau_oxygene;
 int niveau_oxygene_max;
 int niveau_fatigue; // 0 à 5
 int perles; // monnaie du jeu
} Plongeur;

// Initialisation du plongeur
void init_plongeur(Plongeur *p, int pv_max, int oxy_max);

// Affichage de l'état du plongeur
int plongeur_is_alive(const Plongeur *p);
int plongeur_use_oxygen(Plongeur *p);
void print_plongeur(const Plongeur *p);
#endif