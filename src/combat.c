#include "combat.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "creatures.h"

#define STRESS_OXYGENE_MIN 1
#define STRESS_OXYGENE_MAX 2
#define COMBAT_BLOC_LARGEUR 58

static const int COMPETENCE_COUTS[COMPETENCE_MAX] = {0, 18, 8, 22};
static const int COMPETENCE_COOLDOWNS[COMPETENCE_MAX] = {4, 3, 2, 4};
static const char *COMPETENCE_NOMS[COMPETENCE_MAX] = {
    "Apnée prolongée",
    "Décharge électrique",
    "Communication marine",
    "Tourbillon aquatique"
};

#if defined(_MSC_VER)
#define OD_SNPRINTF(buf, size, ...) \
    _snprintf_s(buf, (size), _TRUNCATE, __VA_ARGS__)
#else
#define OD_SNPRINTF(buf, size, ...) \
    snprintf(buf, (size), __VA_ARGS__)
#endif

static int random_interval(int min, int max) // génère un entier aléatoire entre min et max inclus
{
    if (max < min) max = min;
    int amplitude = max - min;
    return min + (amplitude > 0 ? rand() % (amplitude + 1) : 0);
}

static int random_stress_oxygene(void) // génère un stress en oxygène aléatoire pour le joueur
{
    return random_interval(STRESS_OXYGENE_MIN, STRESS_OXYGENE_MAX); // entre 1 et 2
}

static int creature_est_kraken(const CreatureMarine *c) // vérifie si la créature est un kraken
{
    return (c && strcmp(c->nom, "Kraken") == 0); 
}

static int creature_est_meduse(const CreatureMarine *c) // vérifie si la créature est une méduse
{
    return (c && strcmp(c->nom, "Méduse") == 0); 
}

static int creature_est_requin(const CreatureMarine *c)
{
    return (c && strcmp(c->nom, "Requin") == 0);
}

static int creature_est_poisson_epee(const CreatureMarine *c)
{
    return (c && strcmp(c->nom, "Poisson-Épée") == 0);
}

static int creature_est_crabe(const CreatureMarine *c)
{
    return (c && strcmp(c->nom, "Crabe Géant") == 0);
}

static void joueur_subit_degats(Plongeur *plongeur, int degats) // applique des dégâts au joueur
{
    if (!plongeur || degats <= 0) return;
    plongeur->points_de_vie -= degats;
    if (plongeur->points_de_vie < 0)
        plongeur->points_de_vie = 0;
}

static void combat_mettre_a_jour_statuts(Plongeur *plongeur) // met à jour les statuts du joueur en début de tour
{
    if (!plongeur) return;

    if (plongeur->statuts.tours_paralysie > 0)
    {
        plongeur->statuts.tours_paralysie--;
        if (plongeur->statuts.tours_paralysie == 0)
            plongeur->statuts.reduction_attaques = 0;
    }
}

static const char *combat_nom_harpon(const Inventaire *inventaire)
{
    if (!inventaire) return "harpon";
    return inventaire_nom_harpon(inventaire->harpon_equipe);
}

static void combat_bloc_debut(void)
{
    printf("╔════════════════════ COMBAT SOUS-MARIN ═══════════════════╗\n");
}

static void combat_bloc_ligne(const char *texte)
{
    const char *ligne = texte ? texte : "";
    printf("║ %-*.*s ║\n", COMBAT_BLOC_LARGEUR, COMBAT_BLOC_LARGEUR, ligne);
}

static void combat_bloc_fin(void)
{
    printf("╚══════════════════════════════════════════════════════════╝\n");
}

