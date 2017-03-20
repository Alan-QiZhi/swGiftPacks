#include"correction.h"
#include "GobalVariables.h"
/*
*		File： Point_Correct.cpp
*
*		目录：	1、初始化赋值
*				2、计算及赋值函数
*				3、滤波及自检函数
*				4、主函数-接口函数
*	程序内容：	对于开始修正量的判断：通过查看正态分布图，来找一个合适的临界值，增大和减小都会有比较好的变化，此时作为开始变化的值，再调整比例系数和可信度关系
*   注		：	1、三次修正值保存是为了避免一种特殊情况，并非无用
*				2、输入值的单位为 cm
*
*	重要变量：	Coff_Correct		放大最后输出的比例系数
*				Ideal_Point			初始理想落点
*				Confidence_Percent	用于计算的可信度，与生活中相反，便于计算，可信度设置为 1 - 真实可信度
*				Correct_Max			修正值限幅
*	
*	3-15 把偏航角修正加入实时计算公式，利用摄像头返回参数计算，加入可信度循环过程
*/

/*1----------初始参数赋值-----------------1
*
*------------------------------------------*/

static float Max_Value[7][10] = { { 0, 0, 0, 0, 8, -8 },
{ 2, -2,  2, -2,   8,  -8,  100,  -100,  220,  -220 },
{ 3, -3,  4, -4,  10, -10,  30,  -30,  210,  -240 },
{ 0, 0, 0, 0,  8, -8  },
{ 0, 0, 0, 0,  8, -8  },
{ 0, 0, 0, 0,  8, -8  },
{ 0, 0, 0, 0,  8, -8  },
};//--范围检查值--

static float Correct_DeltaAll[7][5] = {
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
};//--修正delta值总和--

static float Correct_BestDelta[8][5] = { 
	{ 7, 200, 1,  0,  },  //台子编号，最小偏差，可信度
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
};//--最好状态存储值--



/*2----------计算高斯分布概率-----------------2
*
* 说明	：改编自matlab程序
*		：
*------------------------------------------*/
float Gauss(float x, int mu, int sigma)
{
	float y;
	const float pi = 3.1416;

	y = exp(-0.5 * ((x - mu) / sigma) * ((x - mu) / sigma)) / (sqrt(2 * pi) * sigma);

	return y;
}



//--计算修正量大小--
void rc17::Correction::Correct_Point(double Correct_Par[], double Point[])
{
	int Coff_Correct = 6;
	int flag = 0;
	double Confidence_Percent;
	double Length;
	double P_Gauss;
	const double Ideal_Point[2] = { 0, 0 };
	double Value_Corct[3] = { Correct_Par[3], Correct_Par[4], Correct_Par[5] };
	double Correct_Max = 38;						//---为40cm处最低可信度时的修正量

												//------参数值计算-----
	Confidence_Percent = Correct_Par[0];
	Point[0] = Point[0] / 20;					//---数据缩放
	Point[1] = Point[1] / 20;

	//------计算距离和高斯分布概率-----
	Length = (Point[0] - Ideal_Point[0]) * (Point[0] - Ideal_Point[0]) + (Point[1] - Ideal_Point[1]) * (Point[1] - Ideal_Point[1]);
	Length = sqrt(Length);
	P_Gauss = Gauss(Length, 0, 1);
	Coff_Correct += (int)Length * 20;

	//------判断是否需要修正以及相应修正量-----
	if (P_Gauss < 0.19)  //--对应25cm
	{
		flag = 1;
		Value_Corct[2] = Value_Corct[1];
		Value_Corct[1] = Value_Corct[0];
		Value_Corct[0] = (1 - P_Gauss) * Confidence_Percent *  Coff_Correct;


		if ((Value_Corct[0] >(Value_Corct[1] + Value_Corct[2])) && (Value_Corct[1] != 0) && (Value_Corct[2] != 0))	//-- 一次限幅
		{
			Value_Corct[0] = Value_Corct[1] + Value_Corct[2];
		}

		if (Value_Corct[0] > Correct_Max)																				//-- 二次限幅
		{
			Value_Corct[0] = Correct_Max;
		}

		Confidence_Percent = Confidence_Percent + (1 - P_Gauss) * Length / 35;  //--信任度修改->减小信任度

		if (Confidence_Percent > 0.97)											//--信任度限幅
			Confidence_Percent = 0.97;
	}
	else
	{
		Value_Corct[2] = Value_Corct[1];
		Value_Corct[1] = Value_Corct[0];
		Value_Corct[0] = 0;
		Confidence_Percent = Confidence_Percent - P_Gauss / 4;      //信任度修改 -> 正常情况

		if (Confidence_Percent < 0.03)								//--信任度限幅
			Confidence_Percent = 0.03;
	}

	//------程序测试用---------
	/*cout << "the input point_x is " << Point[0] << "  the input point_y is  " << Point[1] << endl;
	cout << " the Confidence_Percent is  " << Confidence_Percent << endl;*/

	//------更新可信度和修正量----------
	Correct_Par[0] = Confidence_Percent;
	if (flag == 1)
	{
		Correct_Par[1] = -Value_Corct[0] * (Point[0] / Length);
		Correct_Par[2] = -Value_Corct[0] * (Point[1] / Length);
	}
	else
	{
		Correct_Par[1] = 0;
		Correct_Par[2] = 0;
	}
	Correct_Par[3] = Value_Corct[0];
	Correct_Par[4] = Value_Corct[1];
	Correct_Par[5] = Value_Corct[2];
}

