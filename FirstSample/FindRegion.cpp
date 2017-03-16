#include <fstream>

#include "GobalVariables.h"
#include "FindRegion.h"
#include "MiscellaneousFunctions.h"


int rc17::FindRegion::saucerCount = 0;

int rc17::FindRegion::unnecessaryToFindNext() const
{
	if (findAgainTimes <= 0)
		return 1;
	if (Row > 495 || Row < -15 || Column > 660 || Column < -20 /*|| Z < 1000*/)//Row > 32755 || Row < -32755 || Column > 32748 || Column < -32748
		return 2;
	if (Z - THRESHOLD >= 65535 || Z <= 0)
		return 3;

	return 0;
}

HObject rc17::FindRegion::findRegionFromGivenArea(HObject &Image)const
{
	HObject Rectangle, ImageReduced, Region, BackgroundRegions, SelectShapeRegion;
	GenRectangle2(&Rectangle, Row, Column, 0, 190000.0 / Z, 120000.0 / Z);
	if (lastArea.D() < 350)
		GenRectangle2(&Rectangle, Row, Column, 0, 330 / 9, 330 / 11);
	if (lastArea.D() < 50)
		GenRectangle2(&Rectangle, Row, Column, 0, 240 / 9, 240 / 13);
	ReduceDomain(Image, Rectangle, &ImageReduced);

	Threshold(ImageReduced, &Region, Z - THRESHOLD, Z + THRESHOLD);
	Connection(Region, &BackgroundRegions);
	SelectShape(BackgroundRegions, &SelectShapeRegion, "area", "and", 33.180000 / (Z / 1000 * (Z / 1000)), 18177.840000 / (Z / 1000 * (Z / 1000)));
	//SelectShape(SelectShapeRegion, &SelectShapeRegion, "row", "and", 0, 450);
	SelectShape(SelectShapeRegion, &SelectShapeRegion, "row", "and", Row - 20, Row + 20);
	Union1(SelectShapeRegion, &Region);
	return Region;
}

HTuple rc17::FindRegion::fillUpRegionHasFound(HObject &Image, HObject &Region)
{
	HTuple Number;
	CountObj(Region, &Number);
	if (!(Number == 0))
	{
		//if (HDevWindowStack::IsOpen())
		//	DispObj(Region, HDevWindowStack::GetActive());
		HTuple A, R, C;
		AreaCenter(Region, &A, &R, &C);

		HObject RectSB;
		GenRectangle2(&RectSB, R, C, 0, 4, 4);
		SetColor(HalconVariables::hv_WindowHandle, "blue");
		if (HDevWindowStack::IsOpen())
			DispObj(RectSB, HDevWindowStack::GetActive());
		SetColor(HalconVariables::hv_WindowHandle, "red");

		//findRegion(Image, &Region, R, C, 500);
		//AreaCenter(Region, &A, &R, &C);

		if (A.D() > 50000)
		{
			Number = 0;
		}
		//if(findTimesCount > 3)
		//	if (A.D() < lastArea.D() * 0.4 || A.D() > lastArea.D() * 1.8)
		//		Number = 0;
		//	else;
		//else
		//if (R < Row - 25 || R > Row + 25 || C < Column - 25 || C > Column + 25)
		//Number = 0;
	}
	return Number;
}

HObject rc17::FindRegion::findNext(HObject &Image)
{
	++findTimesCount;
	HObject Region, EmptyRegion;
	Row = lastRow + vy * (MAXFINDAGAINTIMES + 1 - findAgainTimes),
		Column = lastColumn + vx * (MAXFINDAGAINTIMES + 1 - findAgainTimes),
		Z = lastGrayval.D() + vz * (MAXFINDAGAINTIMES + 1 - findAgainTimes);
	GenEmptyRegion(&EmptyRegion);

	int res = unnecessaryToFindNext();

	if (res)
	{
		isLost = true;
		return EmptyRegion;
	}

	Region = findRegionFromGivenArea(Image);//是否有多个区域？




	if (fillUpRegionHasFound(Image, Region) == 0)
	{
		--findAgainTimes;
		return EmptyRegion;
	}
	else//更新数据
	{

		Result = Region;
		HTuple Area, Row, Column, Grayval, Deviation;
		AreaCenter(Result, &Area, &Row, &Column);
		HalconCpp::Intensity(Result, Image, &Grayval, &Deviation);

		vx = (Column - lastColumn) / (MAXFINDAGAINTIMES + 1 - findAgainTimes);
		vy = (Row - lastRow) / (MAXFINDAGAINTIMES + 1 - findAgainTimes);
		vz = (Grayval - lastGrayval) / (MAXFINDAGAINTIMES + 1 - findAgainTimes);
		lastArea = Area * 0.65 + lastArea * lastGrayval * lastGrayval / Grayval / Grayval*0.35;
		lastColumn = Column;
		lastRow = Row;
		lastGrayval = Grayval;
		findAgainTimes = MAXFINDAGAINTIMES;
		return Result;

	}

}

