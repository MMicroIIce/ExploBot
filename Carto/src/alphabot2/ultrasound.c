/**
 * \file  ultrasound.c
 * \version  1.0
 * \author Thomas ROCHER
 * \date Nov 24, 2023
 * \brief Source file of the motor peripheral.
 *
 * \see ultrasound.h
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
#include "ultrasound.h"

#include <wiringPi.h>
/* ----------------------  PRIVATE CONFIGURATIONS  -------------------------- */
/**
 * \def TRIG
 * ECHO pin.
 */
#define TRIG 3
/**
 * \def ECHO
 * ECHO pin.
 */
#define ECHO 2
/* ----------------------  PRIVATE TYPE DEFINITIONS  ------------------------ */
/* ----------------------  PRIVATE STRUCTURES  ------------------------------ */
/* ----------------------  PRIVATE ENUMERATIONS  ---------------------------- */
/* ----------------------  PRIVATE FUNCTIONS PROTOTYPES  -------------------- */
double get_obstacle_distance();
/* ----------------------  PRIVATE VARIABLES  ------------------------------- */
/* ----------------------  PUBLIC FUNCTIONS  -------------------------------- */

void ULTRASOUND_create()
{
    wiringPiSetup();  // Initialize WiringPi
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    delay(30);  // Allow the sensor to settle
}

bool ULTRASOUND_check_obstacle() {
    bool check = (get_obstacle_distance() <= 12.0) ? true : false;
    return check;
}

void ULTRASOUND_destroy(){}

/* ----------------------  PRIVATE FUNCTIONS  -------------------- */

double get_obstacle_distance()
{
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    //printf("Attente d'un signal\n");
    while (digitalRead(ECHO) == LOW);
    long startTime = micros();
    while (digitalRead(ECHO) == HIGH);
    long travelTime = micros() - startTime;
    double distance = travelTime / 58.0;
    return distance;
}