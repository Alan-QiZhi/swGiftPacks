#include "GobalVariables.h"
#include "PillarState.h"
#include "MiscellaneousFunctions.h"
#include "Protocol.h"

rc17::Coor3D const rc17::PillarState::pillarWorldCoor[7]
{
	rc17::Coor3D(7550, 4075, 1000),
	rc17::Coor3D(3525, 7075, 500),
	rc17::Coor3D(5525, 7075, 1000),
	rc17::Coor3D(7525, 7075, 1500),
	rc17::Coor3D(9525, 7075, 1000),
	rc17::Coor3D(11525, 7075, 500),
	rc17::Coor3D(7550, 10075, 1000),
};

rc17::Coor3D rc17::PillarState::middleFilter(Coor3D pillarCoor)
{
	static const int FILTERSIZE = 30;
	static Coor3D sFCoor[FILTERSIZE];
	Coor3D sortedCoor[FILTERSIZE];
	static int dataCount = 0;
	static bool isFull = true;
	Coor3D resultCoor;

	if (dataCount > FILTERSIZE - 1)
	{
		isFull = false;
		dataCount = 0;
	}
	sFCoor[dataCount] = pillarCoor;
	if (isFull)
	{
		for (int i = 0; i < dataCount + 1; i++)
		{
			sortedCoor[i] = sFCoor[i];
		}

		for (int j = 0; j < dataCount; j++)
		{
			for (int k = j + 1; k < dataCount + 1; k++)
			{
				if (sortedCoor[k].y < sortedCoor[j].y)
				{
					Coor3D tmp = sortedCoor[k];
					sortedCoor[k] = sortedCoor[j];
					sortedCoor[j] = tmp;
				}
			}
		}
		resultCoor = sortedCoor[dataCount / 2];
	}

	else
	{
		for (int i = 0; i < FILTERSIZE; i++)
		{
			sortedCoor[i] = sFCoor[i];
		}

		for (int j = 0; j < FILTERSIZE - 1; j++)
		{
			for (int k = j + 1; k < FILTERSIZE; k++)
				if (sortedCoor[k].y < sortedCoor[j].y)
				{
					Coor3D tmp = sortedCoor[k];
					sortedCoor[k] = sortedCoor[j];
					sortedCoor[j] = tmp;
				}
		}
		resultCoor = sortedCoor[FILTERSIZE / 2];
	}
	dataCount++;
	return resultCoor;
}

rc17::Coor3D rc17::PillarState::getPillarCoor()
{
	Coor2D pillarPixel = getPillarPixel();
# ifdef USESOCKET
	//char tmp[3] = { 0xaa, 0xbb, pillarPixelColumn - pillarColumn };
	//send(sockClient, tmp, 3, 0);
# endif
	if(pillarPixel.row == -1)
		return Coor3D::empty();
	Coor3D pillarCoor = CameraVar::getWorldCoor(pillarPixel.row, pillarPixel.column);
	//中值滤波
	pillarCoor = middleFilter(pillarCoor);
	return pillarCoor;
}

rc17::Coor2D rc17::PillarState::getPillarPixel()
{
	Coor3D calculateCoor = CoorTransform::worldToCamera(CameraVar::cameraParam, pillarWorldCoor[PillarVar::index]);

	double pillarRow, pillarColumn;
	bool notBeyond = CoorTransform::cameraToPixel(calculateCoor, pillarRow, pillarColumn);
	if (ThreadFlag::flags[2])
		cout << "caculate point:Row:" << pillarRow << "   Column:" << pillarColumn << endl;
#ifdef DEBUG
	SetColor(HalconVar::hv_WindowHandle, "red");
	HObject RectSB;
	GenRectangle2(&RectSB, pillarRow, pillarColumn, 0, 4, 4);
	if (HDevWindowStack::IsOpen())
		DispObj(RectSB, HDevWindowStack::GetActive());
#endif

	if (notBeyond)
	{
		double rowStart, columnStart, rowEnd, columnEnd;
		rowStart = pillarRow - 10;
		rowEnd = 479;
		columnStart = pillarColumn - 40;
		columnEnd = pillarColumn + 40;

		HObject pillarRect;
		GenRectangle1(&pillarRect, (HTuple)rowStart, (HTuple)columnStart, (HTuple)rowEnd, (HTuple)columnEnd);
		HObject pillarImage;
		ReduceDomain(CameraVar::depthImage, pillarRect, &pillarImage);
		HObject pillarRegion;
		//防止越界
		int low = calculateCoor.z - 800 > 0 ? calculateCoor.z - 800 : 0;
		int high = calculateCoor.z + 800 > 0 ? calculateCoor.z + 800 : 0;
		Threshold(pillarImage, &pillarRegion, (HTuple)(low), (HTuple)(high));
		//if (HDevWindowStack::IsOpen())
		//		DispObj(pillarRegion, HDevWindowStack::GetActive());
		HObject ho_ConnectedRegions;
		Connection(pillarRegion, &ho_ConnectedRegions);

		HObject hv_EmptyRegion;
		GenEmptyRegion(&hv_EmptyRegion);
		if (ho_ConnectedRegions == hv_EmptyRegion)
		{
			return Coor2D::empty();
		}

		HObject ho_SelectedRegions;
		SelectShapeStd(ho_ConnectedRegions, &ho_SelectedRegions, "max_area", 70);
		HTuple row, column;
		GetRegionPoints(ho_SelectedRegions, &row, &column);	
		HObject plRegion;
		findRegion(CameraVar::depthImage, &plRegion, row[0], column[0], 1000);

#ifdef DEBUG
		SetColor(HalconVar::hv_WindowHandle, "green");
		
#endif

		HTuple hv_Value;
		RegionFeatures(plRegion, ((HTuple("row").Append("column")).Append("row1")),
			&hv_Value);

		HObject ho_Rectangle;
		GenRectangle1(&ho_Rectangle, HTuple(hv_Value[0]), HTuple(hv_Value[1]) - 50, 480,
			HTuple(hv_Value[1]) + 50);
		HObject ho_ImageReduced;
		ReduceDomain(CameraVar::depthImage, ho_Rectangle, &ho_ImageReduced);
		HObject ho_PillarPart;
		Threshold(ho_ImageReduced, &ho_PillarPart, (HTuple)(calculateCoor.z - 800), (HTuple)(calculateCoor.z + 800));
		if (HDevWindowStack::IsOpen())
			DispObj(ho_PillarPart, HDevWindowStack::GetActive());
		HTuple hv_PillarCol;
		RegionFeatures(ho_PillarPart, "column", &hv_PillarCol);

		Coor2D pillarPixel;
		pillarPixel.column = static_cast<int>(hv_PillarCol.D());
		pillarPixel.row = static_cast<int>(hv_Value[2].D() + 10) < 480 ? static_cast<int>(hv_Value[2].D() + 10) : 480;

#ifdef DEBUG
		SetColor(HalconVar::hv_WindowHandle, "red");
		HObject RectDSB;
		GenRectangle2(&RectDSB, pillarPixel.row, pillarPixel.column, 0, 4, 4);
		if (HDevWindowStack::IsOpen())
			DispObj(RectDSB, HDevWindowStack::GetActive());
#endif

		return pillarPixel;
	}
	return Coor2D::empty();
}

