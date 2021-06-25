#include "Control.h"


float Control::MilliRadToDegree(int16_t mRadian)
{
	return (float)((mRadian) / 1000 * (180.0 / M_PI));
}
int16_t Control::DegreeToMilliRad(float degree)
{
	return (int16_t)(degree * 1000.0 * (M_PI / 180.0));
}

float Control::CmPerSecTokmPerHour(int16_t cmPerSec)
{
	return (float)(cmPerSec * 0.036);
}

int16_t kmPerHourToCmPerSec(float kmPerh)
{
	return (int16_t)(kmPerh * 1000 / 36);
}

void Control::TargetDatasFromGUI(float targetSpeed, float targetSWA)
{
	this->targetSpeed = Gui.readGuiSpeed;
	this->targetSWA = Gui.readGuiSWA;
}

void Control::CarStateMsgFromServer(uint8_t msgId, uint8_t sequenceCounter, bool engaged, int16_t actualSpeed, int16_t actualSWA)
{
	if (Client.prevSequenceNum == sequenceCounter)
	{
		this->actualSpeed = CmPerSecTokmPerHour(actualSpeed); //The received actualSpeed is in cm/s.
		this->actualSWA = MilliRadToDegree(actualSWA); //The received actualSWA is in mrad.
		if (engaged != this->engage)
		{
			printf("The server do not get the selfDrive message!\n");
		}
		cout << "actualSWA: " << actualSWA << endl;
		Gui.setData(actualSpeed, actualSWA, (int16_t) targetSpeed, (int16_t) targetSWA);
	}
}

uint8_t Control::getNewSequenceNumber()
{
	if (255 <= newSequenceNumber)
	{
		newSequenceNumber = 1;		
	}
	else
	{
		newSequenceNumber++;
	}
	return newSequenceNumber;
}

void Control::sendMsgToServer(uint8_t msgId)
{
	cout << "sendMsgToServer " << (int)msgId << endl;
	switch (msgId) 
	{
		case MSG_CAR_STATE_REQ:
			Client.CarStateReq(getNewSequenceNumber());
			break;
		case MSG_ENGAGE_REQ:
			Client.EngageReq(getNewSequenceNumber(), engage);
			break;
		case MSG_SET_REFERENCE:
			Client.SetReference(getNewSequenceNumber(), kmPerHourToCmPerSec(targetSpeed), DegreeToMilliRad(targetSWA));
			break;
	}
}

void Control::engageSelfDrive(bool engage)
{
	if (this->engage != engage)
	{
		cout << "engageSelfDrive" << endl;
		this->engage = engage;
		sendMsgToServer(MSG_ENGAGE_REQ);
	}
}

