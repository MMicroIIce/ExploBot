/**
 * \file  starter.c
 * \version  1.1
 * \author Joshua MONTREUIL
 * \date Apr 30, 2023
 * \brief entry of the program.
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
#include <stdio.h>

#include <unistd.h>
#include <stdint.h>
#include <termios.h>
#include <stdlib.h>

#include "com/postman.h"
#include "com/dispatcher.h"
#include "lib/defs.h"
/* ----------------------  PRIVATE CONFIGURATIONS  -------------------------- */
/* ----------------------  PRIVATE TYPE DEFINITIONS  ------------------------ */
/* ----------------------  PRIVATE STRUCTURES  ------------------------------ */
/* ----------------------  PRIVATE ENUMERATIONS  ---------------------------- */
/**
 * \enum log_key_e
 * \brief Defines the keys that can be used.
 */
typedef enum {
    LOG_STOP = 'q',
} log_key_e;
/* ----------------------  PRIVATE FUNCTIONS PROTOTYPES  -------------------- */
/**
 * \fn static void STARTER_capture_choice(void)
 * \brief Capture the user key entry.
 * \author Joshua MONTREUIL.
 */
static void STARTER_capture_choice(void);
/**
 * \fn static void STARTER_display(void)
 * \brief Starts the display of ihm.
 * \author Joshua MONTREUIL.
 */
static void STARTER_display(void);
/**
 * \fn static void STARTER_stop_all(void)
 * \brief Stops the app.
 * \author Joshua MONTREUIL.
 */
static void STARTER_stop_all(void);
/* ----------------------  PRIVATE VARIABLES  ------------------------------- */
/**
 * \var static bool_e quit_case
 * Used to quit the app.
 */
static bool_e quit_case = FALSE;
/* ----------------------  PUBLIC FUNCTIONS  -------------------------------- */
/* ----------------------  PRIVATE FUNCTIONS  ------------------------------- */
int main (int argc, char * argv[])
{
	printf("Hello swarmbots\n\n");
    /* MODULE CREATION */
    if(POSTMAN_create() == -1) {
        printf("ERROR on postman creation.\n");
        return -1;
    }
    if(DISPATCHER_create() == -1) {
        printf("ERROR on postman creation.\n");
        goto error_dispatcher_create;
    }
    // /* MODULE START */
    if(POSTMAN_start() == -1) {
        printf("ERROR on postman start.\n");
        goto error_postman_start;
    }
    if(DISPATCHER_start() == -1) {
        printf("ERROR on postman start.\n");
        goto error_dispatcher_start;
    }
    while(!quit_case) {
        STARTER_display();
    }
    return 0;

    error_dispatcher_start :
        POSTMAN_stop();
    error_postman_start :
        DISPATCHER_destroy();
    error_dispatcher_create :
        POSTMAN_destroy();

    return -1;
}

static void STARTER_capture_choice(void) {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    log_key_e command = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    switch(command) {
        case LOG_STOP:
        {
            STARTER_stop_all();
            break;
        }
        default:
        {
            break;
        }
    }
}

static void STARTER_display(void) {
    printf("-------------- SWARMBOTS PROG -----------------\n");
    printf("--------- Press 'q' to stop the app. --------- \n");
    STARTER_capture_choice();
}

static void STARTER_stop_all(void) {
    printf("Bye swarmbots\n\n");

    /* MODULE STOP */
    if(DISPATCHER_stop() == -1) {
        printf("ERROR on dispatcher stop.\n");
    }
    if(POSTMAN_stop() == -1) {
        printf("ERROR on postman stop.\n");
    }
    /* MODULE DESTROY */
    if(DISPATCHER_destroy() == -1) {
        printf("ERROR on dispatcher destroy.\n");
    }
    if(POSTMAN_destroy() == -1) {
        printf("ERROR on postman destroy.\n");
    }
    printf("END OK \n");
    quit_case = TRUE;
}