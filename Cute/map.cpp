//// Fichier source de la classe Map permettant de créer et de modifier la matrice
// Auteur : Simon FÉDÉLI (simon.fedeli@reseau.eseo.fr)
// Pour le projet ExploBot confié par Thales, dans le cadre du PFE 2023-2024 à l'ESEO.

#include "map.h"
#include <QDebug>           // qDebug() permet d'afficher des messages dans la console
#include <QPushButton>
#include <QApplication>
#include <QProgressBar>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QtWidgets>
#include <QPixmap>
#include <QString>
#include <QStringList>

// Constructeur de la classe Map
Map::Map(QWidget *parent)
    : QWidget(parent),
    point_de_destination_x(-1),
    point_de_destination_y(-1),
    robot_position_x(-1),
    robot_position_y(-1)
{
    init_map();     // Initialise la matrice en la remplissant de zones non cartographiées
}

// Méthode initialisant la matrice en la remplissant de zones non cartographiées
void Map::init_map() {
    // Initialisation de la matrice à zéro
    for (int i = 0; i < rows; ++i) {            // Pour chaque ligne de la matrice
        for (int j = 0; j < cols; ++j) {            // Pour chaque colonne de la matrice
            matrix[i][j] = non_cartographied;           // Met la case de la matrice en "non cartographiée"
        }
    }
}

// Méthode pour retourner la matrice
int (*Map::get_map())[20] {
    return matrix;
}

// Pour obtenir la view dans le code window.cpp et l'ajouter sur l'IHM
CustomGraphicsView* Map::getCustomView() const {
    return view;    // 'view' est l'instance de CustomGraphicsView dans Carte
}

// Méthode pour modifier une case de la matrice en lui donnant une valeur spécifique. 0 : zone non cartographiée, 1 : zone cartographiée, 2 : zone avec un obstacle
void Map::update_matrix_element(int y, int x, int new_value) {
    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        this->matrix[y][x] = new_value;
        // Met à jour la carte sur l'IHM
        emit map_updated();
    }
}

// Méthode pour ajouter un obstacle dans la matrice
void Map::set_obstacle(int y, int x) {
    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        this->matrix[y][x] = obstacle;
        // Met à jour la carte sur l'IHM
        emit map_updated();
    }
}

// Méthode pour ajouter une case cartographiée dans la matrice
void Map::set_cartographied_area(int y, int x) {
    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        this->matrix[y][x] = cartographied;
        // Met à jour la carte sur l'IHM
        emit map_updated();
    }
}

// Méthode pour ajouter une case non cartographiée dans la matrice
void Map::set_non_cartographied_area(int y, int x) {
    if (x >= 0 && x < cols && y >= 0 && y < rows) {
        this->matrix[y][x] = non_cartographied;
        // Met à jour la carte sur l'IHM
        emit map_updated();
    }
}

// Méthode pour modifier toute la matrice d'un coup en lui donnant une nouvelle matrice (à tester)
void Map::update_entire_matrix(int new_matrix[20][20]) {
    // Remplace la matrice par la nouvelle matrice
    memcpy(matrix, new_matrix, sizeof(int) * rows * cols);

    // Met à jour la carte sur l'IHM
    emit map_updated();
}

// Méthdoe permettant de modifier la matrice en la remplaçant par la matrice obtenue par le code Python trajectory.py
void Map::update_entire_matrix_from_trajectory_result(const QString &waypoints_string2) {

    QString waypoints_string = waypoints_string2;               // Pour éviter d'avoir une erreur avec les disqualifiers

    waypoints_string.replace("{{", "").replace("}}", "");       // Enlever les accolades extérieures
    QStringList waypoints_qstringlist = waypoints_string.split("},{", Qt::SkipEmptyParts);  // Diviser en lignes

    int matrix_converted[20][20];   // Matrice temporaire pour convertir la matrice de QString en matrice d'entiers

    for (int i = 0; i < waypoints_qstringlist.size(); ++i) {
        QStringList nums = waypoints_qstringlist[i].split(',', Qt::SkipEmptyParts); // Diviser en nombres

        for (int j = 0; j < nums.size(); ++j) {
            matrix_converted[i][j] = nums[j].trimmed().toInt();     // Convertir en entier
        }
    }

    // Remplace la matrice par la nouvelle matrice récupérée ci-dessus
    memcpy(matrix, matrix_converted, sizeof(int) * rows * cols);

    // Met à jour la carte sur l'IHM
    emit map_updated();
}


