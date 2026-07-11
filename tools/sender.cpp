#include "sender.hpp"
#include "header.hpp"
#include <arpa/inet.h>
#include <cassert>
#include <cstdint>
#include <endian.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <span>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <random>

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
  std::array<std::byte, 1472> buffer;

  // random message count.
  // limit is 20 (arbitrary)
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> distr(1, 20);
  int message_count = distr(gen);

  moldcast::Header header{};
  std::string name = "Session 1";
  for (int i{}; i < name.size(); ++i)
    header.session_name[i] = std::byte{name[i]};
  header.sequence = htobe64(1234);
  header.message_count = htons(message_count);
  
  assert(sizeof(header) == 20);
  size_t offset {sizeof(header)};
  static_assert(std::is_trivially_copyable_v<moldcast::Header> == true);
  
  // copy header into buffer
  std::memcpy(&buffer, &header, sizeof(header));

  // copy messages into buffer
  for (int i{}; i < message_count; ++i) {
    std::uint16_t messageLength = 12; // TODO: change this
    assert(sizeof(messageLength) == 2);
    messageLength = htons(messageLength);
    std::memcpy(buffer.data() + offset, &messageLength, sizeof(messageLength)); // note: not &buffer! use buffer.data()
    offset += sizeof(messageLength);
    std::string message = "123456789012";
    assert(message.size() == 12);
    std::memcpy(buffer.data() + offset, message.data(), message.size());
    offset += message.size();
  }
 sendto(sender.getFileDesc(), buffer.data(), offset, 0,
         reinterpret_cast<sockaddr *>(&destAddr), addrLen);

  return 0;
}
