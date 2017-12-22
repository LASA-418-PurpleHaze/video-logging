#include <iostream>
#include <memory>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <zmq.hpp>
#include "zhelpers.hpp"
#include "json.hpp"

using namespace std;
using namespace std::chrono;
using namespace zmq;
using json = nlohmann::json;
using json_parse_error = nlohmann::detail::parse_error;

struct Source {
  string address;
  string ns;
};

void from_json(const json& json, Source& source) {
  source.address = json.at("address").get<string>();
  source.ns = json.at("ns").get<string>();
}

class Event {
  public:
    long timestamp;
    string ns;
    string key;
    string val;
    Event(long timestamp, string ns, string key, string val);
    Event(string message);
    friend ostream& operator<<(ostream& os, const Event& event);
    friend bool operator==(const Event& l, const Event& r);
};

Event::Event(long timestamp, string ns, string key, string val) :
  timestamp(timestamp), ns(ns), key(key), val(val) {}

Event::Event(string message) {
  timestamp = duration_cast<milliseconds>(
      system_clock::now().time_since_epoch()
  ).count();

  try {
    auto dot_pos = message.find(".");
    auto colon_pos = message.find(":");
    ns = message.substr(0, dot_pos);
    key = message.substr(dot_pos + 1, colon_pos - (dot_pos + 1));
    val = message.substr(colon_pos + 1);
  } catch (out_of_range e) {
    ostringstream what;
    what << "message " << message << " couldn't be parsed" << endl;
    what << e.what();
    throw invalid_argument(what.str());
  }
}

ostream& operator<<(ostream& os, const Event& event) {
  os << event.timestamp << ","
     << event.ns << ","
     << event.key << ","
     << event.val;
  return os;
}

bool operator==(const Event& l, const Event& r) {
  return l.ns == r.ns &&
         l.key == r.key &&
         l.val == r.val;
}

class Server {
  public:
    Server(vector<Source> sources, int num_io_threads);
    friend ostream& operator<<(ostream& os, const Server& server);
    ~Server();
  private:
    context_t context;
    vector<shared_ptr<socket_t>> sockets;
    vector<pollitem_t> pollitems;
    vector<Event> event_log;
    static Event stop_event;
    bool running = true;
    void add_source(Source source);
    void main_loop();
    void handle_message(string message);
};

Server::Server(vector<Source> sources, int num_io_threads) :
  context(num_io_threads) {
  for (auto source : sources) {
    add_source(source);
  }
  main_loop();
}

ostream& operator<<(ostream& os, const Server& server) {
  os << "timestamp,ns,key,val" << endl;
  for (auto event : server.event_log) {
    os << event << endl;
  }
}

Event Server::stop_event("logger.control:stop");

void Server::add_source(Source source) {
  shared_ptr<socket_t> socket = make_shared<socket_t>(context, ZMQ_SUB);
  socket->setsockopt(ZMQ_SUBSCRIBE, source.ns.data(), source.ns.size());
  socket->connect(source.address.data());
  sockets.push_back(socket);
  pollitems.push_back({
      *socket.get(),
      0,
      ZMQ_POLLIN,
      0
  });
}

void Server::main_loop() {
  while (running) {
    poll(&pollitems[0], pollitems.size(), -1);
    for (auto i = 0; i < pollitems.size(); i++) {
      auto item = pollitems[i];
      auto socket = sockets[i];
      if (item.revents & ZMQ_POLLIN) {
        string message = s_recv(*socket.get());
        handle_message(message);
      }
    }
  }
}

void Server::handle_message(string message) {
  try {
    Event event(message);
    event_log.push_back(event);

    if (event == stop_event) {
      running = false;
    }
  } catch (invalid_argument e) {
    cerr << e.what() << endl;
  }
}

Server::~Server() {
  for (auto socket : sockets) {
    socket->close();
  }
}

int main(int argc, char *argv[]) {
  try {
    json config;
    fstream config_file("config.json", fstream::in);
    config_file >> config;
    vector<Source> sources = config.get<vector<Source>>();

    Server server(sources, 1);

    fstream csv_file(argv[1], fstream::out);
    csv_file << server;
  } catch (json_parse_error e) {
    cerr << "failed to parse config.json" << endl;
    cerr << e.what() << endl;
    exit(1);
  }
}
