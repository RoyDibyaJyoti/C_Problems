#include <stdio.h>

int main(){
    int number;
    printf("Enter a number: ");
    if(scanf("%d", &number) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    if(number < 0) number = -number;

    int digits[10] = {0};
    while(number > 0){
        digits[number%10]++;
        number /= 10;
    }
    for(int i = 0; i < 10; i++) if(digits[i] != 0) printf("%2d is present %2d time(s).\n", i, digits[i]);

    return 0;
}