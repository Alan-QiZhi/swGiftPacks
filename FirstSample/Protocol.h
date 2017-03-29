#pragma once
#include "MySerial.h"
#include "GobalVariables.h"
#ifdef SENDTOCLOUDDECK
#define sendDataBySerialPort(...) sendToCloudDeck(__VA_ARGS__)
#endif 
#ifdef SENDTOUNDERPAN
#define sendDataBySerialPort(...) sendToUnderPan(__VA_ARGS__)
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

		static void send7bytes(int type, float data, float data2 = 0);
	public :
		enum COMCMD
		{
			NoBallPara = 0,
			BallPara,
			Shoot,
			CorrectFinish
		};
		enum
		{
			SPEED_SEM = 101,
			PITCH_SEM = 102,
			YAW_SEM   = 103
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

// 将基础类型与字节数组相互转换
namespace BitConverter
{
	// 记得释放内存
	template <typename T>
	char* GetBytes(T val)
	{
		union
		{
			T _val;
			char _bytes[sizeof(T)];
		};
		_val = val;
		char* pRlt = new char[sizeof(T)];
		for (int i = 0; i < sizeof(T); i++)
		{
			pRlt[i] = _bytes[i];
		}
		return pRlt;
	}

	template <typename T>
	T FromBytes(char* bytes)
	{
		union
		{
			T _val;
			char _bytes[sizeof(T)];
		};
		for (int i = 0; i < sizeof(T); i++)
		{
			_bytes[i] = bytes[i];
		}
		return _val;
	}
}; 
