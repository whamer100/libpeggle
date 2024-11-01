#ifndef BINSTREAM_H
#define BINSTREAM_H

// inspiration taken from https://github.com/DuckMonster/BinaryStream/blob/master/BinaryStream/BinaryStream/bstream.hpp
// expanded (and simplified) by my

#define ENDIANNESS_PORTABLE_CONVERSION
#include <functional>

#include "vendor/endianness.h"

#include <vector>

#define generic template<typename T>

class binstream {
public:
    binstream() = default;
    binstream(const void* ptr, size_t size);

    // write value
    generic void write(const T& value);
    // write buffer
    void write(const void* ptr, size_t size);

    // read type
    generic T read();
    // read series of bytes as a pointer
    void* bytes(size_t size);
    // read type without advancing cursor
    generic T peek() const;

    // pipe in and out data
    generic binstream& operator<<(T& value);
    generic binstream& operator>>(T& output);

    // wipe underlying buffer and reset
    void empty();

    size_t tell() const;
    void seek(size_t pos);

    size_t size() const;

    // get underlying buffer
    const uint8_t* buffer() const;

    // transform bytes using function
    void transform(const std::function<uint8_t(uint8_t&)>& f);

private:
    std::vector<uint8_t> buf;
    size_t cursor = 0;
};

// TODO: document these
inline binstream::binstream(const void* ptr, const size_t size) {
    write(ptr, size);
}

generic inline void binstream::write(const T& value) {
    write(&value, sizeof(T));
}

inline void binstream::write(const void *ptr, const size_t size) {
    buf.resize(this->size() + size);

    memcpy(&buf[this->size() - size], ptr, size);
}

generic inline T binstream::read() {
    T value = peek<T>();
    cursor += sizeof(T);

    return value;
}

inline void* binstream::bytes(const size_t size) {
    auto* data = malloc(size);
    memcpy(data, &buf[cursor], size);
    cursor += size;

    return data;
}

generic inline T binstream::peek() const {
    if (cursor + sizeof(T) > size())
        throw std::exception("Tried to read beyond stream size");

    T value;
    memcpy(&value, &buf[cursor], sizeof(T));
    return value;
}

generic inline binstream& binstream::operator<<(T& value) {
    write(value);
    return *this;
}

generic inline binstream& binstream::operator>>(T& output) {
    T value = read<T>();
    memcpy(&output, &value, sizeof(T));
    return *this;
}

inline void binstream::empty() {
    buf.clear();
    seek(0);
}

inline size_t binstream::tell() const {
    return cursor;
}

inline void binstream::seek(const size_t pos) {
    if (pos > size())
        throw std::exception("Tried to set cursor position beyond stream size");

    cursor = pos;
}

inline size_t binstream::size() const {
    return buf.size();
}

inline const uint8_t* binstream::buffer() const {
    if (buf.empty())
        return nullptr;

    return &buf[0];
}

inline void binstream::transform(const std::function<uint8_t(uint8_t&)>& f) {
    for(uint8_t& i : buf)
        i = f(i);
}

#undef generic
#endif //BINSTREAM_H