/*
*		File：	Yaw_Correct.cpp
*
*	程序内容：	通过测试飞盘落到近台上的情况，来尽量减小偏航角误差，因为近台数据测试情况比较稳定，可信度较高
*   注		：	函数内外均采用角度为统一传递单位
*				需要修改平台编号，Plat_Num 具体数值商榷
*
*	重要变量：
*				Coe_Correct		修正比例
*				Length			台子到发射点的距离，精确一点比较好，水平距离，不需要考虑z轴影响,cm精度
*				Coe_Error		实际测量由于盘子误差导致的水平最大偏差和最小偏查的比例
*				Radius_Max		用于防止盘子y方向偏移过大
*`				Coe_Judge_Model	用于区分哪种修正的系数，之前单纯比较x y大小效果不是太好
*/


double rc17::Correction::Coe_Correct_2m = 0.42;
double rc17::Correction::Coe_Correct_5m = 0.51;
double rc17::Correction::Length_2m = 300;
double rc17::Correction::Length_5m = 600;
double rc17::Correction::Coe_Error = 1.3;
double rc17::Correction::pi = 3.14159265;
double rc17::Correction::Length_X_Min = 15;
double rc17::Correction::Length_Y_Max = 20;
double rc17::Correction::Length_Y_Min = -35;
double rc17::Correction::Radius_Max = 20;
double rc17::Correction::Coe_Judge_Model = 1.2;

/*----------计算偏航角修正量-----------------
*
* 说明	：	Par_Input[0]	当前左右水平误差	cm
*			Par_Input[1]	前后误差
*			Par_Input[2]	平均误差			cm
*			Par_Input[3]	累积修正值			角度
*			Par_Input[4]	当前最大偏差值		角度
*
*			Par_Output[0]	当前修正值			角度
*			Par_Output[3]	平均误差			cm
*			Par_Output[1]	累积修正值			角度
*			Par_Output[2]	当前最大偏差值		角度
*		：
*------------------------------------------*/
void rc17::Correction::Yaw_Correct_My(double Length, double Coe_Correct, double Par_Input[], double Par_Output[])
{
	double Correct_Yaw;

	//--是否第一次进入--
	if (Par_Input[2] == 0)
	{
		Par_Input[2] = Par_Input[0];
		Par_Output[3] = Par_Input[2];
		Correct_Yaw = -atan(Par_Input[2] / Length) / pi * 180;
		Par_Output[2] = Correct_Yaw;
	}
	else
	{
		Par_Input[2] = Par_Input[2] / 3 + Par_Input[0] * 2 / 3;
		Par_Output[3] = Par_Input[2];
		Correct_Yaw = -atan(Par_Input[2] / Length) / pi * 180;
		if (abs(Correct_Yaw) > abs(Par_Input[4]))
			Par_Input[4] = Correct_Yaw;
	}

	//--累积修正--
	Par_Output[0] = Coe_Correct * Correct_Yaw;
	Par_Output[1] = Par_Output[1] + Par_Output[0];

	//--修正量自检--
	if (abs(Par_Output[1]) > abs(Par_Output[2]) * Coe_Error)
	{
		Par_Output[1] = Par_Output[1] - Par_Output[0];
		Par_Output[0] = 0;
	}
}



