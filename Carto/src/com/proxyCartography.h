/**
 * \file  proxyCartography.h
 * \version  0.1
 * \author Thomas ROCHER
 * \date November 27, 2023
 * \brief TODO
 *
 * \see proxyCartography.c
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
#ifndef SRC_COM_PROXYCARTOGRAPHY_H_
#define SRC_COM_PROXYCARTOGRAPHY_H_
/* ----------------------  INCLUDES ------------------------------------------*/
/* ----------------------  PUBLIC TYPE DEFINITIONS ---------------------------*/
/* ----------------------  PUBLIC ENUMERATIONS -------------------------------*/
/* ----------------------  PUBLIC STRUCTURES ---------------------------------*/
/* ----------------------  PUBLIC VARIABLES ----------------------------------*/
/* ----------------------  PUBLIC FUNCTIONS PROTOTYPES  ----------------------*/
/**
 * \fn extern void PROXYCARTOGRAPHY_robot_position_received()
 * \brief Sends a confirmation that the robot's position has been received.
 * \author Thomas Rocher
 */
extern void PROXYCARTOGRAPHY_robot_position_received();
/**
 * \fn extern void PROXYCARTOGRAPHY_move_done()
 * \brief Sends a confirmation that the command order has been executed.
 * \author Thomas Rocher
 */
extern void PROXYCARTOGRAPHY_move_done();

#endif /* SRC_COM_PROXYCARTOGRAPHY_H_ */