bool rc17::PillarState::lockPillar(int type)
{
	const int thresL = 3;
	const int thresH = 128;
	int pixelOffset = 0;
	int cmd = 0;
	switch(type)
	{
	case WithBall:
	{
		pixelOffset = PillarVar::pillarBallCol[PillarIndex(PillarVar::index)]
			- PillarVar::pixelCoor.column;
		cmd = Protocol::BallPara;
		break;
	}
	case NoBall:
	{
		pixelOffset = PillarVar::pillarLocCol[PillarIndex(PillarVar::index)]
			- PillarVar::pixelCoor.column + PillarVar::correctedYaw[PillarVar::index]/57.0*640;
		cmd = Protocol::NoBallPara;
		break;
	}
	default:
		return false;
	}

	

	if (abs(pixelOffset) > thresL && abs(pixelOffset) < thresH)
	{
		double kP = 0.7;
		double kI = 0.0;
		double kD = 0;

		static int pixelOffsetSum = 0;
		static int lastPixel = PillarVar::pixelCoor.column;
		pixelOffsetSum += pixelOffset;
		double yawToFix = kP * (pixelOffset / 640. * 57.) + kI * pixelOffsetSum + kD * (lastPixel - PillarVar::pixelCoor.column);
		cout << yawToFix << endl;
		lastPixel = PillarVar::pixelCoor.column;
#ifdef USESERIALPORT		
		if (Protocol::correctPara[PillarVar::index].haveDataNum != 0)//捎带发送上次修正值
		{
			//if (abs(yawToFix) < 1)
			//	if (yawToFix > 0)
			//	{
			//		Protocol::sendDataBySerialPort(cmd, 0, 0, yawToFix + 1.1, 0, 0);
			//		this_thread::sleep_for(chrono::milliseconds(500));
			//		Protocol::sendDataBySerialPort(cmd, -1.1, Protocol::correctPara[PillarVar::index]);
			//	}
			//	else
			//	{
			//		Protocol::sendDataBySerialPort(cmd, 0, 0, yawToFix - 1.1, 0, 0);
			//		this_thread::sleep_for(chrono::milliseconds(500));
			//		Protocol::sendDataBySerialPort(cmd, 1.1, Protocol::correctPara[PillarVar::index]);
			//	}
			//else
				Protocol::sendDataBySerialPort(cmd, yawToFix, Protocol::correctPara[PillarVar::index]);
			Protocol::correctPara[PillarVar::index].haveDataNum = 0;
		}
		else
		{
			//if (abs(yawToFix) < 1)
			//	if (yawToFix > 0)
			//	{
			//		Protocol::sendDataBySerialPort(cmd, 0, 0, yawToFix + 1.1, 0, 0);
			//		this_thread::sleep_for(chrono::milliseconds(500));
			//		Protocol::sendDataBySerialPort(cmd, 0, 0, -1.1, 0, 0);
			//	}
			//	else
			//	{
			//		Protocol::sendDataBySerialPort(cmd, 0, 0, yawToFix - 1.1, 0, 0);
			//		this_thread::sleep_for(chrono::milliseconds(500));
			//		Protocol::sendDataBySerialPort(cmd, 0, 0, 1.1, 0, 0);
			//	}

			//else
			//{
				Protocol::sendDataBySerialPort(cmd, 0, 0, yawToFix, 0, 0);
			//}
		}
#endif
	}
	if (abs(pixelOffset) < thresL)
		return true;
	return false;
}

bool rc17::PillarState::hasBall()
{
	int thresBall = 15; // 超出这个阈值则认为有球。
	if(PillarVar::pillarLocRow[PillarIndex(PillarVar::index)] - PillarVar::pixelCoor.row > thresBall)
		return true;	
	return false;
}
