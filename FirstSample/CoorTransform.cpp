#define _USE_MATH_DEFINES
#include <math.h>
#include "GobalVariables.h"
#include "CoorTransform.h"

bool rc17::CoorTransform::cameraToPixel(const Coor3D& CameraCoor, double& Row, double& Column)
{
	double thetaRow, thetaColumn;
	thetaRow = atan(CameraCoor.y / CameraCoor.z) / M_PI * 180;
	thetaColumn = atan(CameraCoor.x / CameraCoor.z) / M_PI * 180;
	Row = thetaRow * HEIGHT / VFOV + HEIGHT / 2;
	Column = thetaColumn * WIDTH / HFOV + WIDTH / 2;

	return isInScreen(Row, Column);
}

rc17::Coor3D rc17::CoorTransform::worldToCamera(const CameraParam& _cameraParam, const Coor3D& WorldCoor)
{
	Coor3D relativeCoor, CameraCoor;
	if (!relativeCoor.IsValid())
	{
		CameraCoor.x = 0;
		CameraCoor.y = 0;
		CameraCoor.z = 0;
		return CameraCoor;
	}
	//相对坐标
#ifdef REDTEAM
	relativeCoor.x = _cameraParam.worldX - WorldCoor.x;
#endif
#ifdef BLUETEAM
	relativeCoor.x = WorldCoor.x - _cameraParam.worldX;
#endif
	relativeCoor.y = WorldCoor.y - _cameraParam.worldY;
	relativeCoor.z = WorldCoor.z - _cameraParam.worldZ;
	CameraCoor.x = relativeCoor.x * cos(_cameraParam.yaw / 180 * M_PI) + relativeCoor.y * sin(_cameraParam.yaw / 180 * M_PI);

	CameraCoor.z = relativeCoor.x * (-sin(_cameraParam.yaw / 180 * M_PI) * cos(_cameraParam.pitch / 180 * M_PI)) +
		relativeCoor.y * (cos(_cameraParam.yaw / 180 * M_PI) * cos(_cameraParam.pitch / 180 * M_PI)) + relativeCoor.z * sin(_cameraParam.pitch / 180 * M_PI);
	CameraCoor.y = -(relativeCoor.x * (sin(_cameraParam.yaw / 180 * M_PI) * sin(_cameraParam.pitch / 180 * M_PI)) +
		relativeCoor.y * (-cos(_cameraParam.yaw / 180 * M_PI) * sin(_cameraParam.pitch / 180 * M_PI)) + relativeCoor.z * cos(_cameraParam.pitch / 180 * M_PI));

	return CameraCoor;
}

rc17::Coor3D rc17::CoorTransform::cameraToWorld(const CameraParam& _cameraParam, Coor3D& CameraCoor)
{
	Coor3D relativeCoor, WorldCoor;
	if (!relativeCoor.IsValid())
	{
		WorldCoor.x = 0;
		WorldCoor.y = 0;
		WorldCoor.z = 0;
		return WorldCoor;
	}
	double tmp;
	tmp = CameraCoor.y;
	CameraCoor.y = CameraCoor.z;
	CameraCoor.z = -tmp;
	relativeCoor.x = CameraCoor.x * cos(_cameraParam.yaw / 180 * M_PI) + CameraCoor.y * (-sin(_cameraParam.yaw / 180 * M_PI) * cos(_cameraParam.pitch / 180 * M_PI)) + CameraCoor.z * (sin(_cameraParam.yaw / 180 * M_PI) * sin(_cameraParam.pitch / 180 * M_PI));
	relativeCoor.y = CameraCoor.x * sin(_cameraParam.yaw / 180 * M_PI) + CameraCoor.y * (cos(_cameraParam.yaw / 180 * M_PI) * cos(_cameraParam.pitch / 180 * M_PI)) + CameraCoor.z * (-cos(_cameraParam.yaw / 180 * M_PI) * sin(_cameraParam.pitch / 180 * M_PI));
	relativeCoor.z = CameraCoor.y * sin(_cameraParam.pitch / 180 * M_PI) + CameraCoor.z * cos(_cameraParam.pitch / 180 * M_PI);
#ifdef REDTEAM
	WorldCoor.x = -relativeCoor.x + _cameraParam.worldX;
#endif
#ifdef BLUETEAM
	WorldCoor.x = relativeCoor.x + _cameraParam.worldX;
#endif
	WorldCoor.y = relativeCoor.y + _cameraParam.worldY;
	WorldCoor.z = relativeCoor.z + _cameraParam.worldZ;
	return WorldCoor;
}

rc17::Coor3D rc17::CoorTransform::pixelToCamera(const double& Row, const double& Column, const double& z)
{
	Coor3D CameraCoor;
	double thetaRow, thetaColumn;
	thetaRow = (Row - HEIGHT / 2 + 3) * VFOV / HEIGHT;
	thetaColumn = (Column - WIDTH / 2 + 14) * HFOV / WIDTH;
	CameraCoor.x = tan(thetaColumn / 180 * M_PI) * z;
	CameraCoor.y = tan(thetaRow / 180 * M_PI) * z;
	CameraCoor.z = z;
	return CameraCoor;
}

rc17::Coor3D rc17::CoorTransform::rotateVector(const Coor3D& origin, const float& rotateAngel)
{
	Coor3D result;
	result.x = cos(rotateAngel / 180 * M_PI) * origin.x - sin(rotateAngel / 180 * M_PI) * origin.y;
	result.y = sin(rotateAngel / 180 * M_PI) * origin.x + cos(rotateAngel / 180 * M_PI) * origin.y;
	return result;
}

bool rc17::CoorTransform::isInScreen(const double& Row, const double& Column)
{
	//判断坐标是不是在屏幕内
	if (Row < 0 || Row > HEIGHT || Column < 25 || Column > WIDTH - 25)
		return false;
	else 
		return true;
}