// Méthodes permettant d'ajouter des obstacles et zones non cartographiées
void Map::ajout_zone_non_cartographiee_carre(int topLeftX, int topLeftY, int size) {
    for (int i = topLeftX; i < topLeftX + size; ++i) {
        for (int j = topLeftY; j < topLeftY + size; ++j) {
            this->matrix[i][j] = non_cartographied;
        }
    }
    // Met à jour la carte sur l'IHM
    emit map_updated();
}
void Map::ajout_zone_non_cartographiee_rond(int centerX, int centerY, int radius) {
    for (int i = centerX - radius; i <= centerX + radius; ++i) {
        for (int j = centerY - radius; j <= centerY + radius; ++j) {
            if ((i - centerX) * (i - centerX) + (j - centerY) * (j - centerY) <= radius * radius) {
                this->matrix[i][j] = non_cartographied;
            }
        }
    }
    // Met à jour la carte sur l'IHM
    emit map_updated();
}
void Map::ajout_obstacle_carre(int topLeftX, int topLeftY, int size) {
    for (int i = topLeftX; i < topLeftX + size; ++i) {
        for (int j = topLeftY; j < topLeftY + size; ++j) {
            this->matrix[i][j] = obstacle;
        }
    }
    // Met à jour la carte sur l'IHM
    emit map_updated();
}
void Map::ajout_obstacle_rond(int centerX, int centerY, int radius) {
    for (int i = centerX - radius; i <= centerX + radius; ++i) {
        for (int j = centerY - radius; j <= centerY + radius; ++j) {
            if ((i - centerX) * (i - centerX) + (j - centerY) * (j - centerY) <= radius * radius) {
                this->matrix[i][j] = obstacle;
            }
        }
    }
    // Met à jour la carte sur l'IHM
    emit map_updated();
}
void Map::ajout_obstacle_rectangulaire(int topLeftX, int topLeftY, int width, int height) {
    for (int i = topLeftX; i < topLeftX + height; ++i) {
        for (int j = topLeftY; j < topLeftY + width; ++j) {
            this->matrix[i][j] = obstacle;
        }
    }
}

// Méthode permettant de rendre la carte cliquable afin que l'utilisateur puisse sélectionner un point de départ et de destination pour le robot
void Map::enable_map_click(bool clickable) {
    is_map_clickable = clickable;
}

// Méthode permettant le set le point de position du robot
void Map::set_robot_position(int y, int x) {
    // Modifie la matrice pour ajouter le point de position
    if (x >= 0 && x < cols && y >= 0 && y < rows) {

        // Supprime l'ancienne position du robot
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (matrix[i][j] == robot_position) {           // Remplace toutes les cases position du robot par des cases position du robot
                    matrix[i][j] = cartographied;
                }
            }
        }

        // Enregistre dans une variable les coordonnées de position du robot
        robot_position_x = x;
        robot_position_y = y;

        // Set la position du robot dans la matrice
        matrix[y][x] = robot_position;

        // Met à jour la carte sur l'IHM
        emit map_updated();
    }
    else
    {
        // La position du robot dépasse les bordures de la matrice.
    }
}

// Méthode permettant le set du point de destination
void Map::set_destination(int y, int x) {
    // Supprime l'ancien point de destination
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (matrix[i][j] == destination) {           // Remplace toutes les cases vertes (destination) par des blanches (zones cartographiées)
                matrix[i][j] = cartographied;
            }
        }
    }

    // Enregistre dans une variable les coordonnées du point de destination
    point_de_destination_x = x;
    point_de_destination_y = y;

    // Set la position du robot dans la matrice
    matrix[y][x] = destination;

    // Met à jour la carte sur l'IHM
    emit map_updated();

    // Met à jour la carte sur l'IHM
    emit map_updated_after_destination_selection();
}

// Méthode appelée lorsque l'utilisateur clique sur la carte
// Elle est connectée à la fonction "canvasClicked" de la classe Window
// Cette méthode permet de set le point de destination et la position du robot
void Map::set_destination_and_robot_position(int x, int y) {
    if (is_map_clickable) {         // Vérifie que l'utilisateur est bien autorisé à cliquer sur la carte (évite de changer le point de destination en cours de route si l'utilisateur n'a pas cliqué au préablable sur le bouton "Trajectoire"
        if (matrix[x][y] == 1) {        // Vérification que le point sélectionné est bien accessible par le robot (que ce ne soit pas une zone non cartographiée ou un obstacle)

            if (robot_position_x == -1 && robot_position_y == -1) {     // Au premier clic sur la carte c'est le point de position qui doit être défninit. Si la position du robot est n'a pas encore été définie, alors c'est elle qu'il faut définir en premier
                set_robot_position(x, y);
            }
            else if (point_de_destination_x == -1 && point_de_destination_y == -1) {    // Sinon, si la position du robot est déjà définie, cela signifie que c'est le point de destination qu'il faut définir
                enable_map_click(false);        // Désactive la possibilité de cliquer sur la carte pour ne pas changer le point de destination en cours de route (sans avoir cliqué sur "Trajectoire" avant etc...)
                set_destination(x, y);
            }
        }
        else {
            // Zone inaccessible pour le robot. Affiche un popup
            emit show_popup(4);    // Signal connecté à la fonction display_popup dans Window.cpp
        }
    }
}

