/**
 * \file  proxyMap.h
 * \version  0.1
 * \author Thomas ROCHER
 * \date Dec 10, 2023
 * \brief TODO
 *
 * \see proxyMap.c
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
#ifndef SRC_COM_PROXYMAP_H_
#define SRC_COM_PROXYMAP_H_
/* ----------------------  INCLUDES ------------------------------------------*/
/* ----------------------  PUBLIC TYPE DEFINITIONS ---------------------------*/
/* ----------------------  PUBLIC ENUMERATIONS -------------------------------*/
/* ----------------------  PUBLIC STRUCTURES ---------------------------------*/
/* ----------------------  PUBLIC VARIABLES ----------------------------------*/
/* ----------------------  PUBLIC FUNCTIONS PROTOTYPES  ----------------------*/
/**
 * \fn extern void PROXYMAP_set_obstacle_position(int coord_x, int coord_y)
 * \brief Sends an obstacle's position.
 * \author Thomas Rocher
 *
 * \param coord_x : x obstacle's coordinate.
 * \param coord_y : y obstacle's coordinate.
 */
extern void PROXYMAP_set_obstacle_position(int coord_x, int coord_y);
/**
 * \fn extern void PROXYMAP_set_robot_position(int coord_x, int coord_y)
 * \brief Send the robot's position.
 * \author Thomas Rocher
 *
 * \param coord_x : x obstacle's coordinate.
 * \param coord_y : y obstacle's coordinate.
 */
extern void PROXYMAP_set_robot_position(int coord_x, int coord_y);

#endif /* SRC_COM_PROXYMAP_H_ */
