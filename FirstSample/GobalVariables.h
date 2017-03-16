#pragma once
#include <fstream>

#include "SocketClient.h"
#include "HalconCpp.h"
#include "CoorTransform.h"
#include "MySerial.h"
using namespace std;

#define CAMERAROTATE 0
#define NEWMETHOD
#define DEBUG
//#define USESERIALPORT
#define BLUETEAM
//#define USESOCKET
namespace rc17
{
	class HalconVariables
	{
	public:
		static HalconCpp::HTuple hv_WindowHandle;
	};

	class CameraVariables
	{
	public:
		static HalconCpp::HObject depthImage;
		static CameraParam cameraParam;
		static Coor3D cameraOffset;

		static double receiveX;
		static double receiveY;
		static double receiveAngle;

		static Coor3D getWorldCoor(double _row, double _column);
	};

	class PillarVariables
	{
	public:
		static Coor3D coor;
		static int index;
		static const double pillarPixels[7];
	};

	class CommunicationVariables
	{
	public:
		static MySerial serialPort;
		static SocketClient mySocketClient;
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