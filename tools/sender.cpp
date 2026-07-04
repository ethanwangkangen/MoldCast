#include "sender.hpp"
#include <arpa/inet.h>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <netinet/in.h>
#include <span>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

namespace moldcast {
Sender::Sender() : fd_{socket(AF_INET, SOCK_DGRAM, 0)} {
  if (fd_ == -1)
    throw std::runtime_error("socket creation failed");
}

Sender::~Sender() {
  int rc = close(fd_);
  if (rc < 0)
    std::cerr << "File descriptor not closed\n";
  else
    std::cerr << "File descriptor closed\n";
}

int Sender::getFileDesc() const { return fd_; }

} // namespace moldcast

int main(int argc, char *argv[]) {

  moldcast::Sender sender{};

  uint16_t port = 9000; // todo change this

  sockaddr_in destAddr{};
  destAddr.sin_family = AF_INET;
  destAddr.sin_port = htons(port);
  inet_pton(AF_INET, "127.0.0.1",
            &destAddr.sin_addr); // converts to binary wire form and writes it
                                 // into the struct
  socklen_t addrLen = sizeof(destAddr);

  // get pointer with .data()
  std::string dummyStr = "This is some test data\n";

  sendto(sender.getFileDesc(), dummyStr.data(), dummyStr.size(), 0,
         reinterpret_cast<sockaddr *>(&destAddr), addrLen);

  return 0;
}
