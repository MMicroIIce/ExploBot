/**
 * \file  postman.c
 * \version  1.1
 * \authors Joshua MONTREUIL, Thomas ROCHER
 * \date May 3, 2023
 * \brief Source file of the postman module. Handles socket connectivity.
 *
 * \see postman.h
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
 * \copyright Prose A2 2023
 * 
 */

/* ----------------------  INCLUDES  ---------------------------------------- */
#include "postman.h"
#include "dispatcher.h"
#include "../lib/defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <mqueue.h>

/* ----------------------  PRIVATE CONFIGURATIONS  -------------------------- */
#define STATE_GENERATION S(S_FORGET) S(S_WAITING_CONNECTION) S(S_WRITE_MSG_ON_SOCKET) S(S_DEATH)
#define S(x) x,
typedef enum {STATE_GENERATION STATE_NB} State_Machine;
#undef STATE_GENERATION
#undef S

#define ACTION_GENERATION A(A_NOP) A(A_DISCONNECT) A(A_CONNECTED) A(A_CONNECTION_POLLING) A(A_SEND) A(A_STOP)
#define A(x) x,
typedef enum {ACTION_GENERATION ACTION_NB} Action;
#undef ACTION_GENERATION
#undef A

#define EVENT_GENERATION E(E_CONNECTION) E(E_WRITE_REQUEST) E(E_DISCONNECTION) E(E_POLL_CONNECTION) E(E_STOP)
#define E(x) x,
typedef enum {EVENT_GENERATION EVENT_NB} Event;
#undef EVENT_GENERATION
#undef E
/**
* \def MAX_PENDING_CONNECTIONS
* Max amount of connected clients.
*/
#define MAX_PENDING_CONNECTIONS 1
/**
 * \def MQ_POSTMAN_BOX_NAME
 * Name of the message queue
 */
#define MQ_POSTMAN_BOX_NAME ("/mb")
/**
 * \def MQ_MSG_COUNT
 * Max amount of message into the message queue.
 */
#define MQ_MSG_COUNT 10
/**
 * \def SERVER_PORT
 * Server port.
 */
#define SERVER_PORT 12345
/* ----------------------  PRIVATE TYPE DEFINITIONS  ------------------------ */
/**
 * \struct Mq_Msg_Data postman.c "com/postman.c"
 * \brief Definition of the Mq_Msg_Data type.
 *
 * Mq_Msg_Data contains an event for the state machine. And the message to send through socket.
 */
typedef struct {
    Event event; /**< Event to change the state of the state machine. */
    uint8_t * data; /**< Data to send through socket. */
} Mq_Msg_Data;
/**
 * \union Mq_Msg postman.c "com/postman.c"
 * \brief Message queue type as an union.
 *
 * This union allows to transform a buffer into the Mq_Msg_Data structure.
 */
typedef union {
	Mq_Msg_Data msg_data; /**< Data structure. */
	char buffer[sizeof(Mq_Msg_Data)]; /**< Raw message. */
} Mq_Msg;
/**
 * \struct Transition postman.c "com/postman.c"
 * \brief Gives an action and state destination for a transition.
 *
 * Transition gives the action to perform when an event has been fired. Also gives the destination state.
 */
typedef struct {
	State_Machine state_destination; /**< Next state. */
	Action action; /**< Action to perform from previous the event. */
} Transition;
/**
 * \typedef int(*Action_Pt)(uint8_t * raw_data)
 * \brief Definition of function pointer for the actions to perform.
 */
typedef int(*Action_Pt)(uint8_t * raw_data);
/* ----------------------  PRIVATE STRUCTURES  ------------------------------ */
/* ----------------------  PRIVATE ENUMERATIONS  ---------------------------- */
/* ----------------------  PRIVATE FUNCTIONS PROTOTYPES  -------------------- */
/* ----- PASSIVES ----- */
/**
 * \fn static uint8_t* POSTMAN_read_msg(void)
 * \brief Reads messages on the socket.
 * \author Joshua MONTREUIL
 *
 * \param raw_message : raw_message pointer.
 *
 * \return uint8_t* : Raw message in a buffer from socket on success. NULL on failure.
 */
static uint8_t* POSTMAN_read_msg(void);
/* ----- ACTIVE ----- */
/**
 * \fn static void * POSTMAN_run(void * arg)
 * \brief Called by a thread. This function is the "active" part of the postman. It connects Cute? It reads messages received in the
 * message queue and sends it through socket.
 * \author Joshua MONTREUIL
 *
 * \param arg : argument pointer.
 *
 * \return void * : On success, returns 0. On error, returns -1.
 */
