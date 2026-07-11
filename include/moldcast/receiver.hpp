#include "EpollWrapper.hpp"
#include "socket.hpp"
#include <arpa/inet.h>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <socket.hpp>
#include <span>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

namespace moldcast {
class Receiver {
public:
  Receiver() = default;
  ~Receiver() = default;
  Receiver(const Receiver &) = delete;
  Receiver(Receiver &&) = delete;
  Receiver &operator=(const Receiver &) = delete;
  Receiver &operator=(Receiver &&) = delete;

  moldcast::Socket socket{};
  void receiveLoop();

private:
};
} // namespace moldcast
