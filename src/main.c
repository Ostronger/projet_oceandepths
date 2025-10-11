#include <stdio.h>
#include "creatures.h"

int main(void) {
    // Initilialisation du générateur de nombres aléatoires
    seed_rng_once();

    CreatureMarine creature;
    init_creature(&creature, TYPE_CREATURE_RANDOM, 0); // -1 pour type

    // Affichage des informations de la créature
    printf("ID: %d\n", creature.id);
    printf("Nom: %s\n", creature.nom);
    printf("Points de vie max: %d\n", creature.points_de_vie_max);
    printf("Points de vie actuels: %d\n", creature.points_de_vie_actuels);
    printf("Attaque minimale: %d\n", creature.attaque_minimale);
    printf("Attaque maximale: %d\n", creature.attaque_maximale);
    printf("Défense: %d\n", creature.defense);
    printf("Vitesse: %d\n", creature.vitesse);
    printf("Effet spécial: %s\n", creature.effet_special);
    printf("Est vivant: %d\n", creature.est_vivant);

    return 0;
}