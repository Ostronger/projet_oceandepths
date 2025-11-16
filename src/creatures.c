#include <stdio.h>
#include "creatures.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Permet de modifier les stats d'une créature sans toucher à la formule générale
static int scale_value(int value, int percent, int minimum) 
{
    int scaled = (value * percent + 99) / 100; // arrondi vers haut
    if (scaled < minimum) scaled = minimum; // garantie d'un minimum
    return scaled;
}

// rendre les créatures de surface plus douces
static void adoucir_surface_creature(CreatureMarine *creature, int palier)
{
    if (!creature) return; // si créature nulle, on sort

    if (palier == 0)
    {
        if (creature->type == POISSON_EPEE || creature->type == REQUIN)
        {
            creature->points_de_vie_max = scale_value(creature->points_de_vie_max, 60, 18);
            creature->attaque_minimale = scale_value(creature->attaque_minimale, 60, 4);
            creature->attaque_maximale = scale_value(creature->attaque_maximale, 60, 7);
            creature->defense = scale_value(creature->defense, 60, 3);
        }
    }
    else if (palier == 1)
    {
        if (creature->type == POISSON_EPEE || creature->type == REQUIN || creature->type == CRABE_GEANT)
        {
            creature->points_de_vie_max = scale_value(creature->points_de_vie_max, 80, 25);
            creature->attaque_minimale = scale_value(creature->attaque_minimale, 80, 6);
            creature->attaque_maximale = scale_value(creature->attaque_maximale, 80, 10);
            creature->defense = scale_value(creature->defense, 80, 4);
        }
    }
}

void seed_rng_once(void) // Fonction pour initialiser le générateur de nombres aléatoires une seule fois
{
    static int initialized = 0; // variable statique pour suivre l'initialisation 
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

    int d = (profondeur < 500) ? profondeur : 500; // si profondeur >500m, on la traite comme 500m
    int k = d / 100; // palier 0..5

    if (typeCreature == TYPE_CREATURE_RANDOM)
    {
        int r = rand() % 100; // nombre aléatoire 0-99

        if (k == 0)
        {
            // Surface : uniquement des créatures fragiles
            typeCreature = (r < 70) ? MEDUSE : POISSON_EPEE;
        }
        else if (k == 1)
        {
            // 100-199 m : petits prédateurs occasionnels
            if (r < 55)      typeCreature = MEDUSE;        // 55%
            else if (r < 85) typeCreature = POISSON_EPEE;  // 30%
            else if (r < 97) typeCreature = REQUIN;        // 12%
            else             typeCreature = CRABE_GEANT;   // 3%
        }
        else if (k == 2)
        {
            // 200-299 m : équilibre et rares boss
            if (r < 35)      typeCreature = MEDUSE;        // 35%
            else if (r < 65) typeCreature = POISSON_EPEE;  // 30%
            else if (r < 85) typeCreature = REQUIN;        // 20%
            else if (r < 95) typeCreature = CRABE_GEANT;   // 10%
            else             typeCreature = KRAKEN;        // 5%
        }
        else
        {
            // Profondeurs >300 m : répartition plus dangereuse
            if (r < 15)      typeCreature = MEDUSE;
            else if (r < 35) typeCreature = POISSON_EPEE;
            else if (r < 60) typeCreature = REQUIN;
            else if (r < 85) typeCreature = CRABE_GEANT;
            else             typeCreature = KRAKEN;
        }
    }

    creature->type = typeCreature; // assigne le type de créature choisi

    creature->niveau = k + 1;
    creature->tours_pacifies = 0; // pas pacifié au départ

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

    adoucir_surface_creature(creature, k);

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

    // Réinitialise les 'max' emplacements (slots vides)
    // vide les données précédentes
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

    // Intervalle [min_n..max_n] de créatures à générer selon la profondeur
    int min_n = 1;
    int max_n;
    if (k <= 1)      max_n = (max >= 2) ? 2 : max;  // 1..2 en surface
    else if (k <= 3) max_n = (max >= 3) ? 3 : max;  // 2..3 au milieu
    else             max_n = max;                   // 3..max en profondeur


    if (min_n > max_n) min_n = max_n; // sécurité si min > max 

    // nombre aléatoire entre min_n et max_n
    // exemple : min=2, max=4 -> peut être 2, 3 ou 4
    int nombre_creatures = min_n + (max_n > min_n ? rand() % (max_n - min_n + 1) : 0);

    // Génération des 'nombre_creatures' premières cases
    for (int i = 0; i < nombre_creatures; i++)
        init_creature(&group[i], TYPE_CREATURE_RANDOM, profondeur);

    return nombre_creatures;
}

void print_creature(const CreatureMarine *creature)
{
    const char *symbole = creature_symbol(creature);

    printf("%s [niv: %d] %-13s | 💙 PV: %3d | ⚔️ ATK: %2d-%2d | 🛡️ DEF: %2d | 💨 VIT: %2d | ✨ Effet: %-10s | %s\n",
           symbole, creature->niveau, creature->nom,
           creature->points_de_vie_max,
           creature->attaque_minimale, creature->attaque_maximale,
           creature->defense, creature->vitesse,
           creature->effet_special,
           creature->est_vivant ? "🟢 Vivant" : "🔴 Mort");
}

void print_group(const CreatureMarine group[], int count)
{
    printf("\n============ Groupe de %d créature(s) ============\n", count);
    for (int i = 0; i < count; i++) {
        print_creature(&group[i]);
    }
    printf("===================================================\n");
}

const char *creature_symbol(const CreatureMarine *c)
{
    if (!c) return "\xF0\x9F\x90\xA1"; // symbole par défaut 
    switch (c->type)
    {
        case KRAKEN:        return "\xF0\x9F\x90\x99"; // emojis : 🐙
        case REQUIN:        return "\xF0\x9F\xA6\x88"; // 🦈
        case MEDUSE:        return "\xF0\x9F\xAA\xBC"; // 🪼
        case POISSON_EPEE:  return "\xF0\x9F\x90\xA0"; // 🐠
        case CRABE_GEANT:   return "\xF0\x9F\xA6\x80"; // 🦀
        default:            return "\xF0\x9F\x90\xA1"; // 🐡
    }
}
