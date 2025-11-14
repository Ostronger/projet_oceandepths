// Module Inventaire — gestion des objets et �quipements
#ifndef INVENTAIRE_H
#define INVENTAIRE_H

#include "joueur.h"

#define INVENTAIRE_TAILLE 8

typedef enum {
    OBJET_NONE = 0,
    OBJET_CAPSULE_O2,
    OBJET_TROUSSE_SOIN,
    OBJET_STIMULANT,
    OBJET_ANTIDOTE
} TypeObjet;

typedef enum {
    HARPOON_RUSTIQUE = 0,
    HARPOON_STANDARD,
    HARPOON_ELECTRIQUE
} TypeHarpon;

typedef enum {
    COMBI_NEOPRENE = 0,
    COMBI_RENFORCEE,
    COMBI_TITANIUM
} TypeCombinaison;

typedef struct {
    TypeObjet type;
    int quantite;
} SlotObjet;

typedef struct {
    SlotObjet slots[INVENTAIRE_TAILLE];
    TypeHarpon harpon_equipe;
    TypeCombinaison combinaison_equipee;
} Inventaire;

void inventaire_init(Inventaire *inv, Plongeur *plongeur);
int inventaire_ajoute_objet(Inventaire *inv, TypeObjet objet, int quantite);
int inventaire_utilise_objet(Inventaire *inv, Plongeur *plongeur, int index_slot);
void inventaire_affiche(const Inventaire *inv);
int inventaire_equipe_harpon(Inventaire *inv, TypeHarpon harpon, Plongeur *plongeur);
int inventaire_equipe_combi(Inventaire *inv, TypeCombinaison combi, Plongeur *plongeur);
const char *inventaire_nom_objet(TypeObjet objet);
const char *inventaire_nom_harpon(TypeHarpon harpon);
const char *inventaire_nom_combinaison(TypeCombinaison combi);
void inventaire_actualise_equipement(Inventaire *inv, Plongeur *plongeur);

#endif // INVENTAIRE_H

