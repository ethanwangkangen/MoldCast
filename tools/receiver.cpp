#define DEFAULT_PORT 9000
#include "receiver.hpp"
#include "EpollWrapper.hpp"
#include <arpa/inet.h>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <endian.h>
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
  // moldcast::Socket socket{};
  moldcast::Receiver receiver{};

  // Bind socket to a port
  if (argc == 1) {
    receiver.socket.bindTo(DEFAULT_PORT);
  } else if (argc == 2) {
    try {
      int port = std::stoi(argv[1]);
      if (port < 1 || port > 65535) {
        moldcast::printUsage();
      }
      receiver.socket.bindTo(static_cast<uint16_t>(port));
      std::cerr << "Socket is bound and file descriptor is "
                << receiver.socket.getFileDesc() << "\n";
    } catch (const std::exception &e) {
      std::cerr << e.what() << "\n";
      moldcast::printUsage();
      return 1;
    }
  } else {
    moldcast::printUsage();
    return 1;
  }

  // join multicast group
  receiver.socket.joinGroup();
  receiver.receiveLoop();
  return 0;
}

void moldcast::Receiver::receiveLoop() {
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
    if (!n || n < 0) {                        // ignore if error
      std::cerr << "Nothing received in 1000ms\n";
      continue;
    }
    for (int i{}; i < n; ++i) {
      int fd_ = events[i].data.fd;
      assert(fd_ = socket.getFileDesc());
      size_t sz = socket.receiveFrom(arr, src, srclen);
      std::cerr << sz << " bytes received\n";

      // 10 bytes for session name
      for (size_t i{}; i < 10; ++i)
        std::cout << static_cast<char>(arr[i]);
      std::cout << "\n";
      // 8 bytes for sequence number
      std::uint64_t sequence_number{};
      std::memcpy(&sequence_number, arr.data() + 10, sizeof(sequence_number));
      sequence_number = be64toh(sequence_number);
      std::cout << "Sequence number is " << sequence_number << "\n";

      // 2 bytes for message count
      std::uint16_t message_count{};
      std::memcpy(&message_count, arr.data() + 18, sizeof(message_count));
      message_count = ntohs(message_count);
      std::cout << "Message count is " << message_count << "\n";

      auto get_message = [](std::array<std::byte, 2048> &arr, size_t &offset) {
        std::uint16_t messageLength{};
        std::memcpy(&messageLength, arr.data() + offset, sizeof(messageLength));
        offset += sizeof(messageLength);
        messageLength = ntohs(messageLength);
        std::cout << "Message of length " << messageLength << "\n";
        for (size_t i{}; i < messageLength; ++i)
          std::cout << static_cast<char>(arr[offset + i]);
        std::cout << "\n";
        offset += messageLength;
      };
      size_t offset{20};
      for (int i{}; i < message_count; ++i) {
        // deserialise messages
        get_message(arr, offset);
      }
    }
  }
}
