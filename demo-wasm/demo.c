#include <stdio.h>
#include <unistd.h>
#include <emscripten.h>
#include "../base36_914.h"

static uint8_t plain_buf[1000000 * 9];
static uint8_t code_buf[1000000 * 14];


EMSCRIPTEN_KEEPALIVE
int EncodePlainBuf(int len) {
  return base36_encode(plain_buf, len, code_buf, BASE36_ENCODE_TABLE_DEFAULT);
}

EMSCRIPTEN_KEEPALIVE
int DecodeCodeBuf(int len) {
  return base36_decode(code_buf, len, plain_buf, BASE36_DECODE_TABLE_DEFAULT);
}

EMSCRIPTEN_KEEPALIVE
uint8_t* GetPlainBuf() {
  return plain_buf;
}

EMSCRIPTEN_KEEPALIVE
uint8_t* GetCodeBuf() {
  return code_buf;
}
