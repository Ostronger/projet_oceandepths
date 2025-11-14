#include "inventaire.h"
#include <stdio.h>

typedef struct {
    const char *nom;
    int attaque_min;
    int attaque_max;
    int cout_oxygene;
} DefinitionHarpon;

typedef struct {
    const char *nom;
    int defense;
    int cout_passif;
} DefinitionCombinaison;

static const DefinitionHarpon HARPOONS_DEF[] = {
    {"Harpon rouille", 12, 18, 0},
    {"Harpon standard", 16, 24, 1},
    {"Harpon electrique", 20, 30, 2}
};

static const DefinitionCombinaison COMBIS_DEF[] = {
    {"Neoprene basique", 8, 0},
    {"Combinaison renforcee", 15, 1},
    {"Armure titanium", 25, 2}
};

const char *inventaire_nom_objet(TypeObjet objet)
{
    switch (objet)
    {
        case OBJET_CAPSULE_O2:   return "Capsule d'oxygene";
        case OBJET_TROUSSE_SOIN: return "Trousse de soin";
        case OBJET_STIMULANT:    return "Stimulant marin";
        case OBJET_ANTIDOTE:     return "Antidote";
        default:                 return "Vide";
    }
}

const char *inventaire_nom_harpon(TypeHarpon harpon)
{
    if (harpon < 0 || harpon >= (int)(sizeof(HARPOONS_DEF) / sizeof(HARPOONS_DEF[0])))
        return "Inconnu";
    return HARPOONS_DEF[harpon].nom;
}

const char *inventaire_nom_combinaison(TypeCombinaison combi)
{
    if (combi < 0 || combi >= (int)(sizeof(COMBIS_DEF) / sizeof(COMBIS_DEF[0])))
        return "Inconnue";
    return COMBIS_DEF[combi].nom;
}

static void appliquer_objet(Plongeur *plongeur, TypeObjet objet)
{
    if (!plongeur) return;

    switch (objet)
    {
        case OBJET_CAPSULE_O2:
            plongeur->niveau_oxygene += 40;
            if (plongeur->niveau_oxygene > plongeur->niveau_oxygene_max)
                plongeur->niveau_oxygene = plongeur->niveau_oxygene_max;
            break;
        case OBJET_TROUSSE_SOIN:
            plongeur->points_de_vie += 25;
            if (plongeur->points_de_vie > plongeur->points_de_vie_max)
                plongeur->points_de_vie = plongeur->points_de_vie_max;
            break;
        case OBJET_STIMULANT:
            joueur_recupere_fatigue(plongeur, 2);
            break;
        case OBJET_ANTIDOTE:
            joueur_reset_statuts(plongeur);
            break;
        default:
            break;
    }
}

static void appliquer_harpon(TypeHarpon harpon, Plongeur *plongeur)
{
    if (!plongeur) return;
    if (harpon < 0 || harpon >= (int)(sizeof(HARPOONS_DEF) / sizeof(HARPOONS_DEF[0])))
        harpon = HARPOON_RUSTIQUE;

    const DefinitionHarpon *def = &HARPOONS_DEF[harpon];
    plongeur->attaque_minimale = (int)(def->attaque_min * joueur_multiplicateur_niveau(plongeur));
    plongeur->attaque_maximale = (int)(def->attaque_max * joueur_multiplicateur_niveau(plongeur));
    plongeur->cout_oxygene_attaque_eq = def->cout_oxygene;
}

static void appliquer_combinaison(TypeCombinaison combi, Plongeur *plongeur)
{
    if (!plongeur) return;
    if (combi < 0 || combi >= (int)(sizeof(COMBIS_DEF) / sizeof(COMBIS_DEF[0])))
        combi = COMBI_NEOPRENE;

    const DefinitionCombinaison *def = &COMBIS_DEF[combi];
    plongeur->defense = (int)(def->defense * joueur_multiplicateur_niveau(plongeur));
    plongeur->cout_oxygene_passif_eq = def->cout_passif;
}

void inventaire_init(Inventaire *inv, Plongeur *plongeur)
{
    if (!inv) return;

    for (int i = 0; i < INVENTAIRE_TAILLE; i++)
    {
        inv->slots[i].type = OBJET_NONE;
        inv->slots[i].quantite = 0;
    }

    inv->harpon_equipe = HARPOON_RUSTIQUE;
    inv->combinaison_equipee = COMBI_NEOPRENE;

    if (plongeur)
    {
        appliquer_harpon(inv->harpon_equipe, plongeur);
        appliquer_combinaison(inv->combinaison_equipee, plongeur);
    }
}

int inventaire_ajoute_objet(Inventaire *inv, TypeObjet objet, int quantite)
{
    if (!inv || objet == OBJET_NONE || quantite <= 0)
        return 0;

    for (int i = 0; i < INVENTAIRE_TAILLE; i++)
    {
        if (inv->slots[i].type == objet)
        {
            inv->slots[i].quantite += quantite;
            return 1;
        }
    }

    for (int i = 0; i < INVENTAIRE_TAILLE; i++)
    {
        if (inv->slots[i].type == OBJET_NONE)
        {
            inv->slots[i].type = objet;
            inv->slots[i].quantite = quantite;
            return 1;
        }
    }

    return 0;
}

int inventaire_utilise_objet(Inventaire *inv, Plongeur *plongeur, int index_slot)
{
    if (!inv || !plongeur || index_slot < 0 || index_slot >= INVENTAIRE_TAILLE)
        return 0;

    SlotObjet *slot = &inv->slots[index_slot];
    if (slot->type == OBJET_NONE || slot->quantite <= 0)
        return 0;

    appliquer_objet(plongeur, slot->type);
    slot->quantite--;
    if (slot->quantite <= 0)
    {
        slot->type = OBJET_NONE;
        slot->quantite = 0;
    }

    return 1;
}

void inventaire_affiche(const Inventaire *inv)
{
    if (!inv) return;

    printf("\n========= INVENTAIRE =========\n");
    printf("Harpon      : %s\n", inventaire_nom_harpon(inv->harpon_equipe));
    printf("Combinaison : %s\n", inventaire_nom_combinaison(inv->combinaison_equipee));
    for (int i = 0; i < INVENTAIRE_TAILLE; i++)
    {
        const SlotObjet *slot = &inv->slots[i];
        printf("[%d] %-20s x%d\n", i + 1, inventaire_nom_objet(slot->type), slot->quantite);
    }
    printf("==============================\n");
}

int inventaire_equipe_harpon(Inventaire *inv, TypeHarpon harpon, Plongeur *plongeur)
{
    if (!inv || !plongeur) return 0;
    inv->harpon_equipe = harpon;
    appliquer_harpon(harpon, plongeur);
    return 1;
}

int inventaire_equipe_combi(Inventaire *inv, TypeCombinaison combi, Plongeur *plongeur)
{
    if (!inv || !plongeur) return 0;
    inv->combinaison_equipee = combi;
    appliquer_combinaison(combi, plongeur);
    return 1;
}

void inventaire_actualise_equipement(Inventaire *inv, Plongeur *plongeur)
{
    if (!inv || !plongeur) return;
    appliquer_harpon(inv->harpon_equipe, plongeur);
    appliquer_combinaison(inv->combinaison_equipee, plongeur);
}