static void combat_affiche_resume_joueur(const Plongeur *plongeur,
                                         const Inventaire *inventaire,
                                         const CreatureMarine *cible,
                                         int degats,
                                         int cout_oxygene,
                                         int pv_avant)
{
    (void)plongeur;
    char buffer[128];
    combat_bloc_debut();
    OD_SNPRINTF(buffer, sizeof(buffer), "Vous attaquez %s avec votre %s", cible->nom, combat_nom_harpon(inventaire));
    combat_bloc_ligne(buffer);
    combat_bloc_ligne("");
    OD_SNPRINTF(buffer, sizeof(buffer), "    PLONGEUR              VS              %s", cible->nom);
    combat_bloc_ligne(buffer);
    OD_SNPRINTF(buffer, sizeof(buffer), "       🤿                            %s", creature_symbol(cible));
    combat_bloc_ligne(buffer);
    combat_bloc_ligne("");
    OD_SNPRINTF(buffer, sizeof(buffer), "Dégâts infligés : %d PV (O₂ -%d)", degats, cout_oxygene);
    combat_bloc_ligne(buffer);
    if (!cible->est_vivant)
        OD_SNPRINTF(buffer, sizeof(buffer), "Victoire ! %s est neutralisé.", cible->nom);
    else
        OD_SNPRINTF(buffer, sizeof(buffer), "PV de %s : %d → %d", cible->nom, pv_avant, cible->points_de_vie_actuels);
    combat_bloc_ligne(buffer);
    combat_bloc_fin();
}

static void combat_affiche_resume_creature(const CreatureMarine *attaquant,
                                           int degats,
                                           int stress,
                                           int pv_avant,
                                           int pv_apres,
                                           const char *commentaire)
{
    char buffer[128];
    combat_bloc_debut();
    OD_SNPRINTF(buffer, sizeof(buffer), "%s attaque !", attaquant->nom);
    combat_bloc_ligne(buffer);
    combat_bloc_ligne("");
    OD_SNPRINTF(buffer, sizeof(buffer), "    PLONGEUR              VS              %s", attaquant->nom);
    combat_bloc_ligne(buffer);
    OD_SNPRINTF(buffer, sizeof(buffer), "       🤿                            %s", creature_symbol(attaquant));
    combat_bloc_ligne(buffer);
    combat_bloc_ligne("");
    OD_SNPRINTF(buffer, sizeof(buffer), "Vous perdez %d PV (stress O₂ -%d)", degats, stress);
    combat_bloc_ligne(buffer);
    OD_SNPRINTF(buffer, sizeof(buffer), "PV du plongeur : %d → %d", pv_avant, pv_apres);
    combat_bloc_ligne(buffer);
    if (commentaire && *commentaire)
        combat_bloc_ligne(commentaire);
    combat_bloc_fin();
}

static void afficher_hud_cible(const CreatureMarine *cible)
{
    if (!cible) return;
    printf("\n--- %s (Niv %d) ---\n", cible->nom, cible->niveau);
    printf("PV ennemi : %d/%d\n", cible->points_de_vie_actuels, cible->points_de_vie_max);
}

static int choisir_cible(const ContexteCombat *ctx)
{
    if (!ctx || !ctx->ennemis) return -1;

    int indices[CREATURES_MAX];
    char descriptions[CREATURES_MAX][64];
    OptionMenu options[CREATURES_MAX];
    int count = 0;

    for (int i = 0; i < ctx->nombre_ennemis; i++)
    {
        const CreatureMarine *c = &ctx->ennemis[i];
        if (!c->est_vivant || c->points_de_vie_actuels <= 0)
            continue;
        indices[count] = i;
        OD_SNPRINTF(descriptions[count], sizeof(descriptions[count]),
                    "%s (%d/%d PV)",
                    c->nom,
                    c->points_de_vie_actuels,
                    c->points_de_vie_max);
        options[count].texte = descriptions[count];
        count++;
    }

    if (count == 0)
        return -1;

    if (count == 1)
        return indices[0];

    ui_menu_actions("Choisissez une cible", options, count);
    int choix = ui_lire_choix(1, count);
    return indices[choix - 1];
}

