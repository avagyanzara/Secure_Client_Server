#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>

#define HARD_ASSERT(condition, message) do {              \
    if (!(condition)) {                                   \
        printf("%s, %s:%d", message, __FILE__, __LINE__); \
        abort();                                          \
    }                                                     \
} while (0)

// ERROR codes
#define MALLOC_FAILURE_CODE               1
#define INVALID_ARGUMENT_CODE             2
#define GET_ADDRESS_INFO_FAILURE_CODE     3
#define CONNECTION_ACCEPT_FAILURE_CODE    4
#define SOCKET_LISTEN_FAILURE_CODE        5
#define SOCKET_SEND_FAILURE_CODE          6
#define SOCKET_RECEIVE_FAILURE_CODE       7

void exit_with_error_message(const char *message, const int exit_code);

// Utility flags
#define LOCAL_BUFFER_SIZE 1024
#define DUMMY_FLAGS 0
#define BIT_COUNT 8

const char* timestamp();
char* copy_string(const char *c_string);

// Bit manipulators
unsigned char swap_bits(unsigned char symbol, const unsigned int bit_position1, const unsigned int bit_position2);
unsigned char rotate_bits_left(const unsigned char symbol, const unsigned int count);
unsigned char rotate_bits_right(const unsigned char symbol, const unsigned int count);

#endif // __UTILITY_H__
