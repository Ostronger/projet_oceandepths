# OceanDepths – Guide du projet

## Présentation

**OceanDepths** est un RPG textuel en C dans lequel vous incarnez un plongeur gérant vie, oxygène, fatigue et équipement pour explorer des zones de plus en plus profondes (0 à 500 m) et affronter des créatures marines jusqu’au Léviathan final. Le code est structuré en modules (`src/*.c` / `include/*.h`) et documenté dans `DOCUMENTATION.txt`.

## Préparation de l’environnement

- **Windows / PowerShell** : avant toute exécution, forcez la console en UTF‑8 pour que les emojis et accents s’affichent correctement :
  1. `chcp 65001`
  2. `$OutputEncoding = [Console]::OutputEncoding = [Text.UTF8Encoding]::new($false)`

- **Compilation commune** :
  ```bash
  gcc src/*.c -Iinclude -std=c11 -Wall -Wextra -g -finput-charset=UTF-8 -fexec-charset=UTF-8 -o projet_oceandepths
  ```

- **Exécution** :
  ```bash
  ./projet_oceandepths
  ```

- Un `Makefile.example` est fourni pour créer votre propre `Makefile` local (non versionné). Les utilisateurs Linux/macOS peuvent simplement faire `make run`.

## Arborescence

```
projet_oceandepths/
├── src/            # combat.c, creatures.c, inventaire.c, joueur.c, map.c, main.c, recompenses.c, sauvegarde.c, ui.c
├── include/        # headers correspondants
├── saves/          # fichier partie.save généré par le jeu
├── DOCUMENTATION.txt
├── PLANO_DE_ACAO.txt
├── README.md
└── Makefile.example
```

## Gameplay (résumé)

- **Exploration** : six lignes (surface → 500 m). Chaque zone génère 1 à 4 créatures ; une fois sécurisée, elle reste vide. Certaines profondeurs exigent des combinaisons spécifiques (upgrade automatique après la ligne 1).
- **Combats** : boucle 1vN avec gestion de fatigue, oxygène et compétences (Apnée, Décharge, Communication, Tourbillon). Les créatures possèdent des effets spéciaux (kraken double attaque, méduse paralysante, etc.).
- **Inventaire** : 8 slots d’objets consommables + harpons/combinaisons influençant ATK/DEF et coût d’O₂. Accessible en combat.
- **Surface** : repos complet, boutique (objets/upgrades), sauvegarde. En quittant la surface, on reprend à la profondeur précédente.
- **Récompenses** : perles et XP proportionnels à la profondeur + choix d’un bonus (objet ou upgrade). Perles servent à acheter équipements/consommables.
- **Sauvegarde** : disponible uniquement en surface, stockée dans `saves/partie.save`. Le chargement restaure le plongeur, son inventaire, la ligne actuelle et les zones sécurisées.

## Ressources utiles

- `DOCUMENTATION.txt` : détaille chaque module (.c/.h), les mécaniques et les instructions rapides.

Bon plongeon et n’oubliez pas vos capsules d’oxygène ! 😉
