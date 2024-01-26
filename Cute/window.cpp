//// Fichier source de la classe Window permettant de créer l'IHM (boutons, carte etc...)
// Auteur : Simon FÉDÉLI (simon.fedeli@reseau.eseo.fr) et Thomas ROCHER
// Pour le projet ExploBot confié par Thales, dans le cadre du PFE 2023-2024 à l'ESEO.

#include "window.h"
#include "map.h"

#include "client_tcp/dispatcher.h"
#include "client_tcp/postman.h"

#include <unistd.h>

#include <QDebug>               // qDebug() permet d'afficher des messages dans la console
#include <QPushButton>
#include <QApplication>
#include <QProgressBar>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>


#include <QtWidgets>
#include <QPixmap>

#include <QProcess>

#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>

#include <QMovie>
#include <QLabel>

#include <QString>
#include <QStringList>

/* ----------------------  Classe interne EscapeKeyPressFilter  -------------------------- */

// Classe permettant de passer la vidéo d'introduction
class EscapeKeyPressFilter : public QObject {
    Window *window; // Pointeur vers Window
    QMediaPlayer *player; // Pointeur vers QMediaPlayer

// Constructeur de la classe
public:
    EscapeKeyPressFilter(Window *window, QMediaPlayer *player)
        : window(window), player(player) {}

// Méthode permettant de gérer les événements clavier
protected:
    bool eventFilter(QObject *watched, QEvent *event) override {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Escape || keyEvent->key() == Qt::Key_Space || keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                QVideoWidget *videoWidget = qobject_cast<QVideoWidget *>(watched); // Cast de watched en QVideoWidget
                if (videoWidget) {
                    player->stop(); // Arrêter le lecteur
                    videoWidget->close(); // Fermer le widget vidéo
                }
                if (window) {
                    window->display_screen(); // Appel de display_screen()
                }
                return true;
            }
        } else if (event->type() == QEvent::MouseButtonPress) {
            QVideoWidget *videoWidget = qobject_cast<QVideoWidget *>(watched);
            if (videoWidget) {
                player->stop(); // Arrêter le lecteur
                videoWidget->close(); // Fermer le widget vidéo
            }
            if (window) {
                window->display_screen(); // Appel de display_screen()
            }
            return true;
        }
        return QObject::eventFilter(watched, event);
    }
};




/* ----------------------  Classe Window  -------------------------- */


// Constructeur de la classe Window
Window::Window(QWidget *parent)
    : QWidget(parent), myMap(Map::getInstance()) {
    matrix = myMap.get_map();                           // Récupère la matrice du singleton Map

    connect(&myMap, &Map::map_updated, this, &Window::display_map);     // A chaque fois que la méthode map_updated sera appelée dans la classe map : la méthode display_map sera appelée
    connect(&myMap, &Map::show_popup, this, &Window::display_popup);    // " "
    connect(&myMap, &Map::map_updated_after_destination_selection, this, &Window::call_make_trajectory);   // " "

    showSplashScreen(); // Lance la vidéo d'introduction
}

