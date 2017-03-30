#pragma once
#include <cmath>

namespace rc17
{
	class Correction
	{
	public:
		//初始输入 第一个数组为  0.2  0   0   0   0  0    
		// 第二个数组为 摄像头检测到的x y误差 相对中心点
		// new
		static void Correct_Point(int Plane_Num, float Correct_Par[], float Point[]);
		static void calculate2(int Plane_Num, float Correct_Par[], float Point[]);
	private:
		static double Coe_Correct_2m;
		static double Coe_Correct_5m;
		static double Length_2m;
		static double Length_5m;
		static double Coe_Error;
		static double pi;
		static double Length_X_Min;
		static double Length_Y_Max;
		static double Length_Y_Min;
		static double Radius_Max;
		static double Coe_Judge_Model;

	};
}






