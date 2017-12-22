#include <unordered_map>
#include <string>
#include <vector>
#include <zmq.hpp>
#include <utility>
#include <memory>
#include <iostream>
#include <chrono>
#include "zhelpers.hpp"

using namespace std;
using namespace zmq;
using namespace std::chrono;

// log entry
struct Entry {
	// time message reveived
	long timestamp;
	// message namespace
	string ns;
	// message key
	string key;
	// message value
	string value;
};

// logger source
struct Source {
	// associated message namespace
	string ns;
	// source address string (e.g., "tcp://0.0.0.0:1234")
	string address;
};

class Server {
	public:
	Server(vector<Source> sources, int num_io_threads);
	~Server();

	private:
	// message log
	vector<Entry> log;

	// zmq context and subscriber sockets
	context_t zmq_context;
	unordered_map<string, shared_ptr<socket_t>> zmq_subscribers;
	pollitem_t* zmq_pollitems;

	void register_source(Source source);
	// updates zmq_pollitems to match zmq_subscribers
	void update_pollitems();
	void main_loop();
	void handle_message(Entry entry);
};

Server::Server(vector<Source> sources, int num_io_threads) : zmq_context(num_io_threads) {
	// register log sources and start main event loop
	for (auto source : sources) {
		register_source(source);
	}
	update_pollitems();
	main_loop();
}
 
void Server::register_source(Source source) {
	// create and save subscriber socket
	auto subscriber = make_shared<socket_t>(zmq_context, ZMQ_SUB);
	subscriber->setsockopt(ZMQ_SUBSCRIBE, source.ns.data(), source.ns.size());
	subscriber->connect(source.address.data());
	zmq_subscribers[source.ns] = subscriber;
}

void Server::update_pollitems() {
	vector<pollitem_t> pollitems_vec;
	for (auto const &pair : zmq_subscribers) {
		socket_t socket = move(*pair.second.get());
		pollitems_vec.push_back({
			socket,
			0,
			ZMQ_POLLIN,
			0
		});
	}
	zmq_pollitems = &pollitems_vec[0];
}

void Server::main_loop() {
	while (true) {
		// poll sockets and find ones with events
		auto num_sockets = zmq_subscribers.size();
		pollitem_t pollitems[] = {
			{ *zmq_subscribers["ROBORIO"].get(), 0, ZMQ_POLLIN, 0 }
		};
		// poll(zmq_pollitems, num_sockets, -1);

		for (auto i = 0; i < num_sockets; i++) {
			pollitem_t pollitem = zmq_pollitems[i];
			// check if socket has message waiting
			if (pollitem.revents & ZMQ_POLLIN) {
				socket_t* socket = (socket_t*) pollitem.socket;
				string message = s_recv(*socket);

				// get timestamp
				auto timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
				// parse message
				auto dot_pos = message.find(".");
				auto colon_pos = message.find(":");
				string ns = message.substr(0, dot_pos);
				string key = message.substr(dot_pos + 1, colon_pos);
				string value = message.substr(colon_pos + 1);

				handle_message({
					timestamp,
					ns,
					key,
					value	
				});
			}
		}
	}
}

void Server::handle_message(Entry entry) {
	cout << "received message:" << endl;
	cout << "namespace = " << entry.ns << endl;
	cout << "key = " << entry.key << endl;
	cout << "value = " << entry.value << endl;	
}

Server::~Server() {
	// close sockets
	for (auto const &pair : zmq_subscribers) {
		auto socket = pair.second;
		socket->close();	
	}
}

int main() {
	vector<Source> sources = {
		{ "ROBORIO", "tcp://0.0.0.0:5802" }
	};
	Server logging_server(sources, 1);
}
