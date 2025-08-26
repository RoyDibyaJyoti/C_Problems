#include <stdio.h>

int find(int number, int digit){
    int position = 1;
    while(number > 0){
        if(number % 10 == digit){
            return position;
        }
        number /= 10;
        position++;
    }
    return -1;
}

int main(){
    int number, digit;
    printf("Enter number and digit: ");
    scanf("%d %d", &number, &digit);

    int pos = find(number, digit);
    if(pos != -1){
        printf("Position: %d\n", pos);
    } else {
        printf("Digit not found in number.\n");
    }
    return 0;
}