static int action_attaque_standard(Plongeur *plongeur, Inventaire *inventaire, ContexteCombat *ctx)
{
    int idx;
    if (!ctx || ctx->nombre_ennemis <= 0)
        idx = -1;
    else if (ctx->nombre_ennemis == 1)
        idx = 0;
    else
        idx = choisir_cible(ctx);
    if (idx < 0) return 0;

    CreatureMarine *cible = &ctx->ennemis[idx];
    afficher_hud_cible(cible);
    int pv_avant = cible->points_de_vie_actuels;
    int cout_oxygene = joueur_cout_attaque_normale(plongeur, ctx->profondeur);
    joueur_consomme_oxygene(plongeur, cout_oxygene);

    int degats = combat_calcul_degats_joueur(plongeur, cible);
    combat_applique_degats_creature(cible, degats);

    combat_affiche_resume_joueur(plongeur, inventaire, cible, degats, cout_oxygene, pv_avant);

    joueur_ajoute_fatigue(plongeur, 1);
    return 1;
}

static int competence_apnee_prolongee(Plongeur *plongeur)
{
    if (!plongeur) return 0;
    int critique = joueur_est_oxygene_critique(plongeur);
    int avant = plongeur->niveau_oxygene;
    plongeur->niveau_oxygene += 20;
    if (plongeur->niveau_oxygene > plongeur->niveau_oxygene_max)
        plongeur->niveau_oxygene = plongeur->niveau_oxygene_max;
    printf("🌬️ Apnée prolongée : +%d O₂ (%d → %d)\n",
           plongeur->niveau_oxygene - avant,
           avant,
           plongeur->niveau_oxygene);

    if (!critique)
        joueur_demarre_cooldown(plongeur, COMP_APNEE_PROLONGEE, COMPETENCE_COOLDOWNS[COMP_APNEE_PROLONGEE]);
    else
        joueur_demarre_cooldown(plongeur, COMP_APNEE_PROLONGEE, 0);
    return 1;
}

static int competence_decharge_electrique(Plongeur *plongeur, ContexteCombat *ctx)
{
    if (!plongeur || !ctx) return 0;
    int cout = COMPETENCE_COUTS[COMP_DECHARGE_ELECTRIQUE];
    if (plongeur->niveau_oxygene < cout)
    {
        printf("Oxygène insuffisant pour la décharge (%d requis).\n", cout);
        return 0;
    }

    joueur_consomme_oxygene(plongeur, cout);
    printf("⚡ Vous libérez une décharge électrique dans l'eau !\n");

    int touches = 0;
    for (int i = 0; i < ctx->nombre_ennemis; i++)
    {
        CreatureMarine *ennemi = &ctx->ennemis[i];
        if (!ennemi->est_vivant || ennemi->points_de_vie_actuels <= 0)
            continue;
        int degats = random_interval(20, 30);
        combat_applique_degats_creature(ennemi, degats);
        printf("   %s subit %d dégâts.\n", ennemi->nom, degats);
        touches++;
    }

    if (touches == 0)
        printf("   Aucun ennemi n'était encore debout.\n");

    joueur_demarre_cooldown(plongeur, COMP_DECHARGE_ELECTRIQUE, COMPETENCE_COOLDOWNS[COMP_DECHARGE_ELECTRIQUE]);
    return 1;
}

static int competence_communication_marine(Plongeur *plongeur, ContexteCombat *ctx)
{
    if (!plongeur || !ctx) return 0;
    int cout = COMPETENCE_COUTS[COMP_COMMUNICATION_MARINE];
    if (plongeur->niveau_oxygene < cout)
    {
        printf("Oxygène insuffisant pour calmer une créature (%d requis).\n", cout);
        return 0;
    }

    int idx = choisir_cible(ctx);
    if (idx < 0) return 0;

    joueur_consomme_oxygene(plongeur, cout);
    CreatureMarine *cible = &ctx->ennemis[idx];
    cible->tours_pacifies = 1;
    printf("🕯️ Vous apaisez %s pour le prochain tour.\n", cible->nom);
    joueur_demarre_cooldown(plongeur, COMP_COMMUNICATION_MARINE, COMPETENCE_COOLDOWNS[COMP_COMMUNICATION_MARINE]);
    return 1;
}

