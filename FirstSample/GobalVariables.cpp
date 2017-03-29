#include "GobalVariables.h"

using namespace HalconCpp;
HTuple rc17::HalconVar::hv_WindowHandle = HTuple();

HObject rc17::CameraVar::depthImage = HObject();
rc17::CameraParam rc17::CameraVar::cameraParam = rc17::CameraParam();
rc17::Coor3D rc17::CameraVar::cameraOffset = rc17::Coor3D();
double rc17::CameraVar::receiveX = 0;
double rc17::CameraVar::receiveY = 0;
double rc17::CameraVar::receiveAngle = 0;

double const rc17::CameraVar::cameraRotate = 20;

extern const void *depthData;


rc17::Coor3D rc17::CameraVar::getWorldCoor(double _row, double _column)
{
	rc17::Coor3D result;
	myCoor3D tmp = *((myCoor3D*)depthData + (int)_row * 640 + (int)_column);
	result.x = tmp.x;
	result.y = tmp.y;
	result.z = tmp.z;
	return result;
}

rc17::Coor3D rc17::PillarVar::worldCoor = Coor3D();
rc17::Coor2D rc17::PillarVar::pixelCoor = Coor2D();
int rc17::PillarVar::index = 5;

int const rc17::PillarVar::pillarLocCol[14]
{ 
	208,399,372,361,237,352,274,
	301,-128,-128,346,-128,-128,-128//车在右侧的数据
};

int const rc17::PillarVar::pillarBallCol[14]
{
	-128,253,274,270,275,295,284
	-128,-128,-128,-128,-128,-128,-128//车在右侧的数据
};

int const rc17::PillarVar::pillarLocRow[14]
{
	358,468,406,347,405,468,431,
	362,480,480,345,480,480,480//车在右侧的数据
};

double rc17::PillarVar::correctedYaw[14]
{
	0, 0, 0, 0, 0, 0, 0
};

rc17::MySerial rc17::ComVar::serialPort;
rc17::SocketClient rc17::ComVar::mySocketClient;

char rc17::ThreadFlag::t_Num = false;
bool rc17::ThreadFlag::run = true;
bool rc17::ThreadFlag::flags[10] = { 0 };
