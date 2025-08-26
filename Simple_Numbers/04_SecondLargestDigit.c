#include<stdio.h>

int main(){
    int number;
    printf("Enter a number: ");
    if(scanf("%d", &number) != 1) {
        printf("Invalid input.\n");
        return 1;
    }
    if(number < 0) number = -number;
    int largest = -1, second_largest = -1;
    while(number > 0){
        int digit = number % 10;
        if(digit > largest){
            second_largest = largest;
            largest = digit;
        } else if(digit > second_largest && digit < largest){
            second_largest = digit;
        }
        number /= 10;
    }
    if(second_largest != -1){
        printf("Second largest digit is: %d\n", second_largest);
    } else {
        printf("No second largest digit found.\n");
    }
    return 0;
}