//Author: Rishi
//Purpose: Super basic subscriber for the jetson
//Created on 19 December 2017 14:56
#include <zmq.hpp>
#include <string>
#include <sstream>
#include "zhelpers.hpp"
#include <iostream>

using namespace std;
using namespace zmq;

int main(){
	//create context and socket
	context_t context(1);
	socket_t subscriber(context, ZMQ_SUB);
	subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);
	//set conflate to true if you only need most recent message
	//this can be done on the publisher side, or adding the sockopt using
	//something like subscriber.setsockopt(ZMQ_CONFLATE);

	subscriber.connect("tcp://10.4.18.2:5809"); //this is the address for a port on the roborio
	
	string message_from_roborio = "";
	while(message_from_roborio != "END"){ //it's better to use a loop condition more specific to your project
		message_from_roborio = s_recv(subscriber); //use zhelpers s_recv to make it easier to read
		cout << message_from_roborio << endl; //it'd be better to do something more interesting with the contents of the message

	}
	return 0;	
}
