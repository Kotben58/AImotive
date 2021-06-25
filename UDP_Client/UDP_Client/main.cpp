#include "Control.h"
#include <Windows.h>

int main(int, char**)
{
	Control ctrl;
	ctrl.Gui.initOpenGL();
	ctrl.Client.Connect();
	int i = 0;

	double startTime = GetTickCount();
	double startTimeForCarStateReq = GetTickCount();

	while (!glfwWindowShouldClose(ctrl.Gui.window))
	{
		double currentTime = GetTickCount() - startTime;
		double currentTimeForSetRef = GetTickCount() - startTimeForCarStateReq;
		if (currentTime >= 25) //25 milliseconds
		{
			ctrl.Gui.work(ctrl);
			ctrl.Client.CheckIncomingMessage(ctrl);
			ctrl.sendMsgToServer(MSG_SET_REFERENCE);
			startTime = GetTickCount();

		}

		if (currentTimeForSetRef >= 50)
		{
			ctrl.sendMsgToServer(MSG_CAR_STATE_REQ);
			startTimeForCarStateReq = GetTickCount();

		}
	}

	ctrl.Client.DisConnect();
	ctrl.Gui.destroyOpenGL();
	return 0;
}
