#include "iohelper.h"

namespace iohelper {
    void set_xor(const uint8_t new_xor) {
        xor8 = new_xor;
        xor16 = new_xor * 0x0101;
        xor32 = new_xor * 0x01010101;
        xor64 = new_xor * 0x0101010101010101;
    }

    uint16_t read_uint16le(FILE* fp) {
        uint16_t val;
        fread(&val, sizeof(uint16_t), 1, fp);
        return end_le16toh(val) ^ xor16;
    }

    uint16_t read_uint16be(FILE* fp) {
        uint16_t val;
        fread(&val, sizeof(uint16_t), 1, fp);
        return end_be16toh(val) ^ xor16;
    }

    uint32_t read_uint32le(FILE* fp) {
        uint32_t val;
        fread(&val, sizeof(uint32_t), 1, fp);
        return end_le32toh(val) ^ xor32;
    }

    uint32_t read_uint32be(FILE* fp) {
        uint32_t val;
        fread(&val, sizeof(uint32_t), 1, fp);
        return end_be32toh(val) ^ xor32;
    }

    uint64_t read_uint64le(FILE* fp) {
        uint64_t val;
        fread(&val, sizeof(uint64_t), 1, fp);
        return end_le64toh(val) ^ xor64;
    }

    uint64_t read_uint64be(FILE* fp) {
        uint64_t val;
        fread(&val, sizeof(uint64_t), 1, fp);
        return end_be64toh(val) ^ xor64;
    }

    uint8_t read_uint8(FILE* fp) {
        uint8_t val;
        fread(&val, sizeof(uint8_t), 1, fp);
        return val ^ xor8;
    }


    void write_uint16le(FILE* fp, const uint16_t x) {
        const auto var = end_htole16(x) ^ xor16;
        fwrite(&var, sizeof(uint16_t), 1, fp);
    }

    void write_uint16be(FILE* fp, const uint16_t x) {
        const auto var = end_htobe16(x ^ xor16);
        fwrite(&var, sizeof(uint16_t), 1, fp);
    }

    void write_uint32le(FILE* fp, const uint32_t x) {
        const auto var = end_htole32(x) ^ xor32;
        fwrite(&var, sizeof(uint32_t), 1, fp);
    }

    void write_uint32be(FILE* fp, const uint32_t x) {
        const auto var = end_htobe32(x) ^ xor32;
        fwrite(&var, sizeof(uint32_t), 1, fp);
    }

    void write_uint64le(FILE* fp, const uint64_t x) {
        const auto var = end_htole64(x) ^ xor64;
        fwrite(&var, sizeof(uint64_t), 1, fp);
    }

    void write_uint64be(FILE* fp, const uint64_t x) {
        const auto var = end_htobe64(x) ^ xor64;
        fwrite(&var, sizeof(uint64_t), 1, fp);
    }

    void write_uint8(FILE* fp, const uint8_t x) {
        const uint8_t var = x ^ xor8;
        fwrite(&var, sizeof(uint8_t), 1, fp);
    }

    void read_bytes(FILE* fp, char* dst, const size_t num_bytes) {
        fread(dst, sizeof(uint8_t), num_bytes, fp);
    }

    void write_bytes(FILE* fp, const char* src, const size_t num_bytes) {
        fwrite(src, sizeof(uint8_t), num_bytes, fp);
    }

    void xor_bytes(char* buf, const size_t num_bytes) {
        for (int i = 0; i < num_bytes; ++i) {
            buf[i] ^= xor8;
        }
    }

    void skip_bytes(FILE* fp, const long amount) {
        const auto pos = ftell(fp);
        fseek(fp, pos + amount, 0);
    }
}