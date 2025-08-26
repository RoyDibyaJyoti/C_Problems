#include<stdio.h>

int main(){
    int number, kth_largest;
    printf("Enter number and k: ");
    scanf("%d %d", &number, &kth_largest);
    int digits[10] = {0};
    if(number < 0) number = -number;
    while(number > 0){
        int digit = number % 10;
        digits[digit] = 1;
        number /= 10;
    }
    int count = 0;
    for(int i = 9; i >= 0; i--){
        if(digits[i]){
            count++;
            if(count == kth_largest){
                printf("%dth largest digit is: %d\n", kth_largest, i);
                return 0;
            }
        }
    }
    printf("Less than %d unique digits found.\n", kth_largest);
    return 0;
}