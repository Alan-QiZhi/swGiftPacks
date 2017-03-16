#pragma once
#include "HalconCpp.h"

#include "CoorTransform.h"

using namespace HalconCpp;
namespace rc17
{
	enum PillarIndex
	{
		nearPillar = 0,
		mostLeftPillar,
		secondLeftPillar,
		middlePillar,
		secondRightPillar,
		mostRightPillar,
		farPillar
	};

	class PillarState
	{
	public:
		static Coor3D getPillarCoor(const CameraParam myCamParam, const PillarIndex pillarToFind);
		static Coor2D getPillarPixel(const CameraParam myCamParam, const PillarIndex pillarToFind);
		static double offsetOfUnderpansYaw(const PillarIndex pillarToFind, const int column);
		static bool hasBaloon();
	private:
		static const Coor3D pillarWorldCoor[7];
		static Coor3D middleFilter(Coor3D pillarCoor);
	};
}