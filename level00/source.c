#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("***********************************\n");
    printf("* 	 	    -Level00 -			  *\n");
    printf("***********************************\n");
    printf("Password: ");
    int num;
    scanf("%d", &num);
    if (num == 5276) {
        printf("Authenticated!\n");
        system("/bin/sh");
        return 0;
    } else {
        printf("Invalid Password!\n");
        return 1;
    }
}