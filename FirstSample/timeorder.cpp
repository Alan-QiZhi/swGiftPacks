#include "timeorder.h"
#include "Protocol.h"
using namespace rc17;
//0�� 1��     ��
int otherLine[2][7]
{ { 1, 2, 6, -1 },{ 0, 3, 4, 5, -1 } };//������һ���߷��̴���ȥ��״��
int AShootOrder[2][7]
{ { 0, 3, 4, 6, -1 },{ 1, 2, 3, 6, -1 } };//���̨��˳��-1��ʾ�Ѿ������Լ��������ߵ�����̨
int BShootOrder[2][7]
{ { 5, 4, 6, -1 },{ 0, 3, 2, 6, -1 } };

void rc17t::timeorder()
{	
	whereiscar lastCar = whereiscar::carAtRight;
	while (ThreadFlag::run)
	{
		if (ThreadFlag::t_Num > 0)
		{
/*�л�̨��*/
			if (PillarVar::hasFrisbee[PillarVar::AshootingIndex] == 1 || //���ڴ�Ĵ���ȥ�� ��
				PillarVar::AshootingIndex == -1)//A  û�ڴ��л�̨��
			{

				PillarVar::AshootingIndex = checkPillarOrder(AShootOrder[rc17t::carAt()], PillarVar::hasFrisbee);//Ҫ���̨��
				if (PillarVar::AshootingIndex != -1)
					cout << "A �л���" << PillarVar::AshootingIndex << endl;
				;//�ж�������Ȼ����ߵ���
				PillarVar::index = PillarVar::AshootingIndex;
			}
			if (PillarVar::hasFrisbee[PillarVar::BshootingIndex] == 1 || //���ڴ�Ĵ���ȥ�� ��
				PillarVar::BshootingIndex == -1)//B  û�ڴ��л�̨��
			{

				PillarVar::BshootingIndex = checkPillarOrder(BShootOrder[rc17t::carAt()], PillarVar::hasFrisbee);//Ҫ���̨��
				if (PillarVar::BshootingIndex != -1)
					cout << "B �л���" << PillarVar::BshootingIndex << endl;
				;//�ж�������Ȼ����ߵ���
			}

/*�л�̨��*/
/*�л�����λ��*/
			if (PillarVar::AshootingIndex == -1 && PillarVar::BshootingIndex == -1 && //���ų̂�Ӵ��� ��
				checkPillarOrder(otherLine[rc17t::carAt()], PillarVar::hasFrisbee) != -1)//��һ��������û���ϵ�
			{
				cout << "�û�����" << endl;
				Sleep(1000);//���ٵȵ��̷�0
			}
/*�л�����λ��*/
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

//����Ҫ���̨��
int rc17t::checkPillarOrder(int order[], bool pillarState[7])
{
	int i = -1;
	while (pillarState[order[++i]])
		if (order[i] == -1)
			break;
	return order[i];
}

