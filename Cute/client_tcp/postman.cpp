/**
 * \file  postman.cpp
 * \version  0.1
 * \authors Thomas ROCHER, Joshua MONTREUIL
 * \date Dec 18, 2023
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

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cstdint>
#include <pthread.h>
#include <mqueue.h>
#include <arpa/inet.h>
#include <cerrno>
#include <unistd.h>

#include "defs.h"
#include "dispatcher.h"
#include "qlogging.h"

/* ----------------------  PRIVATE CONFIGURATIONS  -------------------------- */

/**
 * \enum State_Machine
 * \brief Defines state machine.
 */
enum State_Machine {
    S_FORGET=0,
    S_WAITING_CONNECTION,
    S_WRITE_MSG_ON_SOCKET,
    S_DEATH,
    STATE_NB
};

/**
 * \enum Action
 * \brief Defines actions of the postman's state machine.
 */
enum Action {
    A_NOP=0,
    A_DISCONNECT,
    A_CONNECTED,
    A_CONNECTION_POLLING,
    A_SEND,
    A_STOP,
    ACTION_NB
};

/**
 * \enum Event
 * \brief Defines events of the postman's state machine.
 */
enum Event {
    E_CONNECTION=0,
    E_WRITE_REQUEST,
    E_DISCONNECTION,
    E_POLL_CONNECTION,
    E_STOP,
    EVENT_NB
};

/**
 * \def MQ_POSTMAN_BOX_NAME
 * Name of the message queue
 */
#define MQ_POSTMAN_BOX_NAME "/mb"
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
/**
 * \def SERVER_PORT
 * IP address.
 */
#define IP_ADDRESS "10.3.141.1"

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
 * \brief Called by a thread. This function is the "active" part of the postman. It connects Carto? It reads messages received in the
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
/**
 * \fn void initialize_state_machine(Transition my_state_machine[][EVENT_NB])
 * \brief Initialize the state machine.
 *
 * \param my_state_machine : 2D array representing the state machine.
 */
static void initialize_state_machine(Transition my_state_machine[][EVENT_NB]);
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
 * \brief Passive function to log when Carto is connected.
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
 * \var static int client_socket
 * \brief Client socket identifier.
 */
static int client_socket;
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
static bool_e is_in_waiting_connection = bool_e::FALSE;
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

static Transition my_state_machine[STATE_NB - 1][EVENT_NB];

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
    // Création du socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        mq_close(my_mail_box);
        mq_unlink(MQ_POSTMAN_BOX_NAME);
        perror("socket failed ");
        return -1;
    }
    my_address.sin_family = AF_INET;
    my_address.sin_port = htons(SERVER_PORT);
    return 0;


        mq_close(my_mail_box);
        mq_unlink(MQ_POSTMAN_BOX_NAME);
    return -1;
}

