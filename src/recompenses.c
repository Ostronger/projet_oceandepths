#include "recompenses.h"
#include <stdio.h>
#include <stdlib.h>
#include "ui.h"

static TypeCreature type_creature(const CreatureMarine *c)
{
    if (!c) return TYPE_CREATURE_RANDOM;
    return c->type;
}

static int is_kraken(const CreatureMarine *c)       { return type_creature(c) == KRAKEN; }
static int is_requin(const CreatureMarine *c)       { return type_creature(c) == REQUIN; }
static int is_meduse(const CreatureMarine *c)       { return type_creature(c) == MEDUSE; }
static int is_poisson_epee(const CreatureMarine *c) { return type_creature(c) == POISSON_EPEE; }
static int is_crabe(const CreatureMarine *c)        { return type_creature(c) == CRABE_GEANT; }

static int perles_creature(const CreatureMarine *creature, int profondeur)
{
    int palier = profondeur / 100;
    if (palier < 0) palier = 0;
    if (palier > 5) palier = 5;

    int base = 10 + palier * 8 + rand() % 8; // progression plus généreuse

    if (creature)
    {
        if (is_kraken(creature))      base += 40;
        else if (is_crabe(creature))  base += 25;
        else if (is_requin(creature)) base += 20;
        else if (is_poisson_epee(creature)) base += 10;
    }

    return base;
}

static int chance_objet(const CreatureMarine *creature, int profondeur)
{
    int chance = 20 + profondeur / 20;
    if (chance > 75) chance = 75;
    if (is_meduse(creature)) chance += 10;
    if (is_poisson_epee(creature)) chance += 5;
    return chance;
}

static TypeObjet loot_objet_aleatoire(void)
{
    int r = rand() % 100;
    if (r < 35) return OBJET_CAPSULE_O2;
    if (r < 60) return OBJET_TROUSSE_SOIN;
    if (r < 80) return OBJET_STIMULANT;
    if (r < 95) return OBJET_ANTIDOTE;
    return OBJET_NONE;
}

static TypeObjet generer_objet_bonus(const CreatureMarine *creature, int profondeur)
{
    if (!creature) return OBJET_NONE;
    int chance = chance_objet(creature, profondeur);
    if (rand() % 100 >= chance) return OBJET_NONE;
    return loot_objet_aleatoire();
}

static int harpon_disponible(const Inventaire *inv, int profondeur)
{
    if (!inv) return 0;
    if (inv->harpon_equipe >= HARPOON_ELECTRIQUE) return 0;
    if (profondeur < 150) return 0;
    return 1;
}

static int combi_disponible(const Inventaire *inv, int profondeur)
{
    if (!inv) return 0;
    if (inv->combinaison_equipee >= COMBI_TITANIUM) return 0;
    if (profondeur < 150) return 0;
    return 1;
}

static int experience_creature(const CreatureMarine *creature)
{
    if (!creature) return 0;
    int base = 30 + creature->niveau * 10;

    switch (creature->type)
    {
        case KRAKEN:
            base += 40;
            break;
        case REQUIN:
        case CRABE_GEANT:
            base += 20;
            break;
        case POISSON_EPEE:
            base += 10;
            break;
        default:
            break;
    }

    return base;
}

void distribuer_recompenses(Plongeur *plongeur,
                            Inventaire *inventaire,
                            const CreatureMarine *groupe,
                            int nb_creatures,
                            int profondeur) // distribue les récompenses après un combat
{
    if (!plongeur || !inventaire || !groupe || nb_creatures <= 0)
        return;

    int total_perles = 0;
    int total_experience = 0;
    int creatures_vaincues = 0;
    TypeObjet objet_bonus = OBJET_NONE;
    TypeHarpon harpon_cible = HARPOON_RUSTIQUE;
    TypeCombinaison combi_cible = COMBI_NEOPRENE;
    int harpon_dispo = 0;
    int combi_dispo = 0;

    for (int i = 0; i < nb_creatures; i++)
    {
        const CreatureMarine *c = &groupe[i];
        if (c->est_vivant && c->points_de_vie_actuels > 0)
            continue;

        creatures_vaincues++;
        total_perles += perles_creature(c, profondeur);
        total_experience += experience_creature(c);
        if (objet_bonus == OBJET_NONE)
            objet_bonus = generer_objet_bonus(c, profondeur);
    }

    if (creatures_vaincues == 0)
    {
        printf("\nAucune récompense : aucune créature n'a été neutralisée.\n");
        return;
    }

    if (harpon_disponible(inventaire, profondeur))
    {
        harpon_cible = (TypeHarpon)(inventaire->harpon_equipe + 1);
        harpon_dispo = 1;
    }
    if (combi_disponible(inventaire, profondeur))
    {
        combi_cible = (TypeCombinaison)(inventaire->combinaison_equipee + 1);
        combi_dispo = 1;
    }

    plongeur->perles += total_perles;

    printf("\n=== RÉCOMPENSES ===\n");
    printf("Perles récupérées : +%d (total %d)\n", total_perles, plongeur->perles);
    printf("Expérience gagnée : +%d\n", total_experience);
    printf("Créatures vaincues : %d\n", creatures_vaincues);

    OptionMenu options[4];
    int action_ids[4];
    int count = 0;
    options[count].texte = "Garder uniquement les perles";
    action_ids[count++] = 0;

    char buffer_objet[64];
    char buffer_harpon[64];
    char buffer_combi[64];

    if (objet_bonus != OBJET_NONE)
    {
        snprintf(buffer_objet, sizeof(buffer_objet), "Ramasser %s", inventaire_nom_objet(objet_bonus));
        options[count].texte = buffer_objet;
        action_ids[count++] = 1;
    }

    if (harpon_dispo)
    {
        snprintf(buffer_harpon, sizeof(buffer_harpon), "Améliorer harpon -> %s", inventaire_nom_harpon(harpon_cible));
        options[count].texte = buffer_harpon;
        action_ids[count++] = 2;
    }

    if (combi_dispo)
    {
        snprintf(buffer_combi, sizeof(buffer_combi), "Améliorer combinaison -> %s", inventaire_nom_combinaison(combi_cible));
        options[count].texte = buffer_combi;
        action_ids[count++] = 3;
    }

    if (count > 1)
    {
        ui_menu_actions("Choisissez une récompense bonus", options, count);
        int choix = ui_lire_choix(1, count);
        int action = action_ids[choix - 1];

        switch (action)
        {
            case 1:
                if (inventaire_ajoute_objet(inventaire, objet_bonus, 1))
                    printf("🎁 %s ajouté à l'inventaire.\n", inventaire_nom_objet(objet_bonus));
                else
                    printf("Inventaire plein, récompense perdue.\n");
                break;
            case 2:
                if (inventaire_equipe_harpon(inventaire, harpon_cible, plongeur))
                    printf("⚓ Harpon amélioré: %s.\n", inventaire_nom_harpon(harpon_cible));
                break;
            case 3:
                if (inventaire_equipe_combi(inventaire, combi_cible, plongeur))
                    printf("🛡️ Combinaison améliorée: %s.\n", inventaire_nom_combinaison(combi_cible));
                break;
            default:
                printf("Vous conservez uniquement les perles.\n");
                break;
        }
    }
    else
    {
        printf("Rien d'autre à récupérer cette fois.\n");
    }

    joueur_gagner_experience(plongeur, total_experience);
    inventaire_actualise_equipement(inventaire, plongeur);

    printf("====================\n");
}

