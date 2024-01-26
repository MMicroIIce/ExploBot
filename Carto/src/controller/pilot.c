/**
 * \file  pilot.c
 * \version  1.0
 * \author Thomas ROCHER
 * \author Fatoumata TRAORE 
 * \date Dec 16, 2024
 * \brief Source file of the motor peripheral.
 *
 * \see pilot.h
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
/* ----------------------  INCLUDES  ---------------------------------------- */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <errno.h>

#include "../lib/defs.h"
#include "../alphabot2/motor.h"
#include "../alphabot2/ultrasound.h"
#include "../com/proxyMap.h"
#include "../com/proxyCartography.h"
#include "../alphabot2/ultrasound.h"
#include "pilot.h"
/* ----------------------  PRIVATE CONFIGURATIONS  -------------------------- */
static Position* robot_position_base ;
static bool_e can_set_command = FALSE;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
/* ----------------------  PRIVATE ENUMERATIONS  ---------------------------- */
/* ----------------------  PRIVATE STRUCTURES  ------------------------------ */
/* ----------------------  PRIVATE FUNCTIONS PROTOTYPES  -------------------- */
/*------------------------STATE MACHINE RELATED FUNCTIONS------------------------*/
/*------------------------ACTIONS RELATED FUNCTIONS------------------------*/
/* ----------------------  PRIVATE VARIABLES  ------------------------------- */
/* ----------------------  PUBLIC FUNCTIONS  -------------------------------- */
extern int PILOT_create(void) {
    MOTOR_create();
    ULTRASOUND_create();
    pthread_mutex_lock(&mutex);
    can_set_command = TRUE;
    pthread_mutex_unlock(&mutex);
    return 0;
}

extern int PILOT_destroy(void) {
    MOTOR_destroy();
    ULTRASOUND_destroy();
    return 0;
}

extern void PILOT_send_robot_position(Position* robot_position_p){
    robot_position_base->coord_x = robot_position_p->coord_x;
    robot_position_base->coord_y = robot_position_p->coord_y;
    robot_position_base->dir = robot_position_p->dir;
    PROXYCARTOGRAPHY_robot_position_received();
}

extern void PILOT_send_move_cartography(Command cmd) {
    PILOT_send_robot_position(robot_position_base);
    if(cmd == FORWARD){
        if (ULTRASOUND_check_obstacle()){
            if(robot_position_base->dir == SOUTH){
                PROXYMAP_set_obstacle_position((robot_position_base->coord_x)+1, (robot_position_base->coord_y));
            }
            else if(robot_position_base->dir == NORTH){
                PROXYMAP_set_obstacle_position((robot_position_base->coord_x)-1, (robot_position_base->coord_y));
            }
            else if(robot_position_base->dir == WEST){
                PROXYMAP_set_obstacle_position((robot_position_base->coord_x), (robot_position_base->coord_y)-1);
            }
            else if(robot_position_base->dir == EAST){
                PROXYMAP_set_obstacle_position((robot_position_base->coord_x), (robot_position_base->coord_y)+1);
            }
        }
        else {    
            MOTOR_set_command(cmd);
            switch(robot_position_base->dir)
            {
                case SOUTH : PROXYMAP_set_robot_position((robot_position_base->coord_x)+1, (robot_position_base->coord_y)); break;
                case NORTH : PROXYMAP_set_robot_position((robot_position_base->coord_x)-1, (robot_position_base->coord_y)); break;
                case WEST : PROXYMAP_set_robot_position((robot_position_base->coord_x), (robot_position_base->coord_y)-1); break;
                case EAST : PROXYMAP_set_robot_position((robot_position_base->coord_x), (robot_position_base->coord_y)+1); break;
                default : break;
            }
        }
    }
    else if(cmd == LEFT)
    {
        MOTOR_set_command(cmd);
        switch(robot_position_base->dir)
        {
            case SOUTH : robot_position_base->dir = EAST; break;
            case NORTH : robot_position_base->dir = WEST; break;
            case WEST : robot_position_base->dir = SOUTH; break;
            case EAST : robot_position_base->dir = NORTH; break;
            default : break;
        }
    }
    else if(cmd == RIGHT)
    {
        MOTOR_set_command(cmd);
        switch(robot_position_base->dir)
        {
            case SOUTH : robot_position_base->dir = WEST; break;
            case NORTH : robot_position_base->dir = EAST; break;
            case WEST : robot_position_base->dir = NORTH; break;
            case EAST : robot_position_base->dir = SOUTH; break;
            default : break;
        }
    }
    PROXYCARTOGRAPHY_move_done();
}

extern void PILOT_send_moves_trajectory(Command list_commands [], int size) {
    int i = 0;
    while(i<size)
    {
        if (can_set_command){
            MOTOR_set_command(list_commands[i]);
        }
        i++;
    }
    pthread_mutex_lock(&mutex);
    can_set_command = TRUE;
    pthread_mutex_unlock(&mutex);
}

extern void PILOT_stop_robot() {
    pthread_mutex_lock(&mutex);
    can_set_command = FALSE;
    pthread_mutex_unlock(&mutex);
    MOTOR_set_command(STOP);
}
/* ----------------------  PRIVATE FUNCTIONS  ------------------------------- */

