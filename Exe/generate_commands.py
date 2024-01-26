#generate.py

############################################################################################################
# Generate commands
############################################################################################################


ROBOT_ORIENTATION_EST = 5 
ROBOT_ORIENTATION_NORD = 6
ROBOT_ORIENTATION_OUEST = 7
ROBOT_ORIENTATION_SUD = 8


FRONT = 0 #: en avant (vers le haut)
RIGHT = 1 #: tournée à droite de 90° en restant sur la même cellule
LEFT = 2 #: tournée à gauche de 90° en restant sur la même cellule


def generate_commands(path, robot_pos):
    
    commands = []

    robot_orientation = ROBOT_ORIENTATION_EST

    if len(path) < 2:
        return commands

    # Insère la position actuelle du robot au début du chemin
    path.insert(0, robot_pos)

    for i in range(1, len(path)):
        current_pos = path[i - 1]
        next_pos = path[i]
        dx, dy = next_pos[0] - current_pos[0], next_pos[1] - current_pos[1]
        needed_direction = determine_direction(dx, dy)

        # Génère les commandes de rotation et met à jour l'orientation du robot
        turn_commands, new_orientation = get_turn_commands(robot_orientation, needed_direction)
        commands.extend(turn_commands)

        robot_orientation = new_orientation  # Mise à jour de l'orientation du robot

        # Ajoute FRONT pour chaque étape du chemin
        commands.append(FRONT)

    return commands

def determine_direction(dx, dy):
    if dx == 1: return ROBOT_ORIENTATION_SUD
    if dx == -1: return ROBOT_ORIENTATION_NORD
    if dy == 1: return ROBOT_ORIENTATION_EST
    if dy == -1: return ROBOT_ORIENTATION_OUEST

def get_turn_commands(current_orientation, needed_direction):
    if current_orientation == needed_direction:
        return [], current_orientation

    next_orientation = {
        ROBOT_ORIENTATION_NORD: [ROBOT_ORIENTATION_EST, ROBOT_ORIENTATION_OUEST],
        ROBOT_ORIENTATION_EST: [ROBOT_ORIENTATION_SUD, ROBOT_ORIENTATION_NORD],
        ROBOT_ORIENTATION_SUD: [ROBOT_ORIENTATION_OUEST, ROBOT_ORIENTATION_EST],
        ROBOT_ORIENTATION_OUEST: [ROBOT_ORIENTATION_NORD, ROBOT_ORIENTATION_SUD]
    }

    if needed_direction == next_orientation[current_orientation][0]:
        return [RIGHT], next_orientation[current_orientation][0]
    else:
        return [LEFT], next_orientation[current_orientation][1]
