import sys
import vulnerable

def xor_chars(x, y, index):
    return chr(ord(x[index]) ^ ord(y[index]))

if __name__ == '__main__':

    C = sys.argv[1]
    DP = sys.argv[2]

    C1, C2, C3 = C

    DP_real_len = len(DP)
    DP_pad_len = 16 - DP_real_len

    DP_padded = pkcs7(DP) 

    new_T = ""

    for i in range(DP_real_len):
        new_T += xor_chars(DP_padded, C2, i)

    for i in range(DP_pad_len):
        new_T += xor_chars(DP_padded, C2, i + DP_real_len)
    
    C2_new = ""

    for i in range(16):
        C2_new += xor_chars(new_T, DP_padded)

    print C1 + C2_new + C3
        
