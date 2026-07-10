#include "sender.hpp"
#include "header.hpp"
#include <arpa/inet.h>
#include <cassert>
#include <cstdint>
#include <endian.h>
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

  // struct for destination
  sockaddr_in destAddr{};
  destAddr.sin_family = AF_INET;
  destAddr.sin_port = htons(port);
  inet_pton(AF_INET, "239.0.0.1",
            &destAddr.sin_addr); // converts to binary wire form and writes it
                                 // into the struct
  socklen_t addrLen = sizeof(destAddr);

  // get pointer with .data()
  // std::string dummyStr = "This is some test data\n";
  moldcast::Header dummy{};
  std::string name = "Session 1";
  for (int i{}; i < name.size(); ++i)
    dummy.session_name[i] = std::byte{name[i]};
  dummy.sequence = htobe64(1234);
  dummy.message_count = htons(12);

  assert(sizeof(dummy) == 20);

  sendto(sender.getFileDesc(), &dummy, sizeof(dummy), 0,
         reinterpret_cast<sockaddr *>(&destAddr), addrLen);

  return 0;
}
