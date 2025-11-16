#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "creatures.h"
#include "map.h"
#include "joueur.h"
#include "inventaire.h"
#include "combat.h"
#include "ui.h"
#include "recompenses.h"
#include "sauvegarde.h"

#define SAVE_FILE "saves/partie.save"

static void preparer_boss_final(CreatureMarine *boss, int profondeur); // prépare le boss final (Kraken amélioré)

static CreatureMarine groupes_zones[MAP_HEIGHT][CREATURES_MAX];
static int groupes_counts[MAP_HEIGHT];
static int groupes_inities[MAP_HEIGHT];
static int progression_positions[MAP_HEIGHT];
static int zones_securisees[MAP_HEIGHT];

static void afficher_carte(const Map *carte, int ligne_plongeur)
{
    map_print(carte, ligne_plongeur, groupes_zones, groupes_counts, groupes_inities, progression_positions);
}

static int groupe_compte_vivants(const CreatureMarine *groupe, int nb) // compte le nombre de créatures vivantes dans un groupe
{
    int vivants = 0;
    for (int i = 0; i < nb; i++)
    {
        if (groupe[i].est_vivant && groupe[i].points_de_vie_actuels > 0)
            vivants++;
    }
    return vivants;
}

static void groupe_reinitialise(int ligne) // réinitialise un groupe de créatures à une ligne donnée
{
    if (ligne < 0 || ligne >= MAP_HEIGHT) return;
    groupes_counts[ligne] = 0;
    groupes_inities[ligne] = 0;
    progression_positions[ligne] = -1;
    for (int i = 0; i < CREATURES_MAX; i++)
    {
        groupes_zones[ligne][i].est_vivant = 0;
        groupes_zones[ligne][i].points_de_vie_actuels = 0;
    }
}

static void adoucir_creature(int ligne, int profondeur, CreatureMarine *c) // adoucit une créature trop dangereuse pour la ligne
{
    if (!c) return;
    if (ligne <= 1 && (c->type == REQUIN || c->type == CRABE_GEANT || c->type == KRAKEN))
    {
        TypeCreature remplacant = (rand() % 2 == 0) ? MEDUSE : POISSON_EPEE;
        init_creature(c, remplacant, profondeur);
    }
    else if (ligne == 2 && (c->type == CRABE_GEANT || c->type == KRAKEN))
    {
        init_creature(c, (rand() % 2 ? POISSON_EPEE : REQUIN), profondeur);
    }
}

static void initialiser_ligne(const Map *carte, int ligne) // initialise les créatures d'une ligne donnée
{
    int profondeur = map_get_depth(carte, 0, ligne);
    if (ligne == MAP_HEIGHT - 1)
    {
        preparer_boss_final(&groupes_zones[ligne][0], profondeur);
        groupes_counts[ligne] = 1;
        groupes_inities[ligne] = 1;
        progression_positions[ligne] = -1;
        return;
    }

    int nb = generate_group(groupes_zones[ligne], CREATURES_MAX, profondeur); // génère un groupe de créatures
    for (int i = 0; i < nb; i++)
        adoucir_creature(ligne, profondeur, &groupes_zones[ligne][i]);

    groupes_counts[ligne] = nb;
    groupes_inities[ligne] = 1;
    progression_positions[ligne] = -1;
}

static void initialiser_groupes(const Map *carte) // initialise les groupes de créatures pour toutes les lignes
{
    for (int ligne = 0; ligne < MAP_HEIGHT; ligne++)
        initialiser_ligne(carte, ligne);
    progression_positions[0] = 0;
}

static void reinitialiser_partie(Plongeur *plongeur, Inventaire *inventaire) // réinitialise une nouvelle partie
{
    joueur_init(plongeur);
    inventaire_init(inventaire, plongeur);
    for (int i = 0; i < MAP_HEIGHT; i++)
    {
        groupe_reinitialise(i);
        zones_securisees[i] = 0;
    }
}

static int demander_chargement(Plongeur *plongeur,
                               Inventaire *inventaire,
                               int *ligne,
                               int zones[]) // demande au joueur s'il veut charger une partie sauvegardée
{
    OptionMenu menu[] = {
        {"Commencer une nouvelle expédition"},
        {"Charger la dernière sauvegarde"}
    };
    ui_menu_actions("Bienvenue !", menu, 2);
    int choix = ui_lire_choix(1, 2);
    if (choix == 2)
    {
        if (charger_partie(SAVE_FILE, plongeur, inventaire, ligne, zones, MAP_HEIGHT))
        {
            printf("Sauvegarde chargée avec succès.\n");
            return 1;
        }
        else
        {
            printf("Aucune sauvegarde valide trouvée. Nouvelle expédition.\n");
        }
    }
    reinitialiser_partie(plongeur, inventaire);
    if (zones)
    {
        for (int i = 0; i < MAP_HEIGHT; i++)
            zones[i] = 0;
    }
    *ligne = 0;
    return 0;
}

