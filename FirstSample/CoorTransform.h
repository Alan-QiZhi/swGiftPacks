#pragma once
namespace rc17
{
	struct Coor3D
	{
		double x;
		double y;
		double z;
		bool IsValid() const { return z == z; }  // check for NAN

		Coor3D(float _x = 0, float _y = 0, float _z = 0)
		{
			x = _x;
			y = _y;
			z = _z;
		}

		static Coor3D empty()
		{
			Coor3D _empty;
			_empty.x = -1;
			_empty.y = -1;
			_empty.z = -1;
			return _empty;
		}
	};

	struct Coor2D
	{
		int row;
		int column;

		static Coor2D empty()
		{
			Coor2D _empty;
			_empty.row = -1;
			_empty.column = -1;
			return _empty;
		}
	};

	struct CameraParam
	{
		double worldX = 0;
		double worldY = 0;
		double worldZ = 0;
		double pitch = 0;
		double yaw = 0;
	};

	class CoorTransform
	{
	public:
		static bool cameraToPixel(const Coor3D& CameraCoor, double& Row, double& Column);
		static Coor3D worldToCamera(const CameraParam& _cameraParam, const Coor3D& WorldCoor);
		static Coor3D cameraToWorld(const CameraParam& _cameraParam, Coor3D& CameraCoor);
		static Coor3D pixelToCamera(const double& Row, const double& Column, const double& z);
		static Coor3D rotateVector(const Coor3D& origin, const float& rotateAngel);
	private:
		static const int HEIGHT = 480, WIDTH = 640;
		static const int HFOV = 53.3, VFOV = 42;
		static bool isInScreen(const double& Row, const double& Column);
	};
}