#include <stdio.h>
#include <string.h>
#include <stdlib.h>      // rand()
#include "creatures.h"
#include "map.h"

// Matrice d'occupation: 0 = libre, 1 = occupée
static int occupied[MAP_HEIGHT][MAP_WIDTH];

// Grille d'icônes pour l'affichage graphique
static const char* icons[MAP_HEIGHT][MAP_WIDTH];


// 🔹 Réinitialise la carte (tout libre, tout 🌊)
static void clear_occupied_and_icons(void)
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            occupied[y][x] = 0;
            icons[y][x] = MAP_ICON_WATER; // icône d’eau par défaut
        }
    }
}


// 🔹 Sélectionne des colonnes libres distinctes sur une ligne (shuffle aléatoire)
static int pick_free_columns_on_line(int y, int want, int xs[])
{
    int free_cols[MAP_WIDTH];
    int nfree = 0;

    for (int x = 0; x < MAP_WIDTH; x++)
    {
        if (!occupied[y][x]) free_cols[nfree++] = x;
    }

    if (nfree == 0) return 0;
    if (want > nfree) want = nfree;

    // Mélange aléatoire (Fisher–Yates)
    for (int i = nfree - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int tmp = free_cols[i];
        free_cols[i] = free_cols[j];
        free_cols[j] = tmp;
    }

    for (int i = 0; i < want; i++)
        xs[i] = free_cols[i];

    return want;
}


// 🔹 Affiche la carte des profondeurs (texte brut)
static void print_depth_map(const Map *carte)
{
    map_print(carte);
}


// 🔹 Affiche la carte graphique finale (🌊 / 🐙 / 🦈)
static void print_icon_map(const Map *carte)
{
    printf("\n=== CARTE GRAPHIQUE (1 créature max par case) ===\n\n");

    // En-tête colonnes
    printf("      ");
    for (int x = 0; x < MAP_WIDTH; x++)
        printf("  x=%d ", x);
    printf("\n");

    // Chaque ligne = profondeur constante
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        int profondeur = map_get_depth(carte, 0, y);
        printf("y=%-3d ", y);
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            printf("  %s  ", icons[y][x]);
        }
        printf(" | %3dm\n", profondeur);
    }

    printf("\nLégende : 🐙 Kraken, 🦈 Requin, %s Eau\n", MAP_ICON_WATER);
}



// =======================================================
// 🧩 PROGRAMME PRINCIPAL
// =======================================================
int main(void)
{
    seed_rng_once();

    // 1️⃣ Initialiser et afficher la carte des profondeurs
    Map carte;
    map_init(&carte);
    print_depth_map(&carte);

    // 2️⃣ Préparer les structures d’occupation et d’icônes
    clear_occupied_and_icons();

    // 3️⃣ Pour CHAQUE ligne : générer un groupe de créatures et les placer
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        int profondeur = map_get_depth(&carte, 0, y);
        CreatureMarine groupe[CREATURES_MAX];

        // Génère entre 1 et CREATURES_MAX créatures selon la profondeur
        int nb = generate_group(groupe, CREATURES_MAX, profondeur);

        // Choisir des colonnes libres distinctes pour cette ligne
        int xs[CREATURES_MAX];
        int placed = pick_free_columns_on_line(y, nb, xs);

        printf("\n=== Répartition des créatures sur la ligne y=%d → %dm ===\n", y, profondeur);

        if (placed == 0)
        {
            printf("Aucune colonne libre sur cette ligne — aucune créature placée.\n");
            continue;
        }

        printf("On place %d créature(s) sur des colonnes distinctes :\n", placed);
        printf("----------------------------------------------------\n");

        for (int i = 0; i < placed; i++)
        {
            int x = xs[i];
            occupied[y][x] = 1;  // marquer la case (x,y) comme occupée
            icons[y][x] = creature_symbol(&groupe[i]); // placer l'icône 🐙 / 🦈

            printf("(x=%d, y=%d) ", x, y);
            print_creature(&groupe[i]);
        }

        printf("----------------------------------------------------\n");
    }

    // 4️⃣ Afficher la carte graphique finale (vue d'ensemble)
    print_icon_map(&carte);

    return 0;
}