static int competence_tourbillon_aquatique(Plongeur *plongeur, ContexteCombat *ctx)
{
    if (!plongeur || !ctx) return 0;
    int cout = COMPETENCE_COUTS[COMP_TOURBILLON_AQUATIQUE];
    if (plongeur->niveau_oxygene < cout)
    {
        printf("Oxygène insuffisant pour créer un tourbillon (%d requis).\n", cout);
        return 0;
    }

    int vivants = combat_compte_creatures_vivantes(ctx);
    if (vivants == 0)
    {
        printf("Aucun ennemi à désorienter.\n");
        return 0;
    }

    joueur_consomme_oxygene(plongeur, cout);
    printf("🌪️ Un tourbillon balaie les ennemis et réduit leur vitesse !\n");

    for (int i = 0; i < ctx->nombre_ennemis; i++)
    {
        CreatureMarine *ennemi = &ctx->ennemis[i];
        if (!ennemi->est_vivant || ennemi->points_de_vie_actuels <= 0)
            continue;
        ennemi->vitesse -= 2;
        if (ennemi->vitesse < 1)
            ennemi->vitesse = 1;
    }

    joueur_demarre_cooldown(plongeur, COMP_TOURBILLON_AQUATIQUE, COMPETENCE_COOLDOWNS[COMP_TOURBILLON_AQUATIQUE]);
    return 1;
}

static int competence_est_prete(const Plongeur *plongeur, TypeCompetence comp)
{
    if (comp == COMP_APNEE_PROLONGEE && joueur_est_oxygene_critique(plongeur))
        return 1;
    return joueur_cooldown_restant(plongeur, comp) == 0;
}

static int action_competences(Plongeur *plongeur, ContexteCombat *ctx)
{
    if (!plongeur || !ctx) return 0;

    OptionMenu options[COMPETENCE_MAX + 1];
    char descriptions[COMPETENCE_MAX][96];
    for (int i = 0; i < COMPETENCE_MAX; i++)
    {
        char statut[32];
        if (competence_est_prete(plongeur, (TypeCompetence)i))
            OD_SNPRINTF(statut, sizeof(statut), "prête");
        else
            OD_SNPRINTF(statut,
                        sizeof(statut),
                        "%d tour(s)",
                        joueur_cooldown_restant(plongeur, (TypeCompetence)i));

        OD_SNPRINTF(descriptions[i], sizeof(descriptions[i]),
                    "%s (O₂ %d | CD %s)",
                    COMPETENCE_NOMS[i],
                    COMPETENCE_COUTS[i],
                    statut);
        options[i].texte = descriptions[i];
    }
    options[COMPETENCE_MAX].texte = "Annuler";

    ui_menu_actions("Choisissez une compétence", options, COMPETENCE_MAX + 1);
    int choix = ui_lire_choix(1, COMPETENCE_MAX + 1);
    if (choix == COMPETENCE_MAX + 1)
        return 0;

    TypeCompetence competence = (TypeCompetence)(choix - 1);
    if (!competence_est_prete(plongeur, competence))
    {
        printf("Cette compétence est encore en récupération.\n");
        return 0;
    }

    int resultat = 0;
    switch (competence)
    {
        case COMP_APNEE_PROLONGEE:
            resultat = competence_apnee_prolongee(plongeur);
            break;
        case COMP_DECHARGE_ELECTRIQUE:
            resultat = competence_decharge_electrique(plongeur, ctx);
            break;
        case COMP_COMMUNICATION_MARINE:
            resultat = competence_communication_marine(plongeur, ctx);
            break;
        case COMP_TOURBILLON_AQUATIQUE:
            resultat = competence_tourbillon_aquatique(plongeur, ctx);
            break;
        default:
            break;
    }

    return resultat;
}

static int action_utiliser_objet(Inventaire *inventaire, Plongeur *plongeur)
{
    if (!inventaire || !plongeur) return 0;
    inventaire_affiche(inventaire);
    printf("Choisissez un slot (0 pour annuler):\n");
    int choix = ui_lire_choix(0, INVENTAIRE_TAILLE);
    if (choix == 0)
        return 0;
    if (!inventaire_utilise_objet(inventaire, plongeur, choix - 1))
    {
        printf("Objet indisponible.\n");
        return 0;
    }

    printf("Objet consommé.\n");
    return 1;
}

