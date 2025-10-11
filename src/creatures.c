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
    (void)profondeur; // Pour éviter un avertissement de variable non utilisée

    static int next_id = 1; // ID unique pour chaque créature
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

    creature->points_de_vie_actuels = creature->points_de_vie_max; // Points de vie actuels au max
    creature->defense = 10 + rand() % 6; // Défense entre 10 et 15
    creature->vitesse = 5  + rand() % 10;  // Vitesse entre 5 et 14
    strcpy(creature->effet_special, "aucun");
    creature->est_vivant = 1;
}