/**
 * \file  proxyPilot.h
 * \version  1.0
 * \author Thomas ROCHER
 * \date Nov 29, 2023
 * \brief TODO
 *
 * \see proxyPilot.cpp
 *
 * \section License
 *
 * The MIT License
 *
 * Copyright (c) 2023, PFE 2024
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \copyright PFE 2024
 */

#include "defs.h"

/**
 * \fn extern void PROXYPILOT_stop_robot()
 * \brief Sends a stop's command to the robot.
 * \author Thomas Rocher
 */
extern void PROXYPILOT_stop_robot();

/**
 * \fn extern void PROXYPILOT_send_robot_position(int coord_x, int coord_y, int direction);
 * \brief Sends the robot's position.
 * \author Thomas Rocher
 */
extern void PROXYPILOT_send_robot_position(int coord_x, int coord_y, int direction);

/**
 * \fn extern void PROXYPILOT_send_move_cartography(Command command);
 * \brief Sends a command to the robot for the cartography.
 * \author Thomas Rocher
 */
extern void PROXYPILOT_send_move_cartography(Command command);

/**
 * \fn extern void PROXYPILOT_send_moves_trajectory(Command command[], int size);
 * \brief Sends a command's list to the robot for the trajectory.
 * \author Thomas Rocher
 */
extern void PROXYPILOT_send_moves_trajectory(Command command[], int size);
