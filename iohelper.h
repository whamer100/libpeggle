#ifndef IOHELPER_H
#define IOHELPER_H

#define ENDIANNESS_PORTABLE_CONVERSION
#include "vendor/endianness.h"

#include <cstdio>
#include <cstdint>

namespace iohelper {
    static uint8_t  xor8  = 0x00;
    static uint16_t xor16 = 0x0000;
    static uint32_t xor32 = 0x00000000;
    static uint64_t xor64 = 0x0000000000000000;
    void set_xor(uint8_t new_xor);

    uint16_t read_uint16le(FILE* fp);
    uint16_t read_uint16be(FILE* fp);
    uint32_t read_uint32le(FILE* fp);
    uint32_t read_uint32be(FILE* fp);
    uint64_t read_uint64le(FILE* fp);
    uint64_t read_uint64be(FILE* fp);
    uint8_t read_uint8(FILE* fp);

    void write_uint16le(FILE* fp, uint16_t x);
    void write_uint16be(FILE* fp, uint16_t x);
    void write_uint32le(FILE* fp, uint32_t x);
    void write_uint32be(FILE* fp, uint32_t x);
    void write_uint64le(FILE* fp, uint64_t x);
    void write_uint64be(FILE* fp, uint64_t x);
    void write_uint8(FILE* fp, uint8_t x);

    void read_bytes(FILE* fp, char* dst, size_t num_bytes);
    void write_bytes(FILE* fp, char* src, size_t num_bytes);

    void xor_bytes(char* buf, size_t num_bytes);

    void skip_bytes(FILE* fp, long amount);
}

#endif //IOHELPER_H
