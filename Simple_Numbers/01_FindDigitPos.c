#include <stdio.h>

int main(){
    char num[20], digit[2];
    printf("Enter number and digit: ");
    scanf("%s %s", num, digit);

    for(int i = 0; num[i] != '\0'; i++){
        if(num[i] == digit[0]){
            printf("Position: %d\n", i + 1);
            return 0;
        }
    }
    printf("Digit not found in number.\n");
    return 0;
}