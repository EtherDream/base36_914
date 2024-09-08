#pragma once
#include <stdint.h>
#include <string.h>

static const uint8_t BASE36_ENCODE_TABLE_DEFAULT[36] = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
  'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
  'u', 'v', 'w', 'x', 'y', 'z',
};
static const uint8_t BASE36_DECODE_TABLE_DEFAULT[256] = {
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
  25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

static void base36_encode_block(
  const uint8_t plain[9],
        uint8_t code[14],
  const uint8_t encode_table[36]
) {
  uint64_t lo =
    (uint64_t) plain[0] << (0 * 8) |
    (uint64_t) plain[1] << (1 * 8) |
    (uint64_t) plain[2] << (2 * 8) |
    (uint64_t) plain[3] << (3 * 8) |
    (uint64_t) plain[4] << (4 * 8) |
    (uint64_t) plain[5] << (5 * 8) |
    (uint64_t) plain[6] << (6 * 8) |
    (uint64_t) plain[7] << (7 * 8);

  uint8_t hi = plain[8];

  const int D = 36 * 36;

  uint32_t remainder = hi * (0xFFFFFFFFFFFFFFFF % D + 1) + (lo % D);

  for (int i = 0; i < 2; i++) {
    code[i] = encode_table[remainder % 36];
    remainder /= 36;
  }

  uint64_t quotient = hi * (0xFFFFFFFFFFFFFFFF / D) + lo / D + remainder;

  for (int i = 2; i < 14; i++) {
    code[i] = encode_table[quotient % 36];
    quotient /= 36;
  }
}


static void base36_decode_block(
  const uint8_t code[14],
        uint8_t plain[9],
  const uint8_t decode_table[256]
) {
  uint64_t lo = 0;
  uint64_t hi = 0;

  // 36^12 < 2^64
  for (int i = 11; i >= 0; i--) {
    lo *= 36;
    lo += decode_table[code[i]];
  }
  for (int i = 13; i >= 12; i--) {
    hi *= 36;
    hi += decode_table[code[i]];
  }

  plain[0] = lo;

  // 36^12 >> 8 = 18509302102818816
  uint64_t value = hi * 18509302102818816 + (lo >> 8);
  plain[1] = value >> (0 * 8);
  plain[2] = value >> (1 * 8);
  plain[3] = value >> (2 * 8);
  plain[4] = value >> (3 * 8);
  plain[5] = value >> (4 * 8);
  plain[6] = value >> (5 * 8);
  plain[7] = value >> (6 * 8);
  plain[8] = value >> (7 * 8);
}


static void base36_encode_last_block(
  const uint8_t plain[],
        int     len,
        uint8_t code[14],
  const uint8_t encode_table[36]
) {
  uint8_t plain_tmp[9] = {0};
  memcpy(plain_tmp, plain, len);
  //
  // 36^13 * 28 > 256^9  =>  last code < 28
  //
  // len: 1 -> last code: 28
  // len: 2 -> last code: 29
  // ...
  // len: 7 -> last code: 34
  // len: 8 -> last code: 35
  //
  base36_encode_block(plain_tmp, code, encode_table);
  code[13] = encode_table[27 + len];
}


static int base36_decode_last_block(
  const uint8_t code[14],
        uint8_t plain[],
  const uint8_t decode_table[256]
) {
  uint8_t flag = decode_table[code[13]];

  if (flag < 28) {
    base36_decode_block(code, plain, decode_table);
    return 9;
  }

  uint8_t code_tmp[14] = {0};
  memcpy(code_tmp, code, 13);

  uint8_t plain_tmp[9];
  base36_decode_block(code_tmp, plain_tmp, decode_table);

  int len = flag - 27;
  if (len > 8) {
    len = 8;
  }
  memcpy(plain, plain_tmp, len);
  return len;
}


static int base36_encode(
  const uint8_t plain[],
        int     len,
        uint8_t code[],
  const uint8_t encode_table[]
) {
  const uint8_t* src = plain;
  uint8_t* dst = code;

  int remain = len;

  while (remain >= 9) {
    remain -= 9;

    base36_encode_block(src, dst, encode_table);
    src += 9;
    dst += 14;
  }

  if (remain) {
    base36_encode_last_block(src, remain, dst, encode_table);
    dst += 14;
  }
  return dst - code;
}


static int base36_decode(
  const uint8_t code[],
        int     len,
        uint8_t plain[],
  const uint8_t decode_table[]
) {
  if (len <= 0) {
    return 0;
  }
  const uint8_t* src_last = code + len - 14;
  const uint8_t* src = code;
  uint8_t* dst = plain;

  while (src < src_last) {
    base36_decode_block(src, dst, decode_table);
    src += 14;
    dst += 9;
  }
  dst += base36_decode_last_block(src, dst, decode_table);;
  return dst - plain;
}
