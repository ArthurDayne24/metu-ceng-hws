#include <string>
#include <vector>

#ifndef __ATTACK__H__
#define __ATTACK__H__

/* Solve for key in aes-128-cbc
 * -> at any size of plaintext
 * -> IV is all zeros
 * -> with a dictionary up to a size of 25600
 * -> with keys up to 16 chars
 */

// TODO put constants

class Attack {

private:
    unsigned char *mCiphertext, *rCiphertext, *mPlaintext, mIv[16];

    int mCiphertextLen, mPlaintextLen;

    std::string mCiphertextFname, mPlaintextFname, mWorddictFname;
    
    bool mDebug;

    bool mPadding;

    // number of times to call EVP_EncryptUpdate()
    int mTours;

    std::vector<std::string> mWorddict;

    // params: error message
    void notify_error(const std::string & );

    // encrypt mPlaintext by given key and compare with mCiphertext
    // params: key trial
    // return: true if succeeds else false
    bool key_trial(const std::string & );

public:
    // params: ciphertext file name, plaintext file name, worddict file name 
    Attack(const std::string & , const std::string & , const std::string & , bool);
    // copy constructor and assignment operator is forbidden
    Attack(const Attack & ) = delete;
    Attack & operator=(const Attack & ) = delete;

    ~Attack();

    void solve();
};

#endif

