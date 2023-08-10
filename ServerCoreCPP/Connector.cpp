#include "pch.h"
#include "Connector.h"
#include "Service.h"

void Connector::Connect(NetAddress address, int count)
{
	for (int i = 0; i < count; i++)
	{
		SessionRef session = service_->CreateSession();
		if (session == nullptr)
		{
			std::cout << "[Connector] ���� ������ �����߽��ϴ�." << std::endl;
		}

		connect_socket_list_.push_back(session->GetSessionSocket());
		SOCKADDR_IN sock_address = address.GetSocketAddress();
		if (connect(session->GetSessionSocket(), (sockaddr*)&sock_address, sizeof(sock_address)) == SOCKET_ERROR)
		{
			std::cout << "[Connector] connect�� �����߽��ϴ�. (" << WSAGetLastError() << ")" << std::endl;
			return;
		}
		else
		{
			session->ProcessConnect();
		}
	}
}