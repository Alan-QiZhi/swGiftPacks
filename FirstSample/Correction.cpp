#include"correction.h"
#include "GobalVariables.h"
/*
*		File�� Point_Correct.cpp
*
*		Ŀ¼��	1����ʼ����ֵ
*				2�����㼰��ֵ����
*				3���˲����Լ캯��
*				4��������-�ӿں���
*	�������ݣ�	���ڿ�ʼ���������жϣ�ͨ���鿴��̬�ֲ�ͼ������һ�����ʵ��ٽ�ֵ������ͼ�С�����бȽϺõı仯����ʱ��Ϊ��ʼ�仯��ֵ���ٵ�������ϵ���Ϳ��Ŷȹ�ϵ
*   ע		��	1����������ֵ������Ϊ�˱���һ�������������������
*				2������ֵ�ĵ�λΪ cm
*
*	��Ҫ������	Coff_Correct		�Ŵ��������ı���ϵ��
*				Ideal_Point			��ʼ�������
*				Confidence_Percent	���ڼ���Ŀ��Ŷȣ����������෴�����ڼ��㣬���Ŷ�����Ϊ 1 - ��ʵ���Ŷ�
*				Correct_Max			����ֵ�޷�
*	
*	3-15 ��ƫ������������ʵʱ���㹫ʽ����������ͷ���ز������㣬������Ŷ�ѭ������
*/

/*1----------��ʼ������ֵ-----------------1
*
*------------------------------------------*/

static float Max_Value[7][10] = { { 0, 0, 0, 0, 8, -8 },
{ 2, -2,  2, -2,   8,  -8,  100,  -100,  220,  -220 },
{ 3, -3,  4, -4,  10, -10,  30,  -30,  210,  -240 },
{ 0, 0, 0, 0,  8, -8  },
{ 0, 0, 0, 0,  8, -8  },
{ 0, 0, 0, 0,  8, -8  },
{ 0, 0, 0, 0,  8, -8  },
};//--��Χ���ֵ--

static float Correct_DeltaAll[7][5] = {
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
};//--����deltaֵ�ܺ�--

static float Correct_BestDelta[8][5] = { 
	{ 7, 200, 1,  0,  },  //̨�ӱ�ţ���Сƫ����Ŷ�
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
	{ 0, 0, 0, 0, 0, },
};//--���״̬�洢ֵ--



/*2----------�����˹�ֲ�����-----------------2
*
* ˵��	���ı���matlab����
*		��
*------------------------------------------*/
float Gauss(float x, int mu, int sigma)
{
	float y;
	const float pi = 3.1416;

	y = exp(-0.5 * ((x - mu) / sigma) * ((x - mu) / sigma)) / (sqrt(2 * pi) * sigma);

	return y;
}



