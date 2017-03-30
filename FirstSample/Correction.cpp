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
{ 2, -2,  2, -2,   8,  -8,  100,  -100,  300,  -300 },
{ 3, -3,  4, -4,  10, -10,  30,  -30,  210,  -240 },
{ 0, 0, 0, 0,  8, -8  },
{ 0, 0, 0, 0,  8, -8  },
{ 0, 0, 0, 0,  8, -8  },
{ 0, 0, 0, 0,  8, -8  },
};//--范围检查值--

static float Disk_OldState[5] ={0, 0, 0, 0, 0}; //符合条件个数， 上一状态x，上一状态y ,不确定状态x，不确定状态y

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
	{ 7, 60, 1,  0,  },  //台子编号，最小偏差，可信度
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
	Point[2] = Point[2] / 10;
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
	
	if (Length > 8100)							//--为了防止传入的数据过大而炸掉
	{
		Length = 8100;
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
				Speed1[0] = 0;		Speed1[1] = 0;			Speed1[2] = 0;		Speed1[3] = 0;
				Speed2[0] = 0;		Speed2[1] = 0;			Speed2[2] = 0;		Speed2[3] = 0;
				Pitch1[0] = 0;		Pitch1[1] = 0;			Pitch1[2] = 0;		Pitch1[3] = 0;
				Roll1[0] = 0;		Roll1[1] = 0;			Roll1[2] = 0;		Roll1[3] = 0;
				Yaw1[0] = 0;		Yaw1[1] = 0;			Yaw1[2] = 0;		Yaw1[3] = 0;
				break;

		case 2:
				Speed1[0] = 0;		Speed1[1] = 0;			Speed1[2] = 0;		Speed1[3] = 0;
				Speed2[0] = 0;		Speed2[1] = 0;			Speed2[2] = 0;		Speed2[3] = 0;
				Pitch1[0] = 0;		Pitch1[1] = 0;			Pitch1[2] = 0;		Pitch1[3] = 0;
				Roll1[0] = 0;		Roll1[1] = 0;			Roll1[2] = 0;		Roll1[3] = 0;
				Yaw1[0] = 0;		Yaw1[1] = 0;			Yaw1[2] = 0;		Yaw1[3] = 0;

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
	
    //--判断具体方向并选取--方案二 坐标y
    if ((Point[1] >= 0))            //落点在前
    {
        Flag_Direction = 0;
    }
    else                            //落点在后
    {
        Flag_Direction = 1;
    }

	//--给具体参数赋值--
	Par_Coe[0] = Pitch1[Flag_Direction];
	Par_Coe[1] = Roll1[Flag_Direction];
	Par_Coe[2] = static_cast<float>( atan((Point[0] - 10) / Point[2]) * 57.325 * Yaw_Coe);
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

		if (Correct_Par > 5)
		{
			Correct_Par = 5;
		}
		else
			if (Correct_Par < -5)
			{
				Correct_Par = -5;
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
void Check_CorrectPar(int Plane_Num, float Correct_Par[] )
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
			
		}
	}
}



/*3----------最佳状态存储器----------------------3
*
* 说明	： 防止过修正，误修正后的情况无法处理
*		：
*		：

*------------------------------------------*/
int  StateBest_Check(int Plane_Num,float  Correct_Par[],float Length_Now)
{
	//int Good_LengthMin = 30;
	int i;
	
	if (Plane_Num == Correct_BestDelta[0][0])											//--检查是否台子变化
	{
		if (Length_Now < Correct_BestDelta[0][1])		//--更新最优状态
		{
			Correct_Par[0] = Correct_Par[0] - 0.2;
			Correct_BestDelta[0][1] = Length_Now;
			for (i = 0; i < 5; i++)
			{
				Correct_BestDelta[Plane_Num+1][i] = Correct_DeltaAll[Plane_Num][i];
			}
			return 1;
		}
		
		return 0;
	}
	else
	{
		Correct_BestDelta[0][1] = Length_Now;
		return 0;
	}
}