static void * POSTMAN_run(void * arg);
/**
 * \fn static int POSTMAN_mq_receive(Mq_Msg * a_msg)
 * \brief Receives the messages from the queue.
 * \author Joshua MONTREUIL
 *
 * \param a_msg : pointer to Mq_Msg struct.
 *
 * \return On success, returns 0. On error, returns -1.
 */
static int POSTMAN_mq_receive(Mq_Msg * a_msg);
/**
 * \fn static int POSTMAN_mq_send(Mq_Msg * a_msg)
 * \brief Sends a message into the queue.
 * \author Joshua MONTREUIL
 *
 * \param a_msg : pointer to Mq_Msg struct.
 *
 * \return On success, returns 0. On error, returns -1.
 */
static int POSTMAN_mq_send(Mq_Msg * a_msg);
/* ----- ACTIONS ----- */
/**
 * \fn static void POSTMAN_action_nop(uint8_t * raw_data)
 * \brief Used to ignore state case that aren't into the state machine.
 * \author Joshua MONTREUIL
 *
 * \param raw_data : raw data to send.
 *
 * \return On success, returns 0. On error, returns -1.
 */
static int POSTMAN_action_nop(uint8_t * raw_data);
/**
 * \fn static int POSTMAN_action_disconnection(uint8_t * raw_data)
 * \brief Handles a disconnection.
 * \author Joshua MONTREUIL
 *
 * \param raw_data : raw data to send.
 *
 * \return On success, returns 0. On error, returns -1.
 */
static int POSTMAN_action_disconnection(uint8_t * raw_data);
/**
 * \fn static void POSTMAN_action_connected(uint8_t * raw_data)
 * \brief Passive function to log when Cute is connected.
 * \author Joshua MONTREUIL
 *
 * \param raw_data : raw data to send.
 *
 * \return On success, returns 0. On error, returns -1.
 */
static int POSTMAN_action_connected(uint8_t * raw_data);
/**
 * \fn static int POSTMAN_action_polling_connection(uint8_t * raw_data))
 * \brief Waits a connection to the socket.
 * \author Joshua MONTREUIL
 *
 * \param a_msg : pointer to Mq_Msg struct.
 *
 * \return On success, returns 0. On error, returns -1.
 */
static int POSTMAN_action_polling_connection(uint8_t * raw_data);
/**
 * \fn static int POSTMAN_action_send_msg(uint8_t * raw_data)
 * \brief Sends a message through socket.
 * \author Joshua MONTREUIL
 *
 * \param raw_data : raw data to send.
 *
 * \return On success, returns 0. On error, returns -1.
 */
static int POSTMAN_action_send_msg(uint8_t * raw_data);
/* ----------------------  PRIVATE VARIABLES  ------------------------------- */
/**
 * \var static int listen_socket
 * \brief Listening socket identifier.
 */
static int listen_socket;
/**
 * \var static int data_socket
 * \brief Listening socket identifier.
 */
static int data_socket;
/**
 * \var static pthread_t postman_thread
 * \brief Postman thread.
 */
static pthread_t postman_thread;
/**
 * \var static mqd_t my_mail_box
 * \brief Message queue reference.
 */
static mqd_t my_mail_box;
/**
 * \var static struct sockaddr_in my_address
 * \brief Address parameters of the server.
 */
static struct sockaddr_in my_address;
/**
 * \var tatic bool_e is_in_waiting_connection
 * \brief Tells if the postman thread is blocked waiting a connection. (used only for stop).
 */
static bool_e is_in_waiting_connection = FALSE;
/**
 * \var static const Action_Pt actions_tab[ACTION_NB]
 * \brief Array of function pointer to call from action to perform.
 */
static const Action_Pt actions_tab[ACTION_NB] = {
    &POSTMAN_action_nop,
    &POSTMAN_action_disconnection,
    &POSTMAN_action_connected,
    &POSTMAN_action_polling_connection,
    &POSTMAN_action_send_msg,
    &POSTMAN_action_nop
};
/**
 * \var static Transition my_state_machine [STATE_NB -1][EVENT_NB]
 * \brief Array representing the state machine.
 */
