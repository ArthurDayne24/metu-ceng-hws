#include <string>
#include <vector>

#ifndef __ATTACK__H__
#define __ATTACK__H__

/* Solve for key in aes-128-cbc
 * -> with plaintext size of 21
 * -> IV is all zeros
 * -> with a dictionary up to a size of 25600
 * -> with keys up to 16 chars
 */

class Attack {

private:
    static const int textBufferLen = 32, ivBufferLen = 16, wordDictBufferLen = 25600, realPlainttextLen = 21, keyBufferLen = 16;
    
    const char *ciphertextFname = "ciphertext", *plaintextFname = "plaintext.txt", *wordDictFname = "words.txt";
    
    unsigned char mCiphertext[textBufferLen], rCiphertext[textBufferLen], mPlaintext[textBufferLen], mIv[ivBufferLen];
    
    std::string mWordDict[wordDictBufferLen];
    int mWordDictLen = 0;

    // params: error message
    void notify_error(const std::string & );

    // encrypt mPlaintext by given key and compare with mCiphertext
    // params: key to be tried
    // return: true if succeeds else false
    bool key_trial(const std::string & );

public:
    Attack();
    // copy constructor and assignment operator is forbidden
    Attack(const Attack & ) = delete;
    Attack & operator=(const Attack & ) = delete;

    void solve();
};

#endif

