#include "GobalVariables.h"
#include "MiscellaneousFunctions.h"

void rc17::findRegion(HObject ho_GrayImage, HObject *ho_Region, HTuple hv_x, HTuple hv_y, HTuple hv_threashold)
{
	// Local iconic variables
	HObject  ho_EmptyRegion, ho_point, ho_Regions;
	HObject  ho_ConnectedRegions, ho_ObjectSelected, ho_ifEmpty;

	// Local control variables
	HTuple  hv_Grayval, hv_FloorX, hv_FloorY, hv_Number;
	HTuple  hv_Index;

	GenEmptyRegion(&ho_EmptyRegion);

	if (hv_x.D() < 1 && hv_y.D() < 1)
	{
		*ho_Region = ho_EmptyRegion;
		return;
	}
	GetGrayval(ho_GrayImage, hv_x, hv_y, &hv_Grayval);

	TupleFloor(hv_x, &hv_FloorX);
	TupleFloor(hv_y, &hv_FloorY);
	GenRegionPoints(&ho_point, hv_FloorX, hv_FloorY);

	Threshold(ho_GrayImage, &ho_Regions, hv_Grayval - hv_threashold, hv_Grayval + hv_threashold);
	Connection(ho_Regions, &ho_ConnectedRegions);
	CountObj(ho_ConnectedRegions, &hv_Number);
	{
		HTuple end_val13 = hv_Number;
		HTuple step_val13 = 1;
		for (hv_Index = 1; hv_Index.Continue(end_val13, step_val13); hv_Index += step_val13)
		{
			SelectObj(ho_ConnectedRegions, &ho_ObjectSelected, hv_Index);
			Intersection(ho_ObjectSelected, ho_point, &ho_ifEmpty);
			if (0 != (ho_ifEmpty != ho_EmptyRegion))
			{
				(*ho_Region) = ho_ObjectSelected;
			}
			//
		}
	}

	return;
}

void rc17::writeImage(int count)
{		
	static HTuple  hv_ImageCount = 0;
	if (count != 0)
	{
		hv_ImageCount = hv_ImageCount + 1;
		if (hv_ImageCount.D() < count)
		{
			WriteImage(CameraVar::depthImage, "tiff", 0, "C:/Users/robocon2017/Desktop/2/depth" + hv_ImageCount);
		}
	}
	else
	{
		WriteImage(CameraVar::depthImage, "tiff", 0, "C:/Users/robocon2017/Desktop/2/depth" + hv_ImageCount);
	}
}
