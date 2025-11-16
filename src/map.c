#include "map.h"
#include <stdio.h>

#define EMOJI_PLONGEUR "\xF0\x9F\xA4\xBF"
#define EMOJI_EAU      "\xF0\x9F\x8C\x8A"


// ------------------------------------------------------
// Fonction : map_init
// Rôle : Remplit la carte avec des profondeurs initiales.
// Chaque ligne (y) représente une profondeur constante.
// ------------------------------------------------------
void map_init(Map *map)
{
    if (!map) return;
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        int profondeur = (y * MAX_DEPTH) / (MAP_HEIGHT - 1);
        for (int x = 0; x < MAP_WIDTH; x++)
            map->grille[y][x].profondeur = profondeur;
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

void map_print(const Map *map, int ligne_plongeur, CreatureMarine groupes[][CREATURES_MAX], const int groupes_counts[], const int groupes_inities[], const int progression_positions[]) // affiche la carte en situant le plongeur et les créatures
{
    (void)groupes_inities;

    if (!map) return; // sécurité si la carte est nulle
    if (ligne_plongeur < 0 || ligne_plongeur >= MAP_HEIGHT) // sécurité si hors limites
        ligne_plongeur = 0;

    printf("=== CARTOGRAPHIE OCÉANIQUE ===\n\n");
    printf("Légende : %s Plongeur | %s Océan sûr | 🦈🪼🐙🐠🦀 = créatures encore actives\n\n",
           EMOJI_PLONGEUR, EMOJI_EAU);

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        int profondeur = map->grille[y][0].profondeur;
        int nb = groupes_counts ? groupes_counts[y] : 0;
        int progression = progression_positions ? progression_positions[y] : -1;

        printf("Ligne %d (≈ %3dm) : ", y, profondeur);
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            const char *symbol = EMOJI_EAU;
            if (x == 0)  // colonne du plongeur on autorise le plongeur à etres là
            {
                if (y == ligne_plongeur && progression == 0) // si c'est la ligne du plongeur et la position 0  
                    symbol = EMOJI_PLONGEUR;
            }
            else // colonnes des créatures pour x > 0
            {
                int slot = x - 1; // décalage de 1 pour accéder aux créatures 
                if (slot < nb) // si le slot est valide
                {
                    const CreatureMarine *c = &groupes[y][slot]; // on recupère les créatures
                    if (c->est_vivant && c->points_de_vie_actuels > 0) // si la créature est vivante 
                        symbol = creature_symbol(c);
                }
                if (y == ligne_plongeur && progression == x) // si c'est la ligne du plongeur et la position x il peut y avoir le plongeur
                    symbol = EMOJI_PLONGEUR;
            }

            if (x > 0)
                printf(" - ");
            printf("%s", symbol);
        }
        printf("\n\n");
    }
}
