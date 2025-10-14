#ifndef MAP_H
#define MAP_H

#include <stdio.h>

#define MAP_WIDTH 4
#define MAP_HEIGHT 4
#define MAX_DEPTH 500 // profondeur maximale de la carte
#define MAP_ICON_WATER "🌊" 

typedef struct {
    int profondeur; // profondeur en mètres
} Cell;

typedef struct {
    Cell grille [MAP_HEIGHT][MAP_WIDTH]; // grille de cellules
} Map;

void map_init(Map *map); // initialise la carte avec des profondeurs aléatoires
void map_print(const Map *map); // affiche la carte dans la console
int map_get_depth(const Map *map, int x, int y); // retourne la profondeur à la position (x, y)

#endif

