#pragma once

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <iostream>
#include <thread>
#include <mutex>

namespace rc17
{
	class MySerial
	{
	public:
		MySerial() :hCom(INVALID_HANDLE_VALUE), listenThreadClose(false), listenThreadID()
		{}

		~MySerial()
		{
			std::cout << "已调用析构函数" << std::endl;
		}

		const int ESC = 27;
		const int SPACE = 16;

		int open(int com_num, int input_buffer = 1024, int ouput_buffer = 1024, int baud = 115200,
			int byte_size = 8, int parity = 0, int stopbits = 0);
		int auto_open(int input_buffer = 1024, int ouput_buffer = 1024, int baud = 115200,
			int byte_size = 8, int parity = 0, int stopbits = 0);
		int send(unsigned char *send_buf, unsigned long data_len) const;
		void openListenThread();
		void receive(const int data_len);

		inline bool isOpened() { return isOpen; };
	private:
		HANDLE hCom;
		std::thread::id listenThreadID;
		bool listenThreadClose;
		int init(int input_buffer, int ouput_buffer, int baud,
			int byte_size, int parity, int stopbits) const;
		int open_file(LPCWSTR file_name, int input_buffer, int ouput_buffer, int baud,
			int byte_size, int parity, int stopbits);
		bool isOpen = false;
	};
}

