#include "plongeur.h"

void init_plongeur(Plongeur *p, int pv_max, int oxy_max)
{
    p->points_de_vie = pv_max;
    p->points_de_vie_max = pv_max;
    p->niveau_oxygene = oxy_max;
    p->niveau_oxygene_max = oxy_max;
    p->niveau_fatigue = 0;
    p->perles = 0;
}

void print_plongeur(const Plongeur *p)
{
    printf("État du plongeur :\n");
    printf("  Points de vie : %d/%d\n", p->points_de_vie, p->points_de_vie_max);
    printf("  Niveau d'oxygène : %d/%d\n", p->niveau_oxygene, p->niveau_oxygene_max);
    printf("  Niveau de fatigue : %d/5\n", p->niveau_fatigue);
    printf("  Perles : %d\n", p->perles);
}

int plongeur_is_alive(const Plongeur *p)
{
    return p->points_de_vie > 0;
}

int plongeur_use_oxygen(Plongeur *p)
{
    if (p->niveau_oxygene > 0)
    {
        p->niveau_oxygene--;
        return 1; // Utilisation réussie
    }
    return 0; // Pas d'oxygène disponible
}