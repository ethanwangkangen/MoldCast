#pragma once
namespace moldcast {
class Sender {
public:
  Sender();
  ~Sender();
  Sender(const Sender &) = delete;
  Sender(Sender &&) = delete;
  Sender &operator=(const Sender &) = delete;
  Sender &operator=(Sender &&) = delete;
  int getFileDesc() const;

private:
  int fd_;
};
} // namespace moldcast