/*----------对外承包函数---------------------
*
* 说明	：
*		：
*------------------------------------------*/
void rc17::Correction::Yaw_Correct(int Plat_Num, double Par_Input[], double Par_Output[])
{
	switch (Plat_Num)
	{
	case 1:
		rc17::Correction::Yaw_Correct_My(Length_2m, Coe_Correct_2m, Par_Input, Par_Output);	//近台
		break;
	case 4:
		rc17::Correction::Yaw_Correct_My(Length_5m, Coe_Correct_5m, Par_Input, Par_Output);	//中台
		break;
	default:
		break;
	}
}

/*----------判断使用哪种修正函数-------------
*
* 说明	：
*		：Plat_Num		平台编号
*		：Point_Hit		落到台子上的位置坐标 [0] x [1] y
*		：Flag			0 不需修正 1 Yaw全体修正  2 普通各自修正

*------------------------------------------*/
int rc17::Correction::Judge_Method(int Plat_Num, double Point_Hit[])
{
	int Flag = 0;
	double Radius = 0;

	//--落点到圆心半径--
	Radius = sqrt(Point_Hit[0] * Point_Hit[0] + Point_Hit[1] * Point_Hit[1]);

	//--近台--
	if (Plat_Num == 1)
	{
		//--飞盘正常降落--
		if (Radius < 37.5)
		{
			if (abs(Point_Hit[0]) < Length_X_Min)
			{
				Flag = 2;
			}
			else if (Point_Hit[1] > Length_Y_Max)
			{
				Flag = 2;
			}
			else
			{
				Flag = 1;
			}
		}
		//--飞盘掉到台下--
		else
		{
			if (abs(Point_Hit[0]) * Coe_Judge_Model > abs(Point_Hit[1]))
			{
				Flag = 1;
			}
			else
				Flag = 2;
		}
	}
	//--中台--
	else if (Plat_Num == 4)
	{
		if (Radius < 50)
			Flag = 2;
		else
			Flag = 1;
	}
	else
	{
		Flag = 2;
	}

	return Flag;
}


void rc17::Correction::calculate(int Plat_Num, double offsetX, double offsetZ, double correctX, double correctZ, double correctYaw)
{
	static double correctPoint1[6] = { 0.2, 0, 0, 0, 0, 0 };
	static double Par_Input[5] = { 0 };
	static double Par_Output[4] = { 0 };
	static int whatMethodUsing = 1;

	double offset[2] = { offsetX ,offsetZ };
	offset[0] /= 10;
	offset[1] /= 10;
	int method = Judge_Method(Plat_Num + 1, offset);
	if (method == 1)
	{
		Par_Input[0] = offset[0];
		Par_Input[1] = offset[1];
		Yaw_Correct(Plat_Num + 1, Par_Input, Par_Output);
		Par_Input[2] = Par_Output[3];
		Par_Input[3] = Par_Output[1];
		Par_Input[4] = Par_Output[2];
		correctX = 0;
		correctZ = 0;
		correctYaw = -Par_Output[0];
		if (whatMethodUsing != 1)
		{
			//上次没用此方法时,初始化其他方法参数
			whatMethodUsing = 1;
			correctPoint1[0] = 0.2;
			for (int i = 1; i < 6; i++)
				correctPoint1[i] = 0;
		}
	}
	else if (method == 2)
	{
		Correct_Point(correctPoint1, offset);
		correctX = correctPoint1[1] * 10;
		correctZ = correctPoint1[2] * 10;
		correctYaw = 0;
		if (whatMethodUsing != 2)
		{
			//上次没用此方法时,初始化其他方法参数
			whatMethodUsing = 2;
			for (int i = 0; i < 5; i++)
				Par_Input[i] = 0;
			for (int i = 0; i < 4; i++)
				Par_Output[i] = 0;
		}
	}
}

