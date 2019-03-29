openssl enc -aes-128-cbc -e -in original.bmp -out cbc-cipher \
    -K 00112233445566778899aabbccddeeff \
    -iv 0102030405060708

openssl enc -aes-128-ecb -e -in original.bmp -out ecb-cipher \
    -K 00112233445566778899aabbccddeeff \
#    -iv 0102030405060708

mv cbc-cipher cbc-cipher.bmp
mv ecb-cipher ecb-cipher.bmp

