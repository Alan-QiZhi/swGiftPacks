#include "GobalVariables.h"
#include "HalconCpp.h"
#include "Execute.h"
#include "PillarState.h"
#include "RegionDetector.h"
#include "Protocol.h"
#include <iomanip>
using namespace HalconCpp;
bool rc17::Execute::init()
{
	try
	{
		std::ofstream datafile;
		datafile.open("C:\\Users\\robocon2017\\Desktop\\datafile.txt");
		datafile.clear();
		datafile.close();

		initPos();
		initHWindow();
#ifdef USESERIALPORT
		initSerialPort();
#endif

#ifdef USESOCKET
		initSocket();
#endif
		return true;
	}
	catch(...)
	{
		return false;
	}
}

void rc17::Execute::run(HObject _depthImage)
{
	CameraVar::depthImage = _depthImage;
	//writeImage(700);
	if (HDevWindowStack::IsOpen())
		DispObj(CameraVar::depthImage, HDevWindowStack::GetActive());

	//设置摄像头位置参数
	setCameraParam();
	/****************************************摄像头修正***************************************/
	PillarVar::pixelCoor = PillarState::getPillarPixel();
	if(ThreadFlag::flags[3])
		cout << "pillar coor:  " << PillarVar::pixelCoor.row << "   " << PillarVar::pixelCoor.column 
		<< "   expect coor:  " << PillarVar::pillarLocRow[CameraVar::cameraParam.worldX < 7000 ? PillarVar::index : PillarVar::index + 7] << "   " << PillarVar::pillarLocCol[CameraVar::cameraParam.worldX < 7000 ? PillarVar::index : PillarVar::index + 7] << endl;

	//获得柱子的世界坐标
	if (PillarVar::pixelCoor.row > 0 && PillarVar::pixelCoor.column > 0) 
	{
		PillarVar::worldCoor = CameraVar::getWorldCoor(PillarVar::pixelCoor.row, PillarVar::pixelCoor.column);
		//cout << "worldCoorZ: " << PillarVar::worldCoor.z << endl;
	}



	//追踪飞盘(没有球的时候)
	if (PillarState::hasBall() == false)
	{
		saucerTrack();
	}
	//ThreadFlag::t_Num = true;
}

void rc17::Execute::initPos()
{
#ifdef REDTEAM
	CameraVar::cameraOffset.x = 218.55;
	CameraVar::cameraOffset.y = 172.55;
#endif
#ifdef BLUETEAM
	CameraVar::cameraOffset.x = -218.55;
	CameraVar::cameraOffset.y = 172.55;
#endif
}

void rc17::Execute::initHWindow()
{
	SetSystem("width", 512);
	SetSystem("height", 512);

	SetSystem("use_window_thread", "true");
	// Local iconic variables

	HObject  ho_Image, ho_GrayImage, ho_Region;

	// Local control variables
	SetWindowAttr("background_color", "black");
	OpenWindow(0, 0, 640, 480, 0, "", "", &HalconVar::hv_WindowHandle);
	HDevWindowStack::Push(HalconVar::hv_WindowHandle);
	SetPart(HalconVar::hv_WindowHandle, 0, 0, 480, 640);
}

void rc17::Execute::initSerialPort()
{
	ComVar::serialPort.auto_open();
	ComVar::serialPort.openListenThread();
	//Sleep(1000);
	//Protocol::sendCmd(Protocol::Shoot);
	//Protocol::sendDataBySerialPort(Protocol::NoBallPara, 0, 0, 0.5 + 1, 0, 0);
	//Sleep(1000);
	//Protocol::sendDataBySerialPort(Protocol::NoBallPara, 0, 0, -1, 0, 0);
	//int a = 0;
	//Protocol::sendDataBySerialPort(Protocol::switchToNoBallPara, 0, 0, 3, 0, 0);
	//Protocol::sendDataBySerialPort(Protocol::switchToNoBallPara, 0, 0, 0, 0, 0);
	//rc17::Protocol::sendDataBySerialPort(Protocol::switchToNoBallPara, -12.43, 15.75, 17, -260, 40);
}

void rc17::Execute::initSocket()
{
	ComVar::mySocketClient.init();
}