void rc17::Correction::calculate2(int Plane_Num, float Correct_Par[], float Point[])
{
	static float par[] = { 0.2f, 0, 0, 0, 0, 0 ,0, 0};
	static int lastPlaneNum = -1;
	if (lastPlaneNum != -1 && lastPlaneNum != Plane_Num)
	{
		par[0] = 0.2f;
		par[1] = 0;
		par[2] = 0;
		par[3] = 0;
		par[4] = 0;
		par[5] = 0;
		par[6] = 0;
		par[7] = 0;
	}
	Point[0] = Point[0] / 10;
	Point[1] = Point[1] / 10;
	Correct_Point(Plane_Num, par, Point);

	lastPlaneNum = Plane_Num;
	for (int i = 0; i < 6; i++)
	{
		Correct_Par[i] = par[i];
	}
}








/*----------计算长度和高斯概率以及修正系数--
*
* 说明	：Length_To_Center发射装置到车体中心的距离
*		：
*------------------------------------------*/
void Length_Coe_Cal(float Point[], const float Ideal_Point[], float Return[])
{
	float Length_Bound;
	float P_Gauss_G;
	static float Bound = 80;
	static float Gauss_Max = 0.48f;
	static float Length_To_Center = 46.6f;
	float Length;
	float Possibility_Gau;
	
	//-------利用已知的偏航角修正值进行xy校准-----
	if (Point[3] > 0)
	{
			Point[0] = Point[0] - Length_To_Center * ( 1 - cos(Point[3]/57.23) );
	}else{	Point[0] = Point[0] + Length_To_Center * ( 1 - cos(Point[3]/57.23) ); }
	
	Point[1] = Point[1] + Length_To_Center * sin(Point[3]/57.23);
	
	Length = (Point[0] - Ideal_Point[0]) * (Point[0] - Ideal_Point[0]) + (Point[1] - Ideal_Point[1]) * (Point[1] - Ideal_Point[1]);
	
	/*Length = ( 25 + i ) * ( 25 + i);--for test*/
	
	if (Length > 6400)							//--为了防止传入的数据过大而炸掉
	{
		Length = 6400;
	}

	Length_Bound = (Bound - sqrt(Length)) / 40;	//--为了高斯函数计算
	
	P_Gauss_G = static_cast<float>(Gauss(Length_Bound, 0, 1));

	Possibility_Gau = P_Gauss_G / Gauss_Max;

	Return[0] = sqrt(Length);					
	Return[1] = P_Gauss_G;
	Return[2] = Possibility_Gau;
}


