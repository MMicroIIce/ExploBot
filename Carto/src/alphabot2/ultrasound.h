/**
 * \file  ultrasound.h
 * \version  1.0
 * \author Thomas ROCHER
 * \date November 24, 2023
 * \brief Header file of the motor peripheral.
 *
 * \see ultrasound.c
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
#ifndef SRC_ALPHABOT2_ULTRASOUND_H_
#define SRC_ALPHABOT2_ULTRASOUND_H_
/* ----------------------  INCLUDES ------------------------------------------*/
#include <stdbool.h>
/* ----------------------  PUBLIC TYPE DEFINITIONS ---------------------------*/
/* ----------------------  PUBLIC ENUMERATIONS -------------------------------*/
/* ----------------------  PUBLIC STRUCTURES ---------------------------------*/
/* ----------------------  PUBLIC VARIABLES -----------------------------------*/
/* ----------------------  PUBLIC FUNCTIONS PROTOTYPES  ----------------------*/
/**
 * \fn extern void ULTRASOUND_create()
 * \brief Initialize Ultrasound.
 * \author Thomas ROCHER
 */
extern void ULTRASOUND_create();
/**
 * \fn extern void ULTRASOUND_destroy()
 * \brief Deinitialize Ultrasound.
 * \author Thomas ROCHER
 */
extern void ULTRASOUND_destroy();

/**
 * \fn extern void ULTRASOUND_check_obstacle()
 * \brief Checks if there is an obstacle less than 12cm from the robot.
 * \author Thomas ROCHER
 */
extern bool ULTRASOUND_check_obstacle();
#endif /* SRC_ALPHABOT2_ULTRASOUND_H_ */
