//// Fichier source de la classe CustomGraphicsView permettant de récupérer le clic sur la carte pour ajouter la position et le point de destination du robot
// Auteur : Simon FÉDÉLI (simon.fedeli@reseau.eseo.fr)
// Pour le projet ExploBot confié par Thales, dans le cadre du PFE 2023-2024 à l'ESEO.

#include "customgraphicsview.h"
#include <QDebug>

CustomGraphicsView::CustomGraphicsView(int pixels_on_screen_per_matrix_cell, QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent), pixels_on_screen_per_matrix_cell(pixels_on_screen_per_matrix_cell) {
}

void CustomGraphicsView::mousePressEvent(QMouseEvent *event) {
    QPointF scenePos = mapToScene(event->pos());

    // Conversion des coordonnées du clic sur la carte en coordonnées de la matrice (une case de matrice = 6 pixels, donc matrice = IHM / 6)
    int matrice_x = static_cast<int>(scenePos.x()) / pixels_on_screen_per_matrix_cell;      // Pour accéder à la variable pixels_on_screen_per_matrix_cell de la classe Carte depuis CustomGraphicsView, il existe une solution : passer pixels_on_screen_per_matrix_cell en tant que paramètre au constructeur de CustomGraphicsView. Pour cela il faut ajouter un constructeur à CustomGraphicsView pour prendre la taille de la case
    int matrice_y = static_cast<int>(scenePos.y()) / pixels_on_screen_per_matrix_cell;

    emit canvasClicked(matrice_y, matrice_x);       // On inverse x et y car en Qt, les coordonnées sont inversées par rapport à la matrice (d'où le static_cast)
    QGraphicsView::mousePressEvent(event);
}
