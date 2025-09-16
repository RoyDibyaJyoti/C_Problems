#include<stdio.h>
#include<stdlib.h>
#include<string.h>

const char* type(const char* x){
    if (x == NULL) return "NULL";
    char* endptr;

    strtol(x, &endptr, 10);
    if (*endptr == '\0') return "int";

    strtof(x, &endptr);
    if (*endptr == '\0') return "float";

    strtod(x, &endptr);
    if (*endptr == '\0') return "double";

    if (strlen(x) == 1) return "char";

    return "string";
}

int main(){
    char input[100];
    printf("Enter value to detect type: ");
    while(fgets(input, sizeof(input), stdin) != NULL){
        input[strcspn(input, "\n")] = 0;
        printf("Detected type: %s\n", type(input));
        printf("Enter value to detect type: ");
    }
    return 0;
}