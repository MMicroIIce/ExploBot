/**
 * \file  proxyCartography.c
 * \version  0.1
 * \author Thomas ROCHER
 * \date Nov 29, 2023
 * \brief TODO
 *
 * \see proxyCartography.h
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
#include "proxyCartography.h"
#include "postman.h"
#include "../lib/defs.h"
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>


/* ----------------------  PRIVATE CONFIGURATIONS  -------------------------- */
/* ----------------------  PRIVATE TYPE DEFINITIONS  ------------------------ */
/* ----------------------  PRIVATE STRUCTURES  ------------------------------ */
/* ----------------------  PRIVATE ENUMERATIONS  ---------------------------- */
/* ----------------------  PRIVATE FUNCTIONS PROTOTYPES  -------------------- */
/* ----------------------  PRIVATE VARIABLES  ------------------------------- */
/* ----------------------  PUBLIC FUNCTIONS  -------------------------------- */

extern void PROXYCARTOGRAPHY_robot_position_received() {
    Communication_Protocol_Head msg_to_send;
    msg_to_send.msg_type = htons(ROBOT_POSITION_RECEIVED);
    msg_to_send.msg_size = htons((0x0002));
    uint8_t * data = (uint8_t*) malloc(2 + msg_to_send.msg_size);
    memcpy(data,&msg_to_send,4);
    POSTMAN_send_request(data);
}

extern void PROXYCARTOGRAPHY_move_done() {
    Communication_Protocol_Head msg_to_send;
    msg_to_send.msg_type = htons(MOVE_DONE);
    msg_to_send.msg_size = htons((0x0002));
    uint8_t * data = (uint8_t*) malloc(2 + msg_to_send.msg_size);
    memcpy(data,&msg_to_send,4);
    POSTMAN_send_request(data);
}

/* ----------------------  PRIVATE FUNCTIONS  ------------------------------- */