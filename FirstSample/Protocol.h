#pragma once
#include "MySerial.h"
#include "GobalVariables.h"
#define SPEED_SEM  101
#define PITCH_SEM  102
#define YAW_SEM 103
#ifdef SENDTOCLOUDDECK
#define sendDataBySerialPort sendToCloudDeck
#endif 
#ifdef SENDTOUNDERPAN
#define sendDataBySerialPort sendToUnderPan
#endif
namespace rc17
{
	class Protocol
	{	
	private:
		class DelayCorrectVariables
		{
		public:
			DelayCorrectVariables() = default;
			double pitch;
			double roll;
			double bigWheel;
			double smallWheel;
			int haveDataNum = 0;
			void assign(float* correctPara);
		};
	public :
		enum COMCMD
		{
			NoBallPara = 0,
			BallPara,
			Shoot,
			CorrectFinish
		};
		static DelayCorrectVariables correctPara[7];
		static void sendToUnderPan(int cmd, double data1, double data2, double data3, double data4, double data5);
		static void sendToUnderPan(int cmd, double yaw, DelayCorrectVariables correctPara);
		static void sendToCloudDeck(double data1, double data2, int16_t data3, int16_t data4);
		static void sendDataForBall();
		static void sendCmd(int cmd);
		static void sendDataBySocket(long data1 = 0, long data2 = 0, long data3 = 0);
		static void sendDataBySocket(double data1 = 0, double data2 = 0, double data3 = 0);
	};
}