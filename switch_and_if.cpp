#include <catch2/catch_test_macros.hpp>
#include <string.h>

static int
escape_json(char *dest, const char *buf, int len)
{
  int escaped_len = 0;

  for (int i = 0; i < len; i++) {
    char c = buf[i];
    switch (c) {
    case '"':
    case '\\': {
      if (dest) {
        *dest++ = '\\';
        *dest++ = c;
      }
      escaped_len += 2;
      break;
    }
    case '\b': {
      if (dest) {
        *dest++ = '\\';
        *dest++ = 'b';
      }
      escaped_len += 2;
      break;
    }
    case '\f': {
      if (dest) {
        *dest++ = '\\';
        *dest++ = 'f';
      }
      escaped_len += 2;
      break;
    }
    case '\n': {
      if (dest) {
        *dest++ = '\\';
        *dest++ = 'n';
      }
      escaped_len += 2;
      break;
    }
    case '\r': {
      if (dest) {
        *dest++ = '\\';
        *dest++ = 'r';
      }
      escaped_len += 2;
      break;
    }
    case '\t': {
      if (dest) {
        *dest++ = '\\';
        *dest++ = 't';
      }
      escaped_len += 2;
      break;
    }
    default: {
      if (c <= '\x1f' || c == '\x7f') {
        if (dest) {
          *dest++ = '\\';
          *dest++ = 'u';
          *dest++ = '0';
          *dest++ = '0';
          *dest++ = '0' + (c >> 4);
          int d   = c & 0x0f;
          *dest++ = (d >= 10 ? 'a' + (d - 10) : '0' + d);
        }
        escaped_len += 6;
      } else {
        if (dest) {
          *dest++ = c;
        }
        escaped_len++;
      }
      break;
    }
    }
  }
  return escaped_len;
}

TEST_CASE("escape json impl#1") {
  char buf[64];
  const char *input = "\"\\\b\f\n\r\t\x1f\x7f" "ab";
  int escaped_len = escape_json(buf, input, strlen(input));
  const char *want = "\\\"\\\\\\b\\f\\n\\r\\t\\u001f\\u007fab";
  CHECK(escaped_len == strlen(want));
  CHECK(strcmp(buf, want) == 0);
}
