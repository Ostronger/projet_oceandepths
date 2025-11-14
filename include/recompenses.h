// Module Récompenses — distribution des perles et loot après combat
#ifndef RECOMPENSES_H
#define RECOMPENSES_H

#include "joueur.h"
#include "inventaire.h"
#include "creatures.h"
#include "ui.h"

void distribuer_recompenses(Plongeur *plongeur,
                            Inventaire *inventaire,
                            const CreatureMarine *groupe,
                            int nb_creatures,
                            int profondeur);

#endif // RECOMPENSES_H

