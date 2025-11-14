#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include "creatures.h"

#define MAP_WIDTH (1 + CREATURES_MAX)
#define MAP_HEIGHT 6
#define MAX_DEPTH 500 // profondeur maximale de la carte

typedef struct {
    int profondeur; // profondeur en mètres
} Cell;

typedef struct {
    Cell grille[MAP_HEIGHT][MAP_WIDTH]; // grille de cellules
} Map;

void map_init(Map *map); // initialise la carte avec des profondeurs aléatoires
void map_print(const Map *map,
               int ligne_plongeur,
               CreatureMarine groupes[][CREATURES_MAX],
               const int groupes_counts[],
               const int groupes_inities[],
               const int progression_positions[]); // affiche la carte en situant le plongeur et les créatures
int map_get_depth(const Map *map, int x, int y); // retourne la profondeur à la position (x, y)

#endif
