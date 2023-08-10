#pragma once

#include <vector>
#include "NetAddress.h"

class Connector
{
private:
	ServiceRef service_;
	std::vector<SOCKET> connect_socket_list_;

public:
	Connector(ServiceRef service) : service_(service) { }
	~Connector() = default;

	void Connect(NetAddress address, int count);
	std::vector<SOCKET> SocketList() { return connect_socket_list_; }

	ServiceRef GetService() const { return service_; }
};