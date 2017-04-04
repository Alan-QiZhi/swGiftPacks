#include "Protocol.h"
#include "GobalVariables.h"

void rc17::Protocol::sendToUnderPan(int cmd, double data1, double data2, double data3, double data4, double data5)
{
	if (ThreadFlag::t_Num == 0 || ThreadFlag::flags[5] == false)
		return;
	long tmp1 = data1 * 10, tmp2 = data2 * 10, tmp3 = data3 * 10,
		tmp4 = data4, tmp5 = data5;
	unsigned char bytesToSend[16];
	bytesToSend[0] = 0xb6;
	bytesToSend[1] = 0xab;
	bytesToSend[2] = (tmp1 & 0xff00) >> 8;
	bytesToSend[3] = (tmp1 & 0xff);
	bytesToSend[4] = (tmp2 & 0xff00) >> 8;
	bytesToSend[5] = (tmp2 & 0xff);
	bytesToSend[6] = (tmp3 & 0xff00) >> 8;
	bytesToSend[7] = (tmp3 & 0xff);
	bytesToSend[8] = (tmp4 & 0xff00) >> 8;
	bytesToSend[9] = (tmp4 & 0xff);
	bytesToSend[10] = (tmp5 & 0xff00) >> 8;
	bytesToSend[11] = (tmp5 & 0xff);
	bytesToSend[12] = cmd;
	bytesToSend[13] = (tmp1 + tmp2 + tmp3 + tmp4 + tmp5) & 0xff;
	bytesToSend[14] = 0xbe;
	bytesToSend[15] = 0xa9;
	
	if (ComVar::serialPort.isOpened())
		ComVar::serialPort.send(bytesToSend, 16);
	else
		throw exception("串口未打开！");

}

void rc17::Protocol::sendToUnderPan(int cmd, double yaw, DelayCorrectVariables correctPara)
{
	sendToUnderPan(cmd, correctPara.pitch / correctPara.haveDataNum, correctPara.roll / correctPara.haveDataNum, yaw, -correctPara.bigWheel / correctPara.haveDataNum, -correctPara.smallWheel / correctPara.haveDataNum);
}

void rc17::Protocol::sendToCloudDeck(double data1, double data2, int16_t data3, int16_t data4)
{
	if (ThreadFlag::flags[5] == false)
		return;
	if (ComVar::serialPort.isOpened())
	{
		send7bytes(SPEED_SEM, data3, data4);
		send7bytes(PITCH_SEM, data1);
		send7bytes(YAW_SEM, data2);
	}
	else
		throw exception("串口未打开！");
}

void rc17::Protocol::sendDataForBall()
{
	unsigned char bytesToSend[16];
	bytesToSend[0] = 0xb6;
	bytesToSend[1] = 0xab;
	bytesToSend[2] = (0 & 0xff00) >> 8;
	bytesToSend[3] = (0 & 0xff);
	bytesToSend[4] = (0 & 0xff00) >> 8;
	bytesToSend[5] = (0 & 0xff);
	bytesToSend[6] = (0 & 0xff00) >> 8;
	bytesToSend[7] = (0 & 0xff);
	bytesToSend[8] = (0 & 0xff00) >> 8;
	bytesToSend[9] = (0 & 0xff);
	bytesToSend[10] = (0 & 0xff00) >> 8;
	bytesToSend[11] = (0 & 0xff);
	bytesToSend[12] = 1;
	bytesToSend[13] = 0 & 0xff;
	bytesToSend[14] = 0xbe;
	bytesToSend[15] = 0xa9;

	if (ComVar::serialPort.isOpened())
		ComVar::serialPort.send(bytesToSend, 16);
	else
		throw exception("串口未打开！");
}

