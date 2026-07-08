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
  ssize_t sz = recvfrom(fd_, buf.data(), buf.size(), 0,
                        reinterpret_cast<sockaddr *>(&src), &srclen);
  return sz;
}

} // namespace moldcast