/*3----------盘子状态检查----------------------3
*
* 说明	：	减小毒盘子的影响，对于两个相邻盘子之间的误差进行判断
*		：	Disk_OldState[0] == -1   进入判断时期
*		：	0 毒盘子
			1 正常盘子
*------------------------------------------*/
int  Disk_Check(int Plane_Num, float  Correct_Par[], float Point[])
{
	//int Good_LengthMin = 30;
	int i;
	int Distance_Safe = 75;
	double Distance_Between;
	double Distance_Between_1;

	Distance_Between = (Point[0] - Disk_OldState[1]) *  (Point[0] - Disk_OldState[1]) + (Point[1] - Disk_OldState[2]) *  (Point[1] - Disk_OldState[2]);
	Distance_Between = sqrt(Distance_Between);
		
		
	if (Plane_Num == Correct_BestDelta[0][0])				//--检查是否台子变化--
	{
		if (Disk_OldState[0] == -1)							//--当前为判断状态
		{
			Distance_Between_1 = (Point[0] - Disk_OldState[3]) *  (Point[0] - Disk_OldState[3]) + (Point[1] - Disk_OldState[4]) *  (Point[1] - Disk_OldState[4]);
			Distance_Between_1 = sqrt(Distance_Between_1);
			
			if ((Distance_Between_1 < 50) || (Distance_Between < 50))
			{
				Disk_OldState[1] = Point[0];
				Disk_OldState[2] = Point[1];
				
				//--更新标志位--
				Disk_OldState[0] = 1;
				return 1;
			}
			else
				return 0;
		}
		
		if (Distance_Between < Distance_Safe)				//--是正常盘子--
		{
				Disk_OldState[0] = Disk_OldState[0] + 1;
				Disk_OldState[1] = Point[0];
				Disk_OldState[2] = Point[1];

				return 1;
		}
		else
		{
			if (Disk_OldState[0] > 2)						//--已经有正常盘子--
				return 0;
			else
			{
				Disk_OldState[0] = -1;						//--没有已经确认的正常盘子--
				Disk_OldState[3] = Point[0];
				Disk_OldState[4] = Point[1];
			}
		}
	}
	else
	{
		if (Distance_Between < 80 )							//--避免第一个盘子炸机--
		{
		Disk_OldState[1] = Point[0];
		Disk_OldState[2] = Point[1];
		Disk_OldState[0] = 0;
		Correct_BestDelta[0][0] = Plane_Num;
		
		return 1;
		}
			
		Disk_OldState[0] = 0;
		Correct_BestDelta[0][0] = Plane_Num;
		
		return 0;
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
	const float P_Gauss_Flag = 0.18f;					//--对应30cm
	const float Correct_Max = 0.9f;
	float Percent_Gau;
	const float Ideal_Point[2] = { 0, 0 };
	float Value_Corct[2] = { Correct_Par[6], Correct_Par[7] };
	float Return[3] = { 0, 0, 0 };
	float Par_Coe[5] = { 0, 0, 0, 0, 0 };
	int Flag_Change = 0;
	int Flag_BestState = 0;
	int Flag_DiskState = 0;
	//------盘子优劣检查---
	Flag_DiskState = Disk_Check(Plane_Num, Correct_Par, Point);

	if (Flag_DiskState == 1)
	{
		//------可信度赋值-----
		Confidence_Percent = Correct_Par[0];
		
		
		//-------读取相应的修正参数-----
		Par_Init(Plane_Num, Point, Par_Coe);


		//------计算距离和高斯分布概率、修正系数-----
		Point[3] = Par_Coe[2] * Confidence_Percent;
		Length_Coe_Cal(Point, Ideal_Point, Return);

		Length_ToCenter = Return[0];
		P_Gauss_G = Return[1];
		Percent_Gau = Return[2];


		//------更新最优状态---
		Flag_BestState = StateBest_Check(Plane_Num, Correct_Par, Length_ToCenter);


		//------判断是否需要修正以及相应修正量-----
		if (P_Gauss_G > P_Gauss_Flag)
		{
			Flag_Change = 1;

			Value_Corct[1] = Value_Corct[0];
			Value_Corct[0] = Percent_Gau;


			Value_Corct[0] = (Value_Corct[1] * 0.2f + Value_Corct[0] * 0.8f);


			if (Value_Corct[0] > Correct_Max)                               //-- 二次限幅
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

		if ((Flag_BestState == 1) && (Length_ToCenter < 40))
		{
			Confidence_Percent = Confidence_Percent - 0.2; 					//奖励最优状态的更新	
		}

		if (Length_ToCenter < 35)
		{
			Confidence_Percent = Confidence_Percent - P_Gauss_G * 1.5;      //信任度修改 -> 正常情况

			if (Confidence_Percent < 0.03f)                                 //--信任度限幅
				Confidence_Percent = 0.03f;
		}
		else
		{
			Confidence_Percent = Confidence_Percent + P_Gauss_G * 4 / 11;       //--信任度修改->减小信任度
			//( P_Gauss * 10 ) / Length_Bound

			if (Confidence_Percent > 0.97f)                                 //--信任度限幅
				Confidence_Percent = 0.97f;
		}
	}
	else
	{
		Flag_Change = 0;
	}

	//------更新修正量----------
	if (Flag_Change == 0) //防止上面赋0-float出错
	{
		if (Flag_DiskState == 1)
			Correct_Par[0] = Confidence_Percent;
		Correct_Par[1] = 0;
		Correct_Par[2] = 0;
		Correct_Par[3] = 0;
		Correct_Par[4] = 0;
		Correct_Par[5] = 0;
	}
	else
	{
		Correct_Par[0] = Confidence_Percent;
		Correct_Par[1] = Confidence_Percent * Percent_Gau * Par_Coe[0];
		Correct_Par[2] = Confidence_Percent * Percent_Gau * Par_Coe[1];
		Correct_Par[3] = Confidence_Percent * Par_Coe[2];
		Correct_Par[4] = Confidence_Percent * Percent_Gau * Par_Coe[3];
		Correct_Par[5] = Confidence_Percent * Percent_Gau * Par_Coe[4];
		/*Correction_Yaw(Plane_Num, Correct_Par, Point);*/
	}

	Check_CorrectPar(Plane_Num, Correct_Par);//---检测是否修正有错误并改到最优

	Correct_Par[6] = Value_Corct[0];
	Correct_Par[7] = Value_Corct[1];

	Fliter_Output(Correct_Par);
}
		
		
		