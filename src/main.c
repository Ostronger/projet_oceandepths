#include <stdio.h>
#include <string.h>
#include <stdlib.h>      // rand()
#include "creatures.h"
#include "map.h"

// Matrice d'occupation: 0 = libre, 1 = occupée
static int occupied[MAP_HEIGHT][MAP_WIDTH];

// Grille d'icônes pour l'affichage graphique
static const char* icons[MAP_HEIGHT][MAP_WIDTH];


// Réinitialise la carte (tout libre, tout eau)
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


// y = ligne actuelle
// want = nb_colonnes_a_placer = combien de créatures on veut mettre sur cette ligne
// xs = colonnes_selectionnees = résultat final
static int pick_free_columns_on_line(int y, int want, int xs[])
{
    int free_cols[MAP_WIDTH]; // sert à stocker les colonnes libres
    int nfree = 0; // nombre de colonnes libres trouvées

    // Cherche les colonnes libres sur la ligne y
    for (int x = 0; x < MAP_WIDTH; x++)
    {
        if (occupied[y][x] == 0) {
            free_cols[nfree] = x;
            nfree++;
        }
    }

    if (nfree == 0) return 0; // aucune colonne libre sur cette ligne
    if (want > nfree) want = nfree; 

    // Mélange aléatoire de fisher-yates
    for (int i = nfree - 1; i > 0; i--)
    {
        int j = rand() % (i + 1); // indice aléatoire entre 0 et i
        int tmp = free_cols[i]; // échange free_cols[i] et free_cols[j]
        free_cols[i] = free_cols[j];
        free_cols[j] = tmp;
    }

    // Copie les 'want' premières colonnes mélangées dans xs[]
    for (int i = 0; i < want; i++)
        xs[i] = free_cols[i];

    return want;
}


// Affiche la carte des profondeurs (texte brut)
static void print_depth_map(const Map *carte)
{
    map_print(carte);
}


// Affiche la carte graphique finale (🌊 / 🐙 / 🦈)
static void print_icon_map(const Map *carte)
{
    printf("\n🌊=== CARTOGRAPHIE OCÉANIQUE ===🌊\n\n");

    int col_width = 4; // largeur d'une cellule
    int width = MAP_WIDTH;
    int height = MAP_HEIGHT;

    // ┌──┬──┐  ligne du haut
    printf("    ┌");
    for (int x = 0; x < width; x++) {
        for (int i = 0; i < col_width; i++) printf("─");
        if (x < width - 1) printf("┬");
    }
    printf("┐\n");

    // Lignes avec contenu
    for (int y = 0; y < height; y++)
    {
        int profondeur = map_get_depth(carte, 0, y); // profondeur de la ligne
        printf("y=%-2d│", y); // numéro de ligne

        for (int x = 0; x < width; x++) {
            const char *symbole = icons[y][x];
            if (strcmp(symbole, "🪼") == 0)
                printf(" %-5s │", symbole); // largeur 5 au lieu de 4
            else
                printf(" %-4s │", symbole);
    }

        printf(" %3dm\n", profondeur);

        // Ligne de séparation entre les rangées
        if (y < height - 1) {
            printf("    ├");
            for (int x = 0; x < width; x++) {
                for (int i = 0; i < col_width; i++) printf("─");
                if (x < width - 1) printf("┼");
            }
            printf("┤\n");
        }
    }

    // └──┴──┘  ligne du bas
    printf("    └");
    for (int x = 0; x < width; x++) {
        for (int i = 0; i < col_width; i++) printf("─");
        if (x < width - 1) printf("┴");
    }
    printf("┘\n");

    // Légende
    printf("\nLégende : 🐙 Kraken | 🦈 Requin | 🪼 Méduse | 🐟 Poisson-Épée | 🦀 Crabe Géant | %s Eau\n", MAP_ICON_WATER);
}



// =======================================================
// PROGRAMME PRINCIPAL
// =======================================================
int main(void)
{
    seed_rng_once(); // Initialise le générateur de nombres aléatoires une seule fois

    // 1️ Initialiser et afficher la carte des profondeurs
    Map carte;
    map_init(&carte);
    print_depth_map(&carte);

    // 2️ Préparer les structures d’occupation et d’icônes
    clear_occupied_and_icons();

    // 3️ Pour CHAQUE ligne : générer un groupe de créatures et les placer
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        int profondeur = map_get_depth(&carte, 0, y);
        CreatureMarine groupe[CREATURES_MAX]; 

        // Génère entre 1 et CREATURES_MAX créatures selon la profondeur
        int nb = generate_group(groupe, CREATURES_MAX, profondeur); // nb = nombre de créatures générées

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

    // Afficher la carte graphique finale (vue d'ensemble)
    print_icon_map(&carte);

    return 0;
}