/*----------初始化对应平台的参数------------
*
* 说明	： 0 最近台   1 中间台    2 侧边中间台
*		：   3 最侧边台     4 最远台
*		： 内存数组- 0 1 2 3  前后左右
* 修正值：修正方向前后左右已有
* slope	：此处斜率以45度划分空间
* Yaw_Coe： 1.5保证出去的长度被修回来，但是对于距离的容差性不太好，改为1.3,保证可信度0.5左右不会炸
*------------------------------------------*/
void Par_Init(int Plane_Num, float Point[], float Par_Coe[])
//	float Speed[],	float Pitch[],
//	float Row[],	float Yaw[]	)
{
	float	Slope_Now;
	const float Slope[2] = { -1, 1 };  
	const float Yaw_Coe = 1.3;
	int		Limit_Y = 28;
	float	Speed1[4];
	float	Speed2[4];
	float	Roll1[4];
	float	Yaw1[4];
	float	Pitch1[4];
	int		Flag_Direction;

	//--计算斜率--
	if (Point[0] != 0)
	{
		Slope_Now = Point[1] / Point[0];
	}
	else
	{
		Slope_Now = Point[1];
	}
	
	//--选取具体台子--
	switch (Plane_Num)
	{
		case 0: 
				Speed1[0] = 0;		Speed1[1] = 0;			Speed1[2] = 0;		Speed1[3] = 0;
				Speed2[0] = 0;		Speed2[1] = 0;			Speed2[2] = 0;		Speed2[3] = 0;
				Pitch1[0] = 0;		Pitch1[1] = 0;			Pitch1[2] = 0;		Pitch1[3] = 0;
				Roll1[0] = 0;		Roll1[1] = 0;			Roll1[2] = 0;		Roll1[3] = 0;
				Yaw1[0] = 0;		Yaw1[1] = 0;			Yaw1[2] = 0;		Yaw1[3] = 0;
				break;

		case 1:
				Speed1[0] = 175 ;	Speed1[1] = -130;		Speed1[2] = 0;		Speed1[3] = 0;
				Speed2[0] = -80;	Speed2[1] = 75;		    Speed2[2] = 0;		Speed2[3] = 0;
				Pitch1[0] =-0.74 ;	Pitch1[1] = 1.27;		Pitch1[2] = 0;		Pitch1[3] = 0;
				Roll1[0] = 0 ;	    Roll1[1] = 0;		    Roll1[2] = 0;		Roll1[3] = 0;
				Yaw1[0] =  -4;	    Yaw1[1] = 3;			Yaw1[2] = 0;		Yaw1[3] = 0;
				break;

		case 2:
				Speed1[0] = 10;		Speed1[1] = -15;		Speed1[2] = 0;		Speed1[3] = 0;
				Speed2[0] = -200;	Speed2[1] = 95;			Speed2[2] = 0;		Speed2[3] = 0;
				Pitch1[0] = -1.82;	Pitch1[1] = 2.69;		Pitch1[2] = 0;		Pitch1[3] = 0;
				Roll1[0] = 3.52;	Roll1[1] = 2.71;		Roll1[2] = 0;		Roll1[3] = 0;
				Yaw1[0] = 2.9;		Yaw1[1] = -1.95;		Yaw1[2] = 0;		Yaw1[3] = 0;

				break;

		case 3:   //前                   后                     左              右
				Speed1[0] = 0;		Speed1[1] = 0;			Speed1[2] = 0;		Speed1[3] = 0;
				Speed2[0] = 0;		Speed2[1] = 0;			Speed2[2] = 0;		Speed2[3] = 0;
				Pitch1[0] = 0;		Pitch1[1] = 0;			Pitch1[2] = 0;		Pitch1[3] = 0;
				Roll1[0] = 0;		Roll1[1] = 0;			Roll1[2] = 0;		Roll1[3] = 0;
				Yaw1[0] = 0;		Yaw1[1] = 0;			Yaw1[2] = 0;		Yaw1[3] = 0;
				break;
		
		case 4:
				Speed1[0] = 0;		Speed1[1] = 0;			Speed1[2] = 0;		Speed1[3] = 0;
				Speed2[0] = 0;		Speed2[1] = 0;			Speed2[2] = 0;		Speed2[3] = 0;
				Pitch1[0] = 0;		Pitch1[1] = 0;			Pitch1[2] = 0;		Pitch1[3] = 0;
				Roll1[0] = 0;		Roll1[1] = 0;			Roll1[2] = 0;		Roll1[3] = 0;
				Yaw1[0] = 0;		Yaw1[1] = 0;			Yaw1[2] = 0;		Yaw1[3] = 0;
				break;

		case 5:
				Speed1[0] = 0;		Speed1[1] = 0;			Speed1[2] = 0;		Speed1[3] = 0;
				Speed2[0] = 0;		Speed2[1] = 0;			Speed2[2] = 0;		Speed2[3] = 0;
				Pitch1[0] = 0;		Pitch1[1] = 0;			Pitch1[2] = 0;		Pitch1[3] = 0;
				Roll1[0] = 0;		Roll1[1] = 0;			Roll1[2] = 0;		Roll1[3] = 0;
				Yaw1[0] = 0;		Yaw1[1] = 0;			Yaw1[2] = 0;		Yaw1[3] = 0;
				break;

		case 6:
				Speed1[0] = 0;		Speed1[1] = 0;			Speed1[2] = 0;		Speed1[3] = 0;
				Speed2[0] = 0;		Speed2[1] = 0;			Speed2[2] = 0;		Speed2[3] = 0;
				Pitch1[0] = 0;		Pitch1[1] = 0;			Pitch1[2] = 0;		Pitch1[3] = 0;
				Roll1[0] = 0;		Roll1[1] = 0;			Roll1[2] = 0;		Roll1[3] = 0;
				Yaw1[0] = 0;		Yaw1[1] = 0;			Yaw1[2] = 0;		Yaw1[3] = 0;
				break;

		default:
				Speed1[0] = 0;		Speed1[1] = 0;			Speed1[2] = 0;		Speed1[3] = 0;
				Speed2[0] = 0;		Speed2[1] = 0;			Speed2[2] = 0;		Speed2[3] = 0;
				Pitch1[0] = 0;		Pitch1[1] = 0;			Pitch1[2] = 0;		Pitch1[3] = 0;
				Roll1[0] = 0;		Roll1[1] = 0;			Roll1[2] = 0;		Roll1[3] = 0;
				Yaw1[0] = 0;		Yaw1[1] = 0;			Yaw1[2] = 0;		Yaw1[3] = 0;
				break;

	}
	
	//--判断具体方向并选取--方案一，用斜率
	if ((Point[0] >= 0) && (Slope_Now >= Slope[0]) && (Slope_Now <= Slope[1]) )			//落点在右
	{
		Flag_Direction = 3;
	}
	else if ((Point[0] <= 0) && (Slope_Now >= Slope[0]) && (Slope_Now <= Slope[1]))		//落点在左
	{	
		Flag_Direction = 2;
	}
	else if (((Point[1] >= Limit_Y) && (Slope_Now <= Slope[0])) || ((Point[1] >= Limit_Y) && (Slope_Now >= Slope[1])))	//落点在前
	{
			Flag_Direction = 0;
	}
	else if (((Point[1] <= -Limit_Y) && (Slope_Now >= Slope[1])) || ((Point[1] <= -Limit_Y) && (Slope_Now <= Slope[0])))	//落点在后
	{
			Flag_Direction = 1;
	}
	else
	{
		if (Point[0] >= 0)
			Flag_Direction = 3;
		else
			Flag_Direction = 2;
	}

	//--给具体参数赋值--
	Par_Coe[0] = Pitch1[Flag_Direction];
	Par_Coe[1] = Roll1[Flag_Direction];
	Par_Coe[2] = static_cast<float>(atan(Point[0] / Point[2])) * 57.325 * Yaw_Coe + Yaw1[Flag_Direction];
	Par_Coe[3] = Speed1[Flag_Direction];
	Par_Coe[4] = Speed2[Flag_Direction];
	
}


