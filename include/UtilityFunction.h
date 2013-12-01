#ifndef UTILITYFUNCTION_H
#define UTILITYFUNCTION_H

#include <vector>
#include <climits>
#include <stack>

int correct_number_byte(int val) {
    int bytes = val;
    while((bytes%4) != 0)
        bytes++;

    return bytes;
}

std::vector<bool> char_to_bitset(char c) {
    std::vector<bool> bools;

    for (int i = CHAR_BIT - 1; i >= 0; --i)
        bools.push_back(c & (1 << i));

    return bools;
}

char bitset_to_char(std::vector<bool> bools) {
    char c = 0;

    for (int i = 0,pos = CHAR_BIT - 1; pos >= 0; --pos, i++) {
        if (bools[i])
            c = c | (1 << pos);
    }

    return c;
}

#endif // UTILITYFUNCTION_H
