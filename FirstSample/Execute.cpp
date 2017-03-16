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
	CameraVariables::depthImage = _depthImage;
	//writeImage(700);
	if (HDevWindowStack::IsOpen())
		DispObj(CameraVariables::depthImage, HDevWindowStack::GetActive());

	//��������ͷλ�ò���
	setCameraParam();
	/****************************************����ͷ����***************************************/
	PillarVariables::pixelCoor = PillarState::getPillarPixel(CameraVariables::cameraParam, PillarIndex(PillarVariables::index));
	cout << PillarVariables::pixelCoor.row << "   " << PillarVariables::pixelCoor.column << endl;
	static int count = 20;
	if (count < 10)
		count++;
	else
	{
#ifdef USESERIALPORT
		Protocol::sendDataBySerialPort(0, 0, -PillarState::offsetOfUnderpansYaw(PillarIndex(PillarVariables::index), pillarPixel.column), 0, 0, CommunicationVariables::serialPort);
#endif
		count = 0;
	}
	/****************************************************************************************/

	//������ӵ���������
	if (PillarVariables::pixelCoor.row>0 && PillarVariables::pixelCoor.column > 0)
		PillarVariables::worldCoor = CameraVariables::getWorldCoor(PillarVariables::pixelCoor.row, PillarVariables::pixelCoor.column);

	//׷�ٷ���
	saucerTrack();
}

void rc17::Execute::initPos()
{
#ifdef REDTEAM
	CameraVariables::cameraOffset.x = 359.10;
	CameraVariables::cameraOffset.y = 315.73;
#endif
#ifdef BLUETEAM
	CameraVariables::cameraOffset.x = -359.10;
	CameraVariables::cameraOffset.y = 315.73;
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
	OpenWindow(0, 0, 640, 480, 0, "", "", &HalconVariables::hv_WindowHandle);
	HDevWindowStack::Push(HalconVariables::hv_WindowHandle);
	SetPart(HalconVariables::hv_WindowHandle, 0, 0, 480, 640);
}

void rc17::Execute::initSerialPort()
{
	CommunicationVariables::serialPort.auto_open();
	CommunicationVariables::serialPort.openListenThread();

	//Protocol::sendDataBySerialPort(0, 0, 3, 0, 0, rc17::CommunicationVariables::serialPort);
	//Protocol::sendDataBySerialPort(0, 0, 0, 0, 0, rc17::CommunicationVariables::serialPort);
	//rc17::Protocol::sendDataBySerialPort(-12.43, 15.75, 17, -260, 40, rc17::CommunicationVariables::serialPort);
}

void rc17::Execute::initSocket()
{
	CommunicationVariables::mySocketClient.init();
}

void rc17::Execute::setCameraParam()
{
#ifdef REDTEAM
	CameraVariables::cameraParam.worldX = CameraVariables::receiveX +
		CoorTransform::rotateVector(CameraVariables::cameraOffset, -CameraVariables::receiveAngle).x;
	//receiveX + ����ͷƫ�Ƴ������ + woc����Ҫ���껻�㡣����
	CameraVariables::cameraParam.worldY = CameraVariables::receiveY +
		CoorTransform::rotateVector(CameraVariables::cameraOffset, -CameraVariables::receiveAngle).y;
	//receiveY + ����ͷƫ�Ƴ������ +
	CameraVariables::cameraParam.yaw = CameraVariables::receiveAngle + CAMERAROTATE;
#endif
#ifdef BLUETEAM
	CameraVariables::cameraParam.worldX = -CameraVariables::receiveX +
		CoorTransform::rotateVector(CameraVariables::cameraOffset, -CameraVariables::receiveAngle).x;
	//receiveX + ����ͷƫ�Ƴ������ + woc����Ҫ���껻�㡣����
	CameraVariables::cameraParam.worldY = CameraVariables::receiveY +
		CoorTransform::rotateVector(CameraVariables::cameraOffset, -CameraVariables::receiveAngle).y;
	//receiveY + ����ͷƫ�Ƴ������ +
	CameraVariables::cameraParam.yaw = CameraVariables::receiveAngle + CAMERAROTATE;
#endif

	//CameraVariables::cameraParam.worldX = 7400 + CoorTransform::rotateVector(CameraVariables::cameraOffset, -(8)).x;
	//CameraVariables::cameraParam.worldY = 1500 + CoorTransform::rotateVector(CameraVariables::cameraOffset, -(8)).y;
	//CameraVariables::cameraParam.yaw = 24 + CAMERAROTATE;

	CameraVariables::cameraParam.worldZ = 728;
	CameraVariables::cameraParam.pitch = 16.3;
}

void rc17::Execute::getPillarCoor()
{
	PillarVariables::worldCoor = PillarState::getPillarCoor(CameraVariables::cameraParam, PillarIndex(PillarVariables::index));
}

void rc17::Execute::saucerTrack()
{
	//������޷��̷ɹ������
	RegionDetector::detectRegion();

	vector<HObject> regionsFound;
	//�洢�ҵ���region
	try
	{
		regionsFound = RegionDetector::RegionsFound(CameraVariables::depthImage);
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
				SaucerCoorTwo[0] = CameraVariables::getWorldCoor((int)hv_SaucerRow.D(), (int)hv_SaucerColumn.D() + 1);
				SaucerCoorTwo[1] = CameraVariables::getWorldCoor((int)hv_SaucerRow.D(), (int)hv_SaucerColumn.D() - 1);
				Coor3D SaucerCoordinate = Coor3D();
				SaucerCoordinate.x = (SaucerCoorTwo[0].x + SaucerCoorTwo[1].x) / 2;
				SaucerCoordinate.y = (SaucerCoorTwo[0].y + SaucerCoorTwo[1].y) / 2;
				SaucerCoordinate.z = (SaucerCoorTwo[0].z + SaucerCoorTwo[1].z) / 2;
				if (SaucerCoordinate.z > 0)
				{
					//��¼����
					RegionDetector::regionVector[i].recordRegionTrack(SaucerCoordinate);
					ofstream datafile;
					datafile.open("C:\\Users\\robocon2017\\Desktop\\datafile.txt", ios::app);
					//TODO
					//��������ʱ�䡣
					datafile << "���̱��: " << RegionDetector::regionVector[i].getSaucerIndex() << " X: " << setw(2) << SaucerCoordinate.x << " Y: " << setw(2) << SaucerCoordinate.y << " Z: " << SaucerCoordinate.z <<"ʱ�䣺"<< GetTickCount() << endl;
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