/*----------输出值滤波---------------------3
*
* 说明	： 防止输出值太大，导致过修正
*		：
*		：

*------------------------------------------*/
float Check_Number(int Choice, float Correct_Par)
{
	if (Choice == 0)
	{

		if (Correct_Par > 23)
		{
			Correct_Par = 23;
		}
		else
			if (Correct_Par < -23)
			{
				Correct_Par = -23;
			}
	}
	else if (Choice == 1)
	{
		if (Correct_Par > 100)
		{
			Correct_Par = 100;
		}
		else
			if (Correct_Par < -100)
			{
				Correct_Par = -100;
			}
	}
	return Correct_Par;
}



/*----------输出值滤波-------------------3
*
* 说明	： 防止输出值太大，导致过修正
*		：
*		：

*------------------------------------------3*/
void Fliter_Output(float Correct_Par[])
{
	int i;
	
	if(Correct_Par[0] > 0.95)
	{
			Correct_Par[0] = 0.3;
	}
	
	for (i = 1; i<4; i++)
	{
		Check_Number(0, Correct_Par[i]);
	}
	Check_Number(1, Correct_Par[4]);
	Check_Number(1, Correct_Par[5]);
}



/*3----------输出值自检----------------------3
*
* 说明	： 当输出值超范围时，及时报错
*		：正常为0
*		：错误为1
		：Confidence_Limit 可信度的，另一个应用，来判断是否需要启动最优值
*------------------------------------------*/
void Check_CorrectPar(int Plane_Num, float Correct_Par[],float Ponit[] )
{
	int i;
	int j;
	float Change_Final[5];
	float Confidence_Limit = 0.8f;
	int Flag = 0;

	for (i = 0; i < 5; i++)
	{
		Change_Final[i] = Correct_Par[i + 1] + Correct_DeltaAll[Plane_Num][i];

		if ( (Change_Final[i] > Max_Value[Plane_Num][2 * i])  ||	(Change_Final[i] < Max_Value[Plane_Num][2 * i + 1]) )
		{
			if (Correct_Par[0] > Confidence_Limit)		//--信任度已炸--					
			{
				Flag = 1;								//--更新总修正量标志--
			}
			else
			{
				Correct_Par[i+1] = 0;
			}

		}//if

	}//for

	if (Flag == 0)
	{
		for (i = 0; i < 5; i++)
		{
			Correct_DeltaAll[Plane_Num][i] = Correct_DeltaAll[Plane_Num][i] + Correct_Par[i+1];
		}
	}
	else
	{
		Correct_Par[0] = 0.3f;
		
		//-------对最优值进行判断，是否符合要求---------
		if( Correct_BestDelta[0][1] < 32 )
		{
			for (j = 0; j < 5; j++)
			{
				Correct_Par[j+1] = -(Correct_DeltaAll[Plane_Num][j] - Correct_BestDelta[Plane_Num+1][j]);
			}
	
			for (j = 0; j < 5; j++)
			{
				Correct_DeltaAll[Plane_Num][j] = Correct_BestDelta[Plane_Num + 1][j];
			}
		}else
		{
			//-----最优值清空，总修正量清空，回到之前比较好的一个状态-------
			for (j = 0; j < 5; j++)	
			{
				Correct_Par[j+1] = -(Correct_DeltaAll[Plane_Num][j] - Correct_BestDelta[Plane_Num+1][j]);
			}
			
			for (j = 0; j < 5; j++)
			{
				Correct_DeltaAll[Plane_Num][j] = 0;
			}
			
			for (j = 0; j < 5; j++)
			{
				Correct_BestDelta[Plane_Num + 1][j] = 0;	
			}
			
			//-----直接操控俯仰角进行变化-------
			if ( Ponit[1] > 0 )
				Correct_Par[1] = Correct_Par[1] - 2 ;
			else
				Correct_Par[1] = Correct_Par[1] + 2 ;
		}
	}
}



