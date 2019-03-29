#include "attack.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <openssl/evp.h>
#include <string>
#include <vector>

Attack::Attack()
{
    std::memset(mCiphertext, 0, textBufferLen);
    std::memset(rCiphertext, 0, textBufferLen);
    std::memset(mPlaintext, 0, textBufferLen);
    std::memset(mIv, 0, ivBufferLen);
    
    char c;

    // read/write plaintext
    std::ifstream infile_plain(plaintextFname, std::ios::binary);
    for (int i = 0; i < realPlainttextLen; i++) {
        c = infile_plain.get();
        mPlaintext[i] = c;
    }

    // read/write ciphertext
    std::ifstream infile_cipher(ciphertextFname, std::ios::binary);
    for (int i = 0; i < textBufferLen; i++) {
        c = infile_cipher.get();
        mCiphertext[i] = c;
    }

    // read words
    std::ifstream infile_word(wordDictFname);
    std::string line;
    
    while (std::getline(infile_word, line)) {
        if (line.length() < keyBufferLen) {
            mWordDict[mWordDictLen++] = line;
        }
    }
}

void Attack::notify_error(const std::string & error_message)
{
    std::cerr << "Unexpected error: " << error_message << std::endl;
}

bool Attack::key_trial(const std::string & pKey)
{
    static unsigned char key[keyBufferLen];
    std::memset(key, 0, keyBufferLen);
    
    std::copy(pKey.begin(), pKey.end(), key);

    EVP_CIPHER_CTX *cipher_ctx;

    /* Create and initialise the context */
    if (0 == (cipher_ctx = EVP_CIPHER_CTX_new())) {
        notify_error("EVP_CIPHER_CTX_new()");
    }

    if (0 == EVP_EncryptInit_ex(cipher_ctx, EVP_aes_128_cbc(), NULL, key, mIv)) {
        notify_error("EVP_EncryptInit_ex()");
    }

    int cur_len, rCiphertext_len = 0;
    
    if (0 == EVP_EncryptUpdate(cipher_ctx, rCiphertext, &cur_len, mPlaintext, realPlainttextLen)) {
        notify_error("EVP_EncryptUpdate()");
    }

    rCiphertext_len += cur_len;

    if (0 == EVP_EncryptFinal_ex(cipher_ctx, rCiphertext + rCiphertext_len, &cur_len)) {
        notify_error("EVP_EncryptFinal_ex()");
    }

    rCiphertext_len += cur_len;

    /* Clean up */
    EVP_CIPHER_CTX_free(cipher_ctx);

    return rCiphertext_len == textBufferLen &&
        0 == std::memcmp(rCiphertext, mCiphertext, textBufferLen);
}

void Attack::solve()
{
    for (int i = 0; i < mWordDictLen; i++) {
        const std::string & key = mWordDict[i];
        if (true == key_trial(key)) {
            std::cout << key;
            return;
        }
    }

    notify_error("No solution exists");
}

