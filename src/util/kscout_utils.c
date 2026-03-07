#include <stdint.h>
#include <string.h>

/* Strip leading/trailing whitespace in-place (operates on a temporary copy) */
const char *trim(const char *s, char *buf, size_t buflen)
{
  if (!s) {
    return "";
  }
  while (*s == ' ' || *s == '\t') {
    s++;
  }
  size_t len = strlen(s);
  while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t' ||
                     s[len - 1] == '\r' || s[len - 1] == '\n')) {
    len--;
  }
  if (len >= buflen) {
    len = buflen - 1;
  }
  memcpy(buf, s, len);
  buf[len] = '\0';
  return buf;
}

int has_suffix(const char *s, const char *suffix)
{
  size_t sl = strlen(s);
  size_t su = strlen(suffix);
  return sl >= su && strcmp(s + sl - su, suffix) == 0;
}