/*3----------最佳状态存储器----------------------3
*
* 说明	： 防止过修正，误修正后的情况无法处理
*		：
*		：

*------------------------------------------*/
void StateBest_Check(int Plane_Num,float  Correct_Par[],float Length_Now)
{
	//int Good_LengthMin = 30;
	int i;
	
	if (Plane_Num == Correct_BestDelta[0][0])											//--检查是否台子变化
	{
		if (Length_Now < Correct_BestDelta[0][1])		//--更新最优状态
		{
			Correct_BestDelta[0][1] = Length_Now;
			for (i = 0; i < 5; i++)
			{
				Correct_BestDelta[Plane_Num+1][i] = Correct_DeltaAll[Plane_Num][i];
			}
		}
	}
	else
	{
		Correct_BestDelta[0][1] = Length_Now;
		Correct_BestDelta[0][0] = Plane_Num;
	}
}




/*----------计算偏航修正量大小------------临时函数
* Turtle
* 说明	： 数据没有测试出来，把偏航角修正改为计算方法
*		：
*------------------------------------------*/
//void Correction_Yaw(int Plane_Num, float Correct_Par[], float Point[])
//{
//	Correct_Par[3] = static_cast<float>(atan(Point[0] / Point[2])) * 57.325;
//}




/*----------计算修正量大小------------main函数
* Turtle
* 说明	：  Correct_Max			修正值限幅（修正值的百分比 0.1-1 ，0.8代表80%）
*		：
*------------------------------------------*/
void rc17::Correction::Correct_Point(int Plane_Num, float Correct_Par[], float Point[])
{
	int Coff_Correct = 6;
	float Confidence_Percent;
	float Length_ToCenter;
	float P_Gauss_G;
	const float P_Gauss_Flag= 0.18f;					//--对应30cm
	const float Correct_Max = 0.9f;
	float Percent_Gau;
	const float Ideal_Point[2] = { 0, 0 };
	float Value_Corct[2] = { Correct_Par[6], Correct_Par[7] };
	float Return[3] = { 0, 0, 0 };
	float Par_Coe[5] = { 0, 0, 0, 0 ,0 };
	int flag;

	//------可信度赋值-----
	Confidence_Percent = Correct_Par[0];

	
	//-------读取相应的修正参数-----
	Par_Init(Plane_Num, Point, Par_Coe);
	
	
	//------计算距离和高斯分布概率、修正系数-----
	Point[3] = Par_Coe[2] * Confidence_Percent;
	Length_Coe_Cal( Point , Ideal_Point , Return );	
	Length_ToCenter = Return[0];
	P_Gauss_G = Return[1];
	Percent_Gau = Return[2];


	//------更新最优状态---
	StateBest_Check(Plane_Num, Correct_Par, Length_ToCenter);


	//------判断是否需要修正以及相应修正量-----
	if (P_Gauss_G > P_Gauss_Flag)  
	{
		flag = 1;

		Value_Corct[1] = Value_Corct[0];
		Value_Corct[0] = Percent_Gau ;
		
		if ( ( Value_Corct[0] - Value_Corct[1] ) > 0.3 )				//-- 一次限幅
		{
			Value_Corct[0] =( Value_Corct[1] * + Value_Corct[0] * 0.7f );
		}

		if (Value_Corct[0] > Correct_Max)								//-- 二次限幅
		{
			Value_Corct[0] = Correct_Max;
		}
	}
	else
	{
		Value_Corct[1] = Value_Corct[0];
		Value_Corct[0] = 0;
	}

	//------程序测试用---------
	/*cout << "the input point_x is " << Point[0] << "  the input point_y is  " << Point[1] << endl;
	cout << " the Confidence_Percent is  " << Confidence_Percent << endl;*/

	//------更新可信度----------
	if (Length_ToCenter < 35)
	{
		Confidence_Percent = Confidence_Percent - P_Gauss_G  / 2;		//信任度修改 -> 正常情况

		if (Confidence_Percent < 0.03f)									//--信任度限幅
			Confidence_Percent = 0.03f;
	}
	else
	{
		Confidence_Percent = Confidence_Percent + P_Gauss_G *4 / 11;		//--信任度修改->减小信任度
		//( P_Gauss * 10 ) / Length_Bound

		if (Confidence_Percent > 0.97f)									//--信任度限幅
			Confidence_Percent = 0.97f;
	}

	//------更新修正量----------
	if (flag == 0) //防止上面赋0-float出错
	{
		Correct_Par[1] = 0;
		Correct_Par[2] = 0;
		Correct_Par[3] = 0;
		Correct_Par[4] = 0;
		Correct_Par[5] = 0;
	}
	else
	{
		Correct_Par[1] = Confidence_Percent * Percent_Gau * Par_Coe[0];
		Correct_Par[2] = Confidence_Percent * Percent_Gau * Par_Coe[1];
		Correct_Par[3] = Confidence_Percent * Par_Coe[2];
		Correct_Par[4] = Confidence_Percent * Percent_Gau * Par_Coe[3];
		Correct_Par[5] = Confidence_Percent * Percent_Gau * Par_Coe[4];
		/*Correction_Yaw(Plane_Num, Correct_Par, Point);*/
	}

	Check_CorrectPar(Plane_Num, Correct_Par, Point);//---检测是否修正有错误并改到最优

	Correct_Par[0] = Confidence_Percent;
	Correct_Par[6] = Value_Corct[0];
	Correct_Par[7] = Value_Corct[1];
	
	//-----输出值滤波------------
	Fliter_Output(Correct_Par);
}