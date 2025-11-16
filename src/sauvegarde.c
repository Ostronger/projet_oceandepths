#include "sauvegarde.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAVE_SIGNATURE_V1 "OCEANDEPTHS_SAVE_V1"
#define SAVE_SIGNATURE_V2 "OCEANDEPTHS_SAVE_V2"
#define SAVE_SIGNATURE_V3 "OCEANDEPTHS_SAVE_V3"

static void trim_newline(char *s) // supprime les retours à la ligne en fin de chaîne
{
    if (!s) return;
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
    {
        s[len - 1] = '\0';
        len--;
    }
}

int sauvegarder_partie(const char *chemin,
                       const Plongeur *plongeur,
                       const Inventaire *inventaire,
                       int ligne_actuelle,
                       const int zones_securisees[],
                       int nb_zones) // sauvegarde la partie dans un fichier
{
    if (!chemin || !plongeur || !inventaire)
        return 0;

    FILE *f = fopen(chemin, "w");
    if (!f)
        return 0;

    fprintf(f, "%s\n", SAVE_SIGNATURE_V3);
    fprintf(f, "LIGNE:%d\n", ligne_actuelle);
    fprintf(f, "PLONGEUR:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
            plongeur->points_de_vie,
            plongeur->points_de_vie_max,
            plongeur->niveau_oxygene,
            plongeur->niveau_oxygene_max,
            plongeur->niveau_fatigue,
            plongeur->perles,
            plongeur->attaque_minimale,
            plongeur->attaque_maximale,
            plongeur->defense,
            plongeur->cout_oxygene_attaque_eq,
            plongeur->cout_oxygene_passif_eq,
            plongeur->niveau,
            plongeur->experience,
            plongeur->experience_seuil);
    for (int i = 0; i < COMPETENCE_MAX; i++)
        fprintf(f, ":%d", plongeur->cooldowns_competences[i]);
    fprintf(f, "\n");

    fprintf(f, "EQUIPEMENT:%d:%d\n", inventaire->harpon_equipe, inventaire->combinaison_equipee);

    fprintf(f, "INVENTAIRE:%d", INVENTAIRE_TAILLE);
    for (int i = 0; i < INVENTAIRE_TAILLE; i++)
    {
        fprintf(f, ":%d:%d", inventaire->slots[i].type, inventaire->slots[i].quantite);
    }
    fprintf(f, "\n");

    if (zones_securisees && nb_zones > 0) // sauvegarde des zones sécurisées
    {
        fprintf(f, "ZONES:%d", nb_zones);
        for (int i = 0; i < nb_zones; i++)
        {
            int valeur = zones_securisees[i] ? 1 : 0;
            fprintf(f, ":%d", valeur);
        }
        fprintf(f, "\n");
    }

    fclose(f);
    return 1;
}

static int lire_entier(const char *token) // lit un entier depuis une chaîne de caractères
{
    if (!token) return 0;
    return (int)strtol(token, NULL, 10);
}

static void vider_slots(Inventaire *inventaire) // vide tous les slots de l'inventaire
{
    if (!inventaire) return;
    for (int i = 0; i < INVENTAIRE_TAILLE; i++)
    {
        inventaire->slots[i].type = OBJET_NONE;
        inventaire->slots[i].quantite = 0;
    }
}

