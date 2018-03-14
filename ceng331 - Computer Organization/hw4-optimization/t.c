#include <stdio.h>
int main() {    
    int n = 8; // 1 2 3 4 5 6 7
    for (int m = n; --m; )
        printf("%d\n", m);
    return 0;
}
