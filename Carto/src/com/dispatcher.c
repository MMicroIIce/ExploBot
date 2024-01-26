/**
 * \file  dispatcher.c
 * \version  1.2
 * \authors Joshua MONTREUIL, Thomas ROCHER
 * \date May 7, 2023
 * \brief Source file for the dispatcher module. The dispatcher is in charge of the reading, parsing and dispatching of
 * any messages on the postman's socket.
 *
 * \see dispatcher.h
 *
 * \section License
 *
 * The MIT License
 *
 * Copyright (c) 2023, Prose A2 2023
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
#include "dispatcher.h"
#include "postman.h"
#include "../lib/defs.h"
#include "../controller/pilot.h"

#include <stdbool.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

/* ----------------------  PRIVATE CONFIGURATIONS  -------------------------- */
#define STATE_GENERATION S(S_IDLE) S(S_READING_MSG) S(S_STOP) S(S_WAITING_RECONNECTION)
#define S(x) x,
typedef enum {STATE_GENERATION STATE_NB} State_Machine;
#undef STATE_GENERATION
#undef S
/* ----------------------  PRIVATE TYPE DEFINITIONS  ------------------------ */
/* DISPATCHER */
/**
 * \def MAX_RECEIVED_BYTES
 * Maximum of possible received bytes.
 */
#define MAX_RECEIVED_BYTES (20)
/* ----------------------  PRIVATE STRUCTURES  ------------------------------ */
/* ----------------------  PRIVATE ENUMERATIONS  ---------------------------- */
/* ----------------------  PRIVATE FUNCTIONS PROTOTYPES  -------------------- */
/**
 * \fn static void * DISPATCHER_run(void * arg)
 * \brief Called by a thread. This function is the "active" part of dispatcher.
 * It reads any messages on postman's socket then parse and dispatch it.
 * \author Joshua MONTREUIL
 * \see postman.h
 *
 * \param arg : argument pointer.
 *
 * \return void * : generic pointer (0 on success -1 on error).
 */
static void *run(void * arg);
/**
 * \fn static void DISPATCHER_dispatch_received_msg(Communication_Protocol_Head msg)
 * \brief Used to parse the incoming msg and to dispatch and pass data to the right functions.
 * \author Joshua MONTREUIL
 *
 * \see PILOT_send_moves_trajectory(Command list_commands [], int size)
 * \see PILOT_send_move_cartography(Command cmd)
 * \see PILOT_send_robot_position(Position* robot_position_p)
 * \see PILOT_stop_robot()
 *
 * \param msg : message received from postman's socket. Type : Communication_Protocol_Head.
 * \see Communication_Protocol_Head
 *
 * \return On success, returns 0. On error, returns -1.
 */
static int dispatch_received_msg(Communication_Protocol_Head msg);
/**
 * \fn static Communication_Protocol_Head DISPATCHER_decode_message(uint8_t* raw_message)
 * \brief Used to decode the raw message from the socket. Separation between the message type, the data size and the rest of the informations.
 * \author Joshua MONTREUIL
 *
 * \param raw_message : raw message from the socket.
 *
 * \return The header of the message.
 * \see Communication_Protocol_Head
 */
static Communication_Protocol_Head decode_message(uint8_t* raw_message);

/* ----------------------  PRIVATE VARIABLES  ------------------------------- */
/**
 * \var static State_Machine state
 * \brief Dispatcher state Machine.
 */
static State_Machine state;
/**
 * \var static pthread_t dispatcher_thread
 * \brief Dispatcher thread.
 */
static pthread_t dispatcher_thread;
/**
 * \var static uint8_t * data_received
 * \brief raw data received from socket;
 */
static uint8_t * data_received;
/**
 * \var static pthread_mutex_t dispatcher_mutex
 * \brief Mutex used to safely read state from state machine
 */
static pthread_mutex_t dispatcher_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * \var static Command* list_commands
 * \brief Command's list to use as parameter for PILOT_send_moves_trajectory
 */
static Command list_commands[50];
/**
 * \var static int count_command
 * \brief Count of command in the current list_commands
 */
static int count_command = 0;

