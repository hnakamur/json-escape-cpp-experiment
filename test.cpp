#include <catch2/catch_test_macros.hpp>
#include <cstring>

namespace
{
class EscLookup
{
public:
  static const char NO_ESCAPE{'\0'};
  static const char LONG_ESCAPE{'\x01'};

  static char result(char c)
  {
    return _lu.table[static_cast<unsigned char>(c)];
  }

private:
  struct _LUT
  {
    _LUT();

    char table[1 << 8];
  };

  inline static _LUT const _lu;
};

EscLookup::_LUT::_LUT()
{
  for (unsigned i = 0; i < ' '; ++i) {
    table[i] = LONG_ESCAPE;
  }
  for (unsigned i = '\x7f'; i < sizeof(table); ++i) {
    table[i] = LONG_ESCAPE;
  }

  // Short escapes.
  //
  table['\b'] = 'b';
  table['\t'] = 't';
  table['\n'] = 'n';
  table['\f'] = 'f';
  table['\r'] = 'r';
  table['\\'] = '\\';
  table['\"'] = '\"';
}

char nibble(int nib)
{
  return nib >= 0xa ? 'a' + (nib - 0xa) : '0' + nib;
}

} // end anonymous namespace

static int
escape_json(char *dest, const char *buf, int len)
{
  int escaped_len = 0;

  for (int i = 0; i < len; i++) {
    char c = buf[i];
    char ec = EscLookup::result(c);
    if (__builtin_expect(EscLookup::NO_ESCAPE == ec, 1)) {
      if (dest) {
        *dest++ = c;
      }
      escaped_len++;

    } else if (EscLookup::LONG_ESCAPE == ec) {
      if (dest) {
        *dest++ = '\\';
        *dest++ = 'u';
        *dest++ = '0';
        *dest++ = '0';
        *dest++ = nibble(static_cast<unsigned char>(c) >> 4);
        *dest++ = nibble(c & 0x0f);
      }
      escaped_len += 6;

    } else { // Short escape.
      if (dest) {
        *dest++ = '\\';
        *dest++ = ec;
      }
      escaped_len += 2;
    }
  } // end for
  return escaped_len;
}

TEST_CASE("escape json") {
#define HELPER(section_name, input, want)                          \
  SECTION(section_name) {                                          \
    char buf[64];                                                  \
    int escaped_len = escape_json(buf, input, std::strlen(input)); \
    CHECK(escaped_len == std::strlen(want));                       \
    CHECK(strncmp(buf, want, escaped_len) == 0);                   \
  }

  HELPER("no escape", "ab", "ab")
  HELPER("doublequote", "\"", "\\\"")
  HELPER("backslash", "\\", "\\\\")
  HELPER("short escape", "\b\f\n\r\t", "\\b\\f\\n\\r\\t")
  HELPER("long escape", "\x1f\x7f", "\\u001f\\u007f")
}
