#include "pch.h"

#include "Service.h"
#include "ObjectPool.h"

#define PROCADEMY_SERVER_PORT 6000

ServerServiceRef service;
SessionRef func()
{
	return MakeShared<Session>(service);
}

int main(void)
{
	/*
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);
	SOCKET socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (socket == INVALID_SOCKET)
	{
		std::cout << "SocketUtils::CreateSocket() => INVALID_SOCKET_ERROR (" << WSAGetLastError() << ")" << std::endl;
		return 0;
	}

	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(9000);

	if (::bind(socket, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR)
	{
		return 0;
	}

	if (listen(socket, 50) == SOCKET_ERROR)
	{
		return 0;
	}
	WSACleanup();
	*/

	service = make_shared<ServerService>(NetAddress(L"127.0.0.1", PROCADEMY_SERVER_PORT), func, 5000);
	service->Start();
	while (true)
	{
		;
	}


	return 0;
}