int charger_partie(const char *chemin,
                   Plongeur *plongeur,
                   Inventaire *inventaire,
                   int *ligne_actuelle,
                   int zones_securisees[],
                   int nb_zones) // charge la partie depuis un fichier
{
    if (!chemin || !plongeur || !inventaire || !ligne_actuelle)
        return 0;

    FILE *f = fopen(chemin, "r");
    if (!f)
        return 0;

    char ligne[512];
    if (!fgets(ligne, sizeof(ligne), f))
    {
        fclose(f);
        return 0;
    }
    trim_newline(ligne);
    int version = 1;
    if (strcmp(ligne, SAVE_SIGNATURE_V3) == 0)
        version = 3;
    else if (strcmp(ligne, SAVE_SIGNATURE_V2) == 0)
        version = 2;
    else if (strcmp(ligne, SAVE_SIGNATURE_V1) != 0)
    {
        fclose(f);
        return 0;
    }

    if (!fgets(ligne, sizeof(ligne), f)) { fclose(f); return 0; }
    if (sscanf(ligne, "LIGNE:%d", ligne_actuelle) != 1)
    {
        fclose(f);
        return 0;
    }

    if (!fgets(ligne, sizeof(ligne), f)) { fclose(f); return 0; }
    char *token = strtok(ligne + 9, ":\n");
    int valeurs[14 + COMPETENCE_MAX];
    int count = 0;
    while (token && count < (14 + COMPETENCE_MAX))
    {
        valeurs[count++] = lire_entier(token);
        token = strtok(NULL, ":\n");
    }
    if (count < 14)
    {
        fclose(f);
        return 0;
    }

    plongeur->points_de_vie = valeurs[0];
    plongeur->points_de_vie_max = valeurs[1];
    plongeur->niveau_oxygene = valeurs[2];
    plongeur->niveau_oxygene_max = valeurs[3];
    plongeur->niveau_fatigue = valeurs[4];
    plongeur->perles = valeurs[5];
    plongeur->attaque_minimale = valeurs[6];
    plongeur->attaque_maximale = valeurs[7];
    plongeur->defense = valeurs[8];
    plongeur->cout_oxygene_attaque_eq = valeurs[9];
    plongeur->cout_oxygene_passif_eq = valeurs[10];
    plongeur->niveau = valeurs[11];
    plongeur->experience = valeurs[12];
    plongeur->experience_seuil = valeurs[13];
    plongeur->statuts.tours_paralysie = 0;
    plongeur->statuts.reduction_attaques = 0;
    joueur_reset_competences(plongeur);
    if (version == 2 && count >= 14 + COMPETENCE_MAX)
    {
        for (int i = 0; i < COMPETENCE_MAX; i++)
            plongeur->cooldowns_competences[i] = valeurs[14 + i];
    }
    joueur_recalcule_stats(plongeur);

    if (!fgets(ligne, sizeof(ligne), f)) { fclose(f); return 0; }
    int harpon = 0, combi = 0;
    if (sscanf(ligne, "EQUIPEMENT:%d:%d", &harpon, &combi) != 2)
    {
        fclose(f);
        return 0;
    }
    inventaire_equipe_harpon(inventaire, (TypeHarpon)harpon, plongeur);
    inventaire_equipe_combi(inventaire, (TypeCombinaison)combi, plongeur);

    if (!fgets(ligne, sizeof(ligne), f)) { fclose(f); return 0; }
    token = strtok(ligne, ":\n");
    int taille = lire_entier(token);
    vider_slots(inventaire);
    for (int i = 0; i < INVENTAIRE_TAILLE && i < taille; i++)
    {
        char *t_type = strtok(NULL, ":\n");
        char *t_qty = strtok(NULL, ":\n");
        TypeObjet type = t_type ? (TypeObjet)lire_entier(t_type) : OBJET_NONE;
        int qty = t_qty ? lire_entier(t_qty) : 0;
        inventaire->slots[i].type = type;
        inventaire->slots[i].quantite = qty;
    }

    for (int i = taille; i < INVENTAIRE_TAILLE; i++)
    {
        inventaire->slots[i].type = OBJET_NONE;
        inventaire->slots[i].quantite = 0;
    }

    if (zones_securisees && nb_zones > 0)
    {
        for (int i = 0; i < nb_zones; i++)
            zones_securisees[i] = 0;
    }

    if (version >= 3 && zones_securisees && nb_zones > 0)
    {
        if (fgets(ligne, sizeof(ligne), f))
        {
            trim_newline(ligne);
            char *zone_token = strtok(ligne, ":\n");
            if (zone_token && strcmp(zone_token, "ZONES") == 0)
            {
                zone_token = strtok(NULL, ":\n");
                int nb = zone_token ? lire_entier(zone_token) : 0;
                for (int i = 0; i < nb && i < nb_zones; i++)
                {
                    zone_token = strtok(NULL, ":\n");
                    int val = zone_token ? lire_entier(zone_token) : 0;
                    zones_securisees[i] = val ? 1 : 0;
                }
            }
        }
    }

    fclose(f);
    return 1;
}

