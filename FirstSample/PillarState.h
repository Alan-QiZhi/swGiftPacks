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
		enum
		{
			WithBall = 0,
			NoBall
		};
		static Coor3D getPillarCoor();
		static Coor2D getPillarPixel();
		static bool lockPillar(int type);
		static bool hasBall();
	private:
		static const Coor3D pillarWorldCoor[7];
		static Coor3D middleFilter(Coor3D pillarCoor);
	};
}