static TypeCombinaison combinaison_minimale(int profondeur) // retourne la combinaison minimale requise pour une profondeur donnée
{
    if (profondeur >= 400) return COMBI_TITANIUM;
    if (profondeur >= 200) return COMBI_RENFORCEE;
    return COMBI_NEOPRENE;
}

static void preparer_boss_final(CreatureMarine *boss, int profondeur) // prépare le boss final (Kraken amélioré)
{
    if (!boss) return;
    init_creature(boss, KRAKEN, profondeur);
    strncpy(boss->nom, "Leviathan", sizeof(boss->nom) - 1);
    boss->nom[sizeof(boss->nom) - 1] = '\0';
    boss->niveau += 5;
    boss->points_de_vie_max = 360 + rand() % 81; // 360-440 PV
    boss->points_de_vie_actuels = boss->points_de_vie_max;
    boss->attaque_minimale = 32 + rand() % 6;
    boss->attaque_maximale = boss->attaque_minimale + 14;
    boss->defense = 20 + rand() % 6;
    boss->vitesse = 8 + rand() % 3;
    strncpy(boss->effet_special, "Cataclysme", sizeof(boss->effet_special) - 1);
    boss->effet_special[sizeof(boss->effet_special) - 1] = '\0';
}

static void retour_surface(Plongeur *plongeur, int *ligne) // fait remonter le joueur à la surface et restaure ses PV/O2/fatigue
{
    if (!plongeur || !ligne) return;
    *ligne = 0;
    plongeur->points_de_vie = plongeur->points_de_vie_max;
    plongeur->niveau_oxygene = plongeur->niveau_oxygene_max;
    plongeur->niveau_fatigue = JOUEUR_FATIGUE_MIN;
    joueur_reset_statuts(plongeur);
    printf("Repos complet : PV/O₂/fatigue restaurés.\n");
}

static void boutique_surface(Plongeur *plongeur, Inventaire *inventaire) // menu de la boutique à la surface
{
    if (!plongeur || !inventaire) return;

    typedef struct { const char *nom; TypeObjet objet; int prix; } ArticleObjet;
    const ArticleObjet objets[] = {
        {"Capsule d'oxygene (+40 O₂)", OBJET_CAPSULE_O2, 20},
        {"Trousse de soin (+25 PV)", OBJET_TROUSSE_SOIN, 25},
        {"Stimulant marin (-2 fatigue)", OBJET_STIMULANT, 30},
        {"Antidote (retire effets)", OBJET_ANTIDOTE, 35}
    };

    int en_cours = 1;
    while (en_cours)
    {
        printf("\n=== BOUTIQUE ===\nPerles : %d\n", plongeur->perles);
        OptionMenu menu[8];
        int actions[8];
        int count = 0;

        for (int i = 0; i < (int)(sizeof(objets) / sizeof(objets[0])); i++)
        {
            static char buffer[4][64];
            snprintf(buffer[i], sizeof(buffer[i]), "%s (%d perles)", objets[i].nom, objets[i].prix);
            menu[count].texte = buffer[i];
            actions[count++] = i + 1;
        }

        static char harpon_txt[64];
        if (inventaire->harpon_equipe < HARPOON_ELECTRIQUE)
        {
            int prix = 60 + inventaire->harpon_equipe * 40;
            snprintf(harpon_txt, sizeof(harpon_txt), "Améliorer harpon -> %s (%d perles)",
                     inventaire_nom_harpon(inventaire->harpon_equipe + 1), prix);
            menu[count].texte = harpon_txt;
            actions[count++] = 100 + prix;
        }

        static char combi_txt[64];
        if (inventaire->combinaison_equipee < COMBI_TITANIUM)
        {
            int prix = 70 + inventaire->combinaison_equipee * 50;
            snprintf(combi_txt, sizeof(combi_txt), "Améliorer combinaison -> %s (%d perles)",
                     inventaire_nom_combinaison(inventaire->combinaison_equipee + 1), prix);
            menu[count].texte = combi_txt;
            actions[count++] = 200 + prix;
        }

        menu[count].texte = "Quitter la boutique";
        actions[count++] = -1;

        ui_menu_actions("Que souhaitez-vous acheter ?", menu, count);
        int choix = ui_lire_choix(1, count);
        int action = actions[choix - 1];

        if (action == -1)
        {
            en_cours = 0;
            continue;
        }

        if (action >= 100 && action < 200)
        {
            int prix = action - 100;
            if (plongeur->perles < prix)
            {
                printf("Perles insuffisantes.\n");
                continue;
            }
            TypeHarpon prochain = inventaire->harpon_equipe + 1;
            inventaire_equipe_harpon(inventaire, prochain, plongeur);
            plongeur->perles -= prix;
            printf("Harpon amélioré au modèle %s.\n", inventaire_nom_harpon(prochain));
        }
        else if (action >= 200)
        {
            int prix = action - 200;
            if (plongeur->perles < prix)
            {
                printf("Perles insuffisantes.\n");
                continue;
            }
            TypeCombinaison prochaine = inventaire->combinaison_equipee + 1;
            inventaire_equipe_combi(inventaire, prochaine, plongeur);
            plongeur->perles -= prix;
            printf("Combinaison améliorée : %s.\n", inventaire_nom_combinaison(prochaine));
        }
        else
        {
            const ArticleObjet *art = &objets[action - 1];
            if (plongeur->perles < art->prix)
            {
                printf("Perles insuffisantes.\n");
                continue;
            }
            if (inventaire_ajoute_objet(inventaire, art->objet, 1))
            {
                plongeur->perles -= art->prix;
                printf("%s ajouté à l'inventaire.\n", inventaire_nom_objet(art->objet));
            }
            else
            {
                printf("Inventaire plein.\n");
            }
        }
    }
}

