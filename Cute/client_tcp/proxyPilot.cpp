/**
 * \file  proxyPilot.cpp
 * \version  1.0
 * \author Thomas ROCHER
 * \date Nov 29, 2023
 * \brief TODO
 *
 * \see proxyPilot.h
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

#include "proxyPilot.h"
#include "defs.h"


#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "postman.h"
#include "../../../../../usr/include/netinet/in.h" //you have to put your own path to in.h

void PROXYPILOT_stop_robot() {
    Communication_Protocol_Head msg_to_send;
    msg_to_send.msg_type = Message_Type::STOP_ROBOT;
    msg_to_send.msg_size = htons(0x0002);
    uint8_t *data = static_cast<uint8_t*>(std::malloc(2 + ntohs(msg_to_send.msg_size)));
    std::memcpy(data, &msg_to_send, 4);
    POSTMAN_send_request(data);
    std::free(data);
}

void PROXYPILOT_send_robot_position(int coord_x, int coord_y, int direction) {
    Communication_Protocol_Head msg_to_send;
    msg_to_send.msg_type = Message_Type::SEND_ROBOT_POSITION;
    msg_to_send.msg_size = htons(0x0005);
    uint8_t buf[3] = {static_cast<uint8_t>(coord_x), static_cast<uint8_t>(coord_y), static_cast<uint8_t>(direction)};
    uint8_t *data = static_cast<uint8_t*>(std::malloc(2 + ntohs(msg_to_send.msg_size))); //ou sans le ntohs
    std::memcpy(data, &msg_to_send, 4);
    std::memcpy(data+4, buf, sizeof(buf));
    POSTMAN_send_request(data);
    std::free(data);
}

void PROXYPILOT_send_move_cartography(Command command) {
    Communication_Protocol_Head msg_to_send;
    msg_to_send.msg_type = Message_Type::SEND_MOVE_CARTOGRAPHY;
    msg_to_send.msg_size = htons(0x0002);
    uint8_t buf = static_cast<uint8_t>(command);
    uint8_t *data = static_cast<uint8_t*>(std::malloc(2 + ntohs(msg_to_send.msg_size)));
    std::memcpy(data, &msg_to_send, 4);
    std::memcpy(data+4, &buf, sizeof(buf));
    POSTMAN_send_request(data);
    std::free(data);
}

void PROXYPILOT_send_moves_trajectory(Command command[], int size) {
    for(int i = 0; i < size; i++) {
        Communication_Protocol_Head msg_to_send;
        msg_to_send.msg_type = Message_Type::SEND_MOVES_TRAJECTORY;
        msg_to_send.msg_size = htons(0x0004);
        uint8_t buf[2] = {static_cast<uint8_t>(size), static_cast<uint8_t>(command[i])};
        uint8_t *data = static_cast<uint8_t*>(std::malloc(2 + ntohs(msg_to_send.msg_size)));
        std::memcpy(data, &msg_to_send, 4);
        std::memcpy(data+4, buf, sizeof(buf));
        POSTMAN_send_request(data);
        std::free(data);
    }
}

