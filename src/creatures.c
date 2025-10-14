#include <stdio.h>
#include "creatures.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

void seed_rng_once(void) // Fonction pour initialiser le générateur de nombres aléatoires une seule fois
{
    static int initialized = 0;
    if (!initialized)
    {
        srand((unsigned)time(NULL));
        initialized = 1;
    }
}

void init_creature(CreatureMarine *creature, TypeCreature typeCreature, int profondeur)
{

    static int next_id = 1;   // ID unique pour chaque créature
    creature->id = next_id++; // Assigne l'ID et incrémente pour la prochaine créature

    int d = (profondeur < 500) ? profondeur : 500;
    int k = d / 100; // palier 0..5

    if (typeCreature == TYPE_CREATURE_RANDOM)
    {

        // On tire un nombre entre 0 et 99 pour simuler une probabilité (%)
        int r = rand() % 100;

        if (k <= 1)
        {
            // Zones superficielles (0 à 100 m) → beaucoup de méduses et poissons
            if (r < 40)
                typeCreature = MEDUSE; // 40%
            else if (r < 70)
                typeCreature = POISSON_EPEE; // 30%
            else if (r < 90)
                typeCreature = REQUIN; // 20%
            else if (r < 99)
                typeCreature = CRABE_GEANT; // 9%
            else
                typeCreature = KRAKEN; // 1%
        }
        else if (k <= 3)
        {
            // Zones moyennes (200–300 m)
            if (r < 25)
                typeCreature = MEDUSE; // 25%
            else if (r < 50)
                typeCreature = POISSON_EPEE; // 25%
            else if (r < 80)
                typeCreature = REQUIN; // 30%
            else if (r < 95)
                typeCreature = CRABE_GEANT; // 15%
            else
                typeCreature = KRAKEN; // 5%
        }
        else
        {
            // Profondeur (400–500 m)
            if (r < 10)
                typeCreature = MEDUSE; // 10%
            else if (r < 30)
                typeCreature = POISSON_EPEE; // 20%
            else if (r < 55)
                typeCreature = REQUIN; // 25%
            else if (r < 85)
                typeCreature = CRABE_GEANT; // 30%
            else
                typeCreature = KRAKEN; // 15%
        }
    }

    switch (typeCreature)
    {
    case KRAKEN:
        strcpy(creature->nom, "Kraken");
        creature->points_de_vie_max = 120 + rand() % 61; // 120–180
        creature->attaque_minimale = 25 + rand() % 3;    // 25–27
        creature->attaque_maximale = 38 + rand() % 3;    // 38–40
        creature->defense = 14 + rand() % 4;             // 14–17
        creature->vitesse = 5 + rand() % 3;              // 5–7 (lent)
        strcpy(creature->effet_special, "Tentacules");
        break;

    case REQUIN:
        strcpy(creature->nom, "Requin");
        creature->points_de_vie_max = 60 + rand() % 41; // 60–100
        creature->attaque_minimale = 15 + rand() % 3;   // 15–17
        creature->attaque_maximale = 23 + rand() % 3;   // 23–25
        creature->defense = 8 + rand() % 5;             // 8–12
        creature->vitesse = 10 + rand() % 5;            // 10–14 (rapide)
        strcpy(creature->effet_special, "Morsure");
        break;

    case MEDUSE:
        strcpy(creature->nom, "Méduse");
        creature->points_de_vie_max = 20 + rand() % 21; // 20–40 (fragile)
        creature->attaque_minimale = 8 + rand() % 3;    // 8–10
        creature->attaque_maximale = 13 + rand() % 3;   // 13–15
        creature->defense = 5 + rand() % 3;             // 5–7 (faible def)
        creature->vitesse = 8 + rand() % 5;             // 8–12 (flottante)
        strcpy(creature->effet_special, "Paralysie");
        break;

    case POISSON_EPEE:
        strcpy(creature->nom, "Poisson-Épée");
        creature->points_de_vie_max = 70 + rand() % 21; // 70–90 (équilibré)
        creature->attaque_minimale = 18 + rand() % 3;   // 18–20
        creature->attaque_maximale = 26 + rand() % 3;   // 26–28
        creature->defense = 9 + rand() % 3;             // 9–11
        creature->vitesse = 8 + rand() % 4;             // 8–11
        strcpy(creature->effet_special, "Percée");
        break;

    case CRABE_GEANT:
        strcpy(creature->nom, "Crabe Géant");
        creature->points_de_vie_max = 80 + rand() % 41; // 80–120 (tank def)
        creature->attaque_minimale = 12 + rand() % 3;   // 12–14
        creature->attaque_maximale = 18 + rand() % 3;   // 18–20
        creature->defense = 18 + rand() % 5;            // 18–22 (haute def)
        creature->vitesse = 3 + rand() % 3;             // 3–5 (lent)
        strcpy(creature->effet_special, "Carapace");
        break;

    default:
        strcpy(creature->nom, "Inconnu");
        creature->points_de_vie_max = 80 + rand() % 41;
        creature->attaque_minimale = 10;
        creature->attaque_maximale = 15;
        creature->defense = 10;
        creature->vitesse = 5;
        strcpy(creature->effet_special, "Aucun");
        break;
    }

    if (typeCreature == KRAKEN)
    {
        // tank offensif
        creature->points_de_vie_max += 25 * k;
        creature->attaque_minimale += 2 * k;
        creature->attaque_maximale += 3 * k;
        creature->defense += 1 * k;

        // limites
        if (creature->points_de_vie_max > 350)
            creature->points_de_vie_max = 350;
        if (creature->attaque_maximale > 60)
            creature->attaque_maximale = 60;
        if (creature->defense > 30)
            creature->defense = 30;
    }
    else if (typeCreature == REQUIN)
    {
        // rapide
        creature->points_de_vie_max += 5 * k;
        creature->attaque_minimale += 1 * k;
        creature->attaque_maximale += 2 * k;
        creature->vitesse += 2 * k;

        // limites (tes bornes)
        if (creature->vitesse > 18)
            creature->vitesse = 18;
        if (creature->points_de_vie_max > 140)
            creature->points_de_vie_max = 140;
        if (creature->attaque_maximale > 35)
            creature->attaque_maximale = 35;
    }
    else if (typeCreature == MEDUSE)
    {
        // fragile mais plus dangereuse en profondeur
        creature->attaque_minimale += 1 * k;
        creature->attaque_maximale += 2 * k;
        // un tout petit gain de PV pour survivre un peu mieux
        creature->points_de_vie_max += 2 * k;

        if (creature->points_de_vie_max > 60)
            creature->points_de_vie_max = 60;
        if (creature->attaque_maximale > 22)
            creature->attaque_maximale = 22;
    }
    else if (typeCreature == POISSON_EPEE)
    {
        // équilibré
        creature->points_de_vie_max += 3 * k;
        creature->attaque_minimale += 1 * k;
        creature->attaque_maximale += 2 * k;

        if (creature->points_de_vie_max > 120)
            creature->points_de_vie_max = 120;
        if (creature->attaque_maximale > 40)
            creature->attaque_maximale = 40;
    }
    else if (typeCreature == CRABE_GEANT)
    {
        // défensif
        creature->points_de_vie_max += 5 * k;
        creature->defense += 2 * k;

        if (creature->points_de_vie_max > 200)
            creature->points_de_vie_max = 200;
        if (creature->defense > 40)
            creature->defense = 40;
    }

    // garde-fous communs
    if (creature->attaque_minimale > creature->attaque_maximale)
        creature->attaque_minimale = creature->attaque_maximale;

    creature->points_de_vie_actuels = creature->points_de_vie_max; // PV actuels = PV max
    creature->est_vivant = 1;
}

