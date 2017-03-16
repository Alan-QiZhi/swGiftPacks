#pragma once
#pragma comment(lib,"ws2_32.lib")
#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <Winsock2.h>
#include "PillarState.h"

#define SERVERIP "192.168.31.211"  //�����IP
#define PORTNUMBER 6000  //���Ӷ˿ں�

namespace rc17
{		
	class SocketClient
	{
	public:
		bool init();
		void connectAndSendOffset(const rc17::PillarIndex a, int offsetX, int offsetZ) const;
		bool closeSocket() const;
		SOCKET get() const { return mySocket; }
	private:
		SOCKET mySocket = SOCKET();//�ͻ���Socket
		SOCKADDR_IN addrServer;//����˵�ַ
	};
}
