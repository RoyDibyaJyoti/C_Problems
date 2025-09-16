#include "hetero_list.h"
#include "type_detect.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static Value data_to_value(const Data* data) {
    Value val = {0};
    switch (data->dtype) {
        case TYPE_INT: val.INT = data->dval.INT; break;
        case TYPE_UINT: val.UINT = data->dval.UINT; break;
        case TYPE_LONG: val.LONG = data->dval.LONG; break;
        case TYPE_ULONG: val.ULONG = data->dval.ULONG; break;
        case TYPE_SHORT: val.SHORT = data->dval.SHORT; break;
        case TYPE_USHORT: val.USHORT = data->dval.USHORT; break;
        case TYPE_FLOAT: val.FLOAT = data->dval.FLOAT; break;
        case TYPE_DOUBLE: val.DOUBLE = data->dval.DOUBLE; break;
        case TYPE_BOOL: val.BOOL = data->dval.BOOL; break;
        case TYPE_CHAR: val.CHAR = data->dval.CHAR; break;
        case TYPE_STRING: val.STRING = strdup(data->dval.STRING); break;
        case TYPE_HEX: val.HEX = data->dval.HEX; break;
        case TYPE_OCTAL: val.OCTAL = data->dval.OCTAL; break;
        case TYPE_BINARY: val.BINARY = data->dval.BINARY; break;
        case TYPE_POINTER: val.POINTER = data->dval.POINTER; break;
        default: val.STRING = strdup("unknown"); break;
    }
    return val;
}

void append(Node** head, Node** tail, const char* input) {
    Data d = type(input);

    Node* node = (Node*)malloc(sizeof(Node));
    node->dtype = d.dtype;
    node->val = data_to_value(&d);
    node->prev = *tail;
    node->next = NULL;

    if (*tail) {
        (*tail)->next = node;
    }
    *tail = node;

    if (!*head) {
        *head = node;
    }

    if (d.dtype == TYPE_STRING) {
        free(d.dval.STRING);
    }
}

void print_list(const Node* head) {
    const Node* curr = head;
    while (curr) {
        printf("Type: %s, Value: ", dtype_to_string(curr->dtype));
        switch (curr->dtype) {
            case TYPE_INT: printf("%d", curr->val.INT); break;
            case TYPE_UINT: printf("%u", curr->val.UINT); break;
            case TYPE_LONG: printf("%ld", curr->val.LONG); break;
            case TYPE_ULONG: printf("%lu", curr->val.ULONG); break;
            case TYPE_SHORT: printf("%d", curr->val.SHORT); break;
            case TYPE_USHORT: printf("%u", curr->val.USHORT); break;
            case TYPE_FLOAT: printf("%f", curr->val.FLOAT); break;
            case TYPE_DOUBLE: printf("%lf", curr->val.DOUBLE); break;
            case TYPE_BOOL: printf("%s", curr->val.BOOL ? "true" : "false"); break;
            case TYPE_CHAR: printf("'%c'", curr->val.CHAR); break;
            case TYPE_STRING: printf("\"%s\"", curr->val.STRING); break;
            case TYPE_HEX: printf("0x%lx", curr->val.HEX); break;
            case TYPE_OCTAL: printf("0o%lo", curr->val.OCTAL); break;
            case TYPE_BINARY: {
                unsigned long val = curr->val.BINARY;
                printf("0b");
                int started = 0;
                for (int i = sizeof(unsigned long)*8 - 1; i >= 0; i--) {
                    int bit = (val >> i) & 1;
                    if (bit) started = 1;
                    if (started) printf("%d", bit);
                }
                if (!started) printf("0");
                break;
            }
            case TYPE_POINTER: printf("%p", curr->val.POINTER); break;
            case TYPE_NAN: printf("NaN"); break;
            case TYPE_INF: printf("Infinity"); break;
            case TYPE_NEG_INF: printf("-Infinity"); break;
            default: printf("Unknown"); break;
        }
        printf("\n");
        curr = curr->next;
    }
}

void free_list(Node** head) {
    Node* curr = *head;
    while (curr) {
        Node* next = curr->next;
        if (curr->dtype == TYPE_STRING && curr->val.STRING) {
            free(curr->val.STRING);
        }
        free(curr);
        curr = next;
    }
    *head = NULL;
}
