# Progression OceanDepths
## Étapes réalisées
- [x] Étape 1 : Génération créatures
- [ ] Étape 2 : Attaque joueur
- [ ] Étape 3 : Attaque créatures
- [ ] Étape 4 : Récompenses
...
## progression réalisés

## Étape 1 — Génération des créatures (résumé)

### Ce que fait le code
- Initialise le **hasard une seule fois** (`seed_rng_once()`).
- Crée une **créature** (`init_creature`) : `KRAKEN` ou `REQUIN` (ou **aléatoire** avec `TYPE_CREATURE_RANDOM`).
- Affecte des **stats plausibles** (PV, ATK min/max, DEF, VIT), puis affiche le résultat dans `main.c`.

### Fichiers
- `include/creatures.h` : struct `CreatureMarine`, enum `TypeCreature`, prototypes.
- `src/creatures.c` : implémentations `seed_rng_once`, `init_creature`.
- `src/main.c` : test simple (génère 1 créature et l’affiche).
- `Makefile` : compile avec `-Iinclude`.

### Module creatures — Fiche technique

Rôle

Gérer la création et l’affichage des créatures marines (Kraken, Requin), avec des statistiques qui évoluent en fonction de la profondeur.

Fichiers
	•	include/creatures.h : types + prototypes publics
	•	src/creatures.c : implémentations
	•	src/main.c : exemple d’utilisation / tests

Structures & types

## Objectif

Mettre en place un système de génération aléatoire de créatures marines selon la profondeur, avec évolution de leurs caractéristiques et répartition dans un tableau fixe.

### Fonctionnalités implémentées

**5 types de créatures :**

- Kraken : tank offensif, lent, rare en surface
- Requin : rapide, équilibré, plus fréquent.
- Méduse : fragile, attaque paralysante.
- Poisson-Épée : équilibré
- Crabe Géant : défensif, lent, très solide.

**Tirage pondéré selon la profondeur**

- En surface(0-100 m) : méduse et poissons-épée dominent.
- En moyenne profondeur (200 - 300 m) : plus de requis et crabes.
- En grande profondeur (400-500 m) : monstres puissants (kraken).

**Evolution des statistiques :**

PV, attaque, défense et vitesse augmentent progressievement avec la profondeur (paliers de 100 m).

**Gestion du groupe :**

Géneration de 1 à 4 créatures selon la profondeur.

**Tableau fixe avec "slots vides" :**

Toutes les cases sont initialisées, même celles non utilisées.



## Captures d'écran
[Insérer vos captures montrant le jeu en action,
pour éviter un malheureux "effet démo" le jour de la soutenance]
## Difficultés rencontrées
[Décrire les problèmes et solutions]

	•	Header introuvable → Ajouter -Iinclude dans CFLAGS du Makefile pour que le compilateur trouve les fichiers .h.
	
    •	Fonction non déclarée (seed_rng_once) → Ajouter le prototype dans creatures.h.
	
    •	Type aléatoire (-1) → Ajouter TYPE_CREATURE_RANDOM dans l’enum et caster :
        typeCreature = (TypeCreature)(rand() % 2);
	
    •	Paramètre non utilisé (profondeur) → Temporaire : (void)profondeur;
	
    •	Switch incomplet → Ajouter un default: de sécurité.
	
    •	Hasard réinitialisé → Ne jamais appeler srand ailleurs que dans seed_rng_once().

    •	Faire évoluer les stats sans complexité → utilisation d’un palier k = profondeur / 100 pour ajuster PV, ATK, DEF, VIT.
	
    •	Trop de Krakens en surface → ajout d’un tirage pondéré (rand() % 100) selon la profondeur.
	
    •	Profondeur non transmise → passage explicite du paramètre profondeur aux fonctions de génération.
	
    •	Grille décalée à cause des emojis → ajustement de la largeur des cellules (col_width).
	
    •	Emplacements non utilisés non gérés → réinitialisation complète du tableau avant génération.