HObject rc17::FindRegion::findNext(HObject &Image, CameraParam _cameraParam)
{
	++findTimesCount;
	HObject Region, EmptyRegion;
	GenEmptyRegion(&EmptyRegion);

	Coor3D whereThisFunctionWillFind;
	whereThisFunctionWillFind.x = lastABSWorldCoor.x + vx * (MAXFINDAGAINTIMES + 1 - findAgainTimes);
	whereThisFunctionWillFind.y = lastABSWorldCoor.y + vy * (MAXFINDAGAINTIMES + 1 - findAgainTimes);
	whereThisFunctionWillFind.z = lastABSWorldCoor.z + vz * (MAXFINDAGAINTIMES + 1 - findAgainTimes);
	Coor3D cameraCoor;
	cameraCoor = CoorTransform::worldToCamera(_cameraParam, whereThisFunctionWillFind);
	Z = cameraCoor.z;

	int res = unnecessaryToFindNext();
	if (res)
	{
		isLost = true;
		return EmptyRegion;
	}

	if (!CoorTransform::cameraToPixel(cameraCoor, Row, Column))
	{	//移动太多移出屏幕了
		--findAgainTimes;
		return EmptyRegion;
	}

	try
	{
		Region = findRegionFromGivenArea(Image);
	}
	catch (...)
	{
		cout << "b" << endl;
		throw 3;
	}
	try
	{
		if (fillUpRegionHasFound(Image, Region) == 0)
		{
			--findAgainTimes;
			return EmptyRegion;
		}

		else//更新数据
		{
			Result = Region;
			HTuple Area, Row, Column, Grayval, Deviation;
			AreaCenter(Result, &Area, &Row, &Column);
			HalconCpp::Intensity(Result, Image, &Grayval, &Deviation);
			Coor3D curABSWorldCoor = CoorTransform::cameraToWorld(_cameraParam, CoorTransform::pixelToCamera(Row.D(), Column.D(), Grayval.D()));
			vx = 0.3 * vx + 0.7 * (curABSWorldCoor.x - lastABSWorldCoor.x) / (MAXFINDAGAINTIMES + 1 - findAgainTimes);
			vy = 0.3 * vy + 0.7 * (curABSWorldCoor.y - lastABSWorldCoor.y) / (MAXFINDAGAINTIMES + 1 - findAgainTimes);
			vz = 0.3 * vz + 0.7 * (curABSWorldCoor.z - lastABSWorldCoor.z) / (MAXFINDAGAINTIMES + 1 - findAgainTimes);
			lastArea = Area * 0.6 + lastArea * lastABSWorldCoor.z * lastABSWorldCoor.z / curABSWorldCoor.z / curABSWorldCoor.z * 0.4;
			lastABSWorldCoor = curABSWorldCoor;
			//if (findTimesCount > 5)
			//cout << "X: " << cameraCoor.x << "  Y: " << cameraCoor.y << "  Z: " << cameraCoor.z << endl;
			findAgainTimes = MAXFINDAGAINTIMES;
			return Result;

		}
	}
	catch (...)
	{
		cout << "a" << endl;
		throw 3;
	}
}