// Méthode permettant d'afficher la vidéo d'introduction
void Window::showSplashScreen() {
    QVideoWidget *videoWidget = new QVideoWidget(this);                 // Création du widget vidéo
    videoWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);  // Supprime les bordures du widget pour faire plus esthétique
    videoWidget->showFullScreen();                                      // Affiche la vidéo en plein écran
    videoWidget->setFocus();                                            // Donne le focus à la fenêtre de la vidéo (afin que de pouvoir la passer grâce à un événement clavier, qui sera capturé par la classe interne EscapeKeyPressFilter

    QMediaPlayer *player = new QMediaPlayer;        // Création du lecteur
    QAudioOutput *audioOutput = new QAudioOutput;   // Création de la sortie audio
    player->setVideoOutput(videoWidget);            // Associer le lecteur au widget vidéo
    player->setAudioOutput(audioOutput);            // Associer le lecteur à la sortie audio
    audioOutput->setVolume(10);                     // Réglage du volume du son
    player->setSource(QUrl("qrc:/video3.mp4"));     // Spécifie la vidéo d'introduction à lancer
    player->play();                                 // Lance la vidéo

    EscapeKeyPressFilter *filter = new EscapeKeyPressFilter(this, player);      // Création de l'objet EscapeKeyPressFilter
    videoWidget->installEventFilter(filter);                                    // Installation de l'objet EscapeKeyPressFilter sur le widget vidéo

    // Création du timer permettant à la fenêtre de l'IHM de ne pas s'afficher au même moment de la vidéo d'introduction et donc de s'y superposer.
    QTimer *timer = new QTimer(this);                               // Création du timer
    connect(timer, &QTimer::timeout, this, [this, timer]() {        // Une fois le timer arrivé à expiration, exécute le code suivant :
        timer->stop();                                                  // Arrête le timer
        display_screen();                                               // Appelle display_screen pour afficher l'écran de l'IHM
        timer->deleteLater();                                           // Supprime le timer
    });
    timer->start(2000);                                             // Délai avant lequel le timer arrivera à expiration (ici 2 secondes), en millisecondes

    // Création de l'animation de fondu permettant de faire disparaître la vidéo d'introduction
    QPropertyAnimation *fadeAnimation = new QPropertyAnimation(videoWidget, "windowOpacity");
    fadeAnimation->setDuration(1000);       // Durée en millisecondes
    fadeAnimation->setStartValue(1.0);      // Opacité de départ
    fadeAnimation->setEndValue(0.0);        // Opacité d'arrivée

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [videoWidget, player, fadeAnimation](QMediaPlayer::MediaStatus status) {       // Une fois que la vidéo est terminée :
        if (status == QMediaPlayer::EndOfMedia) {                               // Si la vidéo est terminée
            fadeAnimation->start(QPropertyAnimation::DeleteWhenStopped);        // Lancer l'animation de fondu écrite ci-dessus
        }
    });

    connect(fadeAnimation, &QPropertyAnimation::finished, player, &QMediaPlayer::deleteLater);  // Supprime le lecteur après l'animation
    connect(fadeAnimation, &QPropertyAnimation::finished, videoWidget, &QWidget::deleteLater);  // Supprime le widget vidéo après l'animation
    connect(fadeAnimation, &QPropertyAnimation::finished, timer, &QTimer::deleteLater);         // Supprime le timer après l'animation
}

// Méthode permettant d'initialiser les paramètres généraux de l'écran de l'IHM
void Window::display_screen()
{
    if (this->layout() == nullptr) {        // Evite de créer plusieurs fois les composants de la fenêtre (à cause du timer de la splashscreen)

        init_general_screen_paramaters();   // Appel de la méthode d'initialisation des paramètres généraux de l'écran de l'IHM

        init_map();         // Appel de la méthode d'initialisation de la map
        init_image();       // Appel de la méthode d'initialisation de l'image
        init_buttons();     // Appel de la méthode d'initialisation des boutons

        add_components_to_screen();     // Ajout les composants à l'écran

        this->showMaximized();          // Affiche la fenêtre en plein écran
    }
}

// Méthode permettant d'initialiser les paramètres généraux de l'écran de l'IHM
void Window::init_map()
{
    // Créer une scène (= la carte)
    scene = new QGraphicsScene(this);

    // Créer un CustomGraphicsView pour afficher la scène (cadre qui contient notre carte)
    view = new CustomGraphicsView(pixels_on_screen_per_matrix_cell, scene, this);

    view->setStyleSheet("border: 4px inset #2C2F73;");  // Définit une bordure à la carte de couleur #2C2F73 dépaisseur 4px

    connect(view, &CustomGraphicsView::canvasClicked, &myMap, &Map::set_destination_and_robot_position);    // A chaque fois que l'utilisateur clique sur la carte, appelle la méthode set_destination_and_robot_position de la classe Map


    view->setFixedSize(myMap.getCols() * pixels_on_screen_per_matrix_cell, myMap.getRows() * pixels_on_screen_per_matrix_cell); // Définit la taille du cadre de la carte en fonction du nombre de lignes et de colonnes de la matrice, afin que celle-ci ne soit pas disprorpotionnée par rapport au reste de l'écran
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);     // Supprime la barre de défilement horizontale du cadre de la carte
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);       // Supprime la barre de défilement verticale du cadre de la carte

    display_map();  // Intialise la map
}

