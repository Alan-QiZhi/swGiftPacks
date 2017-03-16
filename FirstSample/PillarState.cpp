#include "GobalVariables.h"
#include "PillarState.h"
#include "MiscellaneousFunctions.h"

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

rc17::Coor3D rc17::PillarState::getPillarCoor(const CameraParam myCamParam, const PillarIndex pillarToFind)
{
	Coor2D pillarPixel = getPillarPixel(myCamParam, pillarToFind);
# ifdef USESOCKET
	//char tmp[3] = { 0xaa, 0xbb, pillarPixelColumn - pillarColumn };
	//send(sockClient, tmp, 3, 0);
# endif
	if(pillarPixel.row == -1)
		return Coor3D::empty();
	Coor3D pillarCoor = CameraVariables::getWorldCoor(pillarPixel.row, pillarPixel.column);
	//中值滤波
	pillarCoor = middleFilter(pillarCoor);
	return pillarCoor;
}

rc17::Coor2D rc17::PillarState::getPillarPixel(const CameraParam myCamParam, const PillarIndex pillarToFind)
{
	Coor3D calculateCoor = CoorTransform::worldToCamera(myCamParam, pillarWorldCoor[pillarToFind]);

	double pillarRow, pillarColumn;
	bool notBeyond = CoorTransform::cameraToPixel(calculateCoor, pillarRow, pillarColumn);

#ifdef DEBUG
	SetColor(HalconVariables::hv_WindowHandle, "red");
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
		ReduceDomain(CameraVariables::depthImage, pillarRect, &pillarImage);
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
		findRegion(CameraVariables::depthImage, &plRegion, row[0], column[0], 700);

#ifdef DEBUG
		SetColor(HalconVariables::hv_WindowHandle, "green");
		if (HDevWindowStack::IsOpen())
			DispObj(plRegion, HDevWindowStack::GetActive());
#endif

		HTuple hv_Value;
		RegionFeatures(plRegion, ((HTuple("row").Append("column")).Append("row1")),
			&hv_Value);

		HObject ho_Rectangle;
		GenRectangle1(&ho_Rectangle, HTuple(hv_Value[0]), HTuple(hv_Value[1]) - 50, 480,
			HTuple(hv_Value[1]) + 50);
		HObject ho_ImageReduced;
		ReduceDomain(CameraVariables::depthImage, ho_Rectangle, &ho_ImageReduced);
		HObject ho_PillarPart;
		Threshold(ho_ImageReduced, &ho_PillarPart, (HTuple)(calculateCoor.z - 700), (HTuple)(calculateCoor.z + 700));
		HTuple hv_PillarCol;
		RegionFeatures(ho_PillarPart, "column", &hv_PillarCol);

		Coor2D pillarPixel;
		pillarPixel.column = static_cast<int>(hv_PillarCol.D());
		pillarPixel.row = static_cast<int>(hv_Value[2].D() + 10) < 480 ? static_cast<int>(hv_Value[2].D() + 10) : 480;

#ifdef DEBUG
		SetColor(HalconVariables::hv_WindowHandle, "red");
		HObject RectDSB;
		GenRectangle2(&RectDSB, pillarPixel.row, pillarPixel.column, 0, 4, 4);
		if (HDevWindowStack::IsOpen())
			DispObj(RectDSB, HDevWindowStack::GetActive());
#endif

		return pillarPixel;
	}
	return Coor2D::empty();
}

double rc17::PillarState::offsetOfUnderpansYaw(const PillarIndex pillarToFind, const int column)
{
	const int pixelThreshold = 3;//经调试发现阈值设为1可以 设为0则无法修过来
	const int pixelThreshold2 = 128;
	int pixelOffset = column - PillarVariables::pillarPixels[PillarIndex(pillarToFind)];
	if (abs(pixelOffset) > pixelThreshold && abs(pixelOffset) < pixelThreshold2)
	{
		double kP = 1;//pid 单纯的给角度很慢 所以又要修pid的参数了
		double kI = 0;
		double kD = 0;//jvjvjiayou  522 12.71 117 -20.05

		static int pixelOffsetSum = 0;
		static int lastPixelOffset = pixelOffset;
		pixelOffsetSum += pixelOffset;
		double yawToFix = kP * (pixelOffset / 640. * 57.) + kI * pixelOffsetSum + kD * (lastPixelOffset - pixelOffset);

		lastPixelOffset = pixelOffset;
		return yawToFix;
	}
	return 0.0;
}