/* ----------------------  PUBLIC FUNCTIONS  -------------------------------- */
int DISPATCHER_create(void) {
    data_received = (uint8_t *) malloc(MAX_RECEIVED_BYTES);
    return 0;
}

int DISPATCHER_start(void) {
    state = S_IDLE;
    if(pthread_create(&dispatcher_thread, NULL, run, NULL) != 0 ) {
        return -1;
    }
    PILOT_create();
    return 0;
}

void DISPATCHER_start_reading() {
    pthread_mutex_lock(&dispatcher_mutex);
    state = S_READING_MSG;
    pthread_mutex_unlock(&dispatcher_mutex);
}

void DISPATCHER_disconnect(void){
    pthread_mutex_lock(&dispatcher_mutex);
    state = S_WAITING_RECONNECTION;
    pthread_mutex_unlock(&dispatcher_mutex);   
}

int DISPATCHER_stop(void) {
    pthread_mutex_lock(&dispatcher_mutex);
    state = S_STOP;
    pthread_mutex_unlock(&dispatcher_mutex);
    if(pthread_join(dispatcher_thread, NULL) != 0) {
        return -1;
    }
    return 0;
}

int DISPATCHER_destroy(void) {
    free(data_received);
    PILOT_destroy();
    return 0;
}

/* ----------------------  PRIVATE FUNCTIONS  ------------------------------- */
static void * run(void * arg) {
    State_Machine my_state;
    pthread_mutex_lock(&dispatcher_mutex);
    my_state = state;
    pthread_mutex_unlock(&dispatcher_mutex);
    while(my_state != S_STOP) {
        pthread_mutex_lock(&dispatcher_mutex);
        my_state = state;
        pthread_mutex_unlock(&dispatcher_mutex);
        if(my_state == S_READING_MSG) {
            uint8_t* raw_message = POSTMAN_read_request();
            if(raw_message != NULL) {
                uint8_t error_value = *raw_message;
                if(error_value == EBADF) {
                    pthread_mutex_lock(&dispatcher_mutex);
                    state = S_WAITING_RECONNECTION;
                    pthread_mutex_unlock(&dispatcher_mutex);
                }
                else {
                    Communication_Protocol_Head msg_decoded = decode_message(raw_message);
                    dispatch_received_msg(msg_decoded);
                }
                free(raw_message);
            }
        }
    }
    return 0;
}

static int dispatch_received_msg(Communication_Protocol_Head msg) {
    switch(msg.msg_type)
    {
        case SEND_MOVES_TRAJECTORY :
        {
            int size = (int)data_received[0];
            if (count_command < size-1) {
                list_commands[count_command] = data_received[1];
                count_command++;
            }
            else {
                list_commands[count_command] = data_received[1];
                PILOT_send_moves_trajectory(list_commands, size);
                count_command = 0;
            }
            break;
        }
        case SEND_MOVE_CARTOGRAPHY :
        {
            switch((int)data_received[0]){
                case 0 : PILOT_send_move_cartography(FORWARD); break;
                case 1 : PILOT_send_move_cartography(RIGHT); break;
                case 2 : PILOT_send_move_cartography(LEFT); break;
                default : break;
            }
            break;
        }
        case STOP_ROBOT :
        {
            PILOT_stop_robot();
            break;
        }
        case SEND_ROBOT_POSITION :
        {
            Position robot_position;
            robot_position.coord_x = (int)data_received[0];
            robot_position.coord_y = (int)data_received[1];
            robot_position.dir = (Direction)data_received[2];
            PILOT_send_robot_position(&robot_position);
            break;
        }
        default :
        {
            //Should not get here
            break;
        }
    }
    return 0;
}

static Communication_Protocol_Head decode_message(uint8_t* raw_message) {
    Communication_Protocol_Head msg;
    msg.msg_size = (raw_message[0] << 8) | raw_message[1];
    msg.msg_type = ntohs((raw_message[2] << 8) | raw_message[3]);
    if(msg.msg_size > 2) {
        memcpy(data_received, raw_message + 4,  msg.msg_size - 2);
    }
    return msg;
}