#include "attack.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <openssl/evp.h>
#include <string>
#include <vector>

Attack::Attack(const std::string  & pCiphertextFname, const std::string  & pPlaintextFname, 
        const std::string  & pWorddictFname, bool pDebug) :
    mCiphertextFname(pCiphertextFname), 
    mPlaintextFname(pPlaintextFname), 
    mWorddictFname(pWorddictFname),
    mDebug(pDebug)
{
    std::memset(mIv, 0, 16);
    // line iterator
    std::string line, plaintextRead;

    // read plaintext
    std::ifstream infile_plain(mPlaintextFname);
    while (std::getline(infile_plain, line)) {
        // TODO newline ? 
        plaintextRead += line;
    }

    mPlaintextLen = mCiphertextLen = line.length();

    if (mCiphertextLen % 16) {
        mCiphertextLen += 16 - (mCiphertextLen % 16);
        mPadding = true;
        mTours = (mCiphertextLen / 16) - 1;
    }
    else {
        mPadding = false;
        mTours = mCiphertextLen / 16;
    }
    
    mCiphertext = new unsigned char[mCiphertextLen*20];
    rCiphertext = new unsigned char[mCiphertextLen*20];
    mPlaintext = new unsigned char[mPlaintextLen*20];

    std::memset(mCiphertext, 0, mCiphertextLen);
    std::memset(rCiphertext, 0, mCiphertextLen);
    std::memset(mPlaintext, 0, mPlaintextLen);

    // write plaintext
    std::copy(plaintextRead.begin(), plaintextRead.end(), mPlaintext);

    // read/write ciphertext
    std::ifstream infile_cipher(mCiphertextFname, std::ios::binary);
    for (int i = 0; i < mCiphertextLen; i++) {
        c = infile_cipher.get();
        mCiphertext[i] = c;
    }

    mWorddict.reserve(25600);

    // read words
    std::ifstream infile_word(mWorddictFname);
    
    while (std::getline(infile_word, line)) {
        mWorddict.push_back(line);
    }
}

void Attack::notify_error(const std::string & error_message)
{
    if (true == mDebug) {
        std::cerr << "Unexpected error: " << error_message << std::endl;
        exit(0);
    }
}

bool Attack::key_trial(const std::string & pKey)
{
    static unsigned char key[16];
    std::memset(key, 0, 16);
    
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
    
    for (int i = 0; i < mTours; i++) {
        if (0 == EVP_EncryptUpdate(cipher_ctx, rCiphertext + rCiphertext_len, &cur_len, mPlaintext, mPlaintextLen)) {
            notify_error("EVP_EncryptUpdate()");
        }
    
        rCiphertext_len += cur_len;
    }

    if (true == mPadding) {
        if (0 == EVP_EncryptFinal_ex(cipher_ctx, rCiphertext + rCiphertext_len, &cur_len)) {
            notify_error("EVP_EncryptFinal_ex()");
        }
    
        rCiphertext_len += cur_len;
    }

    /* Clean up */
    EVP_CIPHER_CTX_free(cipher_ctx);

    return rCiphertext_len == mCiphertextLen &&
        0 == std::memcmp(rCiphertext, mCiphertext, mCiphertextLen);
}

void Attack::solve()
{
    for (const std::string & key: mWorddict) {
        if (true == key_trial(key)) {
            // TODO ask
            std::cout << key;
            return;
        }
    }

    notify_error("No solution exists");
}

Attack::~Attack()
{
    delete[] mPlaintext;
    delete[] mCiphertext;
    delete[] rCiphertext;
}