int rc17::FindRegion::getOffset(double offset[2])
{
	//平台高度平面
	static const int HEIGHTOFFSET = 100;
	//滤去数据
	static const int HEIGHTFILTRE = 80;
	static const int ZFILTER = 1000;
	//最后一次在平台上的飞盘的坐标的编号
	if (!regionTrack.size())
		return 3;
	size_t lastCoorIndex = regionTrack.size() - 1;

	for (; lastCoorIndex > 0; lastCoorIndex--)
	{
		if (regionTrack[lastCoorIndex].y < PillarVariables::coor.y - HEIGHTFILTRE && regionTrack[lastCoorIndex].z < PillarVariables::coor.z + ZFILTER /*&& abs(regionTrack[lastCoorIndex].x - CameraVariables::pillarcoor.x ) < 300*/)
		{
			// 保证使用的点的z大于上一帧的z
			if(regionTrack[lastCoorIndex].z > regionTrack[lastCoorIndex - 1].z)
				break;
		}
	}

	//lastCoorIndex--;
	//如果追踪点的数量太少 或者追踪的region过早丢失
	if (lastCoorIndex < 2)
		return 1;
	else if (PillarVariables::coor.z - regionTrack[lastCoorIndex].z > 1000 || PillarVariables::coor.y - regionTrack[lastCoorIndex].y > 10000)
		return 2;


	//交点坐标
	Coor3D intersectionCoor;
	double ratioThree[3];
	int index0 = lastCoorIndex, index1 = lastCoorIndex - 1, index2;
	while (regionTrack[index1].y == regionTrack[index0].y && index1 > 0)
		index1--;
	if (index1 < 0)
		return 1;
	index2 = index1 - 1;
	while (regionTrack[index2].y == regionTrack[index1].y && index2 > 0)
		index2--;
	if (index2 < 0)
		return 1;
	ratioThree[0] = (regionTrack[index1].z - regionTrack[index0].z) /
		(regionTrack[index1].y - regionTrack[index0].y);
	ratioThree[1] = (regionTrack[index2].z - regionTrack[index1].z) /
		(regionTrack[index2].y - (regionTrack[index1].y));
	ratioThree[2] = (regionTrack[index2].z - regionTrack[index0].z) /
		(regionTrack[index2].y - regionTrack[index0].y);
	for (int i = 0; i < 3; i++)
	{
		if (ratioThree[i] > 4)
			ratioThree[i] = 4;
		if (ratioThree[i] < 0)
			ratioThree[i] = 0;
	}
	double ratio = tan((atan(ratioThree[0]) + atan(ratioThree[1]) + atan(ratioThree[2])) / 3);
	intersectionCoor.x = regionTrack[lastCoorIndex].x + (regionTrack[index0].y - regionTrack[index1].y) * ratio;
	intersectionCoor.y = PillarVariables::coor.y;
	intersectionCoor.z = regionTrack[lastCoorIndex].z + (regionTrack[index0].y - regionTrack[index1].y) * ratio;

	if (regionTrack[lastCoorIndex].z > 6000)
	{
		double k = 0.0013333;
		double deltaZ = -k / 2 * (PillarVariables::coor.y*PillarVariables::coor.y - regionTrack[lastCoorIndex - 2].y*regionTrack[lastCoorIndex - 2].y) + (1500 * k + 2)*(PillarVariables::coor.y - regionTrack[lastCoorIndex - 2].y);
		intersectionCoor.z = regionTrack[lastCoorIndex - 2].z + deltaZ;
	}

	//if (regionTrack[lastCoorIndex].z > 3000 && regionTrack[lastCoorIndex].z < 6000)
	//{
	//	double k = 620;
	//	double b = 1.43;
	//	double deltaZ = k*(log(PillarVariables::coor.y) - log(regionTrack[lastCoorIndex].y)) + b*(PillarVariables::coor.y - regionTrack[lastCoorIndex].y);
	//	intersectionCoor.z = regionTrack[lastCoorIndex].z + deltaZ;
	//}
	offset[0] = intersectionCoor.x - PillarVariables::coor.x;
	//因为柱子有厚度 加100
	offset[1] = intersectionCoor.z - PillarVariables::coor.z - 100;


	std::ofstream datafile;
	datafile.open("C:\\Users\\robocon2017\\Desktop\\datafile.txt", std::ios::app);
	datafile << "使用了倒数第" << regionTrack.size() - lastCoorIndex << " 个数据" << endl;
	datafile << "柱子坐标 X：" << PillarVariables::coor.x << "  Y：" << PillarVariables::coor.y << "  Z：" << PillarVariables::coor.z << endl;
	datafile.close();
	return 0;
}
