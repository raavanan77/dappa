#ifndef SCH_H
#define SCH_H

#include <ctype.h>
#include <string.h>

typedef char* string;

bool install_flag = 0;

struct hasher {
    string hash_seed;
    string pri;
    string pub;
};

#endif // SCH_H