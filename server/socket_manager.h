#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#define PACKET_SIZE 256

class Server;

#include <vector>
#include <thread>
#include "server.h"
#include "ethernet_card.h"

using namespace std;

class Socket_manager
{
public:
	Socket_manager(Server* srv);
	virtual ~Socket_manager();

	static void accept_connection(Socket_manager* socket_manager);
	void send(char* buffer, int cli_fd);
	static void receive(Socket_manager* socket_manager, int cli_sock, Ethernet_card* ethernet_card);

	int get_accept_sockfd() { return accept_sockfd; }
	Server* get_server() { return server; }

	void set_accept_sockfd(int sockfd) { this->accept_sockfd = sockfd; }

	void connect_to_switch(Ethernet_card* my_ethernet_card, int port_num);
	int current_cli_fd ;

private:

	int accept_sockfd;
	thread* accept_thread;
	vector<thread*> receiver_threads;
	Server* server;
	thread* receiver;
};

#endif
