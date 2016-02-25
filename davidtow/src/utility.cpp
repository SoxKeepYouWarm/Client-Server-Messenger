#include "utility.h"


int atoi(char* str) {
    int res = 0; 
  
    for (int i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';
  
    return res;
}


int str_equals(char* a, char* b) {
    
    if ((a[0] == '\0') ^ (b[0] == '\0')) {return 0;} // catch single empty string
    for (int i = 0; (a[i] != '\0' && b[i] != '\0'); i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}