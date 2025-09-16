#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_STRUCT
} Dtype;

typedef struct {
    void* data;
    size_t size;
} Array;

typedef struct {
    void* data;
    size_t size;
} Struct;

typedef union {
    int i;
    float f;
    double d;
    char c;
    bool b;
    char* s;
    Array arr;
    Struct st;
} Value;

typedef struct Node {
    Dtype type;
    Value val;
    struct Node* next;
    struct Node* prev;
} Node;

Node* create_node(Dtype type, Value val) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->type = type;
    node->next = node->prev = NULL;
    if (type == TYPE_STRING && val.s) {
        node->val.s = strdup(val.s);
    } else if (type == TYPE_ARRAY) {
        node->val.arr.data = malloc(val.arr.size);
        memcpy(node->val.arr.data, val.arr.data, val.arr.size);
        node->val.arr.size = val.arr.size;
    } else if (type == TYPE_STRUCT) {
        node->val.st.data = malloc(val.st.size);
        memcpy(node->val.st.data, val.st.data, val.st.size);
        node->val.st.size = val.st.size;
    } else {
        node->val = val;
    }
    return node;
}

void append_node(Node** head, Node** tail, Dtype type, Value val) {
    Node* node = create_node(type, val);
    if (!*head) {
        *head = *tail = node;
    } else {
        (*tail)->next = node;
        node->prev = *tail;
        *tail = node;
    }
}

void _append_int(Node** head, Node** tail, int x) {
    Value v; v.i = x;
    append_node(head, tail, TYPE_INT, v);
}
void _append_float(Node** head, Node** tail, float x) {
    Value v; v.f = x;
    append_node(head, tail, TYPE_FLOAT, v);
}
void _append_double(Node** head, Node** tail, double x) {
    Value v; v.d = x;
    append_node(head, tail, TYPE_DOUBLE, v);
}
void _append_char(Node** head, Node** tail, char x) {
    Value v; v.c = x;
    append_node(head, tail, TYPE_CHAR, v);
}
void _append_bool(Node** head, Node** tail, bool x) {
    Value v; v.b = x;
    append_node(head, tail, TYPE_BOOL, v);
}
void _append_string(Node** head, Node** tail, const char* x) {
    Value v; v.s = (char*)x;
    append_node(head, tail, TYPE_STRING, v);
}
void _append_array(Node** head, Node** tail, void* data, size_t size) {
    Value v; v.arr.data = data; v.arr.size = size;
    append_node(head, tail, TYPE_ARRAY, v);
}
void _append_struct(Node** head, Node** tail, void* data, size_t size) {
    Value v; v.st.data = data; v.st.size = size;
    append_node(head, tail, TYPE_STRUCT, v);
}

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define append(head, tail, val) _Generic((val),       \
    int:        _append_int,                          \
    float:      _append_float,                        \
    double:     _append_double,                       \
    char:       _append_char,                         \
    bool:       _append_bool,                         \
    char*:      _append_string,                       \
    const char*: _append_string                       \
)(head, tail, val)
#endif

void print_node(Node* n) {
    switch (n->type) {
        case TYPE_INT: printf("int: %d\n", n->val.i); break;
        case TYPE_FLOAT: printf("float: %.6f\n", n->val.f); break;
        case TYPE_DOUBLE: printf("double: %.10g\n", n->val.d); break;
        case TYPE_CHAR: printf("char: '%c'\n", n->val.c); break;
        case TYPE_BOOL: printf("bool: %s\n", n->val.b ? "true" : "false"); break;
        case TYPE_STRING: printf("string: \"%s\"\n", n->val.s); break;
        case TYPE_ARRAY: printf("array[%zu bytes]\n", n->val.arr.size); break;
        case TYPE_STRUCT: printf("struct[%zu bytes]\n", n->val.st.size); break;
    }
}

void print_list(Node* head) {
    while (head) {
        print_node(head);
        head = head->next;
    }
}

Node* get_node(Node* head, int index) {
    int i = 0;
    while (head && i < index) {
        head = head->next;
        i++;
    }
    return head;
}

Node* find_first(Node* head, Dtype type) {
    while (head) {
        if (head->type == type) return head;
        head = head->next;
    }
    return NULL;
}

void free_list(Node* head) {
    while (head) {
        Node* next = head->next;
        if (head->type == TYPE_STRING) free(head->val.s);
        else if (head->type == TYPE_ARRAY) free(head->val.arr.data);
        else if (head->type == TYPE_STRUCT) free(head->val.st.data);
        free(head);
        head = next;
    }
}

typedef struct {
    char name[32];
    int age;
} Person;

int main() {
    Node* head = NULL;
    Node* tail = NULL;

    append(&head, &tail, 42);
    append(&head, &tail, 3.14f);
    append(&head, &tail, 2.718281828);
    append(&head, &tail, "hello");
    append(&head, &tail, 'Z');
    append(&head, &tail, true);

    int arr[] = {1, 2, 3, 4};
    _append_array(&head, &tail, arr, sizeof(arr));

    Person p = {"Alice", 30};
    _append_struct(&head, &tail, &p, sizeof(Person));

    print_list(head);
    Node* third = get_node(head, 2);
    if (third) print_node(third);

    Node* first_string = find_first(head, TYPE_STRING);
    if (first_string) print_node(first_string);

    free_list(head);
    return 0;
}
