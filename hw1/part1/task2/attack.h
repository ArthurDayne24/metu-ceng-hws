#include <string>
#include <vector>

#ifndef __ATTACK__H__
#define __ATTACK__H__

// Solve for key in aes-128-cbc
class Attack {

private:
    const static int bufferSize = 128;

    unsigned char
        mCiphertext[bufferSize], 
        mDecryptedtext[bufferSize],
        mIv[bufferSize], 
        mPlaintext[bufferSize];

    int mCiphertextLen, mPlaintextLen;

    std::string mCiphertextFname, mPlaintextFname, mWorddictFname;
    
    bool mDebug;

    std::vector<std::string> mWorddict;

    // read file to buffer
    // TODO assume one line input for now
    // params: file name, buffer
    // return: number of bytes read from file
    int read_to_buffer(const std::string & , unsigned char[]);

    // read word dictionary into string vector
    void read_word_dictionary();

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

    void solve();
};

#endif

