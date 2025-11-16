
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int clamp(int value, int min, int max) // borne une valeur entre min et max
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void ui_barre(const char *label, int valeur, int maximum, int largeur) // affiche une barre de progression dans la console
{
    if (maximum <= 0) maximum = 1;
    if (largeur <= 0) largeur = 30;

    valeur = clamp(valeur, 0, maximum);
    int remplis = (valeur * largeur + maximum - 1) / maximum;
    if (remplis > largeur) remplis = largeur;

    printf("%-10s [", label ? label : "");
    for (int i = 0; i < largeur; i++)
        putchar(i < remplis ? '#' : '-');
    printf("] %d/%d\n", valeur, maximum);
}

void ui_affiche_etat(const Plongeur *plongeur) // affiche l'état du plongeur (vie, oxygène, fatigue, etc.)
{
    if (!plongeur) return;

    printf("\n==== ÉTAT DU PLONGEUR ====\n");
    ui_barre("Vie", plongeur->points_de_vie, plongeur->points_de_vie_max, 30);
    ui_barre("Oxygéne", plongeur->niveau_oxygene, plongeur->niveau_oxygene_max, 30);
    ui_barre("Fatigue", JOUEUR_FATIGUE_MAX - plongeur->niveau_fatigue,
             JOUEUR_FATIGUE_MAX, JOUEUR_FATIGUE_MAX);
    printf("Perles    : %d\n", plongeur->perles);
    if (plongeur->experience_seuil > 0)
        printf("Niveau    : %d (%d/%d XP)\n", plongeur->niveau, plongeur->experience, plongeur->experience_seuil);
    else
        printf("Niveau    : %d (MAX)\n", plongeur->niveau);
    printf("Attaque   : %d-%d | Défense : %d\n",
           plongeur->attaque_minimale,
           plongeur->attaque_maximale,
           plongeur->defense);
    printf("===========================\n");
}

void ui_menu_actions(const char *titre, const OptionMenu *options, int count) // affiche un menu d'options dans la console
{
    if (titre)
        printf("\n%s\n", titre);

    for (int i = 0; i < count; i++)
    {
        const char *texte = options && options[i].texte ? options[i].texte : "Option";
        printf("%d - %s\n", i + 1, texte);
    }
}

static char *trim(char *str) // supprime les espaces en début et fin de chaîne
{
    if (!str) return str;
    char *end;
    while (*str && isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

int ui_lire_choix(int min, int max) // lit un choix numérique de l'utilisateur entre min et max
{
    if (min > max)
    {
        int tmp = min;
        min = max;
        max = tmp;
    }

    char buffer[64];
    for (;;)
    {
        printf("> ");
        fflush(stdout);
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            clearerr(stdin);
            continue;
        }

        char *trimmed = trim(buffer);
        if (*trimmed == '\0')
        {
            printf("Entrée vide. Merci d'indiquer un nombre entre %d et %d.\n", min, max);
            continue;
        }

        char *endptr = NULL;
        long valeur = strtol(trimmed, &endptr, 10);
        if (endptr == trimmed || *trim(endptr) != '\0')
        {
            printf("Entrée invalide. Merci de saisir un nombre entre %d et %d.\n", min, max);
            continue;
        }

        if (valeur < min || valeur > max)
        {
            printf("Choix hors limites (%d-%d).\n", min, max);
            continue;
        }

        return (int)valeur;
    }
}