// Méthode permettant d'initialiser la map
void Window::display_map()
{
    // Définition des couleurs pour chaque valeur de la matrice (0 : noir, 1 : gris, ...)
    static const QColor colors[6] = {Qt::black, Qt::gray, Qt::white, Qt::red, QColorConstants::Svg::orange, Qt::blue};

    // Efface les éléments précédents de la scène si nécessaire
    scene->clear();

    // Redessine la carte
    for (int i = 0; i < myMap.getRows(); ++i) {
        for (int j = 0; j < myMap.getCols(); ++j) {
            if (matrix[i][j] >= 0 && matrix[i][j] < myMap.get_number_of_possible_values_for_a_matrix_cell()) {
                scene->addRect(j * pixels_on_screen_per_matrix_cell, i * pixels_on_screen_per_matrix_cell, pixels_on_screen_per_matrix_cell, pixels_on_screen_per_matrix_cell, QPen(Qt::NoPen), colors[this->matrix[i][j]]);    // Ajoute un rectangle sur la carte pour chaque cas de la matrice
            } else {
                // Gestion de l'erreur pour les valeurs de this->map incorrectes
            }
        }
    }
}

// Créer une fonction affichant une popup dont le texte dépend de l'ID passer en paramètre de la fonction
void Window::display_popup(int id_popup)
{
    switch (id_popup)
    {
        case 1:
            // Créer une pop_up
            QMessageBox::information(this, "Information", "Sélectionnez le point de destination du robot sur la carte.");
            break;
        case 2:
            QMessageBox::information(this, "Information", "Le point de destination a été atteint par le robot.");
            break;
        case 3:
        {
            QMessageBox msgBox;
            msgBox.setWindowIcon(QIcon(":icon.png"));   // set l'icône de la fenêtre de la popup
            msgBox.setWindowTitle("Information");       // set le titre de la fenêtre de la popup
            msgBox.setText("La cartographie est terminée. \n\nVoulez-vous démarrer la trajectoire ?");
               msgBox.addButton("Oui", QMessageBox::AcceptRole);    // Ajoute un bouton "Oui" à la popup
               msgBox.addButton("Non", QMessageBox::RejectRole);    // Ajoute un bouton "Non" à la popup

            int ret = msgBox.exec();                    // Affiche la popup et attend que l'utilisateur appuie sur un bouton

            if (ret == QMessageBox::AcceptRole) {       // Si l'utilisateur appuie sur le bouton "Oui"
                start_trajectory();                         // Alors lance le processus de déplacement du robot (appelle la méthode start_trajectory)
            } else {
                // Ne rien faire dans ce cas
            }
        }
            break;

        case 4:
            QMessageBox::information(this, "Information", "Veuillez sélectionner un point de destination accessible et connu par le robot.");
            break;
    }
}

// Méthode permettant d'initialiser les paramètres généraux de l'écran de l'IHM
void Window::init_general_screen_paramaters()
{
    setWindowTitle("ExploBot");                 // Nom de la fenêtre
    setWindowIcon(QIcon(":icon.png"));          // Ajout de l'icône du logiciel. Tuto : 1) Ajouter l'icône au format max de 192x192 à la racine du projet (dans l'explorateur de ficheirs). 2) Faire un clic droit sur le projet --> Fichier --> nouveau fichier --> Fichier de ressources Qt. 3) Ouvrir le projet dans l'explorateur de fichiers, modifier le contenu avec un éditeur de texte et écrire : <!DOCTYPE RCC> <RCC version="1.0"> <qresource prefix="/"> <file>icon.png</file> </qresource> </RCC>
    setStyleSheet("background-color: white;");  // Fond de la fenêtre en blanc
}

