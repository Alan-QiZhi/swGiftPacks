#pragma once
#include "MySerial.h"
namespace rc17
{
	class Protocol
	{
	public :
		enum COMCMD
		{
			NoBallPara = 0,
			BallPara = 1,
			Shoot,
			CorrectFinish
		};
		static void sendDataBySerialPort(long data1 = 0, long data2 = 0, long data3 = 0);
		static void sendDataBySerialPort(double data1 = 0, double data2 = 0, double data3 = 0);
		static void sendDataBySerialPort(int cmd, double data1, double data2, double data3, double data4, double data5);
		static void sendDataForBall();
		static void sendCmd(int cmd);
		static void sendDataBySocket(long data1 = 0, long data2 = 0, long data3 = 0);
		static void sendDataBySocket(double data1 = 0, double data2 = 0, double data3 = 0);
	};
}