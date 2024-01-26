SwarmBots & Explobot
ProSE équipe A2 & Explobot PFE ESEO 2023-2024
Joshua MONTREUIL (joshua.montreuil@reseau.eseo.fr) & Thomas ROCHER
Version 0.0.1

# Installation Pré-requis

    Ce guide comporte deux parties, la première explique ce qu'il faut faire pour configurer le makefile pour une utilisation sur Raspberry Pi. La deuxième explique la configuration pour utiliser le makefile sur un pc de dev. 
    
    Si vous souhaitez pouvoir configurer le makefile dans les deux cas, appliquez les instructions des deux parties.
    
    -> Installez cmake
    -> Installez doxygen
    
    Etre en root toute l'installation

    WARNING : TOUS LES DOSSIERS A CREER SONT A CREER EN DEHORS DU REPERTOIRE DU PROJET ET EN DEHORS DU REPERTOIRE GIT DE L'EQUIPE.

## Configuration Makefile (utilisation sur Raspberry PI)

### Compilation croisée simple : (Pré-requis : un compte GitHub)

    Créez un dossier, et dans ce même dossier, exécutez la commande:

        $ git clone https://github.com/raspberrypi/tools.git

    Puis changez le chemin d'accès à ce répertoire dans le Makefile principal (à la ligne 11).

    Ensuite, changez le chemin vers le compilateur croisé dans le Makefile principal (à la ligne 16), suivez EXEMPLE (ligne 14).

### Compilation croisée bibliothèques tierces : (Pré-requis : une raspberryPi3B+)

    2 façons de faire :

	- Dans le même répertoire que précédemment créez un dossier "rootfs_bplus" et éxecutez la commande :
	
	  $ rsync -rl --delete-after --safe-links --copy-unsafe-links pi@<IP_de_la_PI>:/{lib,usr} <chemin_du_dossier_rootfs_bplus>

	-> <IP_de_la_PI> : 127.0.0.1 car sur réseau local lors du dev sinon vérifier sur la pi.
	-> <chemin_du_dossier_rootfs_bplus> : chemin du dossier créé précédemment.

    Puis changez le chemin d'accès à ce dossier sur le Makefile principal (à la ligne 22), suivez EXEMPLE1 (ligne 19).

	- Dans le répertoire de votre choix, créez un dossier "rootfs_bplus" et éxecutez la commande :

	  $ rsync -rl --delete-after --safe-links --copy-unsafe-links pi@<IP_de_la_PI>:/{lib,usr} <chemin_du_dossier_rootfs_bplus>

	-> <IP_de_la_PI> : 127.0.0.1 car sur réseau local lors du dev sinon vérifier sur la pi.
	-> <chemin_du_dossier_rootfs_bplus> : chemin du dossier créé précédemment.

    Puis changez le chemin d'accès à ce dossier sur le Makefile principal (à la ligne 22), suivez EXEMPLE2 (ligne 20).


# Lancement de la compilation

    De la même façon que pour la configuration du makefile, cette explication est en deux partie, pour la Raspberry Pi et pour le pc de dev.

    Remarque : Avant la compilation pour une nouvelle cible, lancez la commande :

        $ make clean

## Lancement de la compilation pour la cible raspberry Pi

    Dans le répertoire du projet où se situe le Makefile principal, lancez la commande :

        $ make TARGET=raspberry all

# Exécution du Programme principal

    De la même façon que pour le lancement de la compilation, cette explication est en deux parties, pour la Raspberry Pi et pour le pc de dev.

## Exécution du Programme principal pour la cible raspberry Pi

    Ici, on suppose qu'une connexion ssh entre le pc de développement et la Raspberry Pi est possible.

    Dans un premier temps, il faut copier l'exécutable sur la Raspberry Pi.

    Exécutez la commande :

        $ scp <nom_exécutable> pi@<IP de la Pi>:<répertoire de copie>

        -> <nom-exécutable> : swarm_bots_raspberry.elf. Si vous avez changé le Makefile, ce sera le nom que vous avez choisi.

        -> <IP de la Pi> ; IP de la Pi.

        -> <répertoire de copie> : répertoire où vous souhaitez mettre l'exécutable.

    Ensuite dans le <répertoire de copie> (sur la Raspberry Pi) :

        $ ./<nom-exécutable>


# Compilation de la documentation Doxygen

    Dans le répertoire où se situe le makefile principal, exécutez la commande :

        $ make documentation

    Cette commande génerera la documentation au format doxygen à partir de vos commentaires dans ce même format et à partir des options du fichier "Doxyfile".

    Vous pourrez consulter cette documentation dans le répertoire "doc" du projet, puis dans "html" et cliquez sur le "index.html".
