/**
 * \file  motor.c
 * \version  0.1
 * \author Fatoumata TRAORE inspiré des réalisations de joshua
 * \date Nov 16, 2023
 * \brief Source file of the motor peripheral.
 *
 * \see motor.h
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
#ifndef SRC_ALPHABOT2_MOTOR_H_
#define SRC_ALPHABOT2_MOTOR_H_
/* ----------------------  INCLUDES ------------------------------------------*/
#include "../lib/defs.h"
/* ----------------------  PUBLIC TYPE DEFINITIONS ---------------------------*/
/* ----------------------  PUBLIC ENUMERATIONS -------------------------------*/
/* ----------------------  PUBLIC STRUCTURES ---------------------------------*/
/* ----------------------  PUBLIC VARIABLES -----------------------------------*/
/* ----------------------  PUBLIC FUNCTIONS PROTOTYPES  ----------------------*/
/**
 * \fn extern int MOTOR_create()
 * \brief Initialize in memory the object Motor.
 * \author Fatoumata TRAORE
 *
 * \return int : error code 0 on success -1 on error.
 */

extern int MOTOR_create(void);
/**
 * \fn extern int MOTOR_destroy(void)
 * \brief Deinitialize the Motor.
 * \author fatoumata TRAORE
 *
 * \return int : error code 0 on success -1 on error.
 */
extern int MOTOR_destroy(void);

/**
 * \fn extern void MOTOR_set_command(Command cmd)
 * \brief Execute a move command.
 * \author Thomas ROCHER
 */
extern void MOTOR_set_command(Command cmd);

#endif /* SRC_ALPHABOT2_MOTOR_H_ */