static void combat_phase_joueur(Plongeur *plongeur, Inventaire *inventaire, ContexteCombat *ctx)
{
    if (!plongeur || !ctx) return;

    int actions_autorisees = combat_actions_max_joueur(plongeur);
    int actions_effectuees = 0;

    while (actions_effectuees < actions_autorisees &&
           combat_compte_creatures_vivantes(ctx) > 0 &&
           joueur_est_en_vie(plongeur))
    {
        printf("\nActions restantes : %d\n", actions_autorisees - actions_effectuees);

        OptionMenu options[4];
        int codes[4];
        int count = 0;

        options[count].texte = "Attaquer";
        codes[count++] = 1;

        options[count].texte = "Compétences marines";
        codes[count++] = 2;

        options[count].texte = "Inventaire / objets";
        codes[count++] = 3;

        options[count].texte = "Terminer le tour";
        codes[count++] = 4;

        ui_menu_actions("Choisissez une action", options, count);
        int action = codes[ui_lire_choix(1, count) - 1];

        if (action == 1)
        {
            if (action_attaque_standard(plongeur, inventaire, ctx))
                actions_effectuees++;
        }
        else if (action == 2)
        {
            if (action_competences(plongeur, ctx))
            {
                joueur_ajoute_fatigue(plongeur, 1);
                actions_effectuees++;
            }
        }
        else if (action == 3)
        {
            if (action_utiliser_objet(inventaire, plongeur))
                actions_effectuees++;
        }
        else
        {
            break;
        }
    }
}

static void combat_phase_alertes(Plongeur *plongeur)
{
    const char *message = joueur_message_alerte_oxygene(plongeur);
    if (message)
        printf("⚠️  %s\n", message);
}

static void combat_phase_creatures(Plongeur *plongeur, ContexteCombat *ctx)
{
    if (!plongeur || !ctx || !ctx->ennemis)
        return;

    int indices[CREATURES_MAX];
    int count = 0;
    for (int i = 0; i < ctx->nombre_ennemis; i++)
    {
        if (ctx->ennemis[i].est_vivant && ctx->ennemis[i].points_de_vie_actuels > 0)
            indices[count++] = i;
    }

    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            int idx_i = indices[i];
            int idx_j = indices[j];
            if (ctx->ennemis[idx_j].vitesse > ctx->ennemis[idx_i].vitesse)
            {
                int tmp = indices[i];
                indices[i] = indices[j];
                indices[j] = tmp;
            }
        }
    }

    for (int i = 0; i < count; i++)
    {
        CreatureMarine *c = &ctx->ennemis[indices[i]];
        if (!joueur_est_en_vie(plongeur))
            break;

        if (c->tours_pacifies > 0)
        {
            printf("🕊️ %s est pacifié et reste immobile.\n", c->nom);
            c->tours_pacifies--;
            continue;
        }

        int attaques = creature_est_kraken(c) ? 2 : 1;
        for (int a = 0; a < attaques; a++)
        {
            if (!joueur_est_en_vie(plongeur))
                break;

            int pv_avant_joueur = plongeur->points_de_vie;
            int degats = combat_calcul_degats_creature(c, plongeur);
            joueur_subit_degats(plongeur, degats);

            int stress = random_stress_oxygene();
            joueur_consomme_oxygene(plongeur, stress);

            const char *commentaire = (attaques > 1) ? "Frappe tentaculaire !" : NULL;
            combat_affiche_resume_creature(c,
                                           degats,
                                           stress,
                                           pv_avant_joueur,
                                           plongeur->points_de_vie,
                                           commentaire);

            if (creature_est_meduse(c))
            {
                plongeur->statuts.tours_paralysie = 1;
                plongeur->statuts.reduction_attaques = 1;
                printf("   ⚡ Vous êtes paralysé, vos attaques seront réduites au prochain tour !\n");
            }

            joueur_applique_suffocation(plongeur);
        }
    }
}

