/**
 * \file postman.h
 * \version  0.1
 * \author Joshua MONTREUIL
 * \date May 3, 2023
 * \brief Header file of the postman module. Handles socket connectivity.
 *
 * \see postman.c
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
 */

#ifndef SRC_COM_POSTMAN_H_
#define SRC_COM_POSTMAN_H_
/* ----------------------  INCLUDES ------------------------------------------*/
#include <stdint.h>
/* ----------------------  PUBLIC TYPE DEFINITIONS ---------------------------*/
/* ----------------------  PUBLIC ENUMERATIONS -------------------------------*/
/* ----------------------  PUBLIC STRUCTURES ---------------------------------*/
/* ----------------------  PUBLIC VARIABLES -----------------------------------*/
/* ----------------------  PUBLIC FUNCTIONS PROTOTYPES  ----------------------*/
/**
 * \fn extern int POSTMAN_create(void)
 * \brief Creates the Postman object in memory.
 * \author Joshua MONTREUIL
 *
 * \return On success, returns 0. On error, returns -1.
 */
extern int POSTMAN_create(void);
/**
 * \fn extern int POSTMAN_destroy(void)
 * \brief Destroys the object Postman from memory.
 * \author Joshua MONTREUIL
 *
 * \return On success, returns 0. On error, returns -1.
 */
extern int POSTMAN_destroy(void);
/**
 * \fn extern int POSTMAN_start(void)
 * \brief Starts the module.
 * \author Joshua MONTREUIL
 *
 * \return On success, returns 0. On error, returns -1.
 */
extern int POSTMAN_start(void);
/**
 * \fn extern int POSTMAN_stop(void)
 * \brief Stops the module.
 * \author Joshua MONTREUIL
 *
 * \return On success, returns 0. On error, returns -1.
 */
extern int POSTMAN_stop(void);
/**
 * \fn extern int POSTMAN_send_request(uint8_t * data)
 * \brief Sends a message through TCP.
 * \author Joshua MONTREUIL
 *
 * \param data : message data to be sent.
 *
 * \return On success, returns 0. On error, returns -1.
 */
extern int POSTMAN_send_request(uint8_t * data);
/**
 * \fn extern uint8_t * POSTMAN_read_request(void)
 * \brief Request a socket read action.
 * \author Joshua MONTREUIL
 *
 * \return uint8_t* : Raw message in a buffer from socket on success. NULL on failure.
 */
extern uint8_t* POSTMAN_read_request(void);
/**
 * \fn extern int POSTMAN_disconnect(void)
 * \brief Disconnect the socket link.
 * \author Joshua MONTREUIL
 */
extern int POSTMAN_disconnect(void);

#endif /* SRC_COM_POSTMAN_H_ */
