#include "GobalVariables.h"

using namespace HalconCpp;
HTuple rc17::HalconVariables::hv_WindowHandle = HTuple();

HObject rc17::CameraVariables::depthImage = HObject();
rc17::CameraParam rc17::CameraVariables::cameraParam = rc17::CameraParam();
rc17::Coor3D rc17::CameraVariables::cameraOffset = rc17::Coor3D();
double rc17::CameraVariables::receiveX = 0;
double rc17::CameraVariables::receiveY = 0;
double rc17::CameraVariables::receiveAngle = 0;

double const rc17::CameraVariables::cameraRotate = 0;

extern const void *depthData;


rc17::Coor3D rc17::CameraVariables::getWorldCoor(double _row, double _column)
{
	rc17::Coor3D result;
	myCoor3D tmp = *((myCoor3D*)depthData + (int)_row * 640 + (int)_column);
	result.x = tmp.x;
	result.y = tmp.y;
	result.z = tmp.z;
	return result;
}

rc17::Coor3D rc17::PillarVariables::worldCoor = Coor3D();
rc17::Coor2D rc17::PillarVariables::pixelCoor = Coor2D();
int rc17::PillarVariables::index = 0;

int const rc17::PillarVariables::pillarLocCol[7]
{
	-128,271,295,285,235,297,274
};

int const rc17::PillarVariables::pillarBallCol[7]
{
	-128,243,264,260,265,285,274
};

int const rc17::PillarVariables::pillarLocRow[7]
{
	367,463,412,341,415,469,425
};

double rc17::PillarVariables::correctedYaw[7]
{
	0, 0, 0, 0, 0, 0, 0
};

rc17::MySerial rc17::CommunicationVariables::serialPort;
rc17::SocketClient rc17::CommunicationVariables::mySocketClient;

bool rc17::ThreadFlag::t_Flag = false;
bool rc17::ThreadFlag::run = true;