int generate_group(CreatureMarine group[], int max, int profondeur)
{
    if (max <= 0) return 0;
    if (max > CREATURES_MAX) max = CREATURES_MAX;

    // 🔹 Réinitialise les 'max' emplacements (slots vides)
    for (int i = 0; i < max; i++) {
        group[i].id = 0;
        group[i].nom[0] = '\0';
        group[i].points_de_vie_max = 0;
        group[i].points_de_vie_actuels = 0;
        group[i].attaque_minimale = 0;
        group[i].attaque_maximale = 0;
        group[i].defense = 0;
        group[i].vitesse = 0;
        group[i].effet_special[0] = '\0';
        group[i].est_vivant = 0;
    }

    // Palier de profondeur 0..5 (borne à 500 m)
    int d = (profondeur < 500) ? profondeur : 500;
    int k = d / 100; // 0..5

    // Intervalle [min_n..max_n] selon la profondeur
    int min_n = 1;
    int max_n;
    if (k <= 1)      max_n = (max >= 2) ? 2 : max;  // 1..2 en surface
    else if (k <= 3) max_n = (max >= 3) ? 3 : max;  // 2..3 au milieu
    else             max_n = max;                   // 3..max en profondeur

    if (min_n > max_n) min_n = max_n;

    int nombre_creatures = min_n + (max_n > min_n ? rand() % (max_n - min_n + 1) : 0);

    // Génération des 'nombre_creatures' premières cases
    for (int i = 0; i < nombre_creatures; i++)
        init_creature(&group[i], TYPE_CREATURE_RANDOM, profondeur);

    return nombre_creatures;
}

void print_creature(const CreatureMarine *creature)
{
    const char *symbole = creature_symbol(creature);

    printf("%s [%d] %-13s | 💙 PV: %3d | ⚔️ ATK: %2d–%2d | 🛡️ DEF: %2d | 💨 VIT: %2d | ✨ Effet: %-10s | %s\n",
           symbole, creature->id, creature->nom,
           creature->points_de_vie_max,
           creature->attaque_minimale, creature->attaque_maximale,
           creature->defense, creature->vitesse,
           creature->effet_special,
           creature->est_vivant ? "🟢 Vivant" : "🔴 Mort");
}

void print_group(const CreatureMarine group[], int count)
{
    printf("Groupe de %d créature(s) générée(s) :\n", count);
    printf("----------------------------------------------------\n");
    for (int i = 0; i < count; i++)
    {
        print_creature(&group[i]);
    }
    printf("----------------------------------------------------\n");
}

const char *creature_symbol(const CreatureMarine *c)
{
    if (strcmp(c->nom, "Kraken") == 0)
        return "🐙"; // Kraken
    if (strcmp(c->nom, "Requin") == 0)
        return "🦈"; // Requin
    if (strcmp(c->nom, "Méduse") == 0)
        return "🪼"; // Méduse
    if (strcmp(c->nom, "Poisson-Épée") == 0)
        return "🐟"; // Poisson-Épée
    if (strcmp(c->nom, "Crabe Géant") == 0)
        return "🦀"; // Crabe Géant

    return "❓"; // Type inconnu
}