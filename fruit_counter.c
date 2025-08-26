#include <stdio.h>
#include <string.h>

typedef struct {
    char fruit[20];
    int count;
} FruitCount;

int main(){
    char sentence[150];
    printf("Enter fruits: ");
    fgets(sentence, sizeof(sentence), stdin);

    FruitCount fruitCounts[10] = {0};
    char *token = strtok(sentence, " ,.-\n");
    int fruitIndex = 0;
    while(token != NULL){
        int found = 0;
        for(int i = 0; i < fruitIndex; i++){
            if(strcmp(fruitCounts[i].fruit, token) == 0){
                fruitCounts[i].count++;
                found = 1;
                break;
            }
        }
        if(!found && fruitIndex < 10){
            strcpy(fruitCounts[fruitIndex].fruit, token);
            fruitCounts[fruitIndex].count = 1;
            fruitIndex++;
        }
        token = strtok(NULL, " ,.-\n");
    }
    for(int i = 0; i < fruitIndex; i++){
        printf("%-20s: %5d\n", fruitCounts[i].fruit, fruitCounts[i].count);
    }
    return 0;
}