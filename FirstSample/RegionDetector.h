#pragma once
#include<vector>

#include "HalconCpp.h"
#include "HDevThread.h"

#include "FindRegion.h"

using namespace HalconCpp;
using namespace std;
namespace rc17
{
	class RegionDetector
	{
	public:
		static std::vector<HObject> RegionsFound(HObject &Image);
		static std::vector<FindRegion> regionVector;
		static void detectRegion();

	private:
		//��region������Ҷ���
		static void pushRegionToFind(HTuple Row, HTuple Column, HTuple Area, HTuple Grayval);
		//������������İ汾
		static void pushRegionToFind(HTuple Row, HTuple Column, HTuple Area, HTuple Grayval, CameraParam mycamParam);
	};
}