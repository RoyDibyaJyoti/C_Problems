#include "hetero_list.h"
#include <stdio.h>

int main() {
    Node* head = NULL;
    Node* tail = NULL;

    append(&head, &tail, "123");
    append(&head, &tail, "45.67");
    append(&head, &tail, "true");
    append(&head, &tail, "a");
    append(&head, &tail, "0x1A3F");
    append(&head, &tail, "0b1011");
    append(&head, &tail, "hello world");
    append(&head, &tail, "-inf");
    append(&head, &tail, "0o755");

    print_list(head);

    free_list(&head);
    return 0;
}
