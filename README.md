# Base36_914

A fast and space-efficient Base36 encoding.

## About

`Base36_914` encodes `9` bytes into `14` chars per block, with a max space efficiency of 99.5% of the theoretical limit.

If the remaining data is less than 9 bytes, the output will be padded to 14 chars. So this algorithm is suitable for longer inputs, or input length close to a multiple of 9.


## Encode API

### Full Block

```c
void base36_encode_block(u8 plain[9], u8 code[14], u8 encode_table[36])
```

Read 9 bytes from `plain` and write 14 bytes to `code`.

The built-in `BASE36_ENCODE_TABLE_DEFAULT` can be used for `encode_table`.（0-9 a-z）

### Last Block

```c
void base36_encode_last_block(u8 plain[], int len, u8 code[14], encode_table[36])
```

Read `len` (0 < len < 9) bytes from `plain` and write 14 bytes to `code`.

### All-in-One

```c
void base36_encode(u8 plain[], int len, u8 code[], encode_table)
```

Read `len` bytes from `plain` and write `ceil(len / 9) * 14` bytes to `code`.

## Decode API

### Full Block

```c
void base36_decode_block(u8 code[14], u8 plain[9], u8 decode_talbe[255])
```

Read 14 bytes from `code` and write 9 bytes to `plain`.

The built-in `BASE36_DECODE_TABLE_DEFAULT` can be used for `decode_talbe`.

### Last Block

```c
int base36_decode_last_block(u8 code[14], u8 plain[], decode_talbe[255])
```

Read 14 bytes from `code` and write `len` bytes to `plain`.

`len` is the return value. (0 < len <= 9)

### All-in-One

```c
int base36_decode(u8 code[], int len, u8 plain[], decode_table)
```

Read `len` (must be a multiple of 14) bytes from `code` and write `len / 14 * 9` bytes to `plain`, return `len / 14 * 9`.

This function calls `base36_decode_last_block` only once.

### Stream Mode

```c
int base36_decode_stream(u8 code[], int len, u8 plain[], decode_table)
```

If `base36_encode` is called multiple times during encoding, the 14-char-padding(s) may be located in the middle of the codes. In this case, this function must be used for decoding.

This function calls `base36_decode_last_block` every time.

### Check

None of the above decoding functions check the input code, it needs to be checked by the caller.

Even if the input code is out of the code table, the decoding functions can still work,  just with wrong result. If the decoded data has an integrity check, the input check can be ignored.


## Demo1

[demo-wsam](demo-wsam)

Online: https://etherdream.com/base36/


## Demo2

[demo-pipe](demo-pipe)

Test:

```bash
./base36 <<< "Hello World 12345"
```

Print `gatk40t14s0idcgzbz9l6nvgmq31`.

Encode:

```bash
./base36 < file > file36.txt
```

Decode:

```bash
./base36 -d < file36.txt > file-re
```

## TODO

SIMD support.
