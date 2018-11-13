#include "attack.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <openssl/evp.h>
#include <string>
#include <vector>

Attack::Attack(const std::string & pCiphertextFname, const std::string & pPlaintextFname, 
        const std::string & pWorddictFname, bool pDebug) :
    mCiphertextFname(pCiphertextFname), 
    mPlaintextFname(pPlaintextFname), 
    mWorddictFname(pWorddictFname),
    mDebug(pDebug)
{
    std::memset(mCiphertext, 0, bufferSize);
    std::memset(mDecryptedtext, 0, bufferSize);
    std::memset(mIv, 0, bufferSize);
    std::memset(mPlaintext, 0, bufferSize);

    mCiphertextLen = read_to_buffer(mCiphertextFname, mCiphertext);
    mPlaintextLen = read_to_buffer(mPlaintextFname, mPlaintext);

    mWorddict.reserve(25600);

    read_word_dictionary();
}

int Attack::read_to_buffer(const std::string & pFname, unsigned char pBuff[])
{
    std::ifstream infile(pFname);
    std::string line;
    
    if (!std::getline(infile, line)) {
        notify_error("Could not read " + pFname);
    }

    int read_length = line.length();

    if (read_length > bufferSize) {
        notify_error("String read from " + pFname + " is too long");
    }

    const char * line_c_str = line.c_str();
    std::copy(line_c_str, line_c_str +  read_length, pBuff);

    return read_length;   
}

void Attack::read_word_dictionary()
{
    std::ifstream infile(mWorddictFname);
    std::string line;
    
    if (!infile) {
        notify_error("File does not exits");
    }
    
    do {
        std::getline(infile, line);
        mWorddict.push_back(line);
    } while (line != "");
}

void Attack::notify_error(const std::string & error_message)
{
    if (true == mDebug) {
        std::cerr << "Unexpected error: "  << error_message << std::endl;
        exit(1);
    }
}

bool Attack::key_trial(const std::string & pKey)
{
    static unsigned char ciphertext[bufferSize], key[bufferSize];
    
    std::memset(ciphertext, 0, bufferSize);
    std::memset(key, 0, bufferSize);

    int ciphertext_len;
    
    const char * pKey_c_str = pKey.c_str();
    std::copy(pKey_c_str, pKey_c_str + pKey.length(), key);

    EVP_CIPHER_CTX *cipher_ctx;

    /* Create and initialise the context */
    if (0 == (cipher_ctx = EVP_CIPHER_CTX_new())) {
        notify_error("EVP_CIPHER_CTX_new()");
    }

    if (0 == EVP_EncryptInit_ex(cipher_ctx, EVP_aes_128_cbc(), NULL, key, mIv)) {
        notify_error("EVP_EncryptInit_ex()");
    }
    
    /* Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if (0 == EVP_EncryptUpdate(cipher_ctx, ciphertext, &ciphertext_len, mPlaintext, mPlaintextLen)) {
        notify_error("EVP_EncryptUpdate()");
    }

    int additional_len;

    /* Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if (0 == EVP_EncryptFinal_ex(cipher_ctx, ciphertext + ciphertext_len, &additional_len)) {
        notify_error("EVP_EncryptFinal_ex()");
    }
    ciphertext_len += additional_len;

    /* Clean up */
    EVP_CIPHER_CTX_free(cipher_ctx);

    return ciphertext_len == mCiphertextLen &&
        0 == std::memcmp(ciphertext, mCiphertext, mCiphertextLen);
}

void Attack::solve()
{
    for (const std::string & key: mWorddict) {
        if (true == key_trial(key)) {
            std::cout << key << std::endl;
            return;
        }
    }

    notify_error("No solution exists");
}

