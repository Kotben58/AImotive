#include "Control.h"

bool charToBool(char value)
{
	if (0 == (int)value)
	{
		return false;
	}
	return true;
}

int Network::createMessage(uint8_t MsgId, uint8_t sequenceCounter, bool engage, int16_t targetSpeed, int16_t targetSWA)
{
	returnMessage[0] = (char)MsgId;
	returnMessage[1] = sequenceCounter;

	if (MSG_ENGAGE_REQ == MsgId)
	{
		returnMessage[2] = (char)engage;
		return 3;
	}
	else if (MSG_SET_REFERENCE == MsgId)
	{
		returnMessage[2] = (char)((targetSpeed >> 8) & 0xFF);
		returnMessage[3] = (char)(targetSpeed & 0xFF);
		returnMessage[4] = (char)((targetSWA >> 8) & 0xFF);
		returnMessage[5] = (char)(targetSWA & 0xFF);
		return 6;

	}
	return 2;
}

void Network::ErrorHandler(int sendOk)
{
	if (sendOk == SOCKET_ERROR)
	{
		cout << "That didn't work! " << WSAGetLastError() << endl;
	}
}

int16_t Network::uint8ToInt16(int8_t firstValue, int8_t secondValue)
{
	int16_t retVal = ((int16_t)firstValue << 8) | (int16_t)secondValue;
	return retVal;
}

void Network::setPrevSequenceNum(uint8_t SequenceNum)
{
	this->prevSequenceNum = SequenceNum;
}

void Network::Connect()
{
	// Startup Winsock
	WSADATA data;
	WORD version = MAKEWORD(2, 2);
	int wsOk = WSAStartup(version, &data);

	if (wsOk != 0)
	{
		cout << "Can't start Winsock! " << wsOk << endl;
		return;
	}
	// Create a hint structure for the server


	server.sin_family = AF_INET;
	server.sin_port = htons(54000);
	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

	// Socket creation
	out = socket(AF_INET, SOCK_DGRAM, 0);
	if (out < 0) {
		printf("socket creation failed");
	}
	sockaddr_in serverHint;
	serverHint.sin_family = AF_INET;
	serverHint.sin_addr.S_un.S_addr = ADDR_ANY;
	serverHint.sin_port = htons(54000); // Convert from little to big endian

	if (bind(out, (const struct sockaddr *)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
	//if (bind(out, (const struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Can't bind socket!  %d\n", WSAGetLastError());
		return;
	}
}

void Network::DisConnect()
{
	// Close socket
	closesocket(out);

	// Close down winsock
	WSACleanup();
}

void Network::CarStateReq(uint8_t sequenceCounter)											// MSG_CAR_STATE_REQ
{
	returnMessageLen = createMessage(MSG_CAR_STATE_REQ, sequenceCounter, true, 0, 0);
	sendOk = sendto(out, returnMessage, returnMessageLen, 0, (sockaddr*)&server, sizeof(server));
	ErrorHandler(sendOk);
	setPrevSequenceNum(sequenceCounter);
	expectData = true;
}
void Network::EngageReq(uint8_t sequenceCounter, bool engage)								// MSG_ENGAGE_REQ
{
	returnMessageLen = createMessage(MSG_ENGAGE_REQ, sequenceCounter, engage, 0, 0);
	sendOk = sendto(out, returnMessage, returnMessageLen, 0, (sockaddr*)&server, sizeof(server));
	ErrorHandler(sendOk);
}
void Network::SetReference(uint8_t sequenceCounter, int16_t targetSpeed, int16_t targetSWA)	// MSG_SET_REFERENCE
{
	returnMessageLen = createMessage(MSG_SET_REFERENCE, sequenceCounter, true, targetSpeed, targetSWA);

	sendOk = sendto(out, returnMessage, returnMessageLen, 0, (sockaddr*)&server, sizeof(server));
	ErrorHandler(sendOk);
}

void Network::MessageHandler(Control& ctrl)
{
	//There are only one kind of message can recevied from the server. This is a MSG_CAR_STATE message.
	if (MSG_CAR_STATE == buf[0])
	{
		if (prevSequenceNum == (uint8_t)buf[1])
		{
			ctrl.CarStateMsgFromServer((uint8_t)buf[0], (uint8_t)buf[1], charToBool(buf[2]), uint8ToInt16((int8_t)buf[3], (int8_t)buf[4]), uint8ToInt16((int8_t)buf[5], (int8_t)buf[6]));
		}
	}
}

int8_t Network::setTimeoutSocket()
{
	fd_set fds;
	int n;
	struct timeval tv;

	// Set up the file descriptor set.
	FD_ZERO(&fds);
	FD_SET(out, &fds);

	// Set up the struct timeval for the timeout.
	tv.tv_sec = 0;
	tv.tv_usec = 2000;

	// Wait until timeout or data received.
	n = select(out, &fds, NULL, NULL, &tv);
	if (n == 0)
	{
		cout << "Timeout.." <<endl;

		/* If during an engaged selfDrive, the client fails to send the setReference message periodically (using a maximum
		* allowed period timeout), the selfDrive shall be terminated
		*/
		/*ctrl.engage = false;
		ctrl.sendMsgToServer(MSG_ENGAGE_REQ);
		fprintf(stderr, "Error: Timeout error %d\n", WSAGetLastError());*/
		return 0;
	}
	else if (n == -1)
	{
		cout << "Error.." << endl;
		return 1;
	}
	return 1;
}

void Network::CheckIncomingMessage(Control& ctrl)
{

	int8_t validTimeout;
	if (true == expectData)
	{
		int serverLength;
		  // If more than one message is received from the server in a loop time.
		do{
			validTimeout = setTimeoutSocket();
			if (0 < validTimeout)
			{			
				serverLength = sizeof(server);
				ZeroMemory(&server, serverLength);

				ZeroMemory(buf, 1024);
				bytesIn = recvfrom(out, buf, 1024, 0, (struct sockaddr*)&server, &serverLength);

				if (bytesIn == SOCKET_ERROR)
				{
					cout << "Error receiving data. Error code: " << WSAGetLastError() << endl;
				}

				if (0 < bytesIn) // If recvfrom() did not receive a message the return value is zero. If bigger than zero the return value is the message byte number. 
				{
					MessageHandler(ctrl);
				}
			}
			else
			{
				bytesIn = 0;
			}
		} while (bytesIn > 0);
	}
	expectData = false;
}