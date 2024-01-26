/**
 * \file  motor.c
 * \version  0.1
 * \authors Fatoumata TRAORE, Joshua Montreuil
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
/* ----------------------  INCLUDES  ---------------------------------------- */
#include "motor.h"
#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <time.h>
#include <math.h>

/* ----------------------  PRIVATE CONFIGURATIONS  -------------------------- */
/**
 * \def AIN1
 * Motor A pin 1.
 */
#define AIN1 26
/**
 * \def AIN2
 * Motor A pin 2.
 */
#define AIN2 23
/**
 * \def PWM_A
 * Motor A pwm pin.
 */
#define PWM_A 22
/**
 * \def BIN1
 * Motor B pin 1.
 */
#define BIN1 28
/**
 * \def BIN2
 * Motor B pin 2.
 */
#define BIN2 29
/**
 * \def PWM_B
 * Motor B pwm pin.
 */
#define PWM_B 25
/**
 * \def VELOCITY_DEFAULT
 * Default velocity
 */
#define VELOCITY_DEFAULT 35 //change depending on the hardware
/* ----------------------  PRIVATE TYPE DEFINITIONS  ------------------------ */
/* ----------------------  PRIVATE STRUCTURES  ------------------------------ */
/* ----------------------  PRIVATE ENUMERATIONS  ---------------------------- */
/* ----------------------  PRIVATE FUNCTIONS PROTOTYPES  -------------------- */
/* ----------------------  PRIVATE VARIABLES  ------------------------------- */
/* ----------------------  PUBLIC FUNCTIONS  -------------------------------- */
/**
 * \def total_distance
 * Distance totale
 */
//static double total_distance = 0.0;
static double distance_cm = 12.0;
static double vitesse_cm_par_seconde = VELOCITY_DEFAULT;
//static double temps_necessaire;
int MOTOR_create(void) {
    wiringPiSetup();
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);

    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);

    softPwmCreate(PWM_A, 0, 100);
    softPwmCreate(PWM_B, 0, 100);
    return 0;
}

void MOTOR_set_command(Command cmd) {
    softPwmWrite(PWM_A, VELOCITY_DEFAULT);
    softPwmWrite(PWM_B, VELOCITY_DEFAULT);
    double temps_necessaire = (double)distance_cm / vitesse_cm_par_seconde;
    switch (cmd) {
        case RIGHT : {
            digitalWrite(AIN1, LOW);
            digitalWrite(AIN2, HIGH);
            digitalWrite(BIN1, HIGH);
            digitalWrite(BIN2, LOW);
            delay(128); // //change depending on the hardware
            digitalWrite(AIN1, LOW);
            digitalWrite(AIN2, LOW);
            digitalWrite(BIN1, LOW);
            digitalWrite(BIN2, LOW);
            break;
        }
        case LEFT : {
            digitalWrite(AIN1, HIGH);
            digitalWrite(AIN2, LOW);
            digitalWrite(BIN1, LOW);
            digitalWrite(BIN2, HIGH);
            delay(205); // // //change depending on the hardware
            digitalWrite(AIN1, LOW);
            digitalWrite(AIN2, LOW);
            digitalWrite(BIN1, LOW);
            digitalWrite(BIN2, LOW);
            break;
        }
        case FORWARD : {
            digitalWrite(AIN1, LOW);
            digitalWrite(AIN2, HIGH);
            digitalWrite(BIN1, LOW);
            digitalWrite(BIN2, HIGH);
            delay((unsigned int)(temps_necessaire* 1500));  
            digitalWrite(AIN1, LOW);
            digitalWrite(AIN2, LOW);
            digitalWrite(BIN1, LOW);
            digitalWrite(BIN2, LOW);
            break;
        }
        case STOP : {
            softPwmWrite(PWM_A, 0);
            softPwmWrite(PWM_B, 0);
            break;
        }
        default : {
            break;
        }
    }
}
int MOTOR_destroy(void) {
    softPwmStop(PWM_A);
    softPwmStop(PWM_B);
    return 0;
}
