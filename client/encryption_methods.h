#ifndef ENCRYPTION_METHODS_H
#define ENCRYPTION_METHODS_H

#include <sys/types.h>
#include <string.h>

class encryption_methods
{
public:
    encryption_methods();
    int generate_key(char*);
    void decrypt_message(char*, int16_t);
    void encrypt_message(char*, int16_t);
};

#endif // ENCRYPTION_METHODS_H
