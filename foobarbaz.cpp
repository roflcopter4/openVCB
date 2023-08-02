#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <chrono>
#include <concepts>
#include <exception>
#include <filesystem>
#include <forward_list>
#include <fstream>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <ostream>
#include <queue>
#include <random>
#include <ranges>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include <cassert>
#include <cerrno>
#include <cinttypes>
#include <climits>
#include <csignal>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>

typedef unsigned int uint;
typedef unsigned char uchar;
typedef signed char schar;
typedef int64_t ssize_t;
namespace openVCB {
using namespace ::std::literals;
namespace util {
[[__gnu__::__format__(printf, 1, 2)]]
extern void logf( char const *const __restrict format, ...);
extern void logs( char const *msg, size_t len) __attribute__((__nonnull__(1)));
extern void logs( char const *msg) __attribute__((__nonnull__(1)));
inline void logs(std::string const &msg) { logs(msg.data(), msg.size()); }
inline void logs(std::string_view const msg) { logs(msg.data(), msg.size()); }
template <typename T1>
constexpr bool eq_any(T1 const &left, T1 const &right)
{
      return left == right;
}
template <typename T1, typename ...Types>
constexpr bool eq_any(T1 const &left, T1 const &right, Types const &...rest)
{
      return left == right || eq_any(left, rest...);
}
namespace impl {
template <typename>
inline constexpr bool always_false = false;
[[nodiscard]] inline uint16_t bswap_native_16(uint16_t const val) { return __builtin_bswap16(val); }
[[nodiscard]] inline uint32_t bswap_native_32(uint32_t const val) { return __builtin_bswap32(val); }
[[nodiscard]] inline uint64_t bswap_native_64(uint64_t const val) { return __builtin_bswap64(val); }
[[nodiscard]] constexpr uint16_t bswap_16(uint16_t const val) noexcept {
      if (::std::is_constant_evaluated())
            return static_cast<unsigned short>((val << 8) | (val >> 8));
      else
            return bswap_native_16(val);
}
[[nodiscard]] constexpr uint32_t bswap_32(uint32_t const val) noexcept
{
      if (::std::is_constant_evaluated())
            return ((val << 030) & UINT32_C(0xFF00'0000)) |
                   ((val << 010) & UINT32_C(0x00FF'0000)) |
                   ((val >> 010) & UINT32_C(0x0000'FF00)) |
                   ((val >> 030) & UINT32_C(0x0000'00FF));
      else
            return bswap_native_32(val);
}
[[nodiscard]] constexpr uint64_t bswap_64(uint64_t const val) noexcept {
      if (::std::is_constant_evaluated())
            return ((val << 070) & UINT64_C(0xFF00'0000'0000'0000)) |
                   ((val << 050) & UINT64_C(0x00FF'0000'0000'0000)) |
                   ((val << 030) & UINT64_C(0x0000'FF00'0000'0000)) |
                   ((val << 010) & UINT64_C(0x0000'00FF'0000'0000)) |
                   ((val >> 010) & UINT64_C(0x0000'0000'FF00'0000)) |
                   ((val >> 030) & UINT64_C(0x0000'0000'00FF'0000)) |
                   ((val >> 050) & UINT64_C(0x0000'0000'0000'FF00)) |
                   ((val >> 070) & UINT64_C(0x0000'0000'0000'00FF));
      else
            return bswap_native_64(val);
}
template <typename T>
concept Swappable = ::std::is_integral_v<T> && sizeof(T) <= 8;
}
template <impl::Swappable T>
[[nodiscard]] constexpr T bswap(T const val) noexcept
{
      if constexpr (sizeof(T) == 1)
            return val;
      else if constexpr (sizeof(T) == 2)
            return static_cast<T>(impl::bswap_16(static_cast<uint16_t>(val)));
      else if constexpr (sizeof(T) == 4)
            return static_cast<T>(impl::bswap_32(static_cast<uint32_t>(val)));
      else if constexpr (sizeof(T) == 8)
            return static_cast<T>(impl::bswap_64(static_cast<uint64_t>(val)));
      static_assert(impl::always_false<T>, "Invalid integer size");
      return -1;
}
template <impl::Swappable T>
[[nodiscard]] constexpr T hton(T const val) noexcept
{
      if constexpr (::std::endian::native == ::std::endian::little)
            return bswap(val);
      else
            return val;
}
template <impl::Swappable T>
[[nodiscard]] constexpr T ntoh(T const val) noexcept
{
      if constexpr (::std::endian::native == ::std::endian::little)
            return bswap(val);
      else
            return val;
}
[[nodiscard]] inline intmax_t xatoi(char const *const ptr, int const base = 0) noexcept(false)
{
      char *eptr;
      auto &errno_ref = errno;
      errno_ref = 0;
      auto const ans = ::strtoimax(ptr, &eptr, base);
      if (ptr == eptr)
            throw ::std::invalid_argument("Invalid atoi argument");
      if (errno_ref == ERANGE)
            throw ::std::range_error("atoi argument out of range");
      return ans;
}
[[nodiscard]] inline uintmax_t xatou(char const *const ptr, int const base = 0) noexcept(false)
{
      char *eptr;
      auto &errno_ref = errno;
      errno_ref = 0;
      auto const ans = ::strtoumax(ptr, &eptr, base);
      if (ptr == eptr)
            throw ::std::invalid_argument("Invalid atou argument");
      if (errno_ref == ERANGE)
            throw ::std::range_error("atou argument out of range");
      return ans;
}
}
}
namespace openVCB {
constexpr uint32_t colorPallet[56] = {
      0x000000,
      0x5A5845,
      0x2E475D,
      0x4D383E,
      0x66788E,
      0x3E4D3E,
      0x384B56,
      0x4D392F,
      0x4D3744,
      0x1A3C56,
      0x4D453E,
      0x433D56,
      0x3B2854,
      0x4D243C,
      0x384D47,
      0x323841,
      0x283769,
      0xA1AB8C,
      0x3F4B5B,
      0x393A52,
      0x646A57,
      0x543836,
      0x4E5537,
      0x542A39,
      0x562660,
      0x55265E,
      0x57265C,
      0x582758,
      0x000000,
      0xA19856,
      0x63B1FF,
      0xFF5E5E,
      0x66788E,
      0x92FF63,
      0x63F2FF,
      0xFFA200,
      0xFF628A,
      0x30D9FF,
      0xFFC663,
      0xAE74FF,
      0xA600FF,
      0xFF0041,
      0x63FF9F,
      0xFFFFFF,
      0x24417A,
      0xA1AB8C,
      0x3F4B5B,
      0x535572,
      0x646A57,
      0xFF6700,
      0xE5FF00,
      0xE00000,
      0xFF00BF,
      0xFF00AF,
      0xFF009F,
      0xFF008F,
};
constexpr std::string_view inkNames[56] = {
      "None",
      "Trace (Off)",
      "Read (Off)",
      "Write (Off)",
      "Cross",
      "Buffer (Off)",
      "OR (Off)",
      "NAND (Off)",
      "NOT (Off)",
      "NOR (Off)",
      "AND (Off)",
      "XOR (Off)",
      "NXOR (Off)",
      "Clock (Off)",
      "Latch (Off)",
      "LED (Off)",
      "Bus (Off)",
      "Filler",
      "Annotation",
      "Tunnel (Off)",
      "Mesh",
      "Timer (Off)",
      "Random (Off)",
      "Break (Off)",
      "RX1 (Off)",
      "RX2 (Off)",
      "RX3 (Off)",
      "RX4 (Off)",
      "UNDEFINED",
      "Trace (On)",
      "Read (On)",
      "Write (On)",
      "INVALID Cross",
      "Buffer (On)",
      "OR (On)",
      "NAND (On)",
      "NOT (On)",
      "NOR (On)",
      "AND (On)",
      "XOR (On)",
      "XNOR (On)",
      "Clock (On)",
      "Latch (On)",
      "LED (On)",
      "Bus (On)",
      "INVALID Filler",
      "INVALID Annotation",
      "Tunnel (On)",
      "INVALID Mesh",
      "Timer (On)",
      "Random (On)",
      "Break (On)",
      "RX1 (On)",
      "RX2 (On)",
      "RX3 (On)",
      "RX4 (On)",
};
constexpr uint32_t traceColors[16] = {
      0x2A3541,
      0x9FA8AE,
      0xA1555E,
      0xA16C56,
      0xA18556,
      0xA19856,
      0x99A156,
      0x88A156,
      0x6CA156,
      0x56A18D,
      0x5693A1,
      0x567BA1,
      0x5662A1,
      0x6656A1,
      0x8756A1,
      0xA15597,
};
constexpr glm::ivec2 fourNeighbors[4] = {
      glm::ivec2(-1, 0), glm::ivec2(0, 1),
      glm::ivec2( 1, 0), glm::ivec2(0, -1)
};
}
namespace openVCB {
enum class Ink : uint8_t {
      None = 0,
      TraceOff,
      ReadOff,
      WriteOff,
      Cross,
      BufferOff,
      OrOff,
      NandOff,
      NotOff,
      NorOff,
      AndOff,
      XorOff,
      XnorOff,
      ClockOff,
      LatchOff,
      LedOff,
      BusOff,
      Filler,
      Annotation,
      TunnelOff,
      Mesh,
      TimerOff,
      RandomOff,
      BreakpointOff,
      Wireless1Off,
      Wireless2Off,
      Wireless3Off,
      Wireless4Off,
      numTypes,
      _ink_on = 0x80,
      Trace = TraceOff | _ink_on,
      Read = ReadOff | _ink_on,
      Write = WriteOff | _ink_on,
      InvalidCross = Cross | _ink_on,
      Buffer = BufferOff | _ink_on,
      Or = OrOff | _ink_on,
      Nand = NandOff | _ink_on,
      Not = NotOff | _ink_on,
      Nor = NorOff | _ink_on,
      And = AndOff | _ink_on,
      Xor = XorOff | _ink_on,
      Xnor = XnorOff | _ink_on,
      Clock = ClockOff | _ink_on,
      Latch = LatchOff | _ink_on,
      Led = LedOff | _ink_on,
      Bus = BusOff | _ink_on,
      InvalidFiller = Filler | _ink_on,
      InvalidAnnotation = Annotation | _ink_on,
      Tunnel = TunnelOff | _ink_on,
      InvalidMesh = Mesh | _ink_on,
      Timer = TimerOff | _ink_on,
      Random = RandomOff | _ink_on,
      Breakpoint = BreakpointOff | _ink_on,
      Wireless1 = Wireless1Off | _ink_on,
      Wireless2 = Wireless2Off | _ink_on,
      Wireless3 = Wireless3Off | _ink_on,
      Wireless4 = Wireless4Off | _ink_on,
};
enum class Logic : uint8_t {
      None = 0,
      NonZeroOff,
      ZeroOff,
      XorOff,
      XnorOff,
      LatchOff,
      ClockOff,
      TimerOff,
      RandomOff,
      BreakpointOff,
      _numTypes,
      _ink_on = 0x80,
      NonZero = NonZeroOff | _ink_on,
      Zero = ZeroOff | _ink_on,
      Xor = XorOff | _ink_on,
      Xnor = XnorOff | _ink_on,
      Latch = LatchOff | _ink_on,
      Clock = ClockOff | _ink_on,
      Timer = TimerOff | _ink_on,
      Random = RandomOff | _ink_on,
      Breakpoint = BreakpointOff | _ink_on,
};
struct InkState {
      int16_t activeInputs;
      bool visited;
      Logic logic;
};
struct SparseMat {
      int n;
      int nnz;
      int *ptr;
      int *rows;
};
struct InkPixel {
      Ink ink;
      int16_t meta;
};
struct InstrumentBuffer {
      InkState *buffer;
      int32_t bufferSize;
      int32_t idx;
};
struct SimulationResult {
      int64_t numEventsProcessed;
      int32_t numTicksProcessed;
      bool breakpoint;
};
struct LatchInterface {
      glm::ivec2 pos;
      glm::ivec2 stride;
      glm::ivec2 size;
      int numBits;
      int gids[64];
};
}
namespace openVCB {
template <typename T> concept Integral = std::is_integral<T>::value;
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Logic operator>>(Logic const val, uint const n) { return static_cast<Logic>(static_cast<uint>(val) >> n); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Logic operator<<(Logic const val, uint const n) { return static_cast<Logic>(static_cast<uint>(val) << n); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Logic operator& (Logic const val1, uint const val2) { return static_cast<Logic>(static_cast<uint>(val1) & val2); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Logic operator| (Logic const val1, uint const val2) { return static_cast<Logic>(static_cast<uint>(val1) | val2); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Logic operator& (Logic const val1, Logic const val2) { return static_cast<Logic>(static_cast<uint>(val1) & static_cast<uint>(val2)); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Logic operator| (Logic const val1, Logic const val2) { return static_cast<Logic>(static_cast<uint>(val1) | static_cast<uint>(val2)); }
template <Integral T>
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline bool operator==(Logic const op1, T const op2)
{
      return op1 == static_cast<Logic>(op2);
}
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Ink operator>>(Ink const val, uint const n) { return static_cast<Ink>(static_cast<uint>(val) >> n); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Ink operator<<(Ink const val, uint const n) { return static_cast<Ink>(static_cast<uint>(val) << n); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Ink operator& (Ink const val1, uint const val2) { return static_cast<Ink>(static_cast<uint>(val1) & val2); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Ink operator| (Ink const val1, uint const val2) { return static_cast<Ink>(static_cast<uint>(val1) | val2); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Ink operator& (Ink const val1, Ink const val2) { return static_cast<Ink>(static_cast<uint>(val1) & static_cast<uint>(val2)); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Ink operator| (Ink const val1, Ink const val2) { return static_cast<Ink>(static_cast<uint>(val1) | static_cast<uint>(val2)); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline int operator+ (Ink const val1, Ink const val2) { return static_cast<int>(val1) + static_cast<int>(val2); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline int operator+ (Ink const val1, int const val2) { return static_cast<int>(val1) + val2; }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline int operator- (Ink const val1, Ink const val2) { return static_cast<int>(val1) - static_cast<int>(val2); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline int operator- (Ink const val1, int const val2) { return static_cast<int>(val1) - val2; }
template <Integral T>
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline bool operator==(Ink const op1, T const op2)
{
      return op1 == static_cast<Ink>(op2);
}
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Ink operator|(Ink const val1, Logic const val2) { return static_cast<Ink>(static_cast<uint>(val1) | static_cast<uint>(val2)); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Ink operator&(Ink const val1, Logic const val2) { return static_cast<Ink>(static_cast<uint>(val1) & static_cast<uint>(val2)); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Logic operator|(Logic const val1, Ink const val2) { return static_cast<Logic>(static_cast<uint>(val1) | static_cast<uint>(val2)); }
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Logic operator&(Logic const val1, Ink const val2) { return static_cast<Logic>(static_cast<uint>(val1) & static_cast<uint>(val2)); }
constexpr bool
operator==(InkPixel const &first, InkPixel const &second) noexcept
{
      return first.ink == second.ink && first.meta == second.meta;
}
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Logic SetOn(Logic const logic, uint const state)
{
      return (logic & 0x7F) | (state << 7);
}
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Logic SetOn(Logic const logic)
{
      return logic | Logic::_ink_on;
}
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Logic SetOff(Logic const logic)
{
      return logic & 0x7F;
}
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline bool IsOn(Logic const logic)
{
      return static_cast<bool>(logic & 0x80);
}
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Ink SetOn(Ink const ink, uint const state)
{
      return (ink & 0x7F) | (state << 7);
}
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Ink SetOn(Ink const ink)
{
      return ink | Ink::_ink_on;
}
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline Ink SetOff(Ink const ink)
{
      return ink & 0x7F;
}
[[nodiscard]] __attribute__((__always_inline__)) constexpr inline bool IsOn(Ink const ink)
{
      return static_cast<bool>(ink & 0x80);
}
inline Logic inkLogicType(Ink ink)
{
      ink = SetOff(ink);
      switch (ink) {
      case Ink::XorOff: return Logic::XorOff;
      case Ink::XnorOff: return Logic::XnorOff;
      case Ink::LatchOff: return Logic::LatchOff;
      case Ink::ClockOff: return Logic::ClockOff;
      case Ink::RandomOff: return Logic::RandomOff;
      case Ink::TimerOff: return Logic::TimerOff;
      case Ink::NotOff:
      case Ink::NorOff:
      case Ink::AndOff:
            return Logic::ZeroOff;
      case Ink::BreakpointOff:
            return Logic::BreakpointOff;
      default:
            return Logic::NonZeroOff;
      }
}
extern char const *getInkString(Ink ink);
union VMemWrapper
{
      uint32_t *i;
      uint8_t *b;
      using value_type = uint8_t;
      [[nodiscard]] auto &def() & noexcept { return b; }
      [[nodiscard]] auto const &def() const & noexcept { return b; }
      VMemWrapper() noexcept = default;
      explicit VMemWrapper(uint32_t *ptr) noexcept
          : i(ptr)
      {}
      explicit VMemWrapper(uint8_t *ptr) noexcept
          : b(ptr)
      {}
      VMemWrapper(nullptr_t) noexcept
          : i(nullptr)
      {}
      [[nodiscard]] auto const &operator[](size_t const idx) const & noexcept
      {
            return b[idx];
      }
      [[nodiscard]] auto &operator[](size_t const idx) & noexcept { return b[idx]; }
      VMemWrapper &operator=(void *ptr) noexcept
      {
            b = static_cast<value_type *>(ptr);
            return *this;
      }
      VMemWrapper &operator=(value_type *ptr) noexcept
      {
            b = ptr;
            return *this;
      }
      VMemWrapper &operator=(std::nullptr_t) noexcept
      {
            b = nullptr;
            return *this;
      }
      [[nodiscard]] bool constexpr operator==(std::nullptr_t) const noexcept
      {
            return b == nullptr;
      }
      [[nodiscard]] explicit constexpr operator bool() const noexcept
      {
            return b != nullptr;
      }
      [[nodiscard]] uint32_t *word_at_byte(size_t const offset) const noexcept
      {
            return reinterpret_cast<uint32_t *>(b + offset);
      }
};
class StringArray
{
      char **array_;
      uint32_t capacity_;
      uint32_t qty_ = 0;
      static constexpr size_t default_capacity = 32;
    public:
      explicit StringArray(uint32_t const capacity = default_capacity)
          : array_(new char *[capacity]),
            capacity_(capacity)
      {}
      ~StringArray()
      {
            if (array_) {
                  for (unsigned i = 0; i < qty_; ++i) {
                        delete[] array_[i];
                        array_[i] = nullptr;
                  }
                  delete[] array_;
                  capacity_ = qty_ = 0;
                  array_ = nullptr;
            }
      }
      StringArray(StringArray const &) = delete;
      StringArray &operator=(StringArray const &) = delete;
      StringArray(StringArray &&) noexcept = delete;
      StringArray &operator=(StringArray &&) noexcept = delete;
      [[nodiscard]] char *push_blank(size_t const len)
      {
            if (qty_ + 1 == capacity_) {
                  capacity_ += default_capacity;
                  auto **tmp = new char *[capacity_];
                  memmove(tmp, array_, qty_ * sizeof(char *));
                  delete[] array_;
                  array_ = tmp;
            }
            auto *str = new char[len + 1];
            array_[qty_++] = str;
            return str;
      }
      void push(char const *orig, size_t const len)
      {
            char *str = push_blank(len);
            memcpy(str, orig, len + 1);
      }
      void push(char const *orig) { push(orig, strlen(orig)); }
      void push(std::string const &orig) { push(orig.data(), orig.size()); }
      void push(std::string_view const &orig) { push(orig.data(), orig.size()); }
      template <size_t N>
      void push(char const (&str)[N])
      {
            push(str, N);
      }
      [[nodiscard]] bool empty() const { return qty_ == 0; }
      [[nodiscard]] uint32_t size() const { return qty_; }
      [[nodiscard]] uint32_t capacity() const { return capacity_; }
      [[nodiscard]] char const *const *data() const
      {
            return array_;
      }
};
class RandomBitProvider
{
      using random_type = std::mt19937;
    public:
      RandomBitProvider()
          : random_engine_(rnd_device_()), current_(random_engine_())
      {}
      explicit RandomBitProvider(uint32_t const seed)
          : random_engine_(seed), current_(random_engine_())
      {}
      [[nodiscard]] __attribute__((__noinline__)) unsigned operator()()
      {
            if (avail_ > 0) [[likely]] {
                  --avail_;
            } else {
                  avail_ = num_bits - 1U;
                  current_ = random_engine_();
            }
            unsigned const ret = current_ & 1U;
            current_ >>= 1;
            return ret;
      }
    private:
      static constexpr int num_bits = 32U;
      inline static std::random_device rnd_device_{};
      random_type random_engine_;
      uint32_t current_;
      int avail_ = num_bits;
};
class ClockCounter
{
    public:
      ClockCounter() = default;
      explicit ClockCounter(uint16_t const low, uint16_t const high)
          : current_(low), low_period_(low), high_period_(high)
      {}
      bool tick()
      {
            if (++counter_ >= current_) {
                  state_ = !state_;
                  counter_ = 0;
                  current_ = get_period();
                  return true;
            }
            return false;
      }
      [[nodiscard]] bool is_zero() const { return counter_ == 0; }
      [[nodiscard]] int counter() const { return counter_; }
      void set_period(uint const high, uint const low)
      {
            high_period_ = static_cast<uint16_t>(high);
            low_period_ = static_cast<uint16_t>(low);
      }
      std::vector<int32_t> GIDs;
    private:
      alignas(int)
      uint16_t current_ = 1;
      uint16_t counter_ = 0;
      uint16_t low_period_ = 1;
      uint16_t high_period_ = 1;
      bool state_ = false;
      [[nodiscard]] uint16_t get_period() const { return state_ ? high_period_ : low_period_; }
};
class TimerCounter
{
    public:
      explicit TimerCounter(uint32_t const period = 1)
          : period_(period)
      {}
      bool tick()
      {
            if (++counter_ >= period_) [[unlikely]] {
                  counter_ = 0;
                  return true;
            }
            return false;
      }
      [[nodiscard]] bool is_zero() const { return counter_ == 0; }
      [[nodiscard]] uint32_t counter() const { return counter_; }
      void set_period(uint32_t const val) { period_ = val; }
      std::vector<int32_t> GIDs;
    private:
      uint32_t period_;
      uint32_t counter_ = 0;
};
}

namespace openVCB {
class Project
{
    public:
      VMemWrapper vmem = nullptr;
      uint32_t vmemSize = 0;
      uint32_t lastVMemAddr = 0;
      LatchInterface vmAddr = {{}, {}, {}, -1, {}};
      LatchInterface vmData = {{}, {}, {}, -1, {}};
      std::string assembly = {};
      int32_t height = 0;
      int32_t width = 0;
      int32_t numGroups = 0;
      uint8_t *originalImage = nullptr;
      InkPixel *image = nullptr;
      int32_t *indexImage = nullptr;
      int32_t *decoration[3] = {nullptr, nullptr, nullptr};
      uint32_t ledPalette[16] = {
            0x323841, 0xFFFFFF,
            0xFF0000, 0x00FF00, 0x0000FF,
            0xFF0000, 0x00FF00, 0x0000FF,
            0xFF0000, 0x00FF00, 0x0000FF,
            0xFF0000, 0x00FF00, 0x0000FF,
            0xFF0000, 0x00FF00
      };
      ClockCounter tickClock;
      TimerCounter realtimeClock;
      SparseMat writeMap = {0, 0, nullptr, nullptr};
      InkState *states = nullptr;
      Ink *stateInks = nullptr;
      std::map<std::string, uint32_t> assemblySymbols;
      std::map<uint32_t, uint32_t> lineNumbers;
      std::vector<InstrumentBuffer> instrumentBuffers;
      uint64_t tickNum = 0;
      int32_t *updateQ[2] = {nullptr, nullptr};
      int16_t *lastActiveInputs = nullptr;
      uint32_t qSize = 0;
      bool states_is_native = false;
      bool const vmemIsBytes;
      StringArray *error_messages = nullptr;
      RandomBitProvider random;
      explicit Project(int64_t seed, bool vmemIsBytes);
      ~Project();
      Project(Project const &) = delete;
      Project &operator=(Project const &) = delete;
      Project(Project &&) noexcept = delete;
      Project &operator=(Project &&) noexcept = delete;
      void readFromVCB(std::string const &filePath);
      bool readFromBlueprint(std::string clipboardData);
      bool processLogicData(std::vector<uint8_t> const &logicData, int32_t headerSize);
      static void processDecorationData(std::vector<uint8_t> const &decorationData,
                                        int32_t *&decoData);
      [[nodiscard]] std::pair<Ink, int32_t> sample(glm::ivec2 pos) const;
      void assembleVmem(char *errp = nullptr, size_t errSize = 256);
      void dumpVMemToText(std::string const &p) const;
      void toggleLatch(glm::ivec2 pos);
      void toggleLatch(int32_t gid);
      [[__gnu__::__hot__]]
      void preprocess();
      void addBreakpoint(int32_t gid);
      void removeBreakpoint(int32_t gid);
      [[__gnu__::__hot__]]
      SimulationResult tick(int32_t numTicks = 1, int64_t maxEvents = INT64_MAX);
    private:
      [[__gnu__::__hot__]]
      [[nodiscard]] __attribute__((__always_inline__)) constexpr inline bool resolve_state(SimulationResult &res, InkState curInk, bool lastActive, int lastInputs);
      [[__gnu__::__hot__]]
      __attribute__((__always_inline__)) constexpr inline bool tryEmit(int32_t gid);
      [[__gnu__::__hot__]]
      __attribute__((__always_inline__)) constexpr inline void handleWordVMemTick();
      __attribute__((__always_inline__)) constexpr inline void handleByteVMemTick();
      __attribute__((__always_inline__)) constexpr inline bool GetRandomBit() { return static_cast<bool>(random()); }
};
}
extern "C" {
[[nodiscard]] uint32_t openVCB_evil_assembly_bit_manipulation_routine_getVMem(uint8_t *vmem, size_t addr);
   void openVCB_evil_assembly_bit_manipulation_routine_setVMem(uint8_t *vmem, size_t addr, uint32_t data, int numBits);
}
namespace openVCB {
[[__gnu__::__hot__]]
SimulationResult
Project::tick(int32_t const numTicks, int64_t const maxEvents)
{
      SimulationResult res{};
      for (; res.numTicksProcessed < numTicks; ++res.numTicksProcessed)
      {
            if (res.numEventsProcessed > maxEvents)
                  return res;
            for (auto const &[buffer, bufferSize, idx] : instrumentBuffers)
                  buffer[tickNum % bufferSize] = states[idx];
            ++tickNum;
            if (vmem) {
                  if (vmemIsBytes)
                        handleByteVMemTick();
                  else
                        handleWordVMemTick();
            }
            if (tickClock.tick())
                  for (auto const gid : tickClock.GIDs)
                        if (!states[gid].visited)
                              updateQ[0][qSize++] = gid;
            if (!realtimeClock.GIDs.empty() && realtimeClock.tick()) [[unlikely]]
                  for (auto const gid : realtimeClock.GIDs)
                        if (!states[gid].visited)
                              updateQ[0][qSize++] = gid;
            for (int traceUpdate = 0; traceUpdate < 2; ++traceUpdate) {
                  uint const numEvents = qSize;
                  qSize = 0;
                  res.numEventsProcessed += numEvents;
                  for (uint i = 0; i < numEvents; ++i) {
                        int const gid = updateQ[0][i];
                        Logic const ink = states[gid].logic;
                        states[gid].visited = false;
                        lastActiveInputs[i] = states[gid].activeInputs;
                        if (SetOff(ink) == Logic::LatchOff)
                              states[gid].activeInputs = 0;
                  }
                  for (uint i = 0; i < numEvents; ++i) {
                        int const gid = updateQ[0][i];
                        auto const curInk = states[gid];
                        bool const lastActive = IsOn(curInk.logic);
                        bool const nextActive = resolve_state(res, curInk, lastActive, lastActiveInputs[i]);
                        if (lastActive == nextActive)
                              continue;
                        states[gid].logic = SetOn(curInk.logic, nextActive);
                        int const delta = nextActive ? 1 : -1;
                        int32_t const end = writeMap.ptr[gid + 1];
                        for (int n = writeMap.ptr[gid]; n < end; ++n) {
                              auto const nxtId = writeMap.rows[n];
                              Logic const nxtInk = SetOff(states[nxtId].logic);
                              if (!nextActive && nxtInk == Logic::LatchOff)
                                    continue;
                              int const lastNxtInput = states[nxtId].activeInputs;
                              states[nxtId].activeInputs = int16_t(lastNxtInput + delta);
                              if (lastNxtInput == 0 || lastNxtInput + delta == 0 || nxtInk == Logic::XorOff || nxtInk == Logic::XnorOff)
                                    tryEmit(nxtId);
                        }
                  }
                  if (res.breakpoint)
                        return res;
                  std::swap(updateQ[0], updateQ[1]);
            }
      }
      return res;
}
[[__gnu__::__hot__]] __attribute__((__always_inline__)) constexpr inline bool
Project::resolve_state(SimulationResult &res,
                       InkState const curInk,
                       bool const lastActive,
                       int const lastInputs)
{
      switch (SetOff(curInk.logic)) {
      case Logic::NonZeroOff: return lastInputs != 0;
      case Logic::ZeroOff: return lastInputs == 0;
      case Logic::XorOff: return lastInputs & 1;
      case Logic::XnorOff: return !(lastInputs & 1);
      case Logic::LatchOff: return lastActive ^ (lastInputs & 1);
      case Logic::RandomOff: return lastInputs > 0 && (lastActive || GetRandomBit());
      case Logic::ClockOff: return tickClock.is_zero() ? !lastActive : lastActive;
      case Logic::TimerOff: return realtimeClock.is_zero() ? !lastActive : lastActive;
      case Logic::BreakpointOff: {
            bool const ret = lastInputs > 0;
            if (ret)
                  res.breakpoint = true;
            return ret;
      }
      default:
            return false;
      }
}
[[__gnu__::__hot__]]
__attribute__((__always_inline__)) constexpr inline bool
Project::tryEmit(int32_t const gid)
{
      if (states[gid].visited)
            return false;
      states[gid].visited = true;
      updateQ[1][qSize++] = gid;
      return true;
}
[[__gnu__::__hot__]]
__attribute__((__always_inline__)) constexpr inline void
Project::handleWordVMemTick()
{
      uint32_t addr = 0;
      for (int k = 0; k < vmAddr.numBits; ++k)
            addr |= static_cast<uint32_t>(IsOn(states[vmAddr.gids[k]].logic)) << k;
      if (addr != lastVMemAddr) {
            uint32_t data = 0;
            lastVMemAddr = addr;
            data = vmem.i[addr];
            for (int k = 0; k < vmData.numBits; ++k) {
                  auto &state = states[vmData.gids[k]];
                  bool const isOn = IsOn(state.logic);
                  if (((data >> k) & 1) != isOn) {
                        state.activeInputs = 1;
                        if (state.visited)
                              continue;
                        state.visited = true;
                        updateQ[0][qSize++] = vmData.gids[k];
                  }
            }
            for (int k = 0; k < vmAddr.numBits; ++k)
                  states[vmAddr.gids[k]].activeInputs = 0;
      } else {
            uint32_t data = 0;
            for (int k = 0; k < vmData.numBits; ++k)
                  data |= static_cast<uint32_t>(IsOn(states[vmData.gids[k]].logic)) << k;
            vmem.i[addr] = data;
      }
}
__attribute__((__always_inline__)) constexpr inline void
Project::handleByteVMemTick()
{
      uint32_t addr = 0;
      for (int k = 0; k < vmAddr.numBits; ++k)
            addr |= static_cast<uint32_t>(IsOn(states[vmAddr.gids[k]].logic)) << k;
      if (addr != lastVMemAddr) {
            uint32_t data = 0;
            lastVMemAddr = addr;
            data = ::openVCB_evil_assembly_bit_manipulation_routine_getVMem(vmem.b, addr);
            for (int k = 0; k < vmData.numBits; ++k) {
                  auto &state = states[vmData.gids[k]];
                  bool const isOn = IsOn(state.logic);
                  if (((data >> k) & 1) != isOn) {
                        state.activeInputs = 1;
                        if (state.visited)
                              continue;
                        state.visited = true;
                        updateQ[0][qSize++] = vmData.gids[k];
                  }
            }
            for (int k = 0; k < vmAddr.numBits; ++k)
                  states[vmAddr.gids[k]].activeInputs = 0;
      } else {
            uint32_t data = 0;
            for (int k = 0; k < vmData.numBits; ++k)
                  data |= static_cast<uint32_t>(IsOn(states[vmData.gids[k]].logic)) << k;
            openVCB_evil_assembly_bit_manipulation_routine_setVMem(vmem.b, addr, data, vmData.numBits);
      }
}
}
