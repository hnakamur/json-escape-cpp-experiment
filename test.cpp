#include <catch2/catch_test_macros.hpp>
#include <string.h>
#include <iostream>

const char short_escapes[0x20] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  'b', 't', 'n', 0, 'f', 'r', 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
};

static int
escape_json(char *dest, const char *buf, int len)
{
  int escaped_len = 0;

  for (int i = 0; i < len; i++) {
    char c = buf[i];
    if (c == '\"' || c == '\\') {
      if (dest) {
        *dest++ = '\\';
        *dest++ = c;
      }
      escaped_len += 2;
    } else if (c == '\x7f') {
        if (dest) {
          *dest++ = '\\';
          *dest++ = 'u';
          *dest++ = '0';
          *dest++ = '0';
          *dest++ = '7';
          *dest++ = 'f';
        }
        escaped_len += 6;        
    } else if (c >= '\x20') {
      if (dest) {
        *dest++ = c;
      }
      escaped_len++;
    } else if (char b = short_escapes[c]; b != 0) {
      if (dest) {
        *dest++ = '\\';
        *dest++ = b;
      }
      escaped_len += 2;
    } else {
      if (dest) {
        *dest++ = '\\';
        *dest++ = 'u';
        *dest++ = '0';
        *dest++ = '0';
        *dest++ = '0' + (c >> 4);
        int d = c & 0x0f;
        *dest++ = (d >= 10 ? 'a' + (d - 10) : '0' + d);
      }
      escaped_len += 6;
    }
  }
  return escaped_len;
}

TEST_CASE("escape json") {
#define HELPER(section_name, input, want)                     \
  SECTION(section_name) {                                     \
    char buf[64];                                             \
    int escaped_len = escape_json(buf, input, strlen(input)); \
    CHECK(escaped_len == strlen(want));                       \
    CHECK(strncmp(buf, want, escaped_len) == 0);              \
  }

  HELPER("no escape", "ab", "ab")
  HELPER("doublequote", "\"", "\\\"")
  HELPER("backslash", "\\", "\\\\")
  HELPER("short escape", "\b\f\n\r\t", "\\b\\f\\n\\r\\t")
  HELPER("long escape", "\x1f\x7f", "\\u001f\\u007f")
}
