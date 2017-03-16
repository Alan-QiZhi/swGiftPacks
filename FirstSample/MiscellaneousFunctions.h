#pragma once

#include "HalconCpp.h"
#include "HDevThread.h"

using namespace HalconCpp;

namespace rc17
{
	void findRegion(HObject ho_GrayImage, HObject *ho_Region, HTuple hv_x, HTuple hv_y, HTuple hv_threashold);
	void writeImage(int count = 0);
}