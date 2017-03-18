#include <fstream>
#include <iomanip>
#include "GobalVariables.h"
#include "RegionDetector.h"
#include "CoorTransform.h"
#include "Correction.h"
#include "Protocol.h"
#include "PillarState.h"


std::vector<rc17::FindRegion> rc17::RegionDetector::regionVector = std::vector<rc17::FindRegion>();

void rc17::RegionDetector::pushRegionToFind(HTuple Row, HTuple Column, HTuple Area, HTuple Grayval)
{
	FindRegion tmp(Row, Column, Area, Grayval);
	regionVector.push_back(tmp);
}

void rc17::RegionDetector::pushRegionToFind(HTuple Row, HTuple Column, HTuple Area, HTuple Grayval, CameraParam mycamParam)
{
	FindRegion tmp(Row, Column, Area, Grayval, mycamParam);
	regionVector.push_back(tmp);
}

void rc17::RegionDetector::detectRegion()
{
	//设置检测区的宽度
	int DETECTMINLIMIT = 1800;
	int DETECTMAXLIMIT = 3500;

	SetColor(HalconVariables::hv_WindowHandle, "red");
	HObject ho_ROI_0, ho_ImageReduced, ho_Region, ho_ConnectedRegions, ho_SelectedRegions, ho_RegionUnion;
	GenEmptyRegion(&ho_RegionUnion);
	HTuple regionNum, hv_Area, hv_Row, hv_Column, hv_Grayval;


	GenRectangle1(&ho_ROI_0, 0, 0, 478.5, 637.5);
	ReduceDomain(CameraVariables::depthImage, ho_ROI_0, &ho_ImageReduced);

	Threshold(ho_ImageReduced, &ho_Region, DETECTMINLIMIT, DETECTMAXLIMIT);
	Connection(ho_Region, &ho_ConnectedRegions);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", 100, 19177);
	SelectShape(ho_SelectedRegions, &ho_SelectedRegions, "column", "and", 100, 500);
	CountObj(ho_SelectedRegions, &regionNum);

	for (HTuple i = 1; i < regionNum + 1; i = i + 1)
	{
		HTuple hv_Deviation;
		AreaCenter(ho_SelectedRegions[i], &hv_Area, &hv_Row, &hv_Column);
		HalconCpp::Intensity(ho_SelectedRegions[i], CameraVariables::depthImage, &hv_Grayval, &hv_Deviation);
		if (hv_Grayval.D()>0 && hv_Grayval.D() < 65535)
		{
			Union2(ho_SelectedRegions[i], ho_RegionUnion, &ho_RegionUnion);
#ifdef NEWMETHOD
			pushRegionToFind(hv_Row, hv_Column, hv_Area, hv_Grayval, rc17::CameraVariables::cameraParam);
#else
			pushRegionToFind(hv_Row, hv_Column, hv_Area, hv_Grayval);
#endif
		}

		if (HDevWindowStack::IsOpen())
			DispObj(ho_RegionUnion, HDevWindowStack::GetActive());
	}
}

