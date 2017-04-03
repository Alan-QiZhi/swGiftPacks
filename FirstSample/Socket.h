#pragma once
#pragma comment(lib,"ws2_32.lib")
#include <Winsock2.h>


class Socket
{
public:
	enum type
	{
		Server,
		Client
	};
	Socket(int type, char* IP = "127.0.0.1", long port = 6000);
	void Send(char* data, int len);
	void Close();
	~Socket() { Close(); };
private:
	SOCKET _socket;
	SOCKET _clientSocket;
	int _type = -1;
	bool _closed = false;

	Socket() {};
	void listeningThread();
	void receive(SOCKET s);
};