void rc17::Execute::setCameraParam()
{
#ifdef REDTEAM
	CameraVar::cameraParam.worldX = CameraVar::receiveX +
		CoorTransform::rotateVector(CameraVar::cameraOffset, -CameraVar::receiveAngle).x;
	//receiveX + 摄像头偏移车坐标点 + woc好像还要坐标换算。。。
	CameraVar::cameraParam.worldY = CameraVar::receiveY +
		CoorTransform::rotateVector(CameraVar::cameraOffset, -CameraVar::receiveAngle).y;
	//receiveY + 摄像头偏移车坐标点 +
	CameraVar::cameraParam.yaw = CameraVar::receiveAngle + CameraVar::cameraRotate;
#endif
#ifdef BLUETEAM
	CameraVar::cameraParam.worldX = -CameraVar::receiveX +
		CoorTransform::rotateVector(CameraVar::cameraOffset, -CameraVar::receiveAngle).x;
	//receiveX + 摄像头偏移车坐标点 + woc好像还要坐标换算。。。
	CameraVar::cameraParam.worldY = CameraVar::receiveY +
		CoorTransform::rotateVector(CameraVar::cameraOffset, -CameraVar::receiveAngle).y;
	//receiveY + 摄像头偏移车坐标点 +
	CameraVar::cameraParam.yaw = CameraVar::receiveAngle + CameraVar::cameraRotate;
#endif
	if (ThreadFlag::flags[1])
		cout << "underpan:   " << CameraVar::receiveX << "   " << CameraVar::receiveY << "   " << CameraVar::receiveAngle << endl;
	//CameraVar::cameraParam.worldX = 9500 + CoorTransform::rotateVector(CameraVar::cameraOffset, -(8)).x;
	//CameraVar::cameraParam.worldY = 1500 + CoorTransform::rotateVector(CameraVar::cameraOffset, -(8)).y;
	//CameraVar::cameraParam.yaw = -29;
	
	CameraVar::cameraParam.worldZ = 650;
	CameraVar::cameraParam.pitch = 18.3;
}


void rc17::Execute::saucerTrack()
{
	//检测有无飞盘飞过检测区
	RegionDetector::detectRegion();

	vector<HObject> regionsFound;
	//存储找到的region
	try
	{
		regionsFound = RegionDetector::RegionsFound(CameraVar::depthImage);
	}
	catch (...)
	{
		cout << "2" << endl;
		throw 5;
	}
	try
	{
		if (regionsFound.size() > 0)
		{
			for (size_t i = 0; i < regionsFound.size(); ++i)
			{
				HTuple hv_SaucerArea, hv_SaucerRow, hv_SaucerColumn;
				if (HDevWindowStack::IsOpen())
					DispObj(regionsFound[i], HDevWindowStack::GetActive());
				AreaCenter(regionsFound[i], &hv_SaucerArea, &hv_SaucerRow, &hv_SaucerColumn);
				Coor3D SaucerCoorTwo[2];
				SaucerCoorTwo[0] = CameraVar::getWorldCoor((int)hv_SaucerRow.D(), (int)hv_SaucerColumn.D() + 1);
				SaucerCoorTwo[1] = CameraVar::getWorldCoor((int)hv_SaucerRow.D(), (int)hv_SaucerColumn.D() - 1);
				Coor3D SaucerCoordinate = Coor3D();
				SaucerCoordinate.x = (SaucerCoorTwo[0].x + SaucerCoorTwo[1].x) / 2;
				SaucerCoordinate.y = (SaucerCoorTwo[0].y + SaucerCoorTwo[1].y) / 2;
				SaucerCoordinate.z = (SaucerCoorTwo[0].z + SaucerCoorTwo[1].z) / 2;
				if (SaucerCoordinate.z > 0)
				{
					//记录坐标
					RegionDetector::regionVector[i].recordRegionTrack(SaucerCoordinate);
					ofstream datafile;
					datafile.open("C:\\Users\\robocon2017\\Desktop\\datafile.txt", ios::app);
					//TODO
					//多加了输出时间。
					datafile << "飞盘编号: " << RegionDetector::regionVector[i].getSaucerIndex() << " X: " << setw(2) << SaucerCoordinate.x << " Y: " << setw(2) << SaucerCoordinate.y << " Z: " << SaucerCoordinate.z <<"时间："<< GetTickCount() << endl;
				}
			}
		}
	}
	catch (...)
	{
		cout << "3" << endl;
		throw 5;
	}
}
