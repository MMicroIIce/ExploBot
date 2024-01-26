/**
 * \file  defs.h
 * \version  0.1
 * \author Fatoumata TRAORE
 * \date Nov 16, 2023
 * \brief Definition of types usable from all of the modules.
 *
 * \see defs.c
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

#ifndef SRC_LIB_DEFS_H_
#define SRC_LIB_DEFS_H_
/* ----------------------  INCLUDES ------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
/* ----------------------  PUBLIC TYPE DEFINITIONS ---------------------------*/

typedef enum __attribute__((__packed__)){
    FORWARD,
    RIGHT,   /**< RIGHT : left motor clockwise, right  motor anti-clockwise. */
    LEFT,    /**< LEFT : left  motor anti-clockwise, right  motor clockwise. */
    STOP     /**< STOP :  motors stop. */
} Command;

/**
 * \enum Message_Type
 * \brief Defines message types.
 *
 * Messag_Type gives all the message type that can go threw TCP.
 */
typedef enum  __attribute__((__packed__)){
    SEND_MOVES_TRAJECTORY = 0x0100,     /**< SEND_MOVES_TRAJECTORY : Cute sends the move commands for the trajectory to Carto. */
    SEND_MOVE_CARTOGRAPHY = 0x0200,     /**< SEND_MOVE_CARTOGRAPHY : Cute sends a move command for the cartography to Carto. */
    MOVE_DONE = 0x0300,                 /**< MOVE_DONE : Carto confirms to Cute that move command has been performed. */
    SET_OBSTACLE_POSITION = 0x0400,     /**< SET_OBSTACLE_POSITION : Carto sends an obstacle position to Cute. */
    SET_ROBOT_POSITION = 0x0500,        /**< SET_ROBOT_POSITION : Carto sends the new robot position to Cute. */
    STOP_ROBOT = 0x0600,                /**< STOP_ROBOT : Cute sends a stop command to the robot.  */
    SEND_ROBOT_POSITION = 0x0700,       /**< SEND_ROBOT_POSITION : Cute sends the robot position to Carto. */
    ROBOT_POSITION_RECEIVED = 0x0800,   /**< ROBOT_POSITION_RECEIVED : Carto confirms to Cute that the robot position has been received. */
} Message_Type;

/**
 * \struct Communication_Protocol_Head defs.h "lib/defs.h"
 * \brief Lists the head sections of a message.
 *
 * Communication_Protocol_Head gives a list of header section for the TCP communication protocol.
 */
typedef struct __attribute__((__packed__, aligned(1))){
    int16_t msg_size; /**< Gives the Message size (type + data) in bytes. Minimum = 2 bytes Maximum = 0xFFFF. */
    Message_Type msg_type; /**< Gives the Message type. Size = 2 bytes */
} Communication_Protocol_Head;

/**
 * \enum bool_e
 * \brief Boolean
 *
 * bool_e represents a boolean, it exists in two states.
 */
typedef enum {
    FALSE = 0,/**< FALSE */
    TRUE = 1  /**< TRUE */
} bool_e;
/**
 * \enum Message_Type
 * \brief Defines message types.
 *
 * Messag_Type gives all the message type that can go threw TCP.
 */
typedef enum  __attribute__((__packed__)){
    SOUTH=0,    /**< SOUTH : Direction SOUTH according to the matrix(+x) */
    NORTH,      /**< DIRUP : Direction NORTH according to the matrix(-x) */
    WEST,    /**< DIRLEFT : Direction WEST according to the matrix(-y) */
    EAST    /**< DIRRIGHT : Direction EAST according to the matrix(+y) */
} Direction;

/**
 * \struct Communication_Protocol_Head defs.h "lib/defs.h"
 * \brief Lists the head sections of a message.
 *
 * Communication_Protocol_Head gives a list of header section for the TCP communication protocol.
 */
typedef struct __attribute__((__packed__, aligned(1))){
    Direction dir;
    int coord_x;
    int coord_y;
} Position ;

/* ----------------------  PUBLIC ENUMERATIONS -------------------------------*/
/* ----------------------  PUBLIC STRUCTURES ---------------------------------*/
/* ----------------------  PUBLIC VARIABLES -----------------------------------*/
/* ----------------------  PUBLIC FUNCTIONS PROTOTYPES  ----------------------*/
#endif /* SRC_LIB_DEFS_H_ */