// Méthode permettant d'ajouter des waypoints à la matrice à partir d'une matrice constituée de 0 et de 1
void Map::set_waypoints(int waypoints_matrix[20][20]) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // Si la case est un waypoint dans la matrice passée en paramètre...
            if (waypoints_matrix[i][j] == 1)
            {
                // ...Et qu'elle n'est pas un obstacle, ni la position du robot ni sa destination dans la matrice de la carte
                if (matrix[i][j] == cartographied && matrix[i][j] != obstacle && matrix[i][j] != robot_position && matrix[i][j] != destination) {
                    // Alors ajoute le waypoint à la matrice
                    matrix[i][j] = waypoint;
                }
                else {
                    // Le waypoint ne peut pas être ajouté car la zone est inaccessible
                }
            }
        }
    }
    // Met à jour la carte sur l'IHM
    emit map_updated();
}

// Méthode permettant d'ajouter un waypoint à la matrice
void Map::set_waypoint(int y, int x)
{
    // Si les coordonnées ne sont pas un obstacle, ni la position du robot ni sa destination
    if (matrix[x][y] == cartographied && matrix[x][y] != obstacle && matrix[x][y] != robot_position && matrix[x][y] != destination) {
        // Alors ajoute le waypoint à la matrice
        matrix[x][y] = waypoint;
    }
    else {
        // Le waypoint ne peut pas être ajouté car la zone est inaccessible
    }
    // Met à jour la carte sur l'IHM
    emit map_updated();
}

// Méthode permettant de supprimer les waypoints de la matrice en passant la matrice des waypoints à enlever en paramètre
void Map::reset_trajectory_with_parameter(int waypoints_matrix[20][20]) {
    // 1) Supprime les waypoints
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // Si la case est un waypoint dans la matrice passée en paramètre...
            if (waypoints_matrix[i][j] == 1)
            {
                // ...Et que c'est bien un waypoint, qu'elle n'est pas un obstacle, ni la position du robot ni sa destination dans la matrice de la carte
                if (matrix[i][j] == waypoint && matrix[i][j] != obstacle && matrix[i][j] != robot_position && matrix[i][j] != destination) {
                    // Alors ajoute le waypoint à la matrice
                    matrix[i][j] = cartographied;
                }
                else {
                    // Le waypoint ne peut pas être ajouté car la zone est inaccessible
                }
            }
        }
    }

    // 2) Supprime le point de destination
    for (int i = 0; i < rows; ++i) {                // Parcours la matrice à la recherche de la destination...
        for (int j = 0; j < cols; ++j) {
            if (matrix[i][j] == destination) {
                matrix[i][j] = cartographied;   // Pour le supprimer et le rempplacer par des cases cartographiées
            }
        }
    }

    // 3) Reset les variables des coordonnées du point de destination
    point_de_destination_x = -1;
    point_de_destination_y = -1;

    // Met à jour la carte sur l'IHM
    emit map_updated();
}

// Méthode permettant de réinitialiser la trajectoire du robot
void Map::reset_trajectory() {

    // 1) Supprime les waypoints
    for (int i = 0; i < rows; ++i) {                // Parcours la matrice à la recherche de waypoints...
        for (int j = 0; j < cols; ++j) {
                if (matrix[i][j] == waypoint) {
                    matrix[i][j] = cartographied;   // Pour les supprimer et les rempplacer par des cases cartographiées
                }
            }
        }

    // 2) Supprime la position du robot
    for (int i = 0; i < rows; ++i) {                // Parcours la matrice à la recherche de la destination...
        for (int j = 0; j < cols; ++j) {
            if (matrix[i][j] == robot_position) {
                    matrix[i][j] = cartographied;   // Pour le supprimer et le rempplacer par des cases cartographiées
            }
        }
    }

    // 3) Supprime le point de destination
    for (int i = 0; i < rows; ++i) {                // Parcours la matrice à la recherche de la destination...
        for (int j = 0; j < cols; ++j) {
            if (matrix[i][j] == destination) {
                matrix[i][j] = cartographied;   // Pour le supprimer et le rempplacer par des cases cartographiées
            }
        }
    }

    // 4) Reset les variables des coordonnées du point de destination
    robot_position_x = -1;
    robot_position_y = -1;

    // 5) Reset les variables des coordonnées de position du robot
    point_de_destination_x = -1;
    point_de_destination_y = -1;

    // Met à jour la carte sur l'IHM
    emit map_updated();
}

// Méthode permettant de réinitialiser la carte
void Map::reset_map() {
    // 1) Met toutes les cases de la map à non_cartographied
    init_map();

    // 2) Reset les variables des coordonnées du point de destination
    point_de_destination_x = -1;
    point_de_destination_y = -1;

    // 3) Reset les variables des coordonnées de position du robot
    robot_position_x = -1;
    robot_position_y = -1;

    // Met à jour la carte sur l'IHM
    emit map_updated();
}
