#include "attack.h"

int main()
{
    Attack("ciphertext", "plaintext.txt", "words.txt", true).solve();
    
    return 0;
}

