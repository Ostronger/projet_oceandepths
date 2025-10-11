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