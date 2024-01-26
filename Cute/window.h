//// Fichier d'en-tête de la classe Window permettant de créer l'IHM (boutons, carte etc...)
// Auteur : Simon FÉDÉLI (simon.fedeli@reseau.eseo.fr) et Thomas ROCHER
// Pour le projet ExploBot confié par Thales, dans le cadre du PFE 2023-2024 à l'ESEO.

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include "map.h"
#include "client_tcp/proxyPilot.h"

class QPushButton;
class QHBoxLayout;
class QLabel;
class QGraphicsScene;
class QProcess;

class Map;

class Window : public QWidget
{
    Q_OBJECT  // Ajoutez cette macro pour utiliser les signals et slots (réactions des composants (ex : clic sur un bouton)

public:
    explicit Window(QWidget *parent = 0);
    void display_screen();

signals:

private slots:  // Ajoutez cette section pour déclarer des slots
    void click_on_cartography_button();
    void click_on_trajectory_button();
    void click_on_reset_button();
    void click_on_stop_button();
    void start_trajectory();

    void init_map();
    void display_map();
    void display_popup(int id_popup);
    void init_general_screen_paramaters();
    void init_image();
    void init_buttons();
    void add_components_to_screen();
    void run_python_code(QString python_file_name, QString argument_1, QString argument_2);

    void showSplashScreen();

    QString convertMatrixToString(int matrix[20][20], int rows);

    void call_make_trajectory();

    QVector<Command> convertStringToCommands(const QString &str);


private:
    QPushButton *m_button;
    QPushButton *button_cartography;
    QPushButton *button_trajectory;
    QPushButton *button_reset;
    QPushButton *button_stop;
    QPushButton *button_test;
    QHBoxLayout *mainLayout;
    QGraphicsScene *scene;
    QLabel      *topImage;
    QProcess *process;

    CustomGraphicsView *view;
    const int pixels_on_screen_per_matrix_cell = 30; // ou 6 pour une matrice 100x100. Nombre de pixels de l'écran pour afficher une case de la matrice

    int m_counter;          // Compteur permettant de fermer la fenêtre au bout de 3 fois

    // Map map;
    int (*matrix)[20]; // Pointeur vers la matrice de Map

    Map& myMap; // Référence vers l'instance de Map
};

#endif // WINDOW_H
