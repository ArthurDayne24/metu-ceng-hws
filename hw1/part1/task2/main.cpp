#include "attack.h"

int main()
{
    Attack attack("ciphertext", "plaintext.txt", "words.txt", true);

    attack.solve();
    
    return 0;
}