void rc17::Protocol::sendPillar(int pillarA, int pillarB)
{
	if (ThreadFlag::t_Num == false || ThreadFlag::flags[5] == false)
		return;
	unsigned char bytesToSend[16];
	bytesToSend[0] = 0xb6;
	bytesToSend[1] = 0xab;
	bytesToSend[2] = 0;
	bytesToSend[3] = 0;
	bytesToSend[4] = 0;
	bytesToSend[5] = 0;
	bytesToSend[6] = 0;
	bytesToSend[7] = 0;
	bytesToSend[8] = 0;
	bytesToSend[9] = 0;
	bytesToSend[10] = 0;
	bytesToSend[11] = 0;
	bytesToSend[12] = pillarA * 16 + pillarB;
	bytesToSend[13] = 0;
	bytesToSend[14] = 0xbe;
	bytesToSend[15] = 0xa9;

	if (ComVar::serialPort.isOpened())
		ComVar::serialPort.send(bytesToSend, 16);
	else
		throw exception("串口未打开！");
}

void rc17::Protocol::toErzi()
{
	char bytesToSend[13];
	int16_t tmpx = CameraVar::receiveX, tmpy = CameraVar::receiveY, tmpang = (CameraVar::receiveAngle-20) * 100;
	bytesToSend[0] = 0xb6;
	bytesToSend[1] = 0xab;
	bytesToSend[2] = tmpx >> 8;
	bytesToSend[3] = tmpx & 0xff;
	bytesToSend[4] = tmpy >> 8;
	bytesToSend[5] = tmpy & 0xff;
	bytesToSend[6] = tmpang >> 8;
	bytesToSend[7] = tmpang & 0xff;
	bytesToSend[8] = bytesToSend[3] + bytesToSend[5] + bytesToSend[7];
	bytesToSend[9] = PillarVar::BshootingIndex;
	bytesToSend[10] = ThreadFlag::t_Num;
	bytesToSend[11] = 0xbe;
	bytesToSend[12] = 0xa9;
	ComVar::socketServer.Send(bytesToSend, 13);
}

void rc17::Protocol::formErzi(char * recvBuf)//转发给底盘
{
	cout << "转发" << endl;
	if (ThreadFlag::t_Num == false || ThreadFlag::flags[5] == false)
		return;
	if (recvBuf[0] == (char)0xb6 && recvBuf[1] == (char)0xab && recvBuf[14] == (char)0xbe && recvBuf[15] == (char)0xa9)
	{
		if (ComVar::serialPort.isOpened())
			ComVar::serialPort.send((unsigned char*)recvBuf, 16);
		else
			throw exception("串口未打开！");
	}
}

void rc17::Protocol::DelayCorrectVariables::assign(float* correctPara)
{
	if (haveDataNum != 0)//防止未修正的值被直接覆盖
	{
		pitch = pitch + correctPara[1];
		roll = roll + correctPara[2];
		bigWheel = bigWheel + correctPara[4];
		smallWheel = smallWheel + correctPara[5];
		haveDataNum++;
	}
	else
	{
		pitch = correctPara[1];
		roll = correctPara[2];
		bigWheel = correctPara[4];
		smallWheel = correctPara[5];
		haveDataNum = 1;
	}
}

void rc17::Protocol::send7bytes(int type, float data, float data2)
{
	unsigned char bytesToSend[7];
	bytesToSend[0] = 0xf0;
	bytesToSend[1] = static_cast<char>(type);
	char* bytes = BitConverter::GetBytes(static_cast<int16_t>(data));
	char* bytes2 = BitConverter::GetBytes(static_cast<int16_t>(data2));
	char* bytesf = BitConverter::GetBytes(data);
	switch (type)
	{
	case SPEED_SEM:
		bytesToSend[2] = bytes[0];
		bytesToSend[3] = bytes[1];
		bytesToSend[4] = bytes2[0];
		bytesToSend[5] = bytes2[1];
		break;
	case YAW_SEM:
	case PITCH_SEM:
		bytesToSend[2] = bytesf[0];
		bytesToSend[3] = bytesf[1];
		bytesToSend[4] = bytesf[2];
		bytesToSend[5] = bytesf[3];
		break;
	default:
		cerr << "错误： 无法识别的指令";
		break;
	}

	bytesToSend[6] = 0x0f;
	ComVar::serialPort.send(bytesToSend, 7);
	delete[] bytes;
	delete[] bytes2;
	delete[] bytesf;
}


rc17::Protocol::DelayCorrectVariables rc17::Protocol::correctPara[14] = {};
