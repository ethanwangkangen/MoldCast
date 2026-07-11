#include "sender.hpp"
#include "header.hpp"
#include <arpa/inet.h>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <endian.h>
#include <iostream>
#include <netinet/in.h>
#include <random>
#include <span>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace moldcast {

void sendLoop(Sender &sender, sockaddr_in &destAddr, socklen_t &addrLen);

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
  moldcast::sendLoop(sender, destAddr, addrLen);
  return 0;
}
void moldcast::sendLoop(moldcast::Sender &sender, sockaddr_in &destAddr,
                        socklen_t &addrLen) {
  // get pointer with .data()
  // std::string dummyStr = "This is some test data\n";
  std::array<std::byte, 1472> buffer;

  // random message count.
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> distr(1, 5); // TODO: settle magic constant

  // initial sequence
  int sequence_number = 1;

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    int message_count = distr(gen);

    moldcast::Header header{};
    std::string name = "Session 1";
    for (int i{}; i < name.size(); ++i)
      header.session_name[i] = std::byte{name[i]};
    header.sequence = htobe64(sequence_number);
    sequence_number += message_count;
    header.message_count = htons(message_count);

    assert(sizeof(header) == 20);
    size_t offset{sizeof(header)};
    static_assert(std::is_trivially_copyable_v<moldcast::Header> == true);

    // copy header into buffer
    std::memcpy(&buffer, &header, sizeof(header));

    // copy messages into buffer
    for (int i{}; i < message_count; ++i) {
      std::uint16_t messageLength = 12; // TODO: change this
      assert(sizeof(messageLength) == 2);
      messageLength = htons(messageLength);
      std::memcpy(
          buffer.data() + offset, &messageLength,
          sizeof(messageLength)); // note: not &buffer! use buffer.data()
      offset += sizeof(messageLength);
      std::string message = "123456789012";
      assert(message.size() == 12);
      std::memcpy(buffer.data() + offset, message.data(), message.size());
      offset += message.size();
    }
    // sendto(sender.socket.getFileDesc(), buffer.data(), offset, 0,
    //   reinterpret_cast<sockaddr *>(&destAddr), addrLen);
    sender.socket.sendTo(buffer, offset, destAddr, addrLen);
  }
}
