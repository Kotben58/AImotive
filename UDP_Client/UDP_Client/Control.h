#ifndef CONTROL_H_
#define CONTROL_H_

#define _USE_MATH_DEFINES

#pragma once


#include "Network.h"
#include "GUI.h"

#include <math.h>
#include <stdint.h>
#include <iostream>
#include <ws2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

using namespace std; // _Saves us from typing std::cout << etc. etc.

#define MSG_CAR_STATE_REQ (0x01)
#define MSG_CAR_STATE     (0x02)
#define MSG_ENGAGE_REQ    (0x03)
#define MSG_SET_REFERENCE (0x04)



class Control
{
	bool engage;
	float actualSpeed;			//in km/h
	float targetSpeed;			//in km/h
	float actualSWA;			//in deg
	float targetSWA;			//in deg
	uint8_t sequenceCounter;
	uint8_t newSequenceNumber;
	friend class Network;
	friend class GUI;
public:
	Network Client;
	GUI Gui;

private:
	float MilliRadToDegree(int16_t mRadian);
	int16_t DegreeToMilliRad(float degree);
	float CmPerSecTokmPerHour(int16_t cmPerSec);
	uint8_t getNewSequenceNumber();
public:
	Control() : newSequenceNumber(0) {}

	void TargetDatasFromGUI(float targetSpeed, float targetSWA);
	void CarStateMsgFromServer(uint8_t msgId, uint8_t sequenceCounter, bool engaged, int16_t actualSpeed, int16_t actualSWA);
	void sendMsgToServer(uint8_t msgId);
	void engageSelfDrive(bool engage);

};

#endif