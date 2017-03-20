#pragma once
#include <fstream>

#include "SocketClient.h"
#include "HalconCpp.h"
#include "CoorTransform.h"
#include "MySerial.h"
using namespace std;

#define NEWMETHOD
#define DEBUG
#define USESERIALPORT
#define BLUETEAM
//#define USESOCKET
namespace rc17
{
	class HalconVar
	{
	public:
		static HTuple hv_WindowHandle;
	};

	class CameraVar
	{
	public:
		static HObject depthImage;
		static CameraParam cameraParam;
		static Coor3D cameraOffset;

		static const double cameraRotate;
		static double receiveX;
		static double receiveY;
		static double receiveAngle;

		static Coor3D getWorldCoor(double _row, double _column);
	};

	class PillarVar
	{
	public:
		static Coor3D worldCoor;
		static Coor2D pixelCoor;
		static int index;
		//摄像头修yaw用
		static const int pillarLocCol[7];
		//摄像头修yaw用(ball)
		static const int pillarBallCol[7];
		//摄像头判断有无球用。这是无球的row
		static const int pillarLocRow[7];
		//摄像头confidence
		static const int pillarConfidence[7];
		//已矫正的Yaw
		static double correctedYaw[7];
	};

	class ComVar
	{
	public:
		static MySerial serialPort;
		static SocketClient mySocketClient;
	};

	class ThreadFlag
	{
	public:
		static bool t_Flag;
		static bool run;
	};
}

struct myCoor3D
{
	float x;
	float y;
	float z;
	bool IsValid() const { return z == z; }  // check for NAN

	myCoor3D(float _x = 0, float _y = 0, float _z = 0)
	{
		x = _x;
		y = _y;
		z = _z;
	}
};