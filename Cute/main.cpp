//// Main de l'application
// Auteur : Simon FÉDÉLI (simon.fedeli@reseau.eseo.fr) et Thomas ROCHER
// Pour le projet ExploBot confié par Thales, dans le cadre du PFE 2023-2024 à l'ESEO.

#include <QApplication>
#include "window.h"

#include "client_tcp/dispatcher.h"
#include "client_tcp/postman.h"

int main(int argc, char **argv)
{
    qputenv("QT3D_RENDERER", "opengl");     // Forcer l'utilisation d'OpenGL pour le rendu de la vidéo

    QApplication app (argc, argv);          // Création de l'application Qt

    POSTMAN_create();           // Création du postman
    DISPATCHER_create();        // Création du dispatcher
    POSTMAN_start();            // Démarrage du postman
    DISPATCHER_start();         // Démarrage du dispatcher

    Window window;      // Création de la fenêtre de l'application
    window.show();      // Affichage de la fenêtre

    return app.exec();  // Exécution de l'application Qt
}
