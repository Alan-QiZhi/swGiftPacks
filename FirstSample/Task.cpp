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
		if(1)//初始修车姿态状态
		{
			if (PillarState::hasBall() == true)
			{
				cout << "ball" << endl;
				//Protocol::sendCmd(Protocol::switchToBallPara);
				readyToShoot = PillarState::lockPillar(PillarState::WithBall);
			}
			else
			{
				//Protocol::sendCmd(0);
				readyToShoot = PillarState::lockPillar(PillarState::NoBall);
			}
			//if (readyToShoot == true)
			//{
			//	//Protocol::sendCmd(Protocol::correctFinish);
			//	//this_thread::sleep_for(chrono::milliseconds(50));
			//	//ThreadFlag::t_Flag = false;//自此进入正常发射阶段
			//	LastHadBall = PillarState::hasBall();
			//	//修车姿态完成
			//}
		}

		if (readyToShoot == true)//发射状态
		{
			if(PillarState::hasBall() == true)
				Protocol::sendCmd(Protocol::switchToBallPara);
			else
				Protocol::sendCmd(Protocol::switchToNoBallPara);
			this_thread::sleep_for(chrono::milliseconds(50));

			Protocol::sendCmd(Protocol::shoot);
			this_thread::sleep_for(chrono::milliseconds(1500));//等一发飞盘发射完毕
			readyToShoot = false;
			//if (LastHadBall != PillarState::hasBall())
			//{
			//	Protocol::sendCmd(0);
			//	readyToShoot = false;
			//	ThreadFlag::t_Flag == true;
			//}
			continue;
		}
		//500ms 执行一次
		
		this_thread::sleep_for(chrono::milliseconds(300));

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
				datafile << "飞盘相比得到的偏差偏后" << endl;
				cout << "检测到按键w 已向文件写入 飞盘相比得到的偏差偏后！" << endl;
				datafile.close();
			}
			catch (...)
			{
				cout << "文件写入失败， 也许文件被占用.";
			}
		}
		if (key == 'a')
		{	
			try
			{
				std::ofstream datafile;
				datafile.open("C:\\Users\\robocon2017\\Desktop\\datafile.txt", std::ios::app);
				datafile << "飞盘相比得到的偏差偏左" << endl;
				cout << "检测到按键a 已向文件写入 飞盘相比得到的偏差偏左！" << endl;
				datafile.close();
			}
			catch (...)
			{
				cout << "文件写入失败， 也许文件被占用.";
			}
		}
		if (key == 's')
		{
			try
			{
				std::ofstream datafile;
				datafile.open("C:\\Users\\robocon2017\\Desktop\\datafile.txt", std::ios::app);
				datafile << "飞盘相比得到的偏差偏前" << endl;
				cout << "检测到按键s 已向文件写入 飞盘相比得到的偏差偏前！" << endl;
				datafile.close();
			}
			catch (...)
			{
				cout << "文件写入失败， 也许文件被占用.";
			}
		}
		if (key == 'd')
		{
			try
			{
				std::ofstream datafile;
				datafile.open("C:\\Users\\robocon2017\\Desktop\\datafile.txt", std::ios::app);
				datafile << "飞盘相比得到的偏差偏右" << endl;
				cout << "检测到按键d 已向文件写入 飞盘相比得到的偏差偏右！" << endl;
				datafile.close();
			}
			catch (...)
			{
				cout << "文件写入失败， 也许文件被占用.";
			}
		}
	}
}
