#ifndef NETWORK_H_
#define NETWORK_H_

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

class Control;

class Network
{
public:
	uint8_t prevSequenceNum; // this is store the previous sequence number.
private:
	bool expectData;
	sockaddr_in server;
	SOCKET out;
	int sendOk;
	int bytesIn;

	char buf[1024]; // buffer for the received messages

	char returnMessage[5];
	uint8_t returnMessageLen;

	int createMessage(uint8_t MsgId, uint8_t sequenceCounter, bool engage, int16_t targetSpeed, int16_t targetSWA);
	void ErrorHandler(int sendOk);

	int16_t uint8ToInt16(int8_t firstValue, int8_t secondValue);

	void setPrevSequenceNum(uint8_t SequenceNum);
public:
	Network() :prevSequenceNum(0), expectData(false){};
	void Connect();
	void DisConnect();

	void CarStateReq(uint8_t sequenceCounter);
	void EngageReq(uint8_t sequenceCounter, bool engage);
	void SetReference(uint8_t sequenceCounter, int16_t targetSpeed, int16_t targetSWA);

	void MessageHandler(Control& ctrl);
	int8_t Network::setTimeoutSocket();
	void CheckIncomingMessage(Control& ctrl);
};

#endif