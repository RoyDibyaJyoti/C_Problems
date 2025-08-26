#include<stdio.h>

int main(){
    int number;
    printf("Enter a number: ");
    if(scanf("%d", &number) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    if(number < 0) number = -number;

    int largest = 0;
    while(number > 0){
        int digit = number % 10;
        if(digit > largest){
            largest = digit;
        }
        number /= 10;
    }
    printf("Largest digit is: %d\n", largest);

    return 0;
}