int combat_actions_max_joueur(const Plongeur *plongeur)
{
    if (!plongeur) return 0;

    int base;
    if (plongeur->niveau_fatigue <= 1)
        base = 3;
    else if (plongeur->niveau_fatigue <= 3)
        base = 2;
    else
        base = 1;

    if (plongeur->statuts.reduction_attaques > 0)
    {
        base -= plongeur->statuts.reduction_attaques;
        if (base < 1) base = 1;
    }

    return base;
}

int combat_calcul_degats_joueur(const Plongeur *plongeur, const CreatureMarine *cible)
{
    if (!plongeur || !cible) return 0;

    int base = random_interval(plongeur->attaque_minimale, plongeur->attaque_maximale);
    int defense = cible->defense;
    int degats = base - defense;

    if (degats < 1) degats = 1;

    if (creature_est_crabe(cible))
    {
        degats = (degats * 80 + 99) / 100;
        if (degats < 1) degats = 1;
    }

    return degats;
}

int combat_calcul_degats_creature(const CreatureMarine *attaquant, const Plongeur *plongeur)
{
    if (!attaquant || !plongeur) return 0;

    int base = random_interval(attaquant->attaque_minimale, attaquant->attaque_maximale);
    int defense = plongeur->defense;

    if (creature_est_poisson_epee(attaquant))
    {
        defense -= 2;
        if (defense < 0) defense = 0;
    }

    int degats = base - defense;
    if (degats < 1) degats = 1;

    if (creature_est_requin(attaquant) &&
        attaquant->points_de_vie_actuels * 2 <= attaquant->points_de_vie_max)
    {
        degats = (degats * 130 + 99) / 100;
        if (degats < 1) degats = 1;
    }

    return degats;
}

int combat_compte_creatures_vivantes(const ContexteCombat *ctx)
{
    if (!ctx || !ctx->ennemis) return 0;

    int vivantes = 0;
    for (int i = 0; i < ctx->nombre_ennemis; i++)
    {
        if (ctx->ennemis[i].est_vivant && ctx->ennemis[i].points_de_vie_actuels > 0)
            vivantes++;
    }
    return vivantes;
}

void combat_applique_degats_creature(CreatureMarine *cible, int degats)
{
    if (!cible || degats <= 0) return;

    cible->points_de_vie_actuels -= degats;
    if (cible->points_de_vie_actuels <= 0)
    {
        cible->points_de_vie_actuels = 0;
        cible->est_vivant = 0;
    }
}

ResultatCombat combat_boucle(Plongeur *plongeur, Inventaire *inventaire, ContexteCombat *ctx)
{
    ResultatCombat resultat = {0, 0, 0, 0};

    if (!plongeur || !ctx || !ctx->ennemis || ctx->nombre_ennemis <= 0)
    {
        resultat.victoire = 1;
        return resultat;
    }

    while (joueur_est_en_vie(plongeur) && combat_compte_creatures_vivantes(ctx) > 0)
    {
        resultat.tours_joues++;

        printf("\n===== Tour %d =====\n", resultat.tours_joues);
        ui_affiche_etat(plongeur);

        combat_phase_joueur(plongeur, inventaire, ctx);

        if (combat_compte_creatures_vivantes(ctx) == 0 || !joueur_est_en_vie(plongeur))
            break;

        combat_phase_creatures(plongeur, ctx);

        int cout_passif = joueur_cout_consommation_passive(plongeur, ctx->profondeur);
        joueur_consomme_oxygene(plongeur, cout_passif);
        joueur_applique_suffocation(plongeur);

        combat_phase_alertes(plongeur);

        joueur_recupere_fatigue(plongeur, 1);
        combat_mettre_a_jour_statuts(plongeur);
        joueur_tick_competences(plongeur);

        if (!joueur_est_en_vie(plongeur))
            break;
    }

    if (combat_compte_creatures_vivantes(ctx) == 0)
        resultat.victoire = 1;
    else if (!joueur_est_en_vie(plongeur))
        resultat.defaite = 1;
    else
        resultat.fuite = 1;

    return resultat;
}
