
// Module UI — affichage texte et lecture sécurisée
#ifndef UI_H
#define UI_H

#include "joueur.h"

typedef struct {
    const char *texte;
} OptionMenu;

void ui_barre(const char *label, int valeur, int maximum, int largeur);
void ui_affiche_etat(const Plongeur *plongeur);
void ui_menu_actions(const char *titre, const OptionMenu *options, int count);
int ui_lire_choix(int min, int max);

#endif // UI_H