static void menu_surface_global(Plongeur *plongeur, Inventaire *inventaire, int *ligne_actuelle) // menu des actions disponibles à la surface
{
    int menu = 1;
    while (menu)
    {
        OptionMenu ops[] = {
            {"Repos complet"},
            {"Boutique"},
            {"Sauvegarder"},
            {"Retour"}
        };
        ui_menu_actions("Surface - actions disponibles", ops, 4);
        int choix = ui_lire_choix(1, 4);

        switch (choix)
        {
            case 1:
                retour_surface(plongeur, ligne_actuelle);
                progression_positions[*ligne_actuelle] = 0;
                break;
            case 2:
                boutique_surface(plongeur, inventaire);
                break;
            case 3:
                if (*ligne_actuelle != 0)
                {
                    printf("Vous devez être sur la surface pour sauvegarder.\n");
                    break;
                }
                if (sauvegarder_partie(SAVE_FILE,
                                       plongeur,
                                       inventaire,
                                       *ligne_actuelle,
                                       zones_securisees,
                                       MAP_HEIGHT))
                    printf("Sauvegarde effectuée.\n");
                else
                    printf("Erreur de sauvegarde.\n");
                break;
            case 4:
            default:
                menu = 0;
                break;
        }
    }
}

static void menu_inventaire_loop(Inventaire *inventaire, Plongeur *plongeur) // boucle de gestion de l'inventaire
{
    int boucle = 1;
    while (boucle)
    {
        inventaire_affiche(inventaire);
        OptionMenu options[] = {
            {"Utiliser un objet"},
            {"Équiper un harpon"},
            {"Équiper une combinaison"},
            {"Retour"}
        };
        ui_menu_actions("Gestion de l'inventaire", options, 4);
        int choix = ui_lire_choix(1, 4);

        switch (choix)
        {
            case 1:
            {
                printf("Choisissez un slot (1-%d):\n", INVENTAIRE_TAILLE);
                int slot = ui_lire_choix(1, INVENTAIRE_TAILLE) - 1;
                if (inventaire_utilise_objet(inventaire, plongeur, slot))
                    printf("Objet utilisé avec succès.\n");
                else
                    printf("Impossible d'utiliser cet objet.\n");
                break;
            }
            case 2:
            {
                OptionMenu harpons[] = {
                    {inventaire_nom_harpon(HARPOON_RUSTIQUE)},
                    {inventaire_nom_harpon(HARPOON_STANDARD)},
                    {inventaire_nom_harpon(HARPOON_ELECTRIQUE)}
                };
                ui_menu_actions("Choisissez un harpon", harpons, 3);
                int h = ui_lire_choix(1, 3) - 1;
                if (inventaire_equipe_harpon(inventaire, (TypeHarpon)h, plongeur))
                    printf("Harpon équipé: %s\n", inventaire_nom_harpon((TypeHarpon)h));
                break;
            }
            case 3:
            {
                OptionMenu combis[] = {
                    {inventaire_nom_combinaison(COMBI_NEOPRENE)},
                    {inventaire_nom_combinaison(COMBI_RENFORCEE)},
                    {inventaire_nom_combinaison(COMBI_TITANIUM)}
                };
                ui_menu_actions("Choisissez une combinaison", combis, 3);
                int c = ui_lire_choix(1, 3) - 1;
                if (inventaire_equipe_combi(inventaire, (TypeCombinaison)c, plongeur))
                    printf("Combinaison équipée: %s\n", inventaire_nom_combinaison((TypeCombinaison)c));
                break;
            }
            case 4:
            default:
                boucle = 0;
                break;
        }
    }
}

