#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../base36_914.h"

static uint8_t plain_buf[1024 * 9];
static uint8_t code_buf[1024 * 14];

void encode() {
  for (;;) {
    ssize_t plain_len = read(STDIN_FILENO, plain_buf, sizeof(plain_buf));
    if (plain_len <= 0) {
      break;
    }
    int code_len = base36_encode(plain_buf, plain_len, code_buf,
                                 BASE36_ENCODE_TABLE_DEFAULT);
    write(STDOUT_FILENO, code_buf, code_len);
  }
}

void decode() {
  int remain = 0;

  for (;;) {
    int read_len = read(STDIN_FILENO, code_buf + remain, sizeof(code_buf) - remain);
    if (read_len <= 0) {
      break;
    }
    int code_len = remain + read_len;
    remain = code_len % 14;

    int plain_len = base36_decode_stream(code_buf, code_len - remain, plain_buf,
                                         BASE36_DECODE_TABLE_DEFAULT);
    write(STDOUT_FILENO, plain_buf, plain_len);

    memmove(code_buf, code_buf + code_len - remain, remain);
  }
}

void usage() {
  fprintf(stderr,
    "encode usage: base36 < input > output \n"
    "decode usage: base36 -d < input > output\n"
  );
}

int main(int argc, char* argv[]) {
  if (argc == 2) {
    if (!strnstr(argv[1], "-d", 2)) {
      usage();
      return 1;
    }
    decode();
  } else {
    encode();
  }
  return 0;
}