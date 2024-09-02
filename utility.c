#include "utility.h"

void exit_with_error_message(const char *message, const int exit_code)
{
    fprintf(stderr, "Error %d: %s\n", exit_code, message);
    exit(exit_code);
}

char* copy_string(const char *c_string)
{
    const unsigned int length = strlen(c_string);
    char *buffer = (char*)malloc(length + 1);
    if (buffer == NULL) {
        exit_with_error_message("Failed to allcate memory", MALLOC_FAILURE_CODE);
    }

    strcpy(buffer, c_string);
    buffer[length] = '\0';
    return buffer;
}

const char* timestamp()
{
    const time_t current_time = time(NULL);
    char* time_string = asctime(localtime(&current_time));
    const int size = strlen(time_string);
    HARD_ASSERT(size > 0, "Invalid timestamp");
    time_string[size - 1] = '\0'; 
    return time_string;
}

unsigned char swap_bits(unsigned char symbol, const unsigned int bit_position1, const unsigned int bit_position2)
{
    const unsigned char is_bit1_set = (symbol >> bit_position1) & 1;
    const unsigned char is_bit2_set = (symbol >> bit_position2) & 1;
    if (is_bit1_set == is_bit2_set) {
        return symbol;
    }

    symbol ^= (1 << bit_position1);
    symbol ^= (1 << bit_position2);
    return symbol;
}

unsigned char rotate_bits_left(const unsigned char symbol, const unsigned int count)
{
    HARD_ASSERT(count < BIT_COUNT, "The bit count is large");
    return ((symbol << count) | (symbol >> (BIT_COUNT - count)));
}

unsigned char rotate_bits_right(const unsigned char symbol, const unsigned int count)
{
    HARD_ASSERT(count < BIT_COUNT, "The bit count is large");
    return ((symbol >> count) | (symbol << (BIT_COUNT - count)));
}

