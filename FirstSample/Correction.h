#pragma once
#include <cmath>

namespace rc17
{
	class Correction
	{
	public:
		//��ʼ���� ��һ������Ϊ  0.2  0   0   0   0  0    
		// �ڶ�������Ϊ ����ͷ��⵽��x y��� ������ĵ�
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






