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
  void getFileDesc() const;

private:
  int fd_;
}
} // namespace moldcast
