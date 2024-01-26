/**
 * \file dispatcher.h
 * \version  1.0
 * \author Joshua MONTREUIL
 * \date May 7, 2023
 * \brief Header file for the dispatcher module. The dispatcher is in charge of the reading, parsing and dispatching of
 * any messages on the postman's socket.
 *
 * \see dispatcher.c
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
#ifndef SRC_COM_DISPATCHER_H_
#define SRC_COM_DISPATCHER_H_
/* ----------------------  INCLUDES ------------------------------------------*/
/* ----------------------  PUBLIC TYPE DEFINITIONS ---------------------------*/
/* ----------------------  PUBLIC ENUMERATIONS -------------------------------*/
/* ----------------------  PUBLIC STRUCTURES ---------------------------------*/
/* ----------------------  PUBLIC VARIABLES ----------------------------------*/
/* ----------------------  PUBLIC FUNCTIONS PROTOTYPES  ----------------------*/
/**
 * \fn extern int DISPATCHER_create(void)
 * \brief Creates the Dispatcher object in memory.
 * \author Joshua MONTREUIL
 */
extern int DISPATCHER_create(void);
/**
 * \fn extern int DISPATCHER_destroy(void)
 * \brief Destroys the object Dispatcher from memory.
 * \author Joshua MONTREUIL
 */
extern int DISPATCHER_destroy(void);
/**
 * \fn extern int DISPATCHER_start(void)
 * \brief Starts the module.
 * \author Joshua MONTREUIL
 *
 * \return On success, returns 0. On error, returns -1.
 */
extern int DISPATCHER_start(void);
/**
 * \fn extern void DISPATCHER_stop(void)
 * \brief Stops the module.
 * \author Joshua MONTREUIL
 *
 * \return On success, returns 0. On error, returns -1.
 */
extern int DISPATCHER_stop(void);
/**
 * \fn extern void DISPATCHER_start_reading(void)
 * \brief Begins to read.
 * \author Joshua MONTREUIL.
 */
extern void DISPATCHER_start_reading(void);
/**
 * \fn extern void DISPATCHER_disconnect(void);
 * \brief TODO
 * \author Thomas ROCHER.
 */
extern void DISPATCHER_disconnect(void);

#endif /* SRC_COM_DISPATCHER_H_ */
