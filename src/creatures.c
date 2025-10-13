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

    if (typeCreature == TYPE_CREATURE_RANDOM) // Si aucun type spécifié, choisir aléatoirement
    {
        typeCreature = (TypeCreature)(rand() % 2); // 0 ou 1 pour KRAKEN ou REQUIN, on met le typeCreature dans une variable de type TypeCreature car rand() renvoie un int, on caste donc le int en TypeCreature
    }

    switch (typeCreature) // Initialisation des attributs selon le type de créature
    {
    case KRAKEN:
        strcpy(creature->nom, "Kraken");
        creature->points_de_vie_max = 120 + rand() % 61;
        creature->attaque_minimale = 25;
        creature->attaque_maximale = 40;
        break;

    case REQUIN:
        strcpy(creature->nom, "Requin");
        creature->points_de_vie_max = 60 + rand() % 41;
        creature->attaque_minimale = 15;
        creature->attaque_maximale = 25;
        break;

    default:
        strcpy(creature->nom, "Inconnu"); // Cas par défaut, ne devrait pas arriver
        creature->points_de_vie_max = 80 + rand() % 41;
        creature->attaque_minimale = 10;
        creature->attaque_maximale = 15;
        break;
    }

    creature->defense = 10 + rand() % 6;                           // Défense entre 10 et 15
    creature->vitesse = 5 + rand() % 10;                           // Vitesse entre 5 et 14
    strcpy(creature->effet_special, "aucun");
    creature->est_vivant = 1;

    int d = (profondeur < 500) ? profondeur : 500; // profondeur maximale prise en compte pour l'augmentation de la difficulté
    int k = d / 100; // palier de profondeur combien de palier de 100 m franchi

    if (typeCreature == KRAKEN)
    {
        // Augmentation des stats du Kraken avec la profondeur
        creature->points_de_vie_max += 25 * k;                // PV max augmente de 25 tous les 100m
        creature->attaque_minimale += 2 * k;                  // Attaque minimale augmente de 2 tous les 100m
        creature->attaque_maximale += 3 * k;                  // Attaque maximale augmente de 3 tous les 100m
        creature->defense += 1 * k;                           // Défense augmente de 1 tous les 100m

        // limite 
        if (creature->points_de_vie_max > 350) creature->points_de_vie_max = 350;
        if (creature->attaque_maximale > 60) creature->attaque_maximale = 60;
        if (creature->defense > 30) creature->defense = 30;
    }
    else if (typeCreature == REQUIN)
    {
        // Augmentation des stats du Requin avec la profondeur
        creature->points_de_vie_max += 5 * k; // PV max augmente de 5 tous les 100m
        creature->attaque_minimale += 1 * k;                 // Attaque minimale augmente de 1 tous les 100m
        creature->attaque_maximale += 2 * k;                 // Attaque maximale augmente de 2 tous les 100m
        creature->vitesse += 2 * k;                          // Vitesse augmente de 2 tous les 100m

        // limite
        if (creature->vitesse > 18 ) creature->vitesse = 18;
        if (creature->points_de_vie_max > 140) creature->points_de_vie_max = 140;
        if (creature->attaque_maximale > 35) creature->attaque_maximale = 35;
    }
    else
    {
        // Si le type de créature est inconnu, on initialise avec des valeurs par défaut
        creature->points_de_vie_max = 80 + rand() % 41; // PV max entre 80 et 120
        creature->attaque_minimale = 10;
        creature->attaque_maximale = 15;
    }

    if (creature->attaque_minimale > creature->attaque_maximale)
        creature->attaque_minimale = creature->attaque_maximale; // Assure que l'attaque minimale n'est pas supérieure à l'attaque maximale
    
    creature->points_de_vie_actuels = creature->points_de_vie_max; // Points de vie actuels au max
}

int generate_group(CreatureMarine group[], int max, int profondeur)
{
    if (max <= 0) return 0;
    if (max > CREATURES_MAX) max = CREATURES_MAX;

    int nombre_creatures = 1 + rand() % max;
    for (int i = 0; i < nombre_creatures; i++)
        init_creature(&group[i], TYPE_CREATURE_RANDOM, profondeur);

    return nombre_creatures;
}

void print_creature(const CreatureMarine *creature) {
    printf("[%d] %s | PV: %d | ATK: %d-%d | DEF: %d | VIT: %d | Effet: %s | Vivant: %d\n",
           creature->id, creature->nom,
           creature->points_de_vie_max,
           creature->attaque_minimale, creature->attaque_maximale,
           creature->defense, creature->vitesse,
           creature->effet_special, creature->est_vivant);
}

void print_group(const CreatureMarine group[], int count) {
    printf("Groupe de %d créature(s) générée(s) :\n", count);
    printf("----------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        print_creature(&group[i]);
    }
    printf("----------------------------------------------------\n");
}

const char* creature_symbol(const CreatureMarine *c)
{
    // On se base sur le nom mis par init_creature
    if (strcmp(c->nom, "Kraken") == 0) return "🐙";
    if (strcmp(c->nom, "Requin") == 0) return "🦈";
    return "❓";
}