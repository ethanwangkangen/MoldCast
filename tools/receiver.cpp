#define DEFAULT_PORT 9000
#include "EpollWrapper.hpp"
#include <arpa/inet.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <netinet/in.h>
#include <socket.hpp>
#include <span>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
namespace moldcast {
void printUsage() { std::cerr << "Usage: ./receiver [port]\n"; }
} // namespace moldcast

int main(int argc, char *argv[]) {
  moldcast::Socket socket{};

  // Bind socket to a port
  if (argc == 1) {
    socket.bindTo(DEFAULT_PORT);
  } else if (argc == 2) {
    try {
      int port = std::stoi(argv[1]);
      if (port < 1 || port > 65535) {
        moldcast::printUsage();
      }
      socket.bindTo(static_cast<uint16_t>(port));
      std::cerr << "Socket is bound and file descriptor is "
                << socket.getFileDesc() << "\n";
    } catch (const std::exception &e) {
      std::cerr << e.what() << "\n";
      moldcast::printUsage();
      return 1;
    }
  } else {
    moldcast::printUsage();
    return 1;
  }

  moldcast::EpollWrapper epoll_wrapper{epoll_create1(0)};

  // struct for the socket
  epoll_event socket_event;
  socket_event.events = EPOLLIN; // data avaiable to read
  socket_event.data.fd = socket.getFileDesc();

  // watch socket for readability
  epoll_wrapper.add(socket.getFileDesc(), &socket_event);

  // kernel writes into this
  epoll_event events[16];

  std::array<std::byte, 2048> arr;
  sockaddr_in src{};
  socklen_t srclen = sizeof(src);

  while (true) {
    int n = epoll_wrapper.wait(events, 1000); // TODO: magic constant
    if (!n || n < 0) { // ignore if error
      std::cerr << "Nothing received in 1000ms\n";
      continue;
    }
    for (int i{}; i < n; ++i) {
      int fd_ = events[i].data.fd;
      assert(fd_ = socket.getFileDesc());
      size_t sz = socket.receiveFrom(arr, src, srclen);
      std::cerr << sz << " bytes received\n";
      for (size_t i{}; i < sz; ++i)
        std::cout << static_cast<char>(arr[i]);
    }
  }

  return 0;
}
