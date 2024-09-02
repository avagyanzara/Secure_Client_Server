#include "encryption.h"

char* encrypt_message(char *message, const int size)
{
    for (int i = 0; i < size; ++i) {
        message[i] = (i & 1) ? rotate_bits_left(message[i], 3) : swap_bits(message[i], 1, 4);
    }
    return message;
}

char* decrypt_message(char *message, const int size)
{
    for (int i = 0; i < size; ++i) {
        message[i] = (i & 1) ? rotate_bits_right(message[i], 3) : swap_bits(message[i], 1, 4);            
    }
    return message;
}