// Méthode permettant d'initialiser l'image "Explobot by Thales"
void Window::init_image()
{
    // Pour créer et placer l'image, on utilise un QLabel
    QPixmap pixmap(":image_logo.png");            // Charge l'image
    topImage = new QLabel(this);                  // Créer un QLabel
    topImage->setPixmap(pixmap);                  // Ajoute l'image au QLabel
    topImage->setScaledContents(true);            // Si vous voulez que l'image soit mise à l'échelle pour s'adapter au QLabel
    topImage->setMinimumSize(310,98);             // Définit la taille minimale de l'image
    topImage->setMaximumSize(434,138);            // Définit la taille maximale de l'image
    topImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);    // Définit la politique de redimensionnement de l'image
}

// Méthode permettant d'ajouter un effet d'ombre au bouton passé en paramètre, pour améliorer l'ésthétique de l'IHM
void addShadowEffectToButton(QPushButton *button) {
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect();  // Créer un effet d'ombre
    shadowEffect->setBlurRadius(5);     // Définit le rayon du flou
    shadowEffect->setXOffset(3);        // Définit le décalage en X
    shadowEffect->setYOffset(3);        // Définit le décalage en Y
    shadowEffect->setColor(QColor(0, 0, 0, 60));    // Définit la couleur de l'ombre en RGBA

    button->setGraphicsEffect(shadowEffect);        // Applique l'effet d'ombre au bouton passé en paramètre de la fonction
}

