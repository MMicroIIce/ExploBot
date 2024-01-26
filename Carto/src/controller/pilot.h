/**
 * \file  pilot.h
 * \version  0.1
 * \author Thomas ROCHER
 * \author Fatoumata TRAORE
 * \date Dec 16, 2024
 * \brief Source file of the motor peripheral.
 *
 * \see pilot.c
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
#ifndef SRC_CONTROLLER_PILOT_H_
#define SRC_CONTROLLER_PILOT_H_
/* ----------------------  INCLUDES ------------------------------------------*/
#include "../lib/defs.h"
/* ----------------------  PUBLIC TYPE DEFINITIONS ---------------------------*/
/* ----------------------  PUBLIC ENUMERATIONS -------------------------------*/
/* ----------------------  PUBLIC STRUCTURES ---------------------------------*/
/* ----------------------  PUBLIC VARIABLES -----------------------------------*/
/* ----------------------  PUBLIC FUNCTIONS PROTOTYPES  ----------------------*/
/**
 * \fn extern int PILOT_create()
 * \brief Initialize in memory the object Motor.
 * \author Fatoumata TRAORE
 *
 * \return int : error code 0 on success -1 on error.
 */

extern int PILOT_create(void);
/**
 * \fn extern int PILOT_destroy(void)
 * \brief Deinitialize the Motor.
 * \author fatoumata TRAORE
 *
 * \return int : error code 0 on success -1 on error.
 */
extern int PILOT_destroy(void);
/**
 * \fn extern void PILOT_send_move_cartography(Command cmd)
 * \brief Move robot to cartography.
 * \author Thomas ROCHER
 * \author fatoumata TRAORE
 *
 * \return int : error code 0 on success -1 on error.
 */
extern void PILOT_send_move_cartography(Command cmd);
/**
 * \fn extern PILOT_send_robot_position(Position robot_position )
 * \brief send robot position
 * \author fatoumata TRAORE
 * \author Thomas ROCHER
 */
extern void PILOT_send_robot_position(Position* robot_position_p);
/**
 * \fn extern PILOT_send_moves_trajectory(Command list_commands [], int taille)
 * \brief Move robot to do trajectory.
 * \author Thomas ROCHER
 * \author fatoumata TRAORE
 */
extern void PILOT_send_moves_trajectory(Command list_commands [], int size);
/**
 * \fn extern void PILOT_stop_robot()
 * \brief stop robot and trajectory.
 * \author fatoumata TRAORE
 */
extern void PILOT_stop_robot();



#endif /* SRC_CONTROLLER_PILOT_H_ */
