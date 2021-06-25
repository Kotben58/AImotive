#ifndef UDP_CLIENT_H_
#define UDP_CLIENT_H_

#define _USE_MATH_DEFINES

#pragma once


//#include "Network.h"
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
	float actualSpeed;
	float targetSpeed;
	float actualSWA;
	float targetSWA;
	uint8_t sequenceCounter;
	friend class Network;
public:
	Network Client;
	GUI Gui;
private:

	double MilliRadToDegree(int16_t mRadian);
	float CmPerSecTokmPerHour(int16_t cmPerSec);
public:

	Control();
	~Control();

	void selfDrive();
	void CarStateMsgFromServer(uint8_t msgId, uint8_t sequenceCounter, bool engaged, int16_t actualSpeed, int16_t actualSWA);

};


class Network : public Control
{
public:
	uint8_t prevSequenceNum; // this is store the previous sequence number.

private:
	sockaddr_in server;
	SOCKET out;
	int sendOk;
	int bytesIn;


	char buf[1024];

	char returnMessage[5];
	uint8_t returnMessageLen;

	int createMessage(uint8_t MsgId, uint8_t sequenceCounter, bool engage, int16_t targetSpeed, int16_t targetSWA);
	void ErrorHandler(int sendOk);

	int16_t uint8ToInt16(uint8_t firstValue, uint8_t secondValue);

	void setPrevSequenceNum(uint8_t SequenceNum);
public:

	void Connect();
	void DisConnect();

	void CarStateReq(uint8_t sequenceCounter);
	void EngageReq(uint8_t sequenceCounter, bool engage);
	void SetReference(uint8_t sequenceCounter, int16_t targetSpeed, int16_t targetSWA);

	void MessageHandler();

	void CheckIncomingMessage();
};

#endif
