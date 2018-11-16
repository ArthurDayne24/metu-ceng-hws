from __future__ import print_function
import sys
import vulnerable

def modify_string(string, index, char):
    return string[:index] + char + string[index+1:]

if __name__ == '__main__':

    C = sys.argv[1]
    DP = sys.argv[2] + chr(1)

    DP_len = len(DP)
    DP_start_index = 16 - DP_len
    
    C1, C2, C3 = C[:16], C[16:32], C[32:48]

    possible_bytes = [chr(i) for i in range(256)]

    T = "*" * 16

    for pad_len, i in enumerate(reversed(range(DP_start_index, 16))):
        # update T[i]
        pad_len += 1
        old_byte = C2[i]
        l_new_byte = []

        temp_C2 = C2

        for j in range(i+1, 16):
            temp_C2 = modify_string(temp_C2, j, chr(ord(T[j]) ^ pad_len))

        for byte in possible_bytes:
            if vulnerable.decr(C1 + modify_string(temp_C2, i, byte) + C3) == "SUCCESS":
                l_new_byte.append(byte)

        if len(l_new_byte) == 1:
            new_byte = l_new_byte[0]
        elif l_new_byte[0] == old_byte:
            new_byte = l_new_byte[1]
        else:
            new_byte = l_new_byte[0]

        new_byte = chr(ord(new_byte) ^ pad_len)
        T = modify_string(T, i, new_byte)

    DP = "*" * (16 - DP_len) + DP

    for i in range(DP_start_index, 16):
        C2 = modify_string(C2, i, chr(ord(DP[i]) ^ ord(T[i])))

    print(C1 + C2 + C3, end='')
    
