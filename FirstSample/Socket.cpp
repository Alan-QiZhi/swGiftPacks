#include <thread>
#include <WS2tcpip.h>
#include "Socket.h"
#include <iostream>

Socket::Socket(void(*phandle)(char*), int type, char* IP, long port) :_phandle(phandle), _type(type)
{
	WSADATA wsa;
	/*初始化socket资源*/
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		std::cout << "WSA init error!" << std::endl;
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN serverAddr;

	switch (type)
	{
	case Server:
	{
		ZeroMemory(reinterpret_cast<char *>(&serverAddr), sizeof(serverAddr));
		inet_pton(AF_INET, IP, static_cast<void*>(&serverAddr.sin_addr.S_un.S_addr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		bind(_socket, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr));
		std::cout << "Create Socket Server success!" << std::endl;
		break;
	}
	case Client:
	{
		inet_pton(AF_INET, IP, static_cast<void*>(&serverAddr.sin_addr.S_un.S_addr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		connect(_socket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR));
		std::cout << "Create Socket Client success!" << std::endl;
		break;
	}
	}
	std::thread t_listen(&Socket::listeningThread, this);
	t_listen.detach();
}

void Socket::Send(char* data, int len)
{
	if (_type == Server)
	{
		if (_clientSocket)
			send(_clientSocket, data, len, 0);
	}
	else
	{
		if (_socket)
			send(_socket, data, len, 0);
	}
}

void Socket::Close()
{
	_closed = true;
	closesocket(_socket);
	WSACleanup();
}

void Socket::listeningThread()
{
	if (_type == Server)
	{
		listen(_socket, 5);
		SOCKADDR_IN addrClient;
		int len = sizeof(sockaddr);
		while (!_closed)
		{
			_clientSocket = accept(_socket, reinterpret_cast<sockaddr *>(&addrClient), &len);
			//会阻塞进程，直到有客户端连接上来为止
			receive(_clientSocket);
		}
	}
	else
	{
		receive(_socket);
	}
}

void Socket::receive(SOCKET s)
{
	char recvBuf[200];
	while (!_closed)
	{
		int res = recv(s, recvBuf, 200, 0);
		if (res < 1)
		{
			s = 0;
			break;
		}
		if (_phandle)
			_phandle(recvBuf);
	}
}
