#include <zmq.hpp>
#include <unistd.h>
#include "zhelpers.hpp"

using namespace std;
using namespace zmq;

int main() {
	context_t context(1);
	socket_t socket(context, ZMQ_PUB);
	socket.bind("tcp://*:5802");

	while (true) {
		s_send(socket, "ROBORIO.key:val");
		sleep(10);	
	}
}
