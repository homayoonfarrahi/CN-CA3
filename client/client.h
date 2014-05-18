#ifndef CLIENT_H
#define CLIENT_H

#define PING_TRIES 5

class Socket_manager;

#include <string>
#include "socket_manager.h"

using namespace std;

class Client
{
public:
	Client();
	virtual ~Client();

	void run();


	string get_ip_address() { return ip_address; }
	string get_mac_address() { return mac_address; }


	void set_mac_address(string mac) { mac_address = mac; }

private:
	string ip_address;
	int subnet_mask;
	string mac_address;
	Socket_manager* socket_manager;
};

#endif
