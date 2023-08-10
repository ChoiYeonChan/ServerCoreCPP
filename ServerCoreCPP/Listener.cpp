#include "pch.h"
#include "Listener.h"

void Listener::Start()
{
	listen_socket_ = SocketUtils::CreateSocket();
	if (listen_socket_ == INVALID_SOCKET)
	{
		std::cout << "[Listener] ���� ���� ������ �����߽��ϴ�." << std::endl;
		return;
	}

	IocpObject::Initialize();

	int accept_count = 8;
	for (int i = 0; i < accept_count; i++)
	{
		IocpEventAccept* accept_event = new IocpEventAccept(shared_from_this());
		accept_event_list_.push_back(accept_event);
	}

	StartAccept();
}

void Listener::Close()
{
	for (IocpEventAccept* accept_event : accept_event_list_)
	{
		delete accept_event;
	}

	accept_event_list_.clear();
	CloseSocket();
}

bool Listener::StartAccept()
{
	if (SocketUtils::SetLinger(listen_socket_, 0, 0) == false)
		return false;

	if (SocketUtils::SetReuseAddress(listen_socket_, true) == false)
		return false;

	SocketUtils::Bind(listen_socket_, service_->GetNetAddress());
	SocketUtils::Listen(listen_socket_);

	for (IocpEventAccept* accept_event : accept_event_list_)
	{
		RegisterAccept(accept_event);
	}

	return true;
}

void Listener::RegisterAccept(IocpEventAccept* accept_event)
{
	accept_event->ZeroMemoryOverlapped();

	// �Ϲ������� Accept�� ��ȯ�ϴ� �������� ������ ����������
	// AcceptEx�� Ŭ���̾�Ʈ ���ϰ� ���� ������ ���� ���۸� ���ڷ� �����Ƿ� ������ �̸� �����Ѵ�.
	SessionRef session = service_->CreateSession();
	if (session == nullptr)
	{
		std::cout << "[Listener] ���� ������ �����߽��ϴ�." << std::endl;
		return;
	}

	accept_event->accept_session_ = session;
	DWORD bytes_received;

	if (!SocketUtils::AcceptEx(listen_socket_, session->GetSessionSocket(), session->recv_buffer_.GetBufferFront(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes_received, (LPOVERLAPPED)(accept_event)))
	{
		int error_code = WSAGetLastError();
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			SocketIOErrorHandler(error_code);
			RegisterAccept(accept_event);
		}
	}
}

void Listener::ProcessAccept(IocpEventAccept* accept_event)
{
	SessionRef session = accept_event->accept_session_;

	// Accept�� �޸� AcceptEx�� Ŭ���̾�Ʈ ������ �̸� �����ص� �����̹Ƿ� ListenSocket��
	// �Ӽ��� ������� �ʴ´�. ���� ������ �Ӽ��� ����ϵ��� �������־�� �Ѵ�.
	if (SocketUtils::SetUpdateAcceptContext(session->GetSessionSocket(), listen_socket_) == false)
	{
		RegisterAccept(accept_event);
		return;
	}

	SOCKADDR_IN client_address;
	int size_of_address = sizeof(client_address);
	if (getpeername(session->GetSessionSocket(), (SOCKADDR*)&client_address, &size_of_address) == SOCKET_ERROR)
	{
		RegisterAccept(accept_event);
		return;
	}

	session->SetAddress(NetAddress(client_address));

	char address[100];
	printf("[Listener] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntop(AF_INET, &client_address, address, 100), ntohs(client_address.sin_port));

	session->Start();

	RegisterAccept(accept_event);
}

void Listener::CloseSocket()
{
	closesocket(listen_socket_);
}

void Listener::Dispatch(IocpEvent* iocp_event, int num_of_bytes)
{
	ASSERT_CRASH(iocp_event->GetType() == IocpEventType::ACCEPT);
	ASSERT_CRASH(iocp_event->GetOwner() == shared_from_this());

	if (num_of_bytes != 0)
	{
		std::cout << "[Listener] Accept�� ���ÿ� ���� : " << num_of_bytes << "bytes" << std::endl;
	}

	switch (iocp_event->GetType())
	{
	case IocpEventType::ACCEPT:
		ProcessAccept(static_cast<IocpEventAccept*>(iocp_event));
		break;
	}
}