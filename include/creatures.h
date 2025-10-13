#ifndef CREATURES_H      // empêche d'inclure deux fois le fichier
#define CREATURES_H

#define CREATURES_MAX 4 // nombre maximum de crétaure générées en m^me temps

typedef enum {
    KRAKEN,
    REQUIN,
    TYPE_CREATURE_RANDOM = -1 // pour choisir aléatoirement le type de créature
} TypeCreature; // types de créatures possibles

typedef struct {
    int id;                         
    char nom[30];                   
    int points_de_vie_max;          
    int points_de_vie_actuels;      
    int attaque_minimale;           
    int attaque_maximale;           
    int defense;
    int vitesse;
    char effet_special[20];
    int est_vivant;
} CreatureMarine; // structure représentant une créature marine

// Prototype de la fonction pour initialiser le générateur de nombres aléatoires une seule fois
void seed_rng_once(void);
// Prototype de la fonction d'initialisation (permet de l'utiliser dans d'autres fichiers)
void init_creature(CreatureMarine *creature, TypeCreature typeCreature, int profondeur);
// Prototype de la fonction de génération de groupe de créatures
int generate_group(CreatureMarine group[], int max, int profondeur);

void print_creature(const CreatureMarine *creature);
void print_group(const CreatureMarine group[], int count);

const char* creature_symbol(const CreatureMarine *c);

#endif