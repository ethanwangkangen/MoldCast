#pragma once
#include <span>
namespace moldcast {
class Socket {
public:
  Socket();
  ~Socket();
  Socket(const Socket &) = delete;
  Socket &operator=(const Socket &) = delete;
  Socket(Socket &&) = delete; // TODO
  Socket(const Socket &&) = delete;
  void bindTo(uint16_t port);
  int getFileDesc() const;
  ssize_t receiveFrom(std::span<std::byte> buf, sockaddr_in &src,
                      socklen_t &srclen);
  void sendTo(std::span<std::byte> buf, size_t length, sockaddr_in &dst,
              socklen_t &addrLen);
  void joinGroup();

private:
  int fd_;
};
} // namespace moldcast
