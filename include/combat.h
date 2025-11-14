// Module Combat — boucle et calculs de dégâts
#ifndef COMBAT_H
#define COMBAT_H

#include "creatures.h"
#include "joueur.h"
#include "inventaire.h"

typedef struct {
    int profondeur;              // profondeur actuelle de l'affrontement
    CreatureMarine *ennemis;     // pointeur vers le tableau d'ennemis
    int nombre_ennemis;          // taille du tableau
} ContexteCombat;

typedef struct {
    int victoire;
    int defaite;
    int fuite;
    int tours_joues;
} ResultatCombat;

int combat_actions_max_joueur(const Plongeur *plongeur);
int combat_calcul_degats_joueur(const Plongeur *plongeur, const CreatureMarine *cible);
int combat_calcul_degats_creature(const CreatureMarine *attaquant, const Plongeur *plongeur);
int combat_compte_creatures_vivantes(const ContexteCombat *ctx);
void combat_applique_degats_creature(CreatureMarine *cible, int degats);
ResultatCombat combat_boucle(Plongeur *plongeur, Inventaire *inventaire, ContexteCombat *ctx);

#endif // COMBAT_H

