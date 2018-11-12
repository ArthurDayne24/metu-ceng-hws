#include "attack.h"

int main()
{
    Attack("ciphertext", "plaintext.txt", "words.txt").solve();
    
    return 0;
}