// Méthode permettant d'initialiser les boutons de l'IHM
void Window::init_buttons()
 {

    // Créer les boutons
    button_cartography = new QPushButton("CARTOGRAPHIE", this);
    button_trajectory = new QPushButton("TRAJECTOIRE", this);
    button_reset = new QPushButton("RESET", this);
    button_stop = new QPushButton("STOP", this);

    button_cartography->setStyleSheet(
        "QPushButton {"
        "background-color: #2C2F73;"  // Couleur de fond bleue
        "color: white;"               // Texte blanc
        "border: none;"               // Pas de bordure
        "border-radius: 1px;"        // Coins arrondis
        "padding: 10px 20px;"         // Espacement interne
        "font-size: 20px;"            // Taille de la police
        "font-weight: bold;"          // Mettre le texte en gras
        "}"
        "QPushButton:hover {"
        "background-color: #151B70;"  // Couleur de fond plus foncée au survol 151B70
        "}"
        "QPushButton:pressed {"
        "background-color: #434672;"  // Couleur de fond encore plus foncée lorsqu'il est enfoncé
        "}"
        );

    button_trajectory->setStyleSheet(
        "QPushButton {"
        "background-color: #2C2F73;"  // Couleur de fond bleue
        "color: white;"               // Texte blanc
        "border: none;"               // Pas de bordure
        "border-radius: 1px;"        // Coins arrondis
        "padding: 10px 20px;"         // Espacement interne
        "font-size: 20px;"            // Taille de la police
        "font-weight: bold;"          // Mettre le texte en gras
        "}"
        "QPushButton:hover {"
        "background-color: #151B70;"  // Couleur de fond plus foncée au survol
        "}"
        "QPushButton:pressed {"
        "background-color: #434672;"  // Couleur de fond encore plus foncée lorsqu'il est enfoncé
        "}"
        );

    button_reset->setStyleSheet(
        "QPushButton {"
        "background-color: #FF9432;"  // Couleur de fond bleue
        "color: white;"               // Texte blanc
        "border: none;"               // Pas de bordure
        "border-radius: 1px;"        // Coins arrondis
        "padding: 10px 20px;"         // Espacement interne
        "font-size: 20px;"            // Taille de la police
        "font-weight: bold;"          // Mettre le texte en gras
        "}"
        "QPushButton:hover {"
        "background-color: #FF7700;"  // Couleur de fond plus foncée au survol
        "}"
        "QPushButton:pressed {"
        "background-color: #FFAD66;"  // Couleur de fond encore plus foncée lorsqu'il est enfoncé
        "}"
        );

    button_stop->setStyleSheet(
        "QPushButton {"
        "background-color: #EF2C2C;"  // Couleur de fond bleue
        "color: white;"               // Texte blanc
        "border: none;"               // Pas de bordure
        "border-radius: 1px;"        // Coins arrondis
        "padding: 10px 20px;"         // Espacement interne
        "font-size: 20px;"            // Taille de la police
        "font-weight: bold;"          // Mettre le texte en gras
        "}"
        "QPushButton:hover {"
        "background-color: #ED0000;"  // Couleur de fond plus foncée au survol
        "}"
        "QPushButton:pressed {"
        "background-color: #EA5B5B;"  // Couleur de fond encore plus foncée lorsqu'il est enfoncé
        "}"
        );

    // Définit la politique de redimensionnement des boutons
    button_cartography->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button_trajectory->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button_reset->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button_stop->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Taille maximum des boutons (pour lorsqu'on redimensionne la fenêtre)
    button_cartography->setMaximumSize(200, 50);
    button_trajectory->setMaximumSize(200, 50);
    button_reset->setMaximumSize(200,50);
    button_stop->setMaximumSize(200,50);

    // Taille minimum des boutons (pour lorsqu'on redimensionne la fenêtre)
    button_cartography->setMinimumSize(100, 50);
    button_trajectory->setMinimumSize(100, 50);
    button_reset->setMinimumSize(100,50);
    button_stop->setMinimumSize(100,50);

    // Ce qui se passe lorsque l'on clique sur les boutons
    // Indique la fonction à appeler lorsque l'on clique sur le bouton
    connect(button_cartography, &QPushButton::clicked, this, &Window::click_on_cartography_button);     // Lorsque l'on clique sur "button_cartography", cela exécute la fonction "click_on_cartography_button"
    connect(button_trajectory, &QPushButton::clicked, this, &Window::click_on_trajectory_button);
    connect(button_reset, &QPushButton::clicked, this, &Window::click_on_reset_button);
    connect(button_stop, &QPushButton::clicked, this, &Window::click_on_stop_button);

    // Ajout de l'effet d'ombre aux boutons
    addShadowEffectToButton(button_cartography);
    addShadowEffectToButton(button_trajectory);
    addShadowEffectToButton(button_reset);
    addShadowEffectToButton(button_stop);
}

// Méthode permettant de placer les composants sur l'écran
void Window::add_components_to_screen()
{
    // Crée un layout horizontal principal pour diviser l'écran en deux parties : gauche et droite
    // La partie gauche comprend la carte
    // La partie droite comprend les boutons et l'image du logo Thales
    mainLayout = new QHBoxLayout();

    // Partie gauche : un layout horizontal pour centrer la carte
    QHBoxLayout *leftLayout = new QHBoxLayout;  // Crée un layout horizontal

    // Créer des espaces vides à gauche et à droite pour ajouter de l'espace autour de la carte (plus ésthétique)
    QSpacerItem* leftSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);    // Crée un espace vide
    QSpacerItem* rightSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);   // " "

    // Place les éléments dans le layout concernant la carte
    leftLayout->addItem(leftSpacer);    // Place l'espace à gauche de la carte
    leftLayout->addWidget(view);        // Place la carte au centre
    leftLayout->addItem(rightSpacer);   // Place l'espace à droite de la carte

    // Place le layout horizontal de la partie gauche au layout horizontal principal
    mainLayout->addLayout(leftLayout);

    // Ajout de la possibilité de scroll l'écran (s'affiche uniquement si l'écran est trop petit)
    QWidget *scrollAreaContent = new QWidget;   // Crée un nouveau widget pour le scroll
    scrollAreaContent->setLayout(mainLayout);   // Ajoute le widget de scroll au layout principal

    QScrollArea *scrollArea = new QScrollArea;                          // Créer une zone de scroll
    scrollArea->setFrameShape(QFrame::NoFrame);                         // Supprime la bordure de la zone de scroll
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // Supprime la barre de scroll horizontale
    scrollArea->setWidgetResizable(true);                               // Permet de redimensionner la zone de scroll
    scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width: 30px;}");   // Définit la largeur de la barre de scroll verticale
    scrollArea->setWidget(scrollAreaContent);                           // Ajoute le widget de scroll à la zone de scroll

    QVBoxLayout *scrollLayout = new QVBoxLayout;        // Crée un layout vertical pour le scroll
    scrollLayout->setContentsMargins(0,0,0,0);          // Supprime les marges du layout
    scrollLayout->addWidget(scrollArea);                // Ajoute la zone de scroll au layout
    setLayout(scrollLayout);                            // Place le layout du scroll sur l'écran

    // Partie droite de l'écran : comprend l'image "ExploBot by Thales" et les boutons
    QVBoxLayout *rightLayout = new QVBoxLayout;     // Crée un layout vertical pour la partie droite de l'écran

    // Partie haute de la partie droite : deux labels disposés verticalement
    QVBoxLayout *topLayout = new QVBoxLayout;       // Crée un layout vertical pour la partie haute de la partie droite de l'écran
    rightLayout->addLayout(topLayout);              // Place ce layout dans la partie droite de l'écran


    // Pour le logo
    QHBoxLayout *topImageLayout = new QHBoxLayout;  // Crée un layout horizontal pour le logo
    // Créer des espaces vides en haut et en bas pour ajouter de l'espace autour du logo (plus ésthétique)
    QSpacerItem* topImageSpacer1 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);   // Crée un espace vide
    QSpacerItem* topImageSpacer2 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);   // " "
    topImageLayout->addItem(topImageSpacer1);       // Place l'espace au-dessus du logo
    topImageLayout->addWidget(topImage);            // Place le logo
    topImageLayout->addItem(topImageSpacer2);       // Place l'espace en-dessous du logo
    topLayout->addLayout(topImageLayout);           // Ajoute le layout du logo au layout de la partie haute de la partie droite de l'écran

    // Ajoute un espace entre le logo et les boutons
    topLayout->addSpacing(50);

    // Pour le bouton button_cartography
    QHBoxLayout *cartographyLayout = new QHBoxLayout;   // Crée un layout horizontal pour le bouton
    // Créer des espaces vides en haut et en bas pour ajouter de l'espace autour du bouton (plus ésthétique)
    QSpacerItem* cartographySpacer1 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);    // Crée un espace vide
    QSpacerItem* cartographySpacer2 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);    // " "
    cartographyLayout->addItem(cartographySpacer1);         // Place l'espace au-dessus du bouton
    cartographyLayout->addWidget(button_cartography);       // Place le bouton
    cartographyLayout->addItem(cartographySpacer2);         // Place l'espace en-dessous du bouton
    topLayout->addLayout(cartographyLayout);                // Ajoute le layout du bouton au layout de la partie haute de la partie droite de l'écran

    // Pour le bouton button_trajectory
    QHBoxLayout *trajectoryLayout = new QHBoxLayout;
    QSpacerItem* trajectorySpacer1 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    QSpacerItem* trajectorySpacer2 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    trajectoryLayout->addItem(trajectorySpacer1);
    trajectoryLayout->addWidget(button_trajectory);
    trajectoryLayout->addItem(trajectorySpacer2);
    topLayout->addLayout(trajectoryLayout);

    // Boutons RESET et STOP
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    QSpacerItem* buttonSpacer1 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed); // Ajouter des espaces
    QSpacerItem* buttonSpacer2 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    QSpacerItem* buttonSpacer3 = new QSpacerItem(60, 1, QSizePolicy::Minimum, QSizePolicy::Fixed);    bottomLayout->addItem(buttonSpacer1);
    bottomLayout->addWidget(button_reset);
    bottomLayout->addItem(buttonSpacer3);
    bottomLayout->addWidget(button_stop);
    bottomLayout->addItem(buttonSpacer2);
    rightLayout->addLayout(bottomLayout);

    // Ajouter le layout vertical de la partie droite au layout horizontal principal
    mainLayout->addLayout(rightLayout);

    // Ajout de l'espace entre les composants
    topLayout->addSpacing(20);      // Ajoute 10 pixels d'espace entre les boutons Cartography et Trajectory
    bottomLayout->addSpacing(10);   // Ajoute 10 pixels d'espace entre les boutons RESET et STOP
    mainLayout->setSpacing(20);     // Ajoute 20 pixels d'espace entre les éléments du layout principal
    topLayout->setSpacing(40);      // Ajoute 20 pixels d'espace entre les éléments du layout principal

    // Définit le layout principal pour cette fenêtre
    setLayout(mainLayout);
}

