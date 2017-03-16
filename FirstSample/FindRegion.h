#pragma once

#include <vector>
#include "HalconCpp.h"
#include "HDevThread.h"
#include "CoorTransform.h"

using namespace HalconCpp;
using namespace std;
namespace rc17
{
	class FindRegion
	{
	public:
		FindRegion(HTuple Row, HTuple Column, HTuple Area, HTuple Grayval)
			:saucerIndex(saucerCount), lastRow(Row), lastColumn(Column), lastArea(Area), lastGrayval(Grayval), Row(0), Column(0), Z(0)
		{
			saucerCount++;
		}

		FindRegion(HTuple Row, HTuple Column, HTuple Area, HTuple Grayval, CameraParam _cameraParam)
			:saucerIndex(saucerCount), lastArea(Area), Row(0), Column(0), Z(0), lastABSWorldCoor(CoorTransform::cameraToWorld(_cameraParam, CoorTransform::pixelToCamera(Row.D(), Column.D(), Grayval.D())))
		{
			saucerCount++;
		}

		int getSaucerIndex() const
		{ return saucerIndex; }
		bool isSaucerLost(){ return isLost;}
		HObject findNext(HObject &Image);
		HObject findNext(HObject &Image, CameraParam _cameraParam);
		void recordRegionTrack(const Coor3D regionCoor){ regionTrack.push_back(regionCoor);}
		int getOffset(double offset[2]);

		inline bool operator == (const FindRegion& rhs)const 
		{
#ifdef NEWMETHOD 
			return (abs(lastABSWorldCoor.x - rhs.lastABSWorldCoor.x < 1000) &&
				abs(lastABSWorldCoor.y - rhs.lastABSWorldCoor.y < 1000) &&
				abs(lastABSWorldCoor.z - rhs.lastABSWorldCoor.z) < 1000);
#else
			return (abs(lastRow.D() - rhs.lastRow.D() < 10) && abs(lastColumn.D() - rhs.lastColumn.D() < 10) && abs(lastGrayval.D() - rhs.lastGrayval.D()) < 100);
#endif
		}

	private:
		static int saucerCount;
		int saucerIndex;
		static const int MAXFINDAGAINTIMES = 5;
		static const int THRESHOLD = 500;
		int findTimesCount = 0;
		HTuple lastRow, lastColumn, lastArea, lastGrayval = 0, vx = 0, vy = 0, vz = 0;
		double Row, Column, Z;
		HObject Result;
		int findAgainTimes = MAXFINDAGAINTIMES;
		bool isLost = false;
		Coor3D lastABSWorldCoor;
		vector<Coor3D> regionTrack;
		int unnecessaryToFindNext() const;
		HObject findRegionFromGivenArea(HObject &Image)const;
		HTuple fillUpRegionHasFound(HObject &Image, HObject &Region);
	};
}