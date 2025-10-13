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

Module creatures — Fiche technique

Rôle

Gérer la création et l’affichage des créatures marines (Kraken, Requin), avec des statistiques qui évoluent en fonction de la profondeur.

Fichiers
	•	include/creatures.h : types + prototypes publics
	•	src/creatures.c : implémentations
	•	src/main.c : exemple d’utilisation / tests

Structures & types

## Captures d'écran
[Insérer vos captures montrant le jeu en action,
pour éviter un malheureux "effet démo" le jour de la soutenance]
## Difficultés rencontrées
[Décrire les problèmes et solutions]

	•	Header introuvable → Ajouter -Iinclude dans CFLAGS.
	
    •	Fonction non déclarée (seed_rng_once) → Ajouter le prototype dans creatures.h.
	
    •	Type aléatoire (-1) → Ajouter TYPE_CREATURE_RANDOM dans l’enum et caster :
        typeCreature = (TypeCreature)(rand() % 2);
	
    •	Paramètre non utilisé (profondeur) → Temporaire : (void)profondeur;
	
    •	Switch incomplet → Ajouter un default: de sécurité.
	
    •	Hasard réinitialisé → Ne jamais appeler srand ailleurs que dans seed_rng_once().