#include "joueur.h"
#include <stddef.h>
#include <stdio.h>

#define JOUEUR_BASE_PV 100
#define JOUEUR_BASE_O2 100
#define JOUEUR_BASE_FATIGUE JOUEUR_FATIGUE_MIN
#define JOUEUR_BASE_ATK_MIN 12
#define JOUEUR_BASE_ATK_MAX 18
#define JOUEUR_BASE_DEF 8
#define JOUEUR_EXP_BASE 100
#define JOUEUR_EXP_PAS 50
#define JOUEUR_NIVEAU_MAX 15
#define JOUEUR_NIVEAU_MULTIPLIER 0.08f

static int clamp(int value, int min, int max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

static int seuil_experience_pour(int niveau)
{
    if (niveau >= JOUEUR_NIVEAU_MAX)
        return 0;
    return JOUEUR_EXP_BASE + (niveau - 1) * JOUEUR_EXP_PAS;
}

static int palier_profondeur(int profondeur)
{
    if (profondeur < 0) profondeur = 0;
    if (profondeur > 500) profondeur = 500;

    if (profondeur < 100) return 0;
    if (profondeur < 250) return 1;
    if (profondeur < 400) return 2;
    return 3; // abysses
}

void joueur_reset_statuts(Plongeur *plongeur)
{
    if (!plongeur) return;
    plongeur->statuts.tours_paralysie = 0;
    plongeur->statuts.reduction_attaques = 0;
}

void joueur_reset_competences(Plongeur *plongeur)
{
    if (!plongeur) return;
    for (int i = 0; i < COMPETENCE_MAX; i++)
        plongeur->cooldowns_competences[i] = 0;
}

void joueur_init(Plongeur *plongeur)
{
    if (!plongeur) return;

    plongeur->niveau = 1;
    plongeur->experience = 0;
    plongeur->experience_seuil = seuil_experience_pour(plongeur->niveau);

    plongeur->points_de_vie_max = JOUEUR_BASE_PV;
    plongeur->points_de_vie = plongeur->points_de_vie_max;
    plongeur->niveau_oxygene_max = JOUEUR_BASE_O2;
    plongeur->niveau_oxygene = plongeur->niveau_oxygene_max;
    plongeur->niveau_fatigue = JOUEUR_BASE_FATIGUE;
    plongeur->perles = 0;
    plongeur->attaque_minimale = JOUEUR_BASE_ATK_MIN;
    plongeur->attaque_maximale = JOUEUR_BASE_ATK_MAX;
    plongeur->defense = JOUEUR_BASE_DEF;
    plongeur->cout_oxygene_attaque_eq = 0;
    plongeur->cout_oxygene_passif_eq = 0;

    joueur_reset_statuts(plongeur);
    joueur_reset_competences(plongeur);
}

void joueur_consomme_oxygene(Plongeur *plongeur, int cout)
{
    if (!plongeur || cout <= 0) return;

    plongeur->niveau_oxygene -= cout;
    if (plongeur->niveau_oxygene < 0)
        plongeur->niveau_oxygene = 0;
}

void joueur_ajoute_fatigue(Plongeur *plongeur, int delta)
{
    if (!plongeur || delta <= 0) return;
    plongeur->niveau_fatigue = clamp(plongeur->niveau_fatigue + delta,
                                     JOUEUR_FATIGUE_MIN,
                                     JOUEUR_FATIGUE_MAX);
}

void joueur_recupere_fatigue(Plongeur *plongeur, int delta)
{
    if (!plongeur || delta <= 0) return;
    plongeur->niveau_fatigue = clamp(plongeur->niveau_fatigue - delta,
                                     JOUEUR_FATIGUE_MIN,
                                     JOUEUR_FATIGUE_MAX);
}

int joueur_est_en_vie(const Plongeur *plongeur)
{
    return (plongeur && plongeur->points_de_vie > 0);
}

int joueur_est_oxygene_critique(const Plongeur *plongeur)
{
    if (!plongeur) return 0;
    return (plongeur->niveau_oxygene <= JOUEUR_OXYGENE_CRITIQUE);
}

int joueur_applique_suffocation(Plongeur *plongeur)
{
    if (!plongeur || plongeur->niveau_oxygene > 0 || plongeur->points_de_vie <= 0)
        return 0;

    int degats = JOUEUR_SUFFOCATION_PV;
    if (degats > plongeur->points_de_vie)
        degats = plongeur->points_de_vie;

    plongeur->points_de_vie -= degats;
    return degats;
}

int joueur_cout_attaque_normale(const Plongeur *plongeur, int profondeur)
{
    int palier = palier_profondeur(profondeur);
    switch (palier)
    {
        case 0: palier = 2; break;
        case 1: palier = 3; break;
        case 2: palier = 4; break;
        default: palier = 4; break;
    }

    int bonus = plongeur ? plongeur->cout_oxygene_attaque_eq : 0;
    int cout = palier + bonus;
    if (cout < 1) cout = 1;
    return cout;
}

int joueur_cout_competence(int profondeur)
{
    int palier = palier_profondeur(profondeur);
    switch (palier)
    {
        case 0: return 5;
        case 1: return 6;
        case 2: return 7;
        default: return 8;
    }
}

int joueur_cout_consommation_passive(const Plongeur *plongeur, int profondeur)
{
    int palier = palier_profondeur(profondeur);
    switch (palier)
    {
        case 0: palier = 2; break;
        case 1: palier = 3; break;
        case 2: palier = 4; break;
        default: palier = 5; break;
    }

    int bonus = plongeur ? plongeur->cout_oxygene_passif_eq : 0;
    int cout = palier + bonus;
    if (cout < 1) cout = 1;
    return cout;
}

const char *joueur_message_alerte_oxygene(const Plongeur *plongeur)
{
    if (!plongeur) return NULL;

    if (plongeur->niveau_oxygene == 0)
        return "ALERTE CRITIQUE : Oxygéne épuisé ! (Perte de 5 PV/ tour)";
    if (plongeur->niveau_oxygene <= JOUEUR_OXYGENE_CRITIQUE)
        return "Alerte : Niveau d'oxygéne critique";

    return NULL;
}

float joueur_multiplicateur_niveau(const Plongeur *plongeur)
{
    if (!plongeur) return 1.0f;
    return 1.0f + JOUEUR_NIVEAU_MULTIPLIER * (plongeur->niveau - 1);
}

void joueur_recalcule_stats(Plongeur *plongeur)
{
    if (!plongeur) return;
    float mult = joueur_multiplicateur_niveau(plongeur);

    int pv_avant = plongeur->points_de_vie;
    int o2_avant = plongeur->niveau_oxygene;

    plongeur->points_de_vie_max = (int)(JOUEUR_BASE_PV * mult);
    plongeur->niveau_oxygene_max = (int)(JOUEUR_BASE_O2 * mult);

    if (plongeur->points_de_vie_max < JOUEUR_BASE_PV)
        plongeur->points_de_vie_max = JOUEUR_BASE_PV;
    if (plongeur->niveau_oxygene_max < JOUEUR_BASE_O2)
        plongeur->niveau_oxygene_max = JOUEUR_BASE_O2;

    plongeur->points_de_vie = clamp(pv_avant, 1, plongeur->points_de_vie_max);
    plongeur->niveau_oxygene = clamp(o2_avant, 0, plongeur->niveau_oxygene_max);
}

void joueur_gagner_experience(Plongeur *plongeur, int experience)
{
    if (!plongeur || experience <= 0) return;
    if (plongeur->niveau >= JOUEUR_NIVEAU_MAX) return;

    plongeur->experience += experience;
    while (plongeur->niveau < JOUEUR_NIVEAU_MAX &&
           plongeur->experience >= plongeur->experience_seuil)
    {
        plongeur->experience -= plongeur->experience_seuil;
        plongeur->niveau++;
        plongeur->experience_seuil = seuil_experience_pour(plongeur->niveau);
        joueur_recalcule_stats(plongeur);
        printf("🎖️  Vous passez niveau %d ! Vos capacités augmentent.\n", plongeur->niveau);
    }

    if (plongeur->niveau >= JOUEUR_NIVEAU_MAX)
        plongeur->experience = 0;
}

void joueur_tick_competences(Plongeur *plongeur)
{
    if (!plongeur) return;
    for (int i = 0; i < COMPETENCE_MAX; i++)
    {
        if (plongeur->cooldowns_competences[i] > 0)
            plongeur->cooldowns_competences[i]--;
    }
}

int joueur_cooldown_restant(const Plongeur *plongeur, TypeCompetence competence)
{
    if (!plongeur || competence < 0 || competence >= COMPETENCE_MAX)
        return 0;
    return plongeur->cooldowns_competences[competence];
}

void joueur_demarre_cooldown(Plongeur *plongeur, TypeCompetence competence, int tours)
{
    if (!plongeur || competence < 0 || competence >= COMPETENCE_MAX)
        return;
    if (tours < 0) tours = 0;
    plongeur->cooldowns_competences[competence] = tours;
}

