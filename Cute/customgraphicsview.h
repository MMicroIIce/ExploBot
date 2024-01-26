//// Fichier d'en-tête de la classe CustomGraphicsView permettant de récupérer le clic sur la carte pour ajouter la position et le point de destination du robot
// Auteur : Simon FÉDÉLI (simon.fedeli@reseau.eseo.fr)
// Pour le projet ExploBot confié par Thales, dans le cadre du PFE 2023-2024 à l'ESEO.

#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H
#include <QGraphicsView>
#include <QMouseEvent>

class CustomGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    using QGraphicsView::QGraphicsView;  // Utiliser le constructeur de QGraphicsView
    explicit CustomGraphicsView(int pixels_on_screen_per_matrix_cell, QGraphicsScene *scene, QWidget *parent = nullptr);

private:
    int pixels_on_screen_per_matrix_cell;

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void canvasClicked(int x, int y);
};

#endif // CUSTOMGRAPHICSVIEW_H