// Méthode permettant de convertir une matrice en QString
// Cette méthode permet de passer la matrice au code Python
// Paramètres : matrice d'entiers, nombre de lignes de la matrice
QString Window::convertMatrixToString(int (*matrix)[20], int rows) {
    QString string;
    QTextStream stream(&string);

    stream << "["; // Début de la matrice

    for (int i = 0; i < rows; ++i) {
        stream << "["; // Début d'une ligne

        for (int j = 0; j < 20; ++j) {
            stream << matrix[i][j];
            if (j < 19) stream << ","; // Séparateur pour les éléments
        }

        stream << "]"; // Fin d'une ligne
        if (i < rows - 1) stream << ","; // Séparateur pour les lignes
    }

    stream << "]"; // Fin de la matrice

    return string;
}

// Méthode permettant de convertir une String en liste de Commandes
// Cette méthdoe permet de convertir la liste des commandes obtenues par le code Python sous forme de string, en liste de commandes à passéer au robot pour qu'il se déplace
QVector<Command> Window::convertStringToCommands(const QString &str) {
    QVector<Command> commands;

    for (QChar ch : str) {
        switch(ch.toLatin1()) { // Convertir le caractère en char
        case '0':
            commands.push_back(FORWARD);    // Un 0 indique au robot d'aller en face
            break;
        case '1':
            commands.push_back(RIGHT);      // Un 1 indique au robot de tourner à droite
            break;
        case '2':
            commands.push_back(LEFT);
            break;
        case '3':
            commands.push_back(STOP);
            break;
        }
    }

    return commands;
}


