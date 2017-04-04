#include "timeorder.h"
#include "Protocol.h"
using namespace rc17;
//0右 1左     ↓
int otherLine[2][7]
{ { 1, 2, 6, -1 },{ 0, 3, 4, 5, -1 } };//遍历另一条线飞盘打上去的状况
int AShootOrder[2][7]
{ { 0, 3, 4, 6, -1 },{ 1, 2, 3, 6, -1 } };//打的台号顺序，-1表示已经打完自己在这条线的所有台
int BShootOrder[2][7]
{ { 5, 4, 6, -1 },{ 0, 3, 2, 6, -1 } };

void rc17t::timeorder()
{	
	whereiscar lastCar = whereiscar::carAtRight;
	while (ThreadFlag::run)
	{
		if (ThreadFlag::t_Num > 0)
		{
/*切换台柱*/
			if (PillarVar::hasFrisbee[PillarVar::AshootingIndex] == 1 || //正在打的打上去了 或
				PillarVar::AshootingIndex == -1)//A  没在打，切换台号
			{

				PillarVar::AshootingIndex = checkPillarOrder(AShootOrder[rc17t::carAt()], PillarVar::hasFrisbee);//要打的台号
				if (PillarVar::AshootingIndex != -1)
					cout << "A 切换到" << PillarVar::AshootingIndex << endl;
				;//判断有无球，然后告诉底盘
				PillarVar::index = PillarVar::AshootingIndex;
			}
			if (PillarVar::hasFrisbee[PillarVar::BshootingIndex] == 1 || //正在打的打上去了 或
				PillarVar::BshootingIndex == -1)//B  没在打，切换台号
			{

				PillarVar::BshootingIndex = checkPillarOrder(BShootOrder[rc17t::carAt()], PillarVar::hasFrisbee);//要打的台号
				if (PillarVar::BshootingIndex != -1)
					cout << "B 切换到" << PillarVar::BshootingIndex << endl;
				;//判断有无球，然后告诉底盘
			}

/*切换台柱*/
/*切换底盘位置*/
			if (PillarVar::AshootingIndex == -1 && PillarVar::BshootingIndex == -1 && //如果没台子打了 且
				checkPillarOrder(otherLine[rc17t::carAt()], PillarVar::hasFrisbee) != -1)//另一条线上有没打上的
			{
				cout << "该换场咯" << endl;
				Sleep(1000);//至少等底盘发0
			}
/*切换底盘位置*/
		}
		Sleep(30);
		if (rc17t::carAt() != lastCar)
		{
			PillarVar::AshootingIndex = checkPillarOrder(AShootOrder[rc17t::carAt()], PillarVar::hasFrisbee);
			PillarVar::BshootingIndex = checkPillarOrder(BShootOrder[rc17t::carAt()], PillarVar::hasFrisbee);
			lastCar = rc17t::carAt();
		}
	}
}
rc17t::whereiscar rc17t::carAt()
{
	return CameraVar::cameraParam.worldX < 7000 ? carAtLeft : carAtRight;
}

//搜索要打的台号
int rc17t::checkPillarOrder(int order[], bool pillarState[7])
{
	int i = -1;
	while (pillarState[order[++i]])
		if (order[i] == -1)
			break;
	return order[i];
}

