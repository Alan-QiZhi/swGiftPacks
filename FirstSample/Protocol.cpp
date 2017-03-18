#include "Protocol.h"
#include "GobalVariables.h"

void rc17::Protocol::sendDataBySerialPort(long data1, long data2, long data3)
{	
	unsigned char bytesToSend[12];
	bytesToSend[0] = 0xb6;
	bytesToSend[1] = 0xab;
	bytesToSend[2] = (data1 & 0xff00) >> 8;
	bytesToSend[3] = (data1 & 0xff);
	bytesToSend[4] = (data2 & 0xff00) >> 8;
	bytesToSend[5] = (data2 & 0xff);
	bytesToSend[6] = (data3 & 0xff00) >> 8;
	bytesToSend[7] = (data3 & 0xff);
	bytesToSend[8] = (data1 + data2 + data3) & 0xff;
	bytesToSend[9] = 0;
	bytesToSend[10] = 0xbe;
	bytesToSend[11] = 0xa9;

	if (CommunicationVariables::serialPort.isOpened())
		CommunicationVariables::serialPort.send(bytesToSend, 12);
	else
		throw exception("串口未打开！");
}

void rc17::Protocol::sendDataBySerialPort(double data1, double data2, double data3)
{
	unsigned char bytesToSend[12];
	bytesToSend[0] = 0xb6;
	bytesToSend[1] = 0xab;
	bytesToSend[2] = (static_cast<long>(data1) & 0xff00) >> 8;
	bytesToSend[3] = (static_cast<long>(data1) & 0xff);
	bytesToSend[4] = (static_cast<long>(data2) & 0xff00) >> 8;
	bytesToSend[5] = (static_cast<long>(data2) & 0xff);
	bytesToSend[6] = (static_cast<long>(data3) & 0xff00) >> 8;
	bytesToSend[7] = (static_cast<long>(data3) & 0xff);
	bytesToSend[8] = (static_cast<long>(data1 + data2 + data3)) & 0xff;
	bytesToSend[9] = 0;
	bytesToSend[10] = 0xbe;
	bytesToSend[11] = 0xa9;

	if (CommunicationVariables::serialPort.isOpened())
		CommunicationVariables::serialPort.send(bytesToSend, 12);
	else
		throw exception("串口未打开！");
}
void rc17::Protocol::sendDataBySerialPort(int cmd, double data1, double data2, double data3, double data4, double data5)
{
	if (ThreadFlag::t_Flag == false)
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
	
	if (CommunicationVariables::serialPort.isOpened())
		CommunicationVariables::serialPort.send(bytesToSend, 16);
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

	if (CommunicationVariables::serialPort.isOpened())
		CommunicationVariables::serialPort.send(bytesToSend, 16);
	else
		throw exception("串口未打开！");
}

void rc17::Protocol::sendCmd(int cmd)
{
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
	bytesToSend[12] = cmd;
	bytesToSend[13] = 0;
	bytesToSend[14] = 0xbe;
	bytesToSend[15] = 0xa9;

	if (CommunicationVariables::serialPort.isOpened())
		CommunicationVariables::serialPort.send(bytesToSend, 16);
	else
		throw exception("串口未打开！");
}

void rc17::Protocol::sendDataBySocket(long data1, long data2, long data3)
{
	unsigned char bytesToSend[12];
	bytesToSend[0] = 0xb6;
	bytesToSend[1] = 0xab;
	bytesToSend[2] = (data1 & 0xff00) >> 8;
	bytesToSend[3] = (data1 & 0xff);
	bytesToSend[4] = (data2 & 0xff00) >> 8;
	bytesToSend[5] = (data2 & 0xff);
	bytesToSend[6] = (data3 & 0xff00) >> 8;
	bytesToSend[7] = (data3 & 0xff);
	bytesToSend[8] = (data1 + data2 + data3) & 0xff;
	bytesToSend[10] = 0xbe;
	bytesToSend[11] = 0xa9;

	send(rc17::CommunicationVariables::mySocketClient.get(), reinterpret_cast<const char *>(bytesToSend), 12, 0);
}

void rc17::Protocol::sendDataBySocket(double data1, double data2, double data3)
{
	unsigned char bytesToSend[12];
	bytesToSend[0] = 0xb6;
	bytesToSend[1] = 0xab;
	bytesToSend[2] = (static_cast<long>(data1) & 0xff00) >> 8;
	bytesToSend[3] = (static_cast<long>(data1) & 0xff);
	bytesToSend[4] = (static_cast<long>(data2) & 0xff00) >> 8;
	bytesToSend[5] = (static_cast<long>(data2) & 0xff);
	bytesToSend[6] = (static_cast<long>(data3) & 0xff00) >> 8;
	bytesToSend[7] = (static_cast<long>(data3) & 0xff);
	bytesToSend[8] = static_cast<long>(data1 + data2 + data3) & 0xff;
	bytesToSend[10] = 0xbe;
	bytesToSend[11] = 0xa9;

	send(rc17::CommunicationVariables::mySocketClient.get(), reinterpret_cast<const char *>(bytesToSend), 12, 0);
}

void rc17::Protocol::DelayCorrectVariables::assign(float* correctPara)
{
	pitch = correctPara[1];
	roll = correctPara[2];
	bigWheel = correctPara[4];
	smallWheel = correctPara[5];
	haveData = true;
}
rc17::Protocol::DelayCorrectVariables rc17::Protocol::delayCorrectVariables[7] = {};