static int explorer_zone(Plongeur *plongeur, Inventaire *inventaire, const Map *carte, int ligne) // explore une zone à une ligne donnée
{
    if (!plongeur || !inventaire || !carte) return 1;
    if (ligne < 0 || ligne >= MAP_HEIGHT) return 1;

    if (zones_securisees[ligne])
    {
        printf("Cette zone est désormais sécurisée, aucune créature restante. Explorez plus profond.\n");
        return 1;
    }

    if (!groupes_inities[ligne])
        initialiser_ligne(carte, ligne);

    int profondeur = map_get_depth(carte, 0, ligne);
    int boss_final = (ligne == MAP_HEIGHT - 1);
    int bonus_combinaison = (ligne == 1 && !zones_securisees[ligne] &&
                             inventaire->combinaison_equipee < COMBI_RENFORCEE);
    CreatureMarine *groupe = groupes_zones[ligne];
    int nb = groupes_counts[ligne];

    if (nb <= 0 || groupe_compte_vivants(groupe, nb) <= 0)
    {
        printf("Il n'y a aucune créature à cette profondeur.\n");
        return 1;
    }

    printf("\nVous explorez la ligne %d (%dm).\n", ligne, profondeur);
    print_group(groupe, nb);

    if (progression_positions[ligne] < 0)
        progression_positions[ligne] = 0;

    ContexteCombat ctx = {
        .profondeur = profondeur,
        .ennemis = groupe,
        .nombre_ennemis = nb
    };

    ResultatCombat resultat = combat_boucle(plongeur, inventaire, &ctx);

    if (resultat.defaite)
    {
        printf("\n💀 Vous succombez après %d tour(s)...\n", resultat.tours_joues);
        groupe_reinitialise(ligne);
        progression_positions[ligne] = -1;
        return 0;
    }

    if (resultat.victoire)
    {
        progression_positions[ligne] = -1;
        distribuer_recompenses(plongeur, inventaire, groupe, nb, profondeur);
        if (boss_final)
            printf("\n🏆 Vous avez vaincu le Léviathan et conquis les abysses !\n");
        else
            printf("\n🌊 Zone sécurisée... jusqu'à la prochaine descente.\n");
        groupe_reinitialise(ligne);
        zones_securisees[ligne] = 1;
        if (bonus_combinaison)
        {
            inventaire_equipe_combi(inventaire, COMBI_RENFORCEE, plongeur);
            printf("🎁 Vous équipez automatiquement la combinaison renforcée pour explorer plus profond.\n");
        }
        return 1;
    }

    progression_positions[ligne] = 0;
    printf("Vous battez en retraite pour vous regrouper.\n");
    return 2;
}

