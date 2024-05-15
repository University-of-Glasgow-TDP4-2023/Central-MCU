// cable_transceiver.h
#ifndef CABLE_TRANSCEIVER_H
#define CABLE_TRANSCEIVER_H

#include <math.h>
#include "./global.h"
// #include <SPI.h>
// #include "printf.h"
#include "hardware/spi.h"
#include "hardware/pwm.h"
#include "./RF24/RF24.h"
#include "./motorDriver.h"
#include "./encoder.h"

// RF24 radio object
extern RF24 radio;
extern uint8_t address[2][6];
extern bool radioNumber;
extern bool role;
extern int payload;
extern int cableLength;

// Function declarations
void setupRadio();
// void displayMotorData(int data);
void createPayload(int command, int data);
void sendCableLength(int length);
void sendMotorPacket(int speed, int direction, int distance, int error);
void setSpeed(int data);
void executePayload(int payload);
void RX_TX();
void stabilisationError();
void stabilisationOff();

#endif // CABLE_TRANSCEIVER_H
