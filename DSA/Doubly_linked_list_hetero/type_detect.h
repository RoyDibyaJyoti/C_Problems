#ifndef TYPE_DETECT_H
#define TYPE_DETECT_H

#include <stdbool.h>

typedef enum {
    TYPE_INT,
    TYPE_UINT,
    TYPE_LONG,
    TYPE_ULONG,
    TYPE_SHORT,
    TYPE_USHORT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_HEX,
    TYPE_OCTAL,
    TYPE_BINARY,
    TYPE_POINTER,
    TYPE_NAN,
    TYPE_INF,
    TYPE_NEG_INF
} Dtype;

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
} DataVal;

typedef struct {
    Dtype dtype;
    DataVal dval;
} Data;

Data type(const char* input);
const char* dtype_to_string(Dtype dtype);

#endif
