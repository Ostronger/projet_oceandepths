// Module Joueur — gestion des ressources vitales du plongeur
#ifndef JOUEUR_H
#define JOUEUR_H

#define JOUEUR_FATIGUE_MIN 0
#define JOUEUR_FATIGUE_MAX 5
#define JOUEUR_OXYGENE_CRITIQUE 10
#define JOUEUR_SUFFOCATION_PV 5

typedef enum {
    COMP_APNEE_PROLONGEE = 0,
    COMP_DECHARGE_ELECTRIQUE,
    COMP_COMMUNICATION_MARINE,
    COMP_TOURBILLON_AQUATIQUE,
    COMPETENCE_MAX
} TypeCompetence;

typedef struct {
    int tours_paralysie;          // nombre de tours restants de paralysie
    int reduction_attaques;       // réduction d'attaques pour le prochain tour
} StatutsJoueur;

typedef struct {
    int points_de_vie;
    int points_de_vie_max;
    int niveau_oxygene;
    int niveau_oxygene_max;
    int niveau_fatigue;           
    int perles;
    int attaque_minimale;
    int attaque_maximale;
    int defense;
    int niveau;
    int experience;
    int experience_seuil;
    int cout_oxygene_attaque_eq;
    int cout_oxygene_passif_eq;
    StatutsJoueur statuts;        // effets temporaires
    int cooldowns_competences[COMPETENCE_MAX];
} Plongeur;

void joueur_init(Plongeur *plongeur);
void joueur_reset_statuts(Plongeur *plongeur);
void joueur_reset_competences(Plongeur *plongeur);
void joueur_consomme_oxygene(Plongeur *plongeur, int cout);
void joueur_ajoute_fatigue(Plongeur *plongeur, int delta);
void joueur_recupere_fatigue(Plongeur *plongeur, int delta);
int joueur_est_en_vie(const Plongeur *plongeur);
int joueur_est_oxygene_critique(const Plongeur *plongeur);
int joueur_applique_suffocation(Plongeur *plongeur);
int joueur_cout_attaque_normale(const Plongeur *plongeur, int profondeur);
int joueur_cout_competence(int profondeur);
int joueur_cout_consommation_passive(const Plongeur *plongeur, int profondeur);
const char *joueur_message_alerte_oxygene(const Plongeur *plongeur);
void joueur_gagner_experience(Plongeur *plongeur, int experience);
void joueur_recalcule_stats(Plongeur *plongeur);
float joueur_multiplicateur_niveau(const Plongeur *plongeur);
void joueur_tick_competences(Plongeur *plongeur);
int joueur_cooldown_restant(const Plongeur *plongeur, TypeCompetence competence);
void joueur_demarre_cooldown(Plongeur *plongeur, TypeCompetence competence, int tours);

#endif // JOUEUR_H
