#pragma once
#include <sys/epoll.h>
namespace moldcast {
class EpollWrapper {
public:
  EpollWrapper(int fd) : fd_{fd} {}

  EpollWrapper(const EpollWrapper &) = delete;
  EpollWrapper(EpollWrapper &&) = delete;
  EpollWrapper &operator=(const EpollWrapper &) = delete;
  EpollWrapper &operator=(EpollWrapper &&) = delete;

  int getFileDesc();
  void add(int socketFd, epoll_event *event);
  int wait(epoll_event events[], int time);

private:
  int fd_;
};
} // namespace moldcast
