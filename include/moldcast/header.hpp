#include <cstddef>
#include <cstdint>

#pragma pack(push, 1)
namespace moldcast {
struct Header {
  std::byte session_name[10];
  uint64_t sequence;
  uint16_t message_count;
};
} // namespace moldcast
#pragma pack(pop)
