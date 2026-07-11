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


  void account_for(std::uint64_t sequence_number, std::uint16_t message_count);
private:
  std::uint64_t next_expected{1};
};
} // namespace moldcast
