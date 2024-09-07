# Base36_914

一个空间利用率超高的 Base36 算法，实现简单，不依赖大数运算。

## 编码细节

该算法每次从输入数据中读取 9 字节，输出 14 字符，直到结束。如果剩余不足 9 字节，输出仍填充到 14 字符（类似 Base64 剩余不足 3 字节仍填充到 4 字符）。

空间效率：

```
9 / 14 ≈ 64.2%
```

Base36 的理论效率：

```
log2(36) / 8 ≈ 64.6%
```

不考虑尾块填充的情况下，空间效率可达 Base36 理论值的 99.5%。

实际应用时由于存在尾块填充，导致输出长度会被补齐到 14 的倍数，因此较短的输入不建议使用本算法。只有较长的输入，或者输入长度接近 9 的倍数时，本算法才有优势。

## 编码 API

### 完整块

```c
void base36_encode_block(u8 plain[9], u8 code[14], u8 encode_table[36])
```

从 `plain` 读取 9 字节进行编码，输出 14 字节到 `code`。

编码表可使用内置值 `BASE36_ENCODE_TABLE_DEFAULT`。（0-9 a-z）

### 尾块

```c
void base36_encode_last_block(u8 plain[], int len, u8 code[14], encode_table[36])
```

从 `plain` 读取 `len` (0 < len < 9) 字节进行编码，输出 14 字节到 `code`。

### 封装

```c
void base36_encode(u8 plain[], int len, u8 code[], encode_table)
```

从 `plain` 读取 `len` 字节进行编码，输出 `ceil(len / 9) * 14` 字节到 `code`。

该函数编码多个完整块和剩余尾块（如果存在的话），方便使用。

如果输入数据未结束，需要分多次调用，不要使用该函数，否则尾块会出现在结果的中间。

## 解码 API

### 完整块

```c
void base36_decode_block(u8 code[14], u8 plain[9], u8 decode_talbe[255])
```

从 `code` 读取 14 字节进行解码，输出 9 字节到 `plain`。

解码表可使用内置值 `BASE36_DECODE_TABLE_DEFAULT`。

### 尾块

```c
int base36_decode_last_block(u8 code[14], u8 plain[], decode_talbe[255])
```

从 `code` 读取 14 字节进行解码，输出 `len` 字节到 `plain`。

`len` 为函数返回值，0 < len <= 9。

### 封装

```c
int base36_decode(u8 code[], int len, u8 plain[], decode_table)
```

从 `code` 读取 `len` (必须为 14 的整数倍) 字节进行解码，输出 `len / 14 * 9` 字节到 `plain`。

该函数先解码完整块，最后一块始终当做尾块。返回解码后所有明文的长度。

### 校验

解码函数不会判断输入数据是否位于码表之外，需调用者自己检测。

即使输入数据不合法，解码函数仍能正常运行，只是结果不正确。如果解码后的数据有校验机制，那么不校验输入数据也没问题。


## 演示

Wasm 在线演示：https://etherdream.com/base36/


## TODO

使用 SIMD 指令优化性能。
