#include <thread>
#include <chrono>
#include <zmq.hpp>
#include "zhelpers.hpp"

using namespace std;
using namespace std::chrono;
using namespace zmq;

int main() {
  context_t context(1);
  socket_t socket(context, ZMQ_PUB);
  socket.bind("tcp://*:8888");

  for (auto i = 0; i < 100; i++) {
    s_send(socket, "test-ns.hello:world");
    this_thread::sleep_for(milliseconds(10));
  }

  s_send(socket, "logger.control:stop");
}