static void deplacement_ligne(const Map *carte, int *ligne, Plongeur *plongeur, const Inventaire *inventaire) // gère le déplacement vertical du plongeur
{
    if (!carte || !ligne || !plongeur || !inventaire) return;

    OptionMenu options[] = {
        {"Descendre"},
        {"Monter"},
        {"Annuler"}
    };
    ui_menu_actions("Déplacement vertical", options, 3);
    int choix = ui_lire_choix(1, 3);

    int cible = *ligne;
    if (choix == 1 && *ligne < MAP_HEIGHT - 1)
        cible = *ligne + 1;
    else if (choix == 2 && *ligne > 0)
        cible = *ligne - 1;
    else if (choix == 3)
        return;
    else
    {
        printf("Déplacement impossible dans cette direction.\n");
        return;
    }

    int profondeur_depart = map_get_depth(carte, 0, *ligne);
    int profondeur_arrivee = map_get_depth(carte, 0, cible);
    TypeCombinaison requise = combinaison_minimale(profondeur_arrivee);

    if (inventaire->combinaison_equipee < requise)
    {
        printf("Équipement insuffisant pour descendre à %dm. Combinaison requise: %s.\n",
               profondeur_arrivee, inventaire_nom_combinaison(requise));
        return;
    }

    int delta = abs(profondeur_arrivee - profondeur_depart);
    int cout = 5 + delta / 50;
    if (profondeur_arrivee > profondeur_depart)
        cout += 1;

    joueur_consomme_oxygene(plongeur, cout);
    int suffocation = joueur_applique_suffocation(plongeur);

    *ligne = cible;
    printf("Déplacement vers %dm : -%d O₂%s\n",
           profondeur_arrivee,
           cout,
           suffocation ? " (suffocation!)" : "");
}

int main(void)
{
    seed_rng_once();

    Plongeur plongeur;
    Inventaire inventaire;
    int ligne_actuelle = 0;

    int partie_chargee = demander_chargement(&plongeur,
                                             &inventaire,
                                             &ligne_actuelle,
                                             zones_securisees);

    Map carte;
    map_init(&carte);
    initialiser_groupes(&carte);

    if (partie_chargee)
    {
        for (int i = 0; i < MAP_HEIGHT; i++)
        {
            if (zones_securisees[i])
                groupe_reinitialise(i);
        }
    }

    if (ligne_actuelle >= 0 && ligne_actuelle < MAP_HEIGHT && progression_positions[ligne_actuelle] < 0)
        progression_positions[ligne_actuelle] = 0;

    int en_jeu = 1;

    printf("Bienvenue dans OceanDepths !\n");
    afficher_carte(&carte, ligne_actuelle);

    while (en_jeu && joueur_est_en_vie(&plongeur))
    {
        int profondeur = map_get_depth(&carte, 0, ligne_actuelle);
        printf("\n==============================\n");
        printf("Position actuelle: Ligne %d (%dm)\n", ligne_actuelle, profondeur);
        ui_affiche_etat(&plongeur);

        OptionMenu menu[] = {
            {"Explorer la zone"},
            {"Afficher la carte"},
            {"Inventaire"},
            {"Changer de profondeur"},
            {"Retourner à la surface (repos)"},
            {"Quitter l'expédition"}
        };

        ui_menu_actions("Actions disponibles", menu, 6);
        int choix = ui_lire_choix(1, 6);

        switch (choix)
        {
            case 1:
            {
                int resultat_zone = explorer_zone(&plongeur, &inventaire, &carte, ligne_actuelle);
                if (resultat_zone == 0)
                {
                    en_jeu = 0;
                }
                else if (resultat_zone == 2)
                {
                    int ligne_precedente = ligne_actuelle;
                    retour_surface(&plongeur, &ligne_actuelle);
                    progression_positions[ligne_actuelle] = 0;
                    menu_surface_global(&plongeur, &inventaire, &ligne_actuelle);
                    if (ligne_precedente != 0)
                    {
                        ligne_actuelle = ligne_precedente;
                        printf("Vous reprenez votre plongée à la ligne %d.\n", ligne_actuelle);
                    }
                }
                else
                {
                    progression_positions[ligne_actuelle] = 0;
                }
                break;
            }
            case 2:
                afficher_carte(&carte, ligne_actuelle);
                break;
            case 3:
                menu_inventaire_loop(&inventaire, &plongeur);
                break;
            case 4:
                deplacement_ligne(&carte, &ligne_actuelle, &plongeur, &inventaire);
                break;
            case 5:
            {
                int ligne_precedente = ligne_actuelle;
                if (ligne_actuelle != 0)
                {
                    printf("Remontée vers la surface...\n");
                    ligne_actuelle = 0;
                }
                menu_surface_global(&plongeur, &inventaire, &ligne_actuelle);
                if (ligne_precedente != 0)
                {
                    ligne_actuelle = ligne_precedente;
                    printf("Vous reprenez votre plongée à la ligne %d.\n", ligne_actuelle);
                }
                break;
            }
            case 6:
            default:
                en_jeu = 0;
                break;
        }
    }

    if (!joueur_est_en_vie(&plongeur))
        printf("\n💀 Votre aventure s'arrête ici...\n");
    else
        printf("\nMerci d'avoir exploré OceanDepths !\n");

    return 0;
}
