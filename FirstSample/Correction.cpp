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
{ 2, -2,  2, -2,   8,  -8,  100,  -100,  300,  -300 },
{ 3, -3,  4, -4,  10, -10,  30,  -30,  210,  -240 },
{ 0, 0, 0, 0,  8, -8  },
{ 0, 0, 0, 0,  8, -8  },
{ 0, 0, 0, 0,  8, -8  },
{ 0, 0, 0, 0,  8, -8  },
};//--��Χ���ֵ--

static float Disk_OldState[5] ={0, 0, 0, 0, 0}; //�������������� ��һ״̬x����һ״̬y ,��ȷ��״̬x����ȷ��״̬y

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
	{ 7, 60, 1,  0,  },  //̨�ӱ�ţ���Сƫ����Ŷ�
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
	
	if (Length > 8100)							//--Ϊ�˷�ֹ��������ݹ����ը��
	{
		Length = 8100;
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
	
    //--�жϾ��巽��ѡȡ--������ ����y
    if ((Point[1] >= 0))            //�����ǰ
    {
        Flag_Direction = 0;
    }
    else                            //����ں�
    {
        Flag_Direction = 1;
    }

	//--�����������ֵ--
	Par_Coe[0] = Pitch1[Flag_Direction];
	Par_Coe[1] = Roll1[Flag_Direction];
	Par_Coe[2] = static_cast<float>( atan((Point[0] - 10) / Point[2]) * 57.325 * Yaw_Coe);
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
			
		}
	}
}



/*3----------���״̬�洢��----------------------3
*
* ˵��	�� ��ֹ���������������������޷�����
*		��
*		��

*------------------------------------------*/
int  StateBest_Check(int Plane_Num,float  Correct_Par[],float Length_Now)
{
	//int Good_LengthMin = 30;
	int i;
	
	if (Plane_Num == Correct_BestDelta[0][0])											//--����Ƿ�̨�ӱ仯
	{
		if (Length_Now < Correct_BestDelta[0][1])		//--��������״̬
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




/*3----------����״̬���----------------------3
*
* ˵��	��	��С�����ӵ�Ӱ�죬����������������֮����������ж�
*		��	Disk_OldState[0] == -1   �����ж�ʱ��
*		��	0 ������
			1 ��������
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
		
		
	if (Plane_Num == Correct_BestDelta[0][0])				//--����Ƿ�̨�ӱ仯--
	{
		if (Disk_OldState[0] == -1)							//--��ǰΪ�ж�״̬
		{
			Distance_Between_1 = (Point[0] - Disk_OldState[3]) *  (Point[0] - Disk_OldState[3]) + (Point[1] - Disk_OldState[4]) *  (Point[1] - Disk_OldState[4]);
			Distance_Between_1 = sqrt(Distance_Between_1);
			
			if ((Distance_Between_1 < 50) || (Distance_Between < 50))
			{
				Disk_OldState[1] = Point[0];
				Disk_OldState[2] = Point[1];
				
				//--���±�־λ--
				Disk_OldState[0] = 1;
				return 1;
			}
			else
				return 0;
		}
		
		if (Distance_Between < Distance_Safe)				//--����������--
		{
				Disk_OldState[0] = Disk_OldState[0] + 1;
				Disk_OldState[1] = Point[0];
				Disk_OldState[2] = Point[1];

				return 1;
		}
		else
		{
			if (Disk_OldState[0] > 2)						//--�Ѿ�����������--
				return 0;
			else
			{
				Disk_OldState[0] = -1;						//--û���Ѿ�ȷ�ϵ���������--
				Disk_OldState[3] = Point[0];
				Disk_OldState[4] = Point[1];
			}
		}
	}
	else
	{
		if (Distance_Between < 80 )							//--�����һ������ը��--
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
	const float P_Gauss_Flag = 0.18f;					//--��Ӧ30cm
	const float Correct_Max = 0.9f;
	float Percent_Gau;
	const float Ideal_Point[2] = { 0, 0 };
	float Value_Corct[2] = { Correct_Par[6], Correct_Par[7] };
	float Return[3] = { 0, 0, 0 };
	float Par_Coe[5] = { 0, 0, 0, 0, 0 };
	int Flag_Change = 0;
	int Flag_BestState = 0;
	int Flag_DiskState = 0;
	//------�������Ӽ��---
	Flag_DiskState = Disk_Check(Plane_Num, Correct_Par, Point);

	if (Flag_DiskState == 1)
	{
		//------���Ŷȸ�ֵ-----
		Confidence_Percent = Correct_Par[0];
		
		
		//-------��ȡ��Ӧ����������-----
		Par_Init(Plane_Num, Point, Par_Coe);


		//------�������͸�˹�ֲ����ʡ�����ϵ��-----
		Point[3] = Par_Coe[2] * Confidence_Percent;
		Length_Coe_Cal(Point, Ideal_Point, Return);

		Length_ToCenter = Return[0];
		P_Gauss_G = Return[1];
		Percent_Gau = Return[2];


		//------��������״̬---
		Flag_BestState = StateBest_Check(Plane_Num, Correct_Par, Length_ToCenter);


		//------�ж��Ƿ���Ҫ�����Լ���Ӧ������-----
		if (P_Gauss_G > P_Gauss_Flag)
		{
			Flag_Change = 1;

			Value_Corct[1] = Value_Corct[0];
			Value_Corct[0] = Percent_Gau;


			Value_Corct[0] = (Value_Corct[1] * 0.2f + Value_Corct[0] * 0.8f);


			if (Value_Corct[0] > Correct_Max)                               //-- �����޷�
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

		if ((Flag_BestState == 1) && (Length_ToCenter < 40))
		{
			Confidence_Percent = Confidence_Percent - 0.2; 					//��������״̬�ĸ���	
		}

		if (Length_ToCenter < 35)
		{
			Confidence_Percent = Confidence_Percent - P_Gauss_G * 1.5;      //���ζ��޸� -> �������

			if (Confidence_Percent < 0.03f)                                 //--���ζ��޷�
				Confidence_Percent = 0.03f;
		}
		else
		{
			Confidence_Percent = Confidence_Percent + P_Gauss_G * 4 / 11;       //--���ζ��޸�->��С���ζ�
			//( P_Gauss * 10 ) / Length_Bound

			if (Confidence_Percent > 0.97f)                                 //--���ζ��޷�
				Confidence_Percent = 0.97f;
		}
	}
	else
	{
		Flag_Change = 0;
	}

	//------����������----------
	if (Flag_Change == 0) //��ֹ���渳0-float����
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

	Check_CorrectPar(Plane_Num, Correct_Par);//---����Ƿ������д��󲢸ĵ�����

	Correct_Par[6] = Value_Corct[0];
	Correct_Par[7] = Value_Corct[1];

	Fliter_Output(Correct_Par);
}
		
		
		