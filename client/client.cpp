#include <iostream>
#include <cstring>
#include <sstream>
#include "client.h"

using namespace std;

Client::Client()
{
	
}


Client::~Client()
{
	delete this->socket_manager;
}


void Client::run()
{
	cout << "Welcome!" << endl << "Please Enter Your Command:" << endl << ">>> ";
	while(1)
	{
		string line;
		getline(cin, line) ;

		char buffer[PACKET_SIZE];
		bzero(buffer, sizeof(buffer));
		// this->socket_manager->send(buffer);

		cout << ">>> ";
		stringstream ss;
		ss << line;

		string command;
		ss >> command;

		if (command == "MAC" || command == "mac")
		{
			string mac;
			ss >> mac;
			this->set_mac_address(mac);
		}

		else if (command == "IP" || command == "ip")
		{
			string ip_with_mask;
			ss >> ip_with_mask;
			int i = ip_with_mask.find('/');
			this->ip_address = ip_with_mask.substr(0, i);
			this->subnet_mask = atoi(ip_with_mask.substr(i+1).c_str());
		}

		else if (command == "Gateway" || command == "gateway")
		{
			int port_num;
			string eth_name;
			ss >> port_num;
			ss >> eth_name;
			this->socket_manager = new Socket_manager(this, port_num, eth_name);
		}

		else if (command == "Ping" || command == "ping")
		{
			string destination_ip_with_mask;
			ss >> destination_ip_with_mask;
			int j = destination_ip_with_mask.find('/');
			string destination_ip = destination_ip_with_mask.substr(0, j);
			int subnet_mask1 = atoi(destination_ip_with_mask.substr(j+1).c_str());

			// Check to see if net id
/*			string temp1 = this->ip_address.substr(0, this->subnet_mask);
			string temp2 = destination_ip.substr(0, subnet_mask1);
			if (temp1 != temp2)
				for (int sequence_number=0 ; sequence_number<PING_TRIES ; ++sequence_number)
					cout << "From " << destination_ip << " icmp_seq=" << sequence_number+1 << " Destination Host Unreachable" << endl;
*/


			for (int sequence_number=0 ; sequence_number<PING_TRIES ; ++sequence_number)
			{
				char buffer[PACKET_SIZE];
				memset(buffer, 0, sizeof(buffer));
				int index = 0;

				// Append source ip address
				for (int i=0 ; i<ip_address.length() ; ++i)
				{
					buffer[index] = ip_address[i];
					index++;
				}
				buffer[index] = ' ';
				index++;

				// Append source mac address
				for (int i=0 ; i<mac_address.length() ; ++i)
				{
					buffer[index] = mac_address[i];
					index++;
				}
				buffer[index] = ' ';
				index++;

				// Append destination ip address
				for (int i=0 ; i<destination_ip.length() ; ++i)
				{
					buffer[index] = destination_ip[i];
					index++;
				}
				buffer[index] = ' ';
				index++;

				// Append sequence number
				char seq_num_char = (char) sequence_number + 1 + 48;
				buffer[index] = seq_num_char;
				index++;
				buffer[index] = ' ';
				index++;

				// Append ping(0) or reply(1)
				char ping_reply = 0 + 48;
				buffer[index] = ping_reply;
				index++;
				buffer[index] = ' ';
				index++;

				// Append default vlan(u)
				char vlan_char = 'u';
				buffer[index] = vlan_char;
				index++;
				buffer[index] = ' ';
				index++;


				// Send buffer
				this->socket_manager->send(buffer);
			}
		}

		else if (command == "Trace" || command == "trace")
		{
			string dest_ip;
			ss >> dest_ip;
			// TODO show routers on the way
		}

		else if (command == "Disconnect" || command == "disconnect")
		{
			char buffer[PACKET_SIZE];
			memset(buffer, 0, sizeof(buffer));
			this->socket_manager->send(buffer);
		}

		else if (command == "Exit" || command == "exit")
			exit(0);
	}
}












