#include <iostream>
#include <string>
#include <cstring>
#include <openssl/evp.h>

void notify_error(std::string error_message)
{
    std::cerr << "Unexpected error: "  << error_message << std::endl;
    exit(1);
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *cipher_ctx;

    int len, ciphertext_len;

    /* Create and initialise the context */
    if (0 == (cipher_ctx = EVP_CIPHER_CTX_new())) {
        notify_error("EVP_CIPHER_CTX_new()");
    }

    if (0 == EVP_EncryptInit_ex(cipher_ctx, EVP_aes_128_cbc(), NULL, key, iv)) {
        notify_error("EVP_EncryptInit_ex()");
    }

    /* Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if (0 == EVP_EncryptUpdate(cipher_ctx, ciphertext, &len, plaintext, plaintext_len)) {
        notify_error("EVP_EncryptUpdate()");
    }
    ciphertext_len = len;

    /* Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if (0 == EVP_EncryptFinal_ex(cipher_ctx, ciphertext + len, &len)) {
        notify_error("EVP_EncryptFinal_ex()");
    }
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(cipher_ctx);

    std::cout <<  "Ciphertext: " << ciphertext << std::endl;

    return ciphertext_len;
}

void writeBuffer(unsigned char buffer, int len) {
    //std::cout.write(buffer, len);
}

int main()
{
    // Buffers
    unsigned char key[28], iv[28], ciphertext[28], decryptedtext[28], plaintext[28];

    // Zero init
    std::memset(key, 0, 28);
    std::memset(iv, 0, 28);
    std::memset(ciphertext, 0, 28);
    std::memset(decryptedtext, 0, 28);
    std::memset(plaintext, 0, 28);

    int decryptedtext_len, ciphertext_len;

    /* Encrypt the plaintext */
    ciphertext_len = encrypt(plaintext, strlen((char *)plaintext), key, iv,
                              ciphertext);

    /*
    BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);

    Decrypt the ciphertext
    decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv,
                                decryptedtext);
    */

    return 0;
}
