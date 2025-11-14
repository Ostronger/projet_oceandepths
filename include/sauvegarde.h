#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

#include "joueur.h"
#include "inventaire.h"

int sauvegarder_partie(const char *chemin,
                       const Plongeur *plongeur,
                       const Inventaire *inventaire,
                       int ligne_actuelle,
                       const int zones_securisees[],
                       int nb_zones);

int charger_partie(const char *chemin,
                   Plongeur *plongeur,
                   Inventaire *inventaire,
                   int *ligne_actuelle,
                   int zones_securisees[],
                   int nb_zones);

#endif // SAUVEGARDE_H
