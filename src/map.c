#include "map.h"
#include <stdio.h>

// ------------------------------------------------------
// Fonction : map_init
// Rôle : Remplit la carte avec des profondeurs initiales.
// Chaque ligne (y) représente une profondeur constante.
// ------------------------------------------------------

void map_init(Map *map)
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        int profondeur = (y * MAX_DEPTH) / (MAP_HEIGHT - 1); // profondeur linéaire de 0 à MAX_DEPTH
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            map->grille[y][x].profondeur = profondeur;
        }
    }
}

// ------------------------------------------------------
// Fonction : map_get_depth
// Rôle : Retourne la profondeur de la case (x, y).
// Si les coordonnées sont hors limites, renvoie 0.
// ------------------------------------------------------
int map_get_depth(const Map *map, int x, int y)
{
    if (map == NULL)
        return 0; // sécurité

    // Hors limites → on renvoie 0 (surface). Choix simple et prévisible.
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
        return 0;

    int d = map->grille[y][x].profondeur;

    // Clamp au cas où (garantit 0..MAX_DEPTH)
    if (d < 0) d = 0;
    if (d > MAX_DEPTH) d = MAX_DEPTH;

    return d;
}


void map_print(const Map *map)
{
    printf("--- CARTOGRAPHIE OCÉANIQUE ---\n\n");

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        printf("Ligne %d (≈ %dm) : ", y, map->grille[y][0].profondeur);

        for (int x = 0; x < MAP_WIDTH; x++)
        {
            printf("%4d ", map->grille[y][x].profondeur);
        }

        printf("\n");
    }

    printf("\n");
}