// Fonction permettant d'éxécuter un code Python et de récupérer son résultat (dans le premier "connect")
// Tous les paramètres sont à mettre entre "quillemets". python_file_name : nom du fichier python à éxécuter (ajouter .py à la fin), argument 1 et 2 : les arguments à passer au code python
void Window::run_python_code(QString python_file_name, QString argument_1, QString argument_2){
    // Python
    process = new QProcess(this);       // Crée un nouveau processus qui exécutera le code Python

    if (python_file_name == "trajectory.py") {      // Si le code python à éxécuter est le code de la trajectoire optimisée

        // Gestion du résultat du code Python
        connect(process, &QProcess::readyReadStandardOutput, this, [this]() {       // Quand le code Python a fini de s'éxécuter, exécute le code suivant :
            QString output = process->readAllStandardOutput();                      // Récupère le résultat du code Python dans la variable "output"

            // La variable "trajectory_result_python_string" est de la forme : "matrice|moves_list_commands|moves_size_of_list_commands"
            // Il faut donc récupérer chacun de ces 3 éléments dans une variable :
            QString trajectory_result_cpp_string = output.trimmed();                // Supprime les espaces avant et après la string

            QStringList trajectory_result_cpp_qstringlist = trajectory_result_cpp_string.split("|", Qt::SkipEmptyParts);    // Divise le résultat du code Python (trajectory_result_python_string) en 3 parties

            // Stocke chacun des 3 éléments dans une variable
            QString waypoints_string = trajectory_result_cpp_qstringlist[0];            // Les waypoints que doit emprunter le robot pour aller au point de destiantion
            QString moves_list_commands = trajectory_result_cpp_qstringlist[1];         // La liste des commandes à envoyer au robot pour qu'il se déplace en suivant les waypoints ci-dessus
            QString moves_size_of_list_commands = trajectory_result_cpp_qstringlist[2]; // La taille de la liste des commandes ci-dessus

            myMap.update_entire_matrix_from_trajectory_result(waypoints_string);        // Met à jour la matrice de la carte avec les waypoints obtenus par le code Python

            QVector<Command> commands = convertStringToCommands(moves_list_commands);   // Convertit la liste des commandes obtenues par le code Python sous forme de string, en liste de commandes à passéer au robot pour qu'il se déplace

            PROXYPILOT_send_moves_trajectory(commands.data(), commands.size());         // Envoie la liste des commandes au robot pour qu'il se déplace

        });
    }

    // Capture des erreurs du code Python
    connect(process, &QProcess::readyReadStandardError, this, [this]() {
        QString errorOutput = process->readAllStandardError();
    });

    // Lance le processus qui exécute le code Python
    process->start("python", QStringList() << python_file_name << argument_1 << argument_2);
}

