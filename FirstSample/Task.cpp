#include <chrono>
#include "Task.h"

#include "Protocol.h"
#include <conio.h>

void rc17::Correct()
{
	bool readyToShoot = true;
	bool LastHadBall = false;
	while(ThreadFlag::run)
	{
		if(ThreadFlag::t_Flag == true)
		{
			if (PillarState::hasBall() == true)
			{
				cout << "ball" << endl;
				readyToShoot = PillarState::lockPillar(PillarState::WithBall);
			}
			else
			{
				readyToShoot = PillarState::lockPillar(PillarState::NoBall);
			}

			if (readyToShoot == true)//����״̬
			{
				//if (PillarState::hasBall() == true)
				//	Protocol::sendCmd(Protocol::BallPara);
				//else
				//	Protocol::sendCmd(Protocol::NoBallPara);
				//this_thread::sleep_for(chrono::milliseconds(50));

				Protocol::sendCmd(Protocol::Shoot);
				this_thread::sleep_for(chrono::milliseconds(1500));//��һ�����̷������
				//if (LastHadBall != PillarState::hasBall())
				//{
				//	Protocol::sendCmd(0);
				//	readyToShoot = false;
				//	ThreadFlag::t_Flag == true;
				//}
				continue;
			}
		}
		//cout << ThreadFlag::t_Flag << endl;

		//500ms ִ��һ��
		
		this_thread::sleep_for(chrono::milliseconds(500));

	}
}

void rc17::scanfKey()
{
	while (rc17::ThreadFlag::run == 0);
	while (rc17::ThreadFlag::run)
	{
		int key = getch();
		if (key == 'w')
		{
			try
			{
				std::ofstream datafile;
				datafile.open("C:\\Users\\robocon2017\\Desktop\\datafile.txt", std::ios::app);
				datafile << "������ȵõ���ƫ��ƫ��" << endl;
				cout << "��⵽����w �����ļ�д�� ������ȵõ���ƫ��ƫ��" << endl;
				datafile.close();
			}
			catch (...)
			{
				cout << "�ļ�д��ʧ�ܣ� Ҳ���ļ���ռ��.";
			}
		}
		if (key == 'a')
		{	
			try
			{
				std::ofstream datafile;
				datafile.open("C:\\Users\\robocon2017\\Desktop\\datafile.txt", std::ios::app);
				datafile << "������ȵõ���ƫ��ƫ��" << endl;
				cout << "��⵽����a �����ļ�д�� ������ȵõ���ƫ��ƫ��" << endl;
				datafile.close();
			}
			catch (...)
			{
				cout << "�ļ�д��ʧ�ܣ� Ҳ���ļ���ռ��.";
			}
		}
		if (key == 's')
		{
			try
			{
				std::ofstream datafile;
				datafile.open("C:\\Users\\robocon2017\\Desktop\\datafile.txt", std::ios::app);
				datafile << "������ȵõ���ƫ��ƫǰ" << endl;
				cout << "��⵽����s �����ļ�д�� ������ȵõ���ƫ��ƫǰ��" << endl;
				datafile.close();
			}
			catch (...)
			{
				cout << "�ļ�д��ʧ�ܣ� Ҳ���ļ���ռ��.";
			}
		}
		if (key == 'd')
		{
			try
			{
				std::ofstream datafile;
				datafile.open("C:\\Users\\robocon2017\\Desktop\\datafile.txt", std::ios::app);
				datafile << "������ȵõ���ƫ��ƫ��" << endl;
				cout << "��⵽����d �����ļ�д�� ������ȵõ���ƫ��ƫ�ң�" << endl;
				datafile.close();
			}
			catch (...)
			{
				cout << "�ļ�д��ʧ�ܣ� Ҳ���ļ���ռ��.";
			}
		}
	}
}