static Transition my_state_machine [STATE_NB -1][EVENT_NB] = {
    [S_WAITING_CONNECTION]  [E_POLL_CONNECTION] = {S_WAITING_CONNECTION,    A_CONNECTION_POLLING},
    [S_WAITING_CONNECTION]  [E_CONNECTION]      = {S_WRITE_MSG_ON_SOCKET,   A_CONNECTED},
    [S_WAITING_CONNECTION]  [E_STOP]            = {S_DEATH,                 A_STOP},
    [S_WRITE_MSG_ON_SOCKET] [E_WRITE_REQUEST]   = {S_WRITE_MSG_ON_SOCKET,   A_SEND},
    [S_WRITE_MSG_ON_SOCKET] [E_DISCONNECTION]   = {S_WAITING_CONNECTION,    A_DISCONNECT},
    [S_WRITE_MSG_ON_SOCKET] [E_CONNECTION]      = {S_WRITE_MSG_ON_SOCKET,   A_CONNECTED},
    [S_WRITE_MSG_ON_SOCKET] [E_STOP]            = {S_DEATH,                 A_STOP},
};
/* ----------------------  PUBLIC FUNCTIONS  -------------------------------- */
int POSTMAN_create(void) {
    struct mq_attr mqa = {
    .mq_maxmsg = MQ_MSG_COUNT,
    .mq_msgsize = sizeof(Mq_Msg)
    };
    errno = 0;

    if((my_mail_box = mq_open(MQ_POSTMAN_BOX_NAME, O_CREAT | O_RDWR | O_EXCL , 0644 ,&mqa)) == -1) {
        if(errno == EEXIST) {
            mq_unlink(MQ_POSTMAN_BOX_NAME);
            if((my_mail_box = mq_open(MQ_POSTMAN_BOX_NAME, O_CREAT | O_RDWR , 0644 ,&mqa)) == -1) {
                perror("mq_open failed ");
                return -1;
            }
        }
        else {
            perror("mq_open failed ");
            return -1;
        }
    }
    if((listen_socket =  socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        //CONTROLLER_LOGGER_log(ERROR, "On socket() : socket failed to be created for the listening socket.");
        goto error_socket;
    }
    my_address.sin_family = AF_INET;
    my_address.sin_port = htons(SERVER_PORT);
    my_address.sin_addr.s_addr = htonl(INADDR_ANY);
    return 0;

    error_socket :
        mq_close(my_mail_box);
        mq_unlink(MQ_POSTMAN_BOX_NAME);
    return -1;
}

int POSTMAN_start(void) {
    if(bind(listen_socket, (struct sockaddr *)&my_address, sizeof(my_address)) == -1) {
        perror("bind() failed");
        return -1;
    }
    if(listen(listen_socket, MAX_PENDING_CONNECTIONS) == -1) {
        perror("listen() failed");
        return -1;
    }
    if(pthread_create(&postman_thread, NULL, POSTMAN_run, NULL) != 0 ) {
        perror("pthread_create failed");
        return -1;
    }
    return 0;
}

int POSTMAN_send_request(uint8_t * data) {
    Mq_Msg my_msg = {.msg_data.event = E_WRITE_REQUEST, .msg_data.data = data};
    if(POSTMAN_mq_send(&my_msg) == -1) {
        return -1;
    }
    return 0;
}

uint8_t* POSTMAN_read_request(void) {
    return POSTMAN_read_msg();
}

int POSTMAN_disconnect(void) {
    Mq_Msg my_msg = {.msg_data.event = E_DISCONNECTION,0};
    if(POSTMAN_mq_send(&my_msg) == -1) {
        return -1;
    }
    return 0;
}

int POSTMAN_stop(void) {
    Mq_Msg my_msg = {.msg_data.event = E_STOP,0};
    if(POSTMAN_mq_send(&my_msg) == 0 ) {
        if(pthread_join(postman_thread, NULL) != 0) {
            perror("pthread_join failed");
            return -1;
        }
    }
    else {
        return -1;
    }
    if(!listen_socket) {
        if(close(listen_socket) == -1) {
            perror("close() failed");
            return -1;
        }
    }
    if(!data_socket) {
        if(close(data_socket) == -1) {
            perror("close() failed");
            return -1;
        }
    }
    if(mq_close(my_mail_box) == -1) {
        perror("mq_close() failed");
        return -1;
    }
    return 0;
}

int POSTMAN_destroy(void) {
    if(mq_unlink(MQ_POSTMAN_BOX_NAME) == -1) {
        perror("mq_unlink() failed");
        return -1;
    }
    return 0;
}
/* ----------------------  PRIVATE FUNCTIONS  ------------------------------- */
static int POSTMAN_action_send_msg(uint8_t * raw_data) {
    int amount_sent;
    int message_size = raw_data[0] << 8 | raw_data[1];
    if((amount_sent = write(data_socket, raw_data, (message_size+2))) == -1 && errno != EPIPE) {
        perror("write() failed");
        return -1;
    }
    else if(errno == EPIPE) {
        Mq_Msg my_msg = {.msg_data.event = E_DISCONNECTION,0};
        if(POSTMAN_mq_send(&my_msg) == -1) {
            perror("POSTMAN_mq_send failed");
            return -1;
        }
    }
    else if(amount_sent < message_size + 2) {
        if(write(data_socket, raw_data, sizeof(raw_data)) == -1) {
            perror("write() failed");
            return -1;
        }
    }
    if(amount_sent == sizeof(raw_data)) {
    }
    free(raw_data);
    return 0;
}

static uint8_t* POSTMAN_read_msg(void) {
    uint8_t size_check[2];
    errno = 0;
    int read_size = read(data_socket, size_check, 2);
    if(read_size == -1 ){
        if(errno == EBADF) {
            printf("The data socket for reading has been closed, a disconnection has been asked or detected.");
            uint8_t * error_buffer = NULL;
            error_buffer = (uint8_t*)malloc(1);
            *error_buffer = (uint8_t) errno;
            return error_buffer;
        }
        else {
            perror("read() failed");
            return NULL;
        }
    }
    else if(read_size == 0)
    {
        printf("Déconnexion\n");
        DISPATCHER_disconnect();
        POSTMAN_disconnect();
        return NULL;
    }
    else {
        int data_size = size_check[0] << 8 | size_check[1];
        uint8_t data_buff[data_size];
        if (read(data_socket, data_buff, data_size) == -1) {
            perror("read() failed");
            return NULL;
        }
        uint8_t * raw_message = NULL;
        if(data_size >= 2) {
            data_size +=2;
            raw_message = (uint8_t *) malloc(data_size);
            memcpy(raw_message, size_check, 2);
            memcpy(raw_message + 2, data_buff, data_size);
        }
        else {
            raw_message = (uint8_t *) malloc(data_size);
            memcpy(raw_message, size_check, 2);
        }

        return raw_message;
    }
}

static void * POSTMAN_run(void * arg) {
    Mq_Msg msg;
    State_Machine my_state = S_WAITING_CONNECTION;
    if(actions_tab[A_CONNECTION_POLLING](NULL) == -1) {
        perror("action_tab failed");
        return NULL;
    }
    Transition * my_transition;
    while(my_state != S_DEATH) {
        if (POSTMAN_mq_receive(&msg) == -1) {
            perror("POSTMAN_mq_receive failed");
            return NULL;
        }
        my_transition = &my_state_machine[my_state][msg.msg_data.event];
        uint8_t * raw_data = msg.msg_data.data;
        if(my_transition->state_destination != S_FORGET) {
            if(actions_tab[my_transition->action](raw_data) == -1) {
                perror("action_tab failed");
                return NULL;
            }
            my_state = my_transition->state_destination;
        }
    }
    return 0;
}

static int POSTMAN_action_polling_connection(uint8_t * raw_data) {
   socklen_t addr_len = sizeof(my_address);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    fd_set l_read_fds;
    FD_ZERO(&l_read_fds);
    FD_SET(listen_socket, &l_read_fds);

    int select_state = select(listen_socket + 1, &l_read_fds, NULL, NULL, &timeout);
    if (select_state == -1) {
        perror("select() failed");
        return -1;
    } 
    else if (select_state) {
        // A connection is pending
        data_socket = accept(listen_socket, (struct sockaddr *)&my_address, &addr_len);
        if(data_socket == -1) {
            perror("accept() failed");
            return -1;
        }

        Mq_Msg msg = {.msg_data.event = E_CONNECTION, NULL};
        if (POSTMAN_mq_send(&msg) == -1) {
            return -1;
        }
        DISPATCHER_start_reading();
        is_in_waiting_connection = FALSE;
        printf("CONNEXION\n");
    }
    else {
        // No connection could be found
        Mq_Msg msg = {.msg_data.event = E_POLL_CONNECTION, NULL};
        if (POSTMAN_mq_send(&msg) == -1) {
            return -1;
        }
    }
    return 0;
}

static int POSTMAN_mq_receive(Mq_Msg * a_msg) {
    if((mq_receive(my_mail_box,a_msg->buffer,sizeof(Mq_Msg), NULL) == -1)) {
        perror("mq_receive failed");
        mq_close(my_mail_box);
        mq_unlink(MQ_POSTMAN_BOX_NAME);
        return -1;
    }
    return 0;
}

static int POSTMAN_mq_send(Mq_Msg * a_msg) {
    if(mq_send(my_mail_box,a_msg->buffer, sizeof(Mq_Msg),0) == -1 ) {
        perror("mq_send failed");
        mq_close(my_mail_box);
        mq_unlink(MQ_POSTMAN_BOX_NAME);
        return -1;
    }
    return 0;
}

static int POSTMAN_action_nop(uint8_t * raw_data) { return 0; }

static int POSTMAN_action_connected(uint8_t * raw_data) {
    return 0;
}

static int POSTMAN_action_disconnection(uint8_t * raw_data) {
    if(close(data_socket) == -1) {
        return -1;
    }
    Mq_Msg my_msg = {.msg_data.event = E_POLL_CONNECTION, NULL};
    if(POSTMAN_mq_send(&my_msg) == -1) {
        return -1;
    }
    is_in_waiting_connection = TRUE;
    return 0;
}