#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#define PACKET_SIZE 256

class Client;

#include <thread>
#include "client.h"

using namespace std;

class Socket_manager
{
public:
	Socket_manager(Client* cli, int port_num, string eth_name);
	virtual ~Socket_manager();

	Client* get_client() { return client; }

	void send(char* buffer);
	static void receive(Socket_manager* socket_manager);

private:
	int server_fd;
	Client* client;
	thread* receiver;
};

#endif