// Méthode permettant de lancer le processus de trajectoire optimisée
void Window::start_trajectory()
{
    // Rend la carte cliquable pour pouvoir sélectionner le point de départ et de destination du robot en cliquant sur la carte
    myMap.enable_map_click(true);

    // Affiche la pop-up invitant à sélectionner le point de destination du robot sur la carte
    display_popup(1);
}

//  Méthode appelée une fois que l'utilisateur à sélectionner un point de destination sur la carte
// Cette méthode est appelée grâce à la connection avec le signal "map_updated_after_destination_selection" de la classe Map
void Window::call_make_trajectory() {
    int (*matrix)[20] = myMap.get_map();                                    // Obtention de la matrice de la carte
    QString matrixString = convertMatrixToString(matrix, myMap.getRows());  // Conversion de la matrice en QString pour pouvoir la passer au code Python
    run_python_code("trajectory.py", matrixString, "5");                    // Exécution du code Python de trajectoire optimisée
}

// Méthode exécutée lorsque le bouton "Cartographie" est cliqué
// Comme la Cartographie n'a pas été implémentée, cette méthode génère une carte contenant des obstacles pour simuler une carte cartographiée
void Window::click_on_cartography_button()
{
    // Obtention de l'instance singleton de Map
    Map& myMap = Map::getInstance();

    // La carte générée contenant des obstacles pour simuler une carte cartographiée
    // Les 1 représentent les zones accessibles par le robot
    // Les 0 représentent les zones obstacles
    int matrix2[20][20] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
    myMap.update_entire_matrix(matrix2);    // Met à jour la matrice de la carte avec la carte générée ci-dessus
}

// Méthode exécutée lorsque le bouton "Trajectoire" est cliqué
void Window::click_on_trajectory_button()
{
    myMap.reset_trajectory();       // Réinitialise la trajectoire du robot
    start_trajectory();             // Lance le processus de trajectoire optimisée
}

// Méthode exécutée lorsque le bouton "RESET" est cliqué
void Window::click_on_reset_button()
{
    myMap.reset_map();                  // Réinitialise la carte
}

// Méthode exécutée lorsque le bouton "STOP" est cliqué
void Window::click_on_stop_button()
{
    PROXYPILOT_stop_robot();    // Stop le robot

    //QApplication::quit();       // Quitte complètement l'application, ou utiliser this->close(); pour fermer la fenêtre = même effet ici

    DISPATCHER_disconnect();    // Déconnecte le dispatcher
    POSTMAN_disconnect();       // Déconnecte le postman
    DISPATCHER_stop();          // Stop le dispatcher
    POSTMAN_stop();             // Stop le postman
    DISPATCHER_destroy();       // Détruit le dispatcher
    POSTMAN_destroy();          // Détruit le postman
}












