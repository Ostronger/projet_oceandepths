#  Projet OceanDepths

##  Présentation

**OceanDepths** est un jeu en C où vous incarnez un plongeur explorant les profondeurs de l’océan à la recherche de trésors et de créatures marines.  
Le projet a pour but d’appliquer une architecture modulaire en C (fichiers `.c` / `.h`) et de travailler en équipe sur un jeu textuel interactif.

---

##  Structure du projet

projet_oceandepths/
├── src/               # Code source (.c)
│   └── main.c
├── include/           # Fichiers d’en-tête (.h)
├── Makefile           # Fichier local non versionné (chaque membre a le sien)
├── Makefile.example   # Modèle de référence pour l’équipe
├── .gitignore
└── README.md


# Configuration du Makefile (multi-systèmes)

**Utilisateurs macOS / Linux**

> Vous pouvez utiliser directement le Makefile classique :

    make run

> Cela compile et exécute automatiquement le programme :

    ./projet_oceandepths

**Utilisateurs Windows**

Si vous êtes sous Windows, procédez ainsi :
	1.	Copiez le fichier modèle :
    cp Makefile.example Makefile

>(ou simplement copier-coller et renommer depuis l’explorateur de fichiers)

    2.	Ouvrez Makefile dans un éditeur de texte et adaptez :
	•	CC = gcc → assurez-vous que MinGW ou TDM-GCC est installé
	•	remplacez les commandes rm par del si vous utilisez CMD
	
    3.	Compilez avec :

    make run

ou, si make n’est pas installé, utilisez directement :

gcc src\main.c -o projet_oceandepths.exe
.\projet_oceandepths.exe

**Remarques**
	•	Le fichier Makefile n’est pas versionné (ajouté au .gitignore) pour permettre à chacun d’avoir sa propre configuration locale.
	•	Le fichier Makefile.example est le modèle officiel partagé du projet.
	•	Si vous rencontrez un problème de compilation, comparez votre Makefile au modèle.

---

##  Gestion des branches Git

Pour assurer un développement organisé et collaboratif, le projet OceanDepths utilise une structure de branches inspirée du workflow **Git Flow**.

###  Structure des branches

| Branche | Rôle | Accès |
|----------|------|--------|
| **main** | Contient la version stable du jeu (toujours prête à livrer) | Lecture / Merge final uniquement |
| **develop** | Branche principale de développement, où toutes les nouvelles fonctionnalités sont intégrées et testées avant d’être fusionnées dans `main` | Écriture (développeurs) |
| **feature/*** | Branches temporaires pour le développement d’une fonctionnalité spécifique (ex : `feature/combat-system`) | Écriture (auteur de la feature) |

---

### Création de la branche `develop`

`bash
git checkout main
git pull origin main
git checkout -b develop
git push -u origin develop

---

## Workflow Git – Récapitulatif rapide

> Voici la méthode standard utilisée par l’équipe pour collaborer proprement sur le projet OceanDepths.

1. Créer une nouvelle branche à partir de `develop` :  
   ```bash
   git checkout develop
   git checkout -b feature/nom-de-la-feature

2. Coder, tester, puis valider les changements :

git add .
git commit -m "Ajout de la fonctionnalité : nom-de-la-feature"
git push -u origin feature/nom-de-la-feature

3. Fusionner la feature terminée dans develop :

git checkout develop
git merge feature/nom-de-la-feature
git push origin develop

4. Quand la version est stable, fusionner develop → main :

git checkout main
git merge develop
git push origin main

5.	Nettoyer les branches terminées :

git branch -d feature/nom-de-la-feature
git push origin --delete feature/nom-de-la-feature

## Structure du code

	•	creatures.h / creatures.c → gestion et génération des créatures
	•	map.h / map.c → carte océanique et profondeurs
	•	main.c → affichage, répartition des créatures et carte graphique


## Fonction importante 

### Fonction seed_rng_once()

Cette fonction initialise le générateur de nombres aléatoires une seule fois dans tout le programme.
Elle encapsule la commande standard srand(time(NULL)) et utilise une variable static pour éviter de réinitialiser le hasard à chaque appel.

 Objectif : garantir un vrai comportement aléatoire tout en gardant la cohérence du tirage.