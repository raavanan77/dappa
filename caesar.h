#ifndef CAESAER_H
#define CAESAER_H

#include <ctype.h>
#include <string.h>

// Constants
#define SMALLCAP 32
#define ALPHALEN 26

// Type definitions
typedef char* string;

// Function declarations
void caesar_encrypt(int key, string word);
void caesar_decrypt(int key, string word);
void sub_encrypt(string key, string word);
void sub_decrypt(string key, string word);

// Implementation
void caesar_encrypt(int key, string word)
{
    for (int i = 0, n = strlen(word); i < n; i++)
    {
        char letter = word[i];
        if (isalpha(letter))
        {
            if (isupper(letter))
            {
                char let = letter + key;
                if (let > 'Z')
                {
                    let -= ALPHALEN;
                }
                word[i] = let;
            }
            else if (islower(letter))
            {
                char slet = letter - SMALLCAP;
                slet = slet + key;
                if (slet > 'Z')
                {
                    slet -= ALPHALEN;
                }
                word[i] = slet + SMALLCAP;
            }
        }
    }
}

void caesar_decrypt(int key, string word)
{
    // Decrypt by using negative key
    caesar_encrypt(-key, word);
}

void sub_encrypt(string key, string word)
{
    for (int i = 0, n = strlen(word); i < n; i++)
    {
        char index = 0;
        if (isupper(word[i]))
        {
            index = key[word[i] - 65];
            word[i] = key[index];
        }
        else if (islower(word[i]))
        {
            char small = word[i] - 32;
            index = key[small - 65];
            word[i] = key[index + 32];
        }
    }
}

void sub_decrypt(string key, string word){

}

#endif // CAESAER_H