int POSTMAN_start(void) {
    if (inet_pton(AF_INET, IP_ADDRESS, &my_address.sin_addr) <= 0) {
        perror("Erreur lors de la conversion de l'adresse IP");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    if(pthread_create(&postman_thread, NULL, POSTMAN_run, NULL) != 0 ) {
        return -1;
    }
    return 0;
}

int POSTMAN_send_request(uint8_t * data) {
    Mq_Msg my_msg;
    my_msg.msg_data.event = E_WRITE_REQUEST;
    my_msg.msg_data.data = data;
    if(POSTMAN_mq_send(&my_msg) == -1) {
        return -1;
    };
    POSTMAN_action_send_msg(data);
    return 0;
}

uint8_t* POSTMAN_read_request(void) {
    return POSTMAN_read_msg();
}

int POSTMAN_disconnect(void) {
    Mq_Msg my_msg;
    my_msg.msg_data.event = E_DISCONNECTION;
    my_msg.msg_data.data = nullptr;
    if(POSTMAN_mq_send(&my_msg) == -1) {
        return -1;
    }
    return 0;
}

int POSTMAN_stop(void) {
    Mq_Msg my_msg;
    my_msg.msg_data.event = E_STOP;
    my_msg.msg_data.data = nullptr;
    if(POSTMAN_mq_send(&my_msg) == 0 ) {
        if(pthread_join(postman_thread, NULL) != 0) {
            return -1;
        }
    }
    else {
        return -1;
    }
    if(!client_socket) {
        if(close(client_socket) == -1) {
            return -1;
        }
    }
    if(mq_close(my_mail_box) == -1) {
        return -1;
    }
    return 0;
}

int POSTMAN_destroy(void) {
    if(mq_unlink(MQ_POSTMAN_BOX_NAME) == -1) {
        return -1;
    }
    return 0;
}
/* ----------------------  PRIVATE FUNCTIONS  ------------------------------- */
static int POSTMAN_action_send_msg(uint8_t * raw_data) {
    int amount_sent;
    int message_size = raw_data[0] << 8 | raw_data[1];
    if((amount_sent = write(client_socket, raw_data, (message_size+2))) == -1 && errno != EPIPE) {
        return -1;
    }
    else if(errno == EPIPE) {
        Mq_Msg my_msg;
        my_msg.msg_data.event = E_DISCONNECTION;
        my_msg.msg_data.data = nullptr;
        if(POSTMAN_mq_send(&my_msg) == -1) {
            return -1;
        }
    }
    else if(amount_sent < message_size + 2) {
        if(write(client_socket, raw_data, sizeof(raw_data)) == -1) {
            return -1;
        }
    }
    return 0;
}

static uint8_t* POSTMAN_read_msg(void) {
    uint8_t size_check[2];
    errno = 0;
    int read_size = read(client_socket, size_check, 2);
    if(read_size == -1 ){
        if(errno == EBADF) {
            uint8_t * error_buffer = NULL;
            error_buffer = (uint8_t*)malloc(1);
            *error_buffer = (uint8_t) errno;
            return error_buffer;
        }
        else {
            return NULL;
        }
    }
    else if(read_size == 0)
    {
        DISPATCHER_disconnect();
        POSTMAN_disconnect();
        return NULL;
    }
    else {
        int data_size = size_check[0] << 8 | size_check[1];
        uint8_t data_buff[data_size];
        if (read(client_socket, data_buff, data_size) == -1) {
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
        return NULL;
    }
    while(my_state != S_DEATH) {
        if (POSTMAN_mq_receive(&msg) == -1) {
            return NULL;
        }

        uint8_t * raw_data = msg.msg_data.data;

        if (my_state == S_WAITING_CONNECTION)
        {
            if(msg.msg_data.event == E_POLL_CONNECTION) {
                POSTMAN_action_polling_connection(raw_data);
                my_state = S_WAITING_CONNECTION;
            }
            else if(msg.msg_data.event == E_CONNECTION) {
                POSTMAN_action_connected(raw_data);
                my_state = S_WRITE_MSG_ON_SOCKET;
            }
            else if(msg.msg_data.event == E_STOP) {
                my_state = S_DEATH;
            }
            else{
                POSTMAN_action_nop(raw_data);
            }
        }
        else if (my_state == S_WRITE_MSG_ON_SOCKET)
        {
            if(msg.msg_data.event == E_WRITE_REQUEST) {
                //POSTMAN_action_send_msg(raw_data);
                my_state = S_WRITE_MSG_ON_SOCKET;
            }
            else if(msg.msg_data.event == E_DISCONNECTION) {
                POSTMAN_action_disconnection(raw_data);
                my_state = S_WAITING_CONNECTION;
            }
            else if(msg.msg_data.event == E_CONNECTION) {
                POSTMAN_action_connected(raw_data);
                my_state = S_WRITE_MSG_ON_SOCKET;
            }
            else if(msg.msg_data.event == E_STOP) {
                my_state = S_DEATH;
            }
            else {
                POSTMAN_action_nop(raw_data);
            }
        }
    }
    return 0;
}

static int POSTMAN_action_polling_connection(uint8_t * raw_data) {
    socklen_t addr_len = sizeof(my_address);
    // Tentative de connexion non bloquante
    int connect_result = connect(client_socket, reinterpret_cast<struct sockaddr*>(&my_address), addr_len);
    if (connect_result == 0) {
        // Connexion réussie immédiatement
        std::cout << "Connexion réussie." << std::endl;
        Mq_Msg my_msg;
        my_msg.msg_data.event = E_CONNECTION;
        my_msg.msg_data.data = nullptr;
        if (POSTMAN_mq_send(&my_msg) == -1) {
            return -1;
        }
        DISPATCHER_start_reading();
        is_in_waiting_connection = bool_e::FALSE;
    }
    else {
        sleep(1);
        Mq_Msg my_msg;
        my_msg.msg_data.event = E_POLL_CONNECTION;
        my_msg.msg_data.data = nullptr;
        if (POSTMAN_mq_send(&my_msg) == -1) {
            return -1;
        }
    }
    return 0;
}

static int POSTMAN_mq_receive(Mq_Msg * a_msg) {
    if((mq_receive(my_mail_box,a_msg->buffer,sizeof(Mq_Msg), NULL) == -1)) {
        mq_close(my_mail_box);
        mq_unlink(MQ_POSTMAN_BOX_NAME);
        return -1;
    }
    return 0;
}

static int POSTMAN_mq_send(Mq_Msg * a_msg) {
    if(mq_send(my_mail_box, a_msg->buffer, sizeof(Mq_Msg), 0) == -1) {
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
    if(close(client_socket) == -1) {
        return -1;
    }
    Mq_Msg my_msg;
    my_msg.msg_data.event = E_POLL_CONNECTION;
    my_msg.msg_data.data = nullptr;
    if(POSTMAN_mq_send(&my_msg) == -1) {
        return -1;
    }
    is_in_waiting_connection = bool_e::TRUE;
    return 0;
}
