#ifndef HETERO_LIST_H
#define HETERO_LIST_H

#include "type_detect.h"

typedef union {
    int INT;
    unsigned int UINT;
    long LONG;
    unsigned long ULONG;
    short SHORT;
    unsigned short USHORT;
    float FLOAT;
    double DOUBLE;
    bool BOOL;
    char CHAR;
    char* STRING;
    unsigned long HEX;
    unsigned long OCTAL;
    unsigned long BINARY;
    void* POINTER;
} Value;

typedef struct Node {
    Dtype dtype;
    Value val;
    struct Node* prev;
    struct Node* next;
} Node;

void append(Node** head, Node** tail, const char* input);
void print_list(const Node* head);
void free_list(Node** head);

#endif
