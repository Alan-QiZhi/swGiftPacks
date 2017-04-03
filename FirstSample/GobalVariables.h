#pragma once
#include <fstream>
#include "HalconCpp.h"
#include "CoorTransform.h"
#include "MySerial.h"
#include "Socket.h"
using namespace std;

#define NEWMETHOD
#define DEBUG
#define USESERIALPORT
#define BLUETEAM
//#define USESOCKET
#define SENDTOUNDERPAN
namespace rc17
{
	class HalconVar
	{
	public:
		static HalconCpp::HTuple hv_WindowHandle;
	};

	class CameraVar
	{
	public:
		static HalconCpp::HObject depthImage;
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
		static int AshootingIndex;
		static int BshootingIndex;
		//����ͷ��yaw��
		static const int pillarLocCol[14];
		//����ͷ��yaw��(ball)
		static const int pillarBallCol[14];
		//����ͷ�ж��������á����������row
		static const int pillarLocRow[14];
		//����ͷconfidence
		static const int pillarConfidence[7];
		//�ѽ�����Yaw
		static double correctedYaw[14];
		//�Ƿ��з���
		static bool hasFrisbee[7];
	};

	class ComVar
	{
	public:
		static MySerial serialPort;
		static Socket socketServer;
	};

	class ThreadFlag
	{
	public:
		static char t_Num;
		static bool run;
		static bool flags[10];
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