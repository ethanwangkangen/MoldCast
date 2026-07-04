#define DEFAULT_PORT 9000
#include <arpa/inet.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <netinet/in.h>
#include <socket.hpp>
#include <span>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

namespace moldcast {
Socket::Socket() : fd_{socket(AF_INET, SOCK_DGRAM, 0)} {
  // socket creation failed
  if (fd_ == -1) {
    throw std::runtime_error("socket creation failed");
  }
}
Socket::~Socket() {
  int rc = close(fd_);
  if (rc < 0)
    std::cerr << "File descriptor not closed\n";
  else
    std::cerr << "File descriptor closed\n";
}

// associate the socket with a port on machine
void Socket::bindTo(uint16_t port) {
  assert(fd_ >= 1 && fd_ <= 65535);
  // create the struct
  sockaddr_in addr{};
  addr.sin_family = PF_INET;   // IPv4
  addr.sin_port = htons(port); // htons changes little endian to big endian
  addr.sin_addr.s_addr = htons(INADDR_ANY); // bind to 0.0.0.0 (all interfaces

  int rc = bind(fd_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
  if (rc < 0)
    throw std::system_error(errno, std::generic_category(), "bind");
  std::cerr << "Bind successful. \n";
}

int Socket::getFileDesc() const { return fd_; }

// src and srclen are the in and out parameters
// std::span is copy converted from std::array, but writes into the correct
// memory hence no reference taken
ssize_t Socket::receiveFrom(std::span<std::byte> buf, sockaddr_in &src,
                            socklen_t &srclen) {
  ssize_t sz = recvfrom(fd_, buf.data(), buf.size(), 0, reinterpret_cast<sockaddr *>(&src), &srclen);
  return sz;
}

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

  std::array<std::byte, 2048> arr;
  sockaddr_in src{};
  socklen_t srclen = sizeof(src);
  while (true) {
    size_t sz = socket.receiveFrom(arr, src, srclen);
    std::cerr << sz << " bytes received\n";
    for (size_t i{}; i < sz; ++i)
      std::cout << static_cast<char>(arr[i]);
  }
  return 0;
}
