/*
Written by jcsb1994
Licensed under the Apache License, Version 2.0 (the "License");
*/


#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>
#include <string.h>

template <unsigned int Size>
class circular_buffer
{

public:
    circular_buffer() {
        for (std::size_t i = 0; i < Size; i++) {
            _buffer[i] = 0;
        }
    }

    ~circular_buffer() {}

    // Peek functions do not affect the w/r indexes
    uint8_t peek_next_read();
    uint8_t peek_last_write();
    // Read functions will affect the read index
    uint8_t read_byte();
    /*! \brief Read len bytes from the buffer */
    uint8_t read(uint8_t *out_buffer, const unsigned int len);
    // Write functions will affect the write index
    uint8_t write_byte(uint8_t byte);
    /*! \return 0 if could successfully write, -1 otherwise */
    int8_t write(uint8_t *data, const unsigned int len);
    /*! \return nb of bytes available to write */
    uint8_t get_available_len();

private:
  uint8_t _buffer[Size];
  unsigned int _write_idx = 0;
  unsigned int _read_idx = 0;
  unsigned int _content_len = 0;
};


#endif

