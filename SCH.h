#ifdef SCH_H
#define SCH_H

#include <ctype.h>
#include <string.h>

typedef char* string;

bool install_flag = 0;

struct {
    string hash_seed;
    string pri;
    string pub;
}

void hash_generator();
void pri_key_generator();
void pub_key_generator();
void sch_encryptor();
void sch_decryptor();
#endif