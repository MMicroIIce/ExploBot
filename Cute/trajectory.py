"""
@file trajectory.py
@author Dehoux Antoine
@brief Ce script génère une trajectoire pour un robot en utilisant l'algorithme A*.
Il transforme une matrice de l'environnement (avec des obstacles, des points de passage, etc.)
en une série de commandes pour le robot.
"""

import numpy as np
import heapq
import sys
from generate_commands import generate_commands

# Constantes pour identifier les différents éléments de la grille.
PASSAGE = 1         # Zone libre où le robot peut passer.
OBSTACLE = 0        # Zone avec un obstacle où le robot ne peut pas passer.
UNKNOWN = 2         # Zone inconnue, considérée comme un obstacle dans ce contexte.
WAYPOINT = 3        # Point de passage intermédiaire sur la trajectoire du robot.
DESTINATION = 4     # Point de destination du robot.

# Définition des différentes orientations possibles du robot.
ROBOT_ORIENTATION_EST = 5 
ROBOT_ORIENTATION_NORD = 6
ROBOT_ORIENTATION_SUD = 7
ROBOT_ORIENTATION_OUEST = 8

# Orientation initiale du robot.
ROBOT = ROBOT_ORIENTATION_EST

# Commandes possibles à envoyer au robot.
FRONT = 0 
RIGHT = 1 
LEFT = 2 

def make_trajectory(matrix):
    """
    @brief Transforme une matrice de l'environnement en une trajectoire pour le robot.
    La matrice est mise à jour avec les waypoints et la trajectoire est retournée sous forme de commandes.
    @param matrix: np.array - La matrice représentant l'environnement du robot.
    @return: tuple - La matrice mise à jour, les commandes générées, et le nombre de commandes.
    """
    matrix[matrix == UNKNOWN] = OBSTACLE
    robot_pos, destination_pos = find_robot_and_destination(matrix)
    if robot_pos is None or destination_pos is None:
        return matrix, []

    path = a_star_search(robot_pos, destination_pos, matrix)
    
    if not path:
        print("No path found")
        return matrix, []

    for x, y in path:
        if matrix[x][y] not in [ROBOT, DESTINATION]:
            matrix[x][y] = WAYPOINT

    commands = generate_commands(path, robot_pos)
    size = len(commands)

    return matrix, commands, size

def find_robot_and_destination(matrix):
    """
    @brief Trouve les positions du robot et de la destination dans la matrice.
    @param matrix: np.array - La matrice de l'environnement.
    @return: tuple - Les positions du robot et de la destination.
    """
    robot = np.argwhere(matrix == ROBOT)
    destination = np.argwhere(matrix == DESTINATION)
    
    robot_pos = tuple(robot[0]) if robot.size > 0 else None
    destination_pos = tuple(destination[0]) if destination.size > 0 else None

    return robot_pos, destination_pos

def a_star_search(start, goal, grid):
    """
    @brief Implémente l'algorithme A* pour trouver le chemin le plus court entre deux points.
    @param start: tuple - Position de départ.
    @param goal: tuple - Position de la destination.
    @param grid: np.array - La grille représentant l'environnement.
    @return: list - Le chemin trouvé, si disponible.
    """
    open_set = []  # Initialise l'ensemble ouvert pour stocker les noeuds à explorer
    heapq.heappush(open_set, (0, start))  # Ajoute le point de départ avec un coût initial de 0

    came_from = {}  # Dictionnaire pour suivre le chemin optimal vers chaque noeud
    g_score = {start: 0}  # Dictionnaire pour stocker le coût du chemin le plus court connu de start à n
    f_score = {start: manhattan_distance(start, goal)}  # Estimation du coût total de start à goal passant par n

    while open_set:
        _, current = heapq.heappop(open_set)  # Récupère le noeud avec le plus petit score f

        if current == goal:
            return reconstruct_path(came_from, current)  # Reconstruit le chemin complet si le but est atteint

        # Explore les voisins du noeud courant
        for neighbor in get_neighbors(current, grid):
            tentative_g_score = g_score[current] + 1  # Coût du chemin de start à neighbor par current

            # Si le chemin vers neighbor est meilleur que tout chemin précédent, enregistre ce nouveau chemin
            if tentative_g_score < g_score.get(neighbor, float('inf')):
                came_from[neighbor] = current  # Met à jour le prédécesseur de neighbor
                g_score[neighbor] = tentative_g_score  # Met à jour le coût du chemin de start à neighbor
                f_score[neighbor] = tentative_g_score + manhattan_distance(neighbor, goal)  # Estime le coût total de start à goal
                
                # Ajoute le voisin à l'ensemble ouvert s'il n'est pas déjà dedans
                if neighbor not in [i[1] for i in open_set]:
                    heapq.heappush(open_set, (f_score[neighbor], neighbor))

    return []  # Retourne une liste vide si aucun chemin n'est trouvé


def manhattan_distance(a, b):
    """
    @brief Calcule la distance de Manhattan entre deux points.
    @param a: tuple - Premier point.
    @param b: tuple - Deuxième point.
    @return: int - Distance de Manhattan.
    """
    return abs(a[0] - b[0]) + abs(a[1] - b[1])

def get_neighbors(node, grid):
    """
    @brief Retourne les voisins d'un noeud dans la grille.
    @param node: tuple - Le noeud dont les voisins sont recherchés.
    @param grid: np.array - La grille.
    @return: list - Liste des voisins.
    """
    directions = [(0, 1), (1, 0), (-1, 0), (0, -1)] 
    neighbors = []
    for dx, dy in directions:
        x, y = node[0] + dx, node[1] + dy
        if 0 <= x < len(grid) and 0 <= y < len(grid[0]) and grid[x][y] in [PASSAGE, DESTINATION]:
            neighbors.append((x, y))
    return neighbors

def reconstruct_path(came_from, current):
    """
    @brief Reconstruit le chemin de la destination au départ en suivant les prédécesseurs.
    @param came_from: dict - Dictionnaire des prédécesseurs.
    @param current: tuple - Le noeud actuel.
    @return: list - Le chemin reconstruit.
    """
    path = []
    while current in came_from:
        path.append(current)
        current = came_from[current]
    return path[::-1]

def formated_matrice(matrice):
    """
    @brief Formate la matrice pour l'affichage.
    @param matrice: np.array - La matrice à formater.
    @return: str - La matrice formatée sous forme de chaîne.
    """
    return "{{{}}}".format(",".join("{{{}}}".format(",".join(map(str, row))) for row in matrice))
    
def format_commands(commands):
    """
    @brief Formate les commandes pour l'affichage.
    @param commands: list - Les commandes à formater.
    @return: str - Les commandes formatées sous forme de chaîne.
    """
    return "{{{}}}".format(",".join(map(str, commands)))

if __name__ == "__main__":
    """
    Bloc principal pour exécuter le script.
    Traite l'entrée de la matrice et produit une trajectoire.
    """
    if len(sys.argv) >= 2:
        matrix_str = sys.argv[1]
        
        if matrix_str.startswith('"') and matrix_str.endswith('"'):
            matrix_str = matrix_str[1:-1]

        try:
            matrix_list = eval(matrix_str)
        except:
            print("Invalid matrix format. Please provide a valid matrix as an argument.")
            sys.exit(1)

        matrix = np.array(matrix_list)

        resulting_matrix, commands, size = make_trajectory(matrix)

        matrix_str = formated_matrice(resulting_matrix)
        commands_str = format_commands(commands)

        result_str = "{}|{}|{}".format(matrix_str, commands_str, size)

        print(result_str)
    else:
        print("Usage: python3 trajectory.py <matrix>")
