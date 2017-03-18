#include <chrono>
#include "Task.h"

#include "Protocol.h"
#include <conio.h>

void rc17::Correct()
{
	while(ThreadFlag::run)
	{
		if(ThreadFlag::t_Flag == true)
		{
			bool readyToShoot;
			if (PillarState::hasBall() == true)
			{
				cout << "ball" << endl;
				readyToShoot = PillarState::lockPillar(PillarState::WithBall);
				if (readyToShoot == true)
					Protocol::sendCmd(Protocol::switchToBallPara);
			}
			else
			{

				readyToShoot = PillarState::lockPillar(PillarState::NoBall);

				if (readyToShoot == true)
				{
					Protocol::sendCmd(Protocol::shoot);
					//ThreadFlag::t_Flag = false;//�Դ˽�����������׶�
					//��������
				}
			}
			//Protocol::sendCmd(Protocol::shoot);
		}
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
