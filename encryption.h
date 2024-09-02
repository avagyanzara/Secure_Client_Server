#ifndef _ENCRYPTION_H_
#define _ENCRYPTION_H_

#include "utility.h"

char* encrypt_message(char *message, const int size);
char* decrypt_message(char *message, const int size);

#endif // _ENCRYPTION_H_