std::vector<HObject> rc17::RegionDetector::RegionsFound(HObject &Image)
{
	std::vector<HObject> result;
	HObject emptyRegion, tmpObj;
	emptyRegion.GenEmptyObj();
	if (regionVector.empty())
		return result;

	for (size_t i = 0; i < regionVector.size(); i++)
	{
		//删两次
		for (size_t j = i + 1; j < regionVector.size(); j++)
		{
			if (regionVector[j] == regionVector[i])
			{
				regionVector.erase(regionVector.begin() + j);
			}
		}

		try
		{
			//将返回的结果加入vector容器
			if (i < regionVector.size())
			{
#ifdef NEWMETHOD
				tmpObj = regionVector[i].findNext(Image, rc17::CameraVariables::cameraParam);
#else
				tmpObj = regionVector[i].findNext(Image);
#endif
			}
		}
		catch(...)
		{
			cout << "233" << endl;
			throw;
		}

		HTuple hv_Area, hv_Row, hv_Column;
		AreaCenter(tmpObj, &hv_Area, &hv_Row, &hv_Column);
		if (hv_Area.D() > 0)
		{
			result.push_back(tmpObj);
		}
		//删除重复的region
		for (size_t j = i + 1; j < regionVector.size(); j++)
		{
			if (regionVector[j] == regionVector[i])
			{
				regionVector.erase(regionVector.begin() + j);
			}
		}
		if (regionVector[i].isSaucerLost())
		{
			int trackSuccess;
			double offset[2];

			trackSuccess = regionVector[i].getOffset(offset);

			if (trackSuccess == 3)
			{
				regionVector.erase(regionVector.begin() + i);
				continue;//这是个锅
			}

			std::ofstream datafile;
			datafile.open("C:\\Users\\robocon2017\\Desktop\\datafile.txt", std::ios::app);
			if (trackSuccess == 0)
			{
				datafile << "飞盘" << regionVector[i].getSaucerIndex() << " x偏差: " << setw(2) << offset[0] << " z偏差: " << setw(2) << offset[1] << endl;
				cout << " x偏差: " << setw(2) << offset[0] << " z偏差: " << setw(2) << offset[1] << endl;
# ifdef USESOCKET
				rc17::CommunicationVariables::mySocketClient.connectAndSendOffset(PillarIndex(rc17::PillarVariables::index), (int)offset[0], (int)offset[1]);
				Coor3D tmpOffset, rotatedOffset;
				
				tmpOffset.x = offset[0];
				tmpOffset.y = offset[1];
				rotatedOffset = rc17::CoorTransform::rotateVector(tmpOffset, CAMERAROTATE);
				//旋转到云台方向偏差
				double rotatedOffsetA[2];
				rotatedOffsetA[0] = rotatedOffset.x / 10;
				rotatedOffsetA[1] = rotatedOffset.y / 10;

				cout << "0:" << rotatedOffsetA[0] << "  1:" << rotatedOffsetA[1] << endl;
				long tmpx = 0, tmpz = 0, tmpyaw = 0;

				int method = rc17::Correction::Judge_Method(rc17::PillarVariables::index + 1, rotatedOffsetA);
				if (method == 1)
				{
					double Par_Input[5] = { 0 };
					double Par_Output[4] = { 0 };
					Par_Input[0] = rotatedOffsetA[0];
					Par_Input[1] = rotatedOffsetA[1];
					rc17::Correction::Yaw_Correct(rc17::PillarVariables::index + 1, Par_Input, Par_Output);
					Par_Input[2] = Par_Output[3];
					Par_Input[3] = Par_Output[1];
					Par_Input[4] = Par_Output[2];
					tmpx = 0;
					tmpz = 0;
					tmpyaw = static_cast<long>(Par_Output[0] * 100 + 10000);
				}
				else if (method == 2)
				{
					double correctPoint1[6] = { 0.2, 0, 0, 0, 0, 0 };
					rc17::Correction::Correct_Point(correctPoint1, rotatedOffsetA);
					tmpx = static_cast<long>(correctPoint1[1] * 10 + 20000);
					tmpz = static_cast<long>(correctPoint1[2] * 10 + 20000);
					tmpyaw = 0;
				}

				rc17::Protocol::sendDataBySocket(tmpx, tmpz, tmpyaw);
# endif
					Coor3D tmpOffset, rotatedOffset;
					tmpOffset.x = offset[0];
					tmpOffset.y = offset[1];
					
					rotatedOffset = rc17::CoorTransform::rotateVector(tmpOffset, CameraVariables::cameraRotate);
					cout << " x偏差: " << setw(2) << rotatedOffset.x << " z偏差: " << setw(2) << rotatedOffset.y << endl;
					//旋转到云台方向偏差

					//double tmpx, tmpz, tmpyaw;
					//rc17::Correction::calculate(rc17::PillarVariables::index, rotatedOffset.x, rotatedOffset.y, tmpx, tmpz, tmpyaw);

					float* Correct_Par = new float[8];
					float* rtOffset = new float[4];
					rtOffset[0] = rotatedOffset.x;
					rtOffset[1] = rotatedOffset.y;
					rtOffset[2] = PillarVariables::worldCoor.z/10;

					//cin >> rtOffset[0];
					//cin >> rtOffset[1];
					//cout << " 输入x偏差: " << rtOffset[0] << " 输入z偏差: " << rtOffset[1] << endl;

					rc17::Correction::calculate2(PillarVariables::index, Correct_Par, rtOffset);
					rc17::Protocol::delayCorrectVariables[PillarVariables::index].assign(Correct_Par);
					PillarVariables::correctedYaw[PillarVariables::index] += Correct_Par[3];
					cout << "   " << Correct_Par[0] << "   " << Correct_Par[1] << "   " << Correct_Par[2] << "   " <<
						Correct_Par[3] << "   " << Correct_Par[4] << "   " << Correct_Par[5] << "   " << endl;
# ifdef USESERIALPORT
					try
					{
						//if(Correct_Par[3])
							//rc17::Protocol::sendDataBySerialPort(Protocol::NoBallPara, (double)Correct_Par[1], (double)Correct_Par[2], (double)Correct_Par[3], -(double)Correct_Par[4], -(double)Correct_Par[5]);
					}
					catch(...)
					{
						cout << "send failed";
					}
					//rc17::Protocol::sendDataBySerialPort(tmpx, tmpz, tmpyaw);
# endif
			}
			else if (trackSuccess == 1)
				datafile << "飞盘" << regionVector[i].getSaucerIndex() << "采集点太少或者是杂物" << endl;
			else if (trackSuccess == 2)
				datafile << "飞盘" << regionVector[i].getSaucerIndex() << "过早跟丢" << endl;
			datafile << "飞盘" << regionVector[i].getSaucerIndex() << " 已丢失！" << std::endl << std::endl;
			regionVector.erase(regionVector.begin() + i);

			datafile.close();
		}
	}

	return result;
}
