import numpy as np
from trajectory import make_trajectory

PASSAGE = 1
OBSTACLE = 0
UNKNOWN = 2
WAYPOINT = 3
DESTINATION = 4

ROBOT_ORIENTATION_EST = 5 

ROBOT = ROBOT_ORIENTATION_EST

FRONT = 0 
RIGHT = 1 
LEFT = 2

F=FRONT
R=RIGHT
L=LEFT

def main():
    print("Test make_trajectory\n")

    # Matrices d'entrée
    example_matrices = [
        np.array([[5, 1, 1, 1],
                  [0, 2, 0, 1],
                  [0, 0, 0, 1],
                  [0, 0, 0, 4]]),

        np.array([[4, 1, 1, 1],
                  [0, 0, 0, 1],
                  [0, 0, 0, 1],
                  [2, 1, 1, 5]]),

        np.array([[5, 1, 0, 0],
                  [0, 1, 1, 0],
                  [0, 0, 1, 0],
                  [0, 0, 1, 4]]),
    ]

    # Matrices attendues
    matrices_attendues = [
        np.array([[5, 3, 3, 3],
                  [0, 0, 0, 3],
                  [0, 0, 0, 3],
                  [0, 0, 0, 4]]),

        np.array([[4, 3, 3, 3],
                  [0, 0, 0, 3],
                  [0, 0, 0, 3],
                  [0, 1, 1, 5]]),

        np.array([[5, 3, 0, 0],
                  [0, 3, 3, 0],
                  [0, 0, 3, 0],
                  [0, 0, 3, 4]]),
    ]

    # Commandes attendues
    commands_attendues = [
        [F, F, F, R, F, F, F],

        [L, F, F, F, L, F, F, F],
        
        [F, R, F, L, F, R, F, F, L, F],
    ]

    sizes_attendues = [
        7,

        8,
        
        10,
    ]

    for i, example_matrix in enumerate(example_matrices):
        
        original_matrix = np.copy(example_matrix)
        
        result_matrix, result_commands, size = make_trajectory(original_matrix)

        matrices_match = np.array_equal(result_matrix, matrices_attendues[i])
        commands_match = (result_commands == commands_attendues[i])
        sizes_match = np.array_equal(sizes_attendues[i], size)

        print(f"Test {i + 1}\n")

        if matrices_match:
            print("Test matrice passed ! \n")

        else:
            print("Test matrice failed \n")

            print("Matrice initiale:")
            print(original_matrix)

            print("\nMatrice attendue:")
            print(matrices_attendues[i])

            print("\nMatrice de sortie:")
            print(result_matrix)

########################################################################
        
        if commands_match:
            print("Test commands passed ! \n")
        else:
            print("Test commands failed \n")

            print("\nCommandes attendues:")
            print(commands_attendues[i])
                
            print("\nCommandes de sortie:")
            print(result_commands)

########################################################################
        if sizes_match:
            print("Test size passed ! \n")
        else:
            print("Test size failed \n")
        
            print("\n size attendue = ")
            print(sizes_attendues[i])
            print("\nsize sortie = ")
            print(size)


        print("\n" + "=" * 50 + "\n")
    
if __name__ == "__main__":
    main()
