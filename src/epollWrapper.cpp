#include "EpollWrapper.hpp"
namespace moldcast {
int EpollWrapper::getFileDesc() { return fd_; }

void EpollWrapper::add(int socketFd, epoll_event *event) {
  epoll_ctl(fd_, EPOLL_CTL_ADD, socketFd, event);
}

int EpollWrapper::wait(epoll_event events[], int time) {
  int n = epoll_wait(fd_, events, 16, time); // TODO: get rid of magic constants
  return n;
}
} // namespace moldcast
