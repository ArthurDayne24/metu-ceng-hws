#include <openssl/evp.h>

int main() {

    unsigned char *key = (unsigned char *) "00112233445566778899aabbccddeeff";
    unsigned char *iv = (unsigned char *) "0102030405060708";

    unsigned char *plaintext = (unsigned char *) 
        "The quick brown fox jumps over the lazy dog";

    unsigned char ciphertext[128];
    unsigned char decryptedtext[128];

    int decryptedtext_len, ciphertext_len;

    ciphertext_len = encrypt(plaintext, strlen((unsigned char *) plaintext), 
            key, iv, ciphertext);
    decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, decryptedtext);

    decryptedtext[decryptedtext_len]

    return 0;
}