//--������������С--
void rc17::Correction::Correct_Point(double Correct_Par[], double Point[])
{
	int Coff_Correct = 6;
	int flag = 0;
	double Confidence_Percent;
	double Length;
	double P_Gauss;
	const double Ideal_Point[2] = { 0, 0 };
	double Value_Corct[3] = { Correct_Par[3], Correct_Par[4], Correct_Par[5] };
	double Correct_Max = 38;						//---Ϊ40cm����Ϳ��Ŷ�ʱ��������

												//------����ֵ����-----
	Confidence_Percent = Correct_Par[0];
	Point[0] = Point[0] / 20;					//---��������
	Point[1] = Point[1] / 20;

	//------�������͸�˹�ֲ�����-----
	Length = (Point[0] - Ideal_Point[0]) * (Point[0] - Ideal_Point[0]) + (Point[1] - Ideal_Point[1]) * (Point[1] - Ideal_Point[1]);
	Length = sqrt(Length);
	P_Gauss = Gauss(Length, 0, 1);
	Coff_Correct += (int)Length * 20;

	//------�ж��Ƿ���Ҫ�����Լ���Ӧ������-----
	if (P_Gauss < 0.19)  //--��Ӧ25cm
	{
		flag = 1;
		Value_Corct[2] = Value_Corct[1];
		Value_Corct[1] = Value_Corct[0];
		Value_Corct[0] = (1 - P_Gauss) * Confidence_Percent *  Coff_Correct;


		if ((Value_Corct[0] >(Value_Corct[1] + Value_Corct[2])) && (Value_Corct[1] != 0) && (Value_Corct[2] != 0))	//-- һ���޷�
		{
			Value_Corct[0] = Value_Corct[1] + Value_Corct[2];
		}

		if (Value_Corct[0] > Correct_Max)																				//-- �����޷�
		{
			Value_Corct[0] = Correct_Max;
		}

		Confidence_Percent = Confidence_Percent + (1 - P_Gauss) * Length / 35;  //--���ζ��޸�->��С���ζ�

		if (Confidence_Percent > 0.97)											//--���ζ��޷�
			Confidence_Percent = 0.97;
	}
	else
	{
		Value_Corct[2] = Value_Corct[1];
		Value_Corct[1] = Value_Corct[0];
		Value_Corct[0] = 0;
		Confidence_Percent = Confidence_Percent - P_Gauss / 4;      //���ζ��޸� -> �������

		if (Confidence_Percent < 0.03)								//--���ζ��޷�
			Confidence_Percent = 0.03;
	}

	//------���������---------
	/*cout << "the input point_x is " << Point[0] << "  the input point_y is  " << Point[1] << endl;
	cout << " the Confidence_Percent is  " << Confidence_Percent << endl;*/

	//------���¿��ŶȺ�������----------
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
*		File��	Yaw_Correct.cpp
*
*	�������ݣ�	ͨ�����Է����䵽��̨�ϵ��������������Сƫ��������Ϊ��̨���ݲ�������Ƚ��ȶ������ŶȽϸ�
*   ע		��	������������ýǶ�Ϊͳһ���ݵ�λ
*				��Ҫ�޸�ƽ̨��ţ�Plat_Num ������ֵ��ȶ
*
*	��Ҫ������
*				Coe_Correct		��������
*				Length			̨�ӵ������ľ��룬��ȷһ��ȽϺã�ˮƽ���룬����Ҫ����z��Ӱ��,cm����
*				Coe_Error		ʵ�ʲ��������������µ�ˮƽ���ƫ�����Сƫ��ı���
*				Radius_Max		���ڷ�ֹ����y����ƫ�ƹ���
*`				Coe_Judge_Model	������������������ϵ����֮ǰ�����Ƚ�x y��СЧ������̫��
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

/*----------����ƫ����������-----------------
*
* ˵��	��	Par_Input[0]	��ǰ����ˮƽ���	cm
*			Par_Input[1]	ǰ�����
*			Par_Input[2]	ƽ�����			cm
*			Par_Input[3]	�ۻ�����ֵ			�Ƕ�
*			Par_Input[4]	��ǰ���ƫ��ֵ		�Ƕ�
*
*			Par_Output[0]	��ǰ����ֵ			�Ƕ�
*			Par_Output[3]	ƽ�����			cm
*			Par_Output[1]	�ۻ�����ֵ			�Ƕ�
*			Par_Output[2]	��ǰ���ƫ��ֵ		�Ƕ�
*		��
*------------------------------------------*/
void rc17::Correction::Yaw_Correct_My(double Length, double Coe_Correct, double Par_Input[], double Par_Output[])
{
	double Correct_Yaw;

	//--�Ƿ��һ�ν���--
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

	//--�ۻ�����--
	Par_Output[0] = Coe_Correct * Correct_Yaw;
	Par_Output[1] = Par_Output[1] + Par_Output[0];

	//--�������Լ�--
	if (abs(Par_Output[1]) > abs(Par_Output[2]) * Coe_Error)
	{
		Par_Output[1] = Par_Output[1] - Par_Output[0];
		Par_Output[0] = 0;
	}
}



/*----------����а�����---------------------
*
* ˵��	��
*		��
*------------------------------------------*/
void rc17::Correction::Yaw_Correct(int Plat_Num, double Par_Input[], double Par_Output[])
{
	switch (Plat_Num)
	{
	case 1:
		rc17::Correction::Yaw_Correct_My(Length_2m, Coe_Correct_2m, Par_Input, Par_Output);	//��̨
		break;
	case 4:
		rc17::Correction::Yaw_Correct_My(Length_5m, Coe_Correct_5m, Par_Input, Par_Output);	//��̨
		break;
	default:
		break;
	}
}

/*----------�ж�ʹ��������������-------------
*
* ˵��	��
*		��Plat_Num		ƽ̨���
*		��Point_Hit		�䵽̨���ϵ�λ������ [0] x [1] y
*		��Flag			0 �������� 1 Yawȫ������  2 ��ͨ��������

*------------------------------------------*/
int rc17::Correction::Judge_Method(int Plat_Num, double Point_Hit[])
{
	int Flag = 0;
	double Radius = 0;

	//--��㵽Բ�İ뾶--
	Radius = sqrt(Point_Hit[0] * Point_Hit[0] + Point_Hit[1] * Point_Hit[1]);

	//--��̨--
	if (Plat_Num == 1)
	{
		//--������������--
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
		//--���̵���̨��--
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
	//--��̨--
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
			//�ϴ�û�ô˷���ʱ,��ʼ��������������
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
			//�ϴ�û�ô˷���ʱ,��ʼ��������������
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








/*----------���㳤�Ⱥ͸�˹�����Լ�����ϵ��--
*
* ˵��	��Length_To_Center����װ�õ��������ĵľ���
*		��
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
	
	//-------������֪��ƫ��������ֵ����xyУ׼-----
	if (Point[3] > 0)
	{
			Point[0] = Point[0] - Length_To_Center * ( 1 - cos(Point[3]/57.23) );
	}else{	Point[0] = Point[0] + Length_To_Center * ( 1 - cos(Point[3]/57.23) ); }
	
	Point[1] = Point[1] + Length_To_Center * sin(Point[3]/57.23);
	
	Length = (Point[0] - Ideal_Point[0]) * (Point[0] - Ideal_Point[0]) + (Point[1] - Ideal_Point[1]) * (Point[1] - Ideal_Point[1]);
	
	/*Length = ( 25 + i ) * ( 25 + i);--for test*/
	
	if (Length > 6400)							//--Ϊ�˷�ֹ��������ݹ����ը��
	{
		Length = 6400;
	}

	Length_Bound = (Bound - sqrt(Length)) / 40;	//--Ϊ�˸�˹��������
	
	P_Gauss_G = static_cast<float>(Gauss(Length_Bound, 0, 1));

	Possibility_Gau = P_Gauss_G / Gauss_Max;

	Return[0] = sqrt(Length);					
	Return[1] = P_Gauss_G;
	Return[2] = Possibility_Gau;
}


/*----------��ʼ����Ӧƽ̨�Ĳ���------------
*
* ˵��	�� 0 ���̨   1 �м�̨    2 ����м�̨
*		��   3 ����̨     4 ��Զ̨
*		�� �ڴ�����- 0 1 2 3  ǰ������
* ����ֵ����������ǰ����������
* slope	���˴�б����45�Ȼ��ֿռ�
* Yaw_Coe�� 1.5��֤��ȥ�ĳ��ȱ��޻��������Ƕ��ھ�����ݲ��Բ�̫�ã���Ϊ1.3,��֤���Ŷ�0.5���Ҳ���ը
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

	//--����б��--
	if (Point[0] != 0)
	{
		Slope_Now = Point[1] / Point[0];
	}
	else
	{
		Slope_Now = Point[1];
	}
	
	//--ѡȡ����̨��--
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

		case 3:   //ǰ                   ��                     ��              ��
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
	
	//--�жϾ��巽��ѡȡ--����һ����б��
	if ((Point[0] >= 0) && (Slope_Now >= Slope[0]) && (Slope_Now <= Slope[1]) )			//�������
	{
		Flag_Direction = 3;
	}
	else if ((Point[0] <= 0) && (Slope_Now >= Slope[0]) && (Slope_Now <= Slope[1]))		//�������
	{	
		Flag_Direction = 2;
	}
	else if (((Point[1] >= Limit_Y) && (Slope_Now <= Slope[0])) || ((Point[1] >= Limit_Y) && (Slope_Now >= Slope[1])))	//�����ǰ
	{
			Flag_Direction = 0;
	}
	else if (((Point[1] <= -Limit_Y) && (Slope_Now >= Slope[1])) || ((Point[1] <= -Limit_Y) && (Slope_Now <= Slope[0])))	//����ں�
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

	//--�����������ֵ--
	Par_Coe[0] = Pitch1[Flag_Direction];
	Par_Coe[1] = Roll1[Flag_Direction];
	Par_Coe[2] = static_cast<float>(atan(Point[0] / Point[2])) * 57.325 * Yaw_Coe + Yaw1[Flag_Direction];
	Par_Coe[3] = Speed1[Flag_Direction];
	Par_Coe[4] = Speed2[Flag_Direction];
	
}


/*----------���ֵ�˲�---------------------3
*
* ˵��	�� ��ֹ���ֵ̫�󣬵��¹�����
*		��
*		��

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



/*----------���ֵ�˲�-------------------3
*
* ˵��	�� ��ֹ���ֵ̫�󣬵��¹�����
*		��
*		��

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



/*3----------���ֵ�Լ�----------------------3
*
* ˵��	�� �����ֵ����Χʱ����ʱ����
*		������Ϊ0
*		������Ϊ1
		��Confidence_Limit ���Ŷȵģ���һ��Ӧ�ã����ж��Ƿ���Ҫ��������ֵ
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
			if (Correct_Par[0] > Confidence_Limit)		//--���ζ���ը--					
			{
				Flag = 1;								//--��������������־--
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
		
		//-------������ֵ�����жϣ��Ƿ����Ҫ��---------
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
			//-----����ֵ��գ�����������գ��ص�֮ǰ�ȽϺõ�һ��״̬-------
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
			
			//-----ֱ�Ӳٿظ����ǽ��б仯-------
			if ( Ponit[1] > 0 )
				Correct_Par[1] = Correct_Par[1] - 2 ;
			else
				Correct_Par[1] = Correct_Par[1] + 2 ;
		}
	}
}



/*3----------���״̬�洢��----------------------3
*
* ˵��	�� ��ֹ���������������������޷�����
*		��
*		��

*------------------------------------------*/
void StateBest_Check(int Plane_Num,float  Correct_Par[],float Length_Now)
{
	//int Good_LengthMin = 30;
	int i;
	
	if (Plane_Num == Correct_BestDelta[0][0])											//--����Ƿ�̨�ӱ仯
	{
		if (Length_Now < Correct_BestDelta[0][1])		//--��������״̬
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




/*----------����ƫ����������С------------��ʱ����
* Turtle
* ˵��	�� ����û�в��Գ�������ƫ����������Ϊ���㷽��
*		��
*------------------------------------------*/
//void Correction_Yaw(int Plane_Num, float Correct_Par[], float Point[])
//{
//	Correct_Par[3] = static_cast<float>(atan(Point[0] / Point[2])) * 57.325;
//}




/*----------������������С------------main����
* Turtle
* ˵��	��  Correct_Max			����ֵ�޷�������ֵ�İٷֱ� 0.1-1 ��0.8����80%��
*		��
*------------------------------------------*/
void rc17::Correction::Correct_Point(int Plane_Num, float Correct_Par[], float Point[])
{
	int Coff_Correct = 6;
	float Confidence_Percent;
	float Length_ToCenter;
	float P_Gauss_G;
	const float P_Gauss_Flag= 0.18f;					//--��Ӧ30cm
	const float Correct_Max = 0.9f;
	float Percent_Gau;
	const float Ideal_Point[2] = { 0, 0 };
	float Value_Corct[2] = { Correct_Par[6], Correct_Par[7] };
	float Return[3] = { 0, 0, 0 };
	float Par_Coe[5] = { 0, 0, 0, 0 ,0 };
	int flag;

	//------���Ŷȸ�ֵ-----
	Confidence_Percent = Correct_Par[0];

	
	//-------��ȡ��Ӧ����������-----
	Par_Init(Plane_Num, Point, Par_Coe);
	
	
	//------�������͸�˹�ֲ����ʡ�����ϵ��-----
	Point[3] = Par_Coe[2] * Confidence_Percent;
	Length_Coe_Cal( Point , Ideal_Point , Return );	
	Length_ToCenter = Return[0];
	P_Gauss_G = Return[1];
	Percent_Gau = Return[2];


	//------��������״̬---
	StateBest_Check(Plane_Num, Correct_Par, Length_ToCenter);


	//------�ж��Ƿ���Ҫ�����Լ���Ӧ������-----
	if (P_Gauss_G > P_Gauss_Flag)  
	{
		flag = 1;

		Value_Corct[1] = Value_Corct[0];
		Value_Corct[0] = Percent_Gau ;
		
		if ( ( Value_Corct[0] - Value_Corct[1] ) > 0.3 )				//-- һ���޷�
		{
			Value_Corct[0] =( Value_Corct[1] * + Value_Corct[0] * 0.7f );
		}

		if (Value_Corct[0] > Correct_Max)								//-- �����޷�
		{
			Value_Corct[0] = Correct_Max;
		}
	}
	else
	{
		Value_Corct[1] = Value_Corct[0];
		Value_Corct[0] = 0;
	}

	//------���������---------
	/*cout << "the input point_x is " << Point[0] << "  the input point_y is  " << Point[1] << endl;
	cout << " the Confidence_Percent is  " << Confidence_Percent << endl;*/

	//------���¿��Ŷ�----------
	if (Length_ToCenter < 35)
	{
		Confidence_Percent = Confidence_Percent - P_Gauss_G  / 2;		//���ζ��޸� -> �������

		if (Confidence_Percent < 0.03f)									//--���ζ��޷�
			Confidence_Percent = 0.03f;
	}
	else
	{
		Confidence_Percent = Confidence_Percent + P_Gauss_G *4 / 11;		//--���ζ��޸�->��С���ζ�
		//( P_Gauss * 10 ) / Length_Bound

		if (Confidence_Percent > 0.97f)									//--���ζ��޷�
			Confidence_Percent = 0.97f;
	}

	//------����������----------
	if (flag == 0) //��ֹ���渳0-float����
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

	Check_CorrectPar(Plane_Num, Correct_Par, Point);//---����Ƿ������д��󲢸ĵ�����

	Correct_Par[0] = Confidence_Percent;
	Correct_Par[6] = Value_Corct[0];
	Correct_Par[7] = Value_Corct[1];
	
	//-----���ֵ�˲�------------
	Fliter_Output(Correct_Par);
}