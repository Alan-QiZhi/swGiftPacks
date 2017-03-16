#include "SocketClient.h"
bool rc17::SocketClient::init()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	mySocket = socket(AF_INET, SOCK_STREAM, 0);
	//����Ҫ���ӵķ���˵�ַ
	addrServer.sin_addr.S_un.S_addr = inet_addr(SERVERIP);
	//inet_pton(AF_INET, SERVERIP, (void*)&addrServer.sin_addr.S_un.S_addr);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(6000);//���Ӷ˿�
	return true;
}


void rc17::SocketClient::connectAndSendOffset(const rc17::PillarIndex pillarIndexShoot, int offsetX, int offsetZ) const
{
	//connect(sockClient, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
	//��������
	char message[50] = "1 120 120";
	sprintf(message, "%d %d %d \0", pillarIndexShoot, offsetX, offsetZ);
	send(mySocket, message, 13, 0);
}

bool rc17::SocketClient::closeSocket() const
{
	closesocket(mySocket);
	WSACleanup();
	return true;
}
