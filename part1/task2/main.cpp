#include <iostream>
#include <string>
#include <openssl/evp.h>

void notify_error(std::string error_message)
{
    std::err << "Unexpected error: "  << error_message << std::endl;
    exit(1);
}

int main()
{
    int len;
    int ciphertext_len;

    // Allocate cipher context
    EVP_CIPHER_CTX *cipher_ctx;
    if (!(cipher_ctx = EVP_CIPHER_CTX_new())) {
        notify_error("Cipher context could not be allocated");
    }

    EVP_CipherInit_ex(ctx);


    return 0;
}
