/*
Written by jcsb1994
Licensed under the Apache License, Version 2.0 (the "License");
*/


#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>
#include <string.h>

template <unsigned int Size>
// requires (Size > 0)

class circular_buffer
{

public:
    circular_buffer() {}
    ~circular_buffer() {}

    // Peek functions do not affect the w/r indexes
    int peek_next_read(uint8_t *next_read);
    int peek_last_write(uint8_t *last_write);

    // Read functions will affect the read index
    int read_byte(uint8_t *out);
    /*! \brief Read len bytes from the buffer */
    int read(uint8_t *out_buffer, const unsigned int len);

    // Write functions will affect the write index
    enum class write_operation {
        whole, // Only write if sufficient space
        fill, // Write up to when the buffer is filled
        overflow // Write over oldest bytes if buffer is filled
    };
    /*! \return 0 if could successfully write, -1 otherwise
        \warning Will overwrite older data if the buffer is full.
            Use write() to safely write without overflow */
    int write_byte(const uint8_t byte);
    /*! \return Number of bytes successfully written */
    unsigned int write(const uint8_t *data, const unsigned int len,
        write_operation w_op);

    /*! \return nb of bytes available to write */
    unsigned int get_available_len() { return (Size - _content_len); }

private:
  uint8_t _buffer[Size];
  unsigned int _write_idx = 0;
  unsigned int _read_idx = 0;
  unsigned int _content_len = 0;
};


// Definition

template <unsigned int Size>
int circular_buffer<Size>::peek_next_read(uint8_t *next_read)
{
    if (_content_len == 0) { return -1; }
    *next_read = _buffer[_read_idx];
    return 0;
}

template <unsigned int Size>
int circular_buffer<Size>::peek_last_write(uint8_t *last_write)
{
    if (_content_len == 0) { return -1; }

    uint8_t last_w_idx = _write_idx == 0 ? Size - 1 : _write_idx - 1;
    *last_write =  _buffer[last_write];
    return 0;
}

template <unsigned int Size>
int circular_buffer<Size>::read_byte(uint8_t *out)
{
    if (_content_len == 0) { return -1; }

    uint8_t read_data = _buffer[_read_idx];
    _read_idx = _read_idx >= Size ? 0 : _read_idx + 1;
    _content_len--;
    *out = read_data;
    return 0;
}

template <unsigned int Size>
int circular_buffer<Size>::read(uint8_t *out_buffer, const unsigned int len)
{
    if (_content_len < len) { return -1; }
    if (!out_buffer) { return -1; }

    uint8_t n = _content_len;
    for (uint8_t i = 0; n > 0; n--, i++) {
        read_byte(&out_buffer[i]); // No need to check return, content length is enough
    }
}

template <unsigned int Size>
int circular_buffer<Size>::write_byte(const uint8_t byte)
{
    // if (_content_len >= Size) { return -1; }

    _buffer[_write_idx] = byte;
    if (++_write_idx >= Size) { _write_idx = 0; }
    if ( _content_len < Size) { _content_len++; }
    return 0;
}

template <unsigned int Size>
unsigned int circular_buffer<Size>::write(const uint8_t *data,
    const unsigned int len, write_operation w_op)
{
    unsigned int write_len = len;
    const unsigned int available = (Size - _content_len);
    if (available < len) {
        // Return if can't write the whole data block
        if (w_op == write_operation::whole) { return 0; }
        // Fill up to the amount available
        else if (w_op == write_operation::fill) {
            write_len = available;
        }
    }

    for (uint8_t i = 0; i < write_len; i++) {
        write_byte(data[i]);
    }

    return write_len;
}


#endif