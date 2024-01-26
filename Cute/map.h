//// Fichier d'en-tête de la classe Map permettant de créer et de modifier la matrice
// Auteur : Simon FÉDÉLI (simon.fedeli@reseau.eseo.fr)
// Pour le projet ExploBot confié par Thales, dans le cadre du PFE 2023-2024 à l'ESEO.

#ifndef MAP_H
#define MAP_H

#include <QWidget>
#include "customgraphicsview.h"

class CustomGraphicsView;
class QGraphicsScene;

class Map : public QWidget {
    Q_OBJECT

private:
    explicit Map(QWidget *parent = nullptr);

    CustomGraphicsView *view;
    QGraphicsScene *scene;

    static const int rows = 20; // Nombre de lignes de la matrice
    static const int cols = 20; // Nombre de colonnes de la matrice
    static const int number_of_possible_values_for_a_matrix_cell = 6;  // TODO : à modifier Nombre de valeurs différentes possible qu'une case de la matrice peut avoir

    int matrix[rows][cols];

    int point_de_destination_x;     // Si la valeur de ces variables est -1, alors cela signifie qu'elles ne sont pas encore affectées
    int point_de_destination_y;
    int robot_position_x;
    int robot_position_y;

    bool is_map_clickable = false;

    enum cell_value {
        obstacle = 0,
        cartographied = 1,
        non_cartographied = 2,
        waypoint = 3,
        destination = 4,
        robot_position = 5
    };

    void ajout_obstacle_carre(int topLeftX, int topLeftY, int size);
    void ajout_obstacle_rond(int centerX, int centerY, int radius);
    void ajout_obstacle_rectangulaire(int topLeftX, int topLeftY, int width, int height);
    void ajout_zone_non_cartographiee_carre(int topLeftX, int topLeftY, int size);
    void ajout_zone_non_cartographiee_rond(int centerX, int centerY, int radius);

public:
    static Map& getInstance() {
        static Map instance;
        return instance;
    }

    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;

    CustomGraphicsView* getCustomView() const;

    // Fonction pour retourner la matrice
    int (*get_map())[20];

    // Fonctions pour retourner les dimensions de la matrice
    static int getRows() { return rows; }
    static int getCols() { return cols; }
    int get_robot_position_x() { return robot_position_x; }
    int get_robot_position_y() { return robot_position_y; }
    static int get_number_of_possible_values_for_a_matrix_cell() { return number_of_possible_values_for_a_matrix_cell; }

    void enable_map_click(bool clickable); // Méthode pour mettre à jour l'état



public slots:
    void init_map();
    void update_matrix_element(int y, int x, int new_value);
    void set_obstacle(int y, int x);
    void set_cartographied_area(int y, int x);
    void set_non_cartographied_area(int y, int x);
    void update_entire_matrix(int new_matrix[rows][cols]); // si marche pas : tester en remplaçant rows et cols par 100
    void set_destination(int y, int x);
    void set_robot_position(int y, int x);
    void set_destination_and_robot_position(int y, int x);
    void set_waypoints(int waypoints_matrix[rows][cols]);    // La matrice contient des 0 et des 1, 1 pour les waypoints
    void set_waypoint(int y, int x);
    void reset_trajectory();
    void reset_trajectory_with_parameter(int waypoints_matrix[rows][cols]);
    void reset_map();

    void update_entire_matrix_from_trajectory_result(const QString &str);  // Met à jour la matrice avvec la matrice que retourne l'algorithme "Trajectory"


signals:
    void map_updated(); // Signal à émettre après que la matrix ait été modifiée pour mettre à jour son affichage sur l'écran
    void map_updated_after_destination_selection(); // Signal à émettre après que l'utilisateur à sélectionner un point de destination sur la carte
    void show_popup(int id_popup); // Signal à émettre pour afficher la popup d'erreur de sélection de point de destination
};

#endif // MAP_H
