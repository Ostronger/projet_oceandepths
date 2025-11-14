#include "map.h"
#include <stdio.h>

#define EMOJI_PLONGEUR "\xF0\x9F\xA4\xBF"
#define EMOJI_EAU      "\xF0\x9F\x8C\x8A"

static const char *emoji_creature(const CreatureMarine *c)
{
    if (!c) return "\xF0\x9F\x90\xA1"; // poisson
    switch (c->type)
    {
        case REQUIN:       return "\xF0\x9F\xA6\x88";
        case MEDUSE:       return "\xF0\x9F\xAA\xA8";
        case KRAKEN:       return "\xF0\x9F\x90\x99";
        case POISSON_EPEE: return "\xF0\x9F\x90\xA0";
        case CRABE_GEANT:  return "\xF0\x9F\xA6\x80";
        default:           return "\xF0\x9F\x90\xA1";
    }
}

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

int map_get_depth(const Map *map, int x, int y)
{
    if (!map) return 0;
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
        return 0;

    int d = map->grille[y][x].profondeur;
    if (d < 0) d = 0;
    if (d > MAX_DEPTH) d = MAX_DEPTH;
    return d;
}

void map_print(const Map *map,
               int ligne_plongeur,
               CreatureMarine groupes[][CREATURES_MAX],
               const int groupes_counts[],
               const int groupes_inities[],
               const int progression_positions[])
{
    (void)groupes_inities;

    if (!map) return;
    if (ligne_plongeur < 0 || ligne_plongeur >= MAP_HEIGHT)
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
            if (x == 0)
            {
                if (y == ligne_plongeur && progression == 0)
                    symbol = EMOJI_PLONGEUR;
            }
            else
            {
                int slot = x - 1;
                if (slot < nb)
                {
                    const CreatureMarine *c = &groupes[y][slot];
                    if (c->est_vivant && c->points_de_vie_actuels > 0)
                        symbol = emoji_creature(c);
                }
                if (y == ligne_plongeur && progression == x)
                    symbol = EMOJI_PLONGEUR;
            }

            if (x > 0)
                printf(" - ");
            printf("%s", symbol);
        }
        printf("\n\n");
    }
}
