#include "encryption_methods.h"

encryption_methods::encryption_methods()
{

}


int encryption_methods::generate_key(char *username)
{
    int key = 0;
    for (int i = 0; i < strlen(username); key += username[i++]);
    return (key * strlen(username)) % 128;
}

void encryption_methods::decrypt_message(char *message, int16_t key)
{
    for (int i = 0; i < strlen(message); message[i++] -= key);
}

void encryption_methods::encrypt_message(char *message, int16_t key)
{
    for (int i = 0; i < strlen(message); message[i++] += key);
}
