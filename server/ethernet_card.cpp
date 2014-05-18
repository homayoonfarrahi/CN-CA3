#include "ethernet_card.h"

Ethernet_card::Ethernet_card(string name)
{
	this->name = name;
	this->vlan = 1;
	this->busy = false;
	this->cli_sock = -1;
	this->ip_addr = "";
	this->cost = 100;	// why 100 maybe max_int or maybe 16?
}

Ethernet_card::~Ethernet_card()
{
	
}
