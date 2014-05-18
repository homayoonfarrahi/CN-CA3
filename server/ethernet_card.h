#ifndef ETHERNET_CARD_H
#define ETHERNET_CARD_H

#include <string>

using namespace std;

class Ethernet_card
{
public:
	Ethernet_card(string name);
	virtual ~Ethernet_card();

	string get_name() { return name; }
	bool get_busy() { return busy; }
	int get_cli_sock() { return cli_sock; }
	int get_vlan() { return vlan; }
	string get_ip_addr() { return ip_addr; }
	int get_cost() { return cost; }

	void set_vlan(int vl) { vlan = vl; }
	void set_busy(bool b) { busy = b; }
	void set_cli_sock(int cs) { cli_sock = cs; }
	void set_ip_addr(string ia) { ip_addr = ia; }
	void set_cost(int c) { cost = c; }

private:
	string name;
	int vlan;
	bool busy;
	int cli_sock;

	string ip_addr;
	int cost;
};

#endif
