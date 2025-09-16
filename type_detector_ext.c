#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>

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

// Trim leading and trailing spaces in place
static void trim(char* str) {
    char* start = str;
    while (isspace((unsigned char)*start)) start++;
    if (start != str) memmove(str, start, strlen(start) + 1);
    char* end = str + strlen(str) - 1;
    while (end >= str && isspace((unsigned char)*end)) *end-- = '\0';
}

// Case-insensitive string compare
static bool str_casecmp(const char* a, const char* b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return false;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

// Check if string is boolean literal
static bool is_bool(const char* s, bool* val) {
    if (str_casecmp(s, "true")) {
        *val = true;
        return true;
    }
    if (str_casecmp(s, "false")) {
        *val = false;
        return true;
    }
    return false;
}

// Check if string is special float literal
static Dtype is_special_float(const char* s) {
    if (str_casecmp(s, "nan")) return TYPE_NAN;
    if (str_casecmp(s, "inf") || str_casecmp(s, "+inf") || str_casecmp(s, "infinity") || str_casecmp(s, "+infinity")) return TYPE_INF;
    if (str_casecmp(s, "-inf") || str_casecmp(s, "-infinity")) return TYPE_NEG_INF;
    return -1;
}

// Check binary literal syntax: 0b or 0B followed by 0/1
static bool is_binary_literal(const char* s) {
    if (strlen(s) > 2 && s[0] == '0' && (s[1] == 'b' || s[1] == 'B')) {
        for (size_t i = 2; s[i]; i++) {
            if (s[i] != '0' && s[i] != '1') return false;
        }
        return true;
    }
    return false;
}

// Convert binary string literal to unsigned long
static unsigned long binary_to_ulong(const char* s) {
    unsigned long val = 0;
    for (size_t i = 2; s[i]; i++) {
        val <<= 1;
        if (s[i] == '1') val |= 1;
    }
    return val;
}

// Parse integer suffixes, returns suffix length and fills flags
static int parse_int_suffix(const char* s, size_t len, bool* has_u, bool* has_l) {
    *has_u = false;
    *has_l = false;
    int suffix_len = 0;
    for (size_t i = len; i > 0; i--) {
        char c = tolower(s[i - 1]);
        if (c == 'u') *has_u = true;
        else if (c == 'l') *has_l = true;
        else break;
        suffix_len++;
    }
    return suffix_len;
}

// Check if suffix is float suffix
static int parse_float_suffix(const char* s, size_t len) {
    if (len == 0) return 0;
    char last = tolower(s[len - 1]);
    if (last == 'f' || last == 'l') return 1;
    return 0;
}

// Detect type and parse value from input string
Data type(const char* input) {
    Data result = {0};
    char buf[256];
    strncpy(buf, input, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    trim(buf);

    if (strlen(buf) == 0) {
        result.dtype = TYPE_STRING;
        result.dval.STRING = strdup("");
        return result;
    }

    bool bval;
    if (is_bool(buf, &bval)) {
        result.dtype = TYPE_BOOL;
        result.dval.BOOL = bval;
        return result;
    }

    Dtype special = is_special_float(buf);
    if (special != -1) {
        result.dtype = special;
        return result;
    }

    if (strlen(buf) == 1 && !isdigit((unsigned char)buf[0])) {
        result.dtype = TYPE_CHAR;
        result.dval.CHAR = buf[0];
        return result;
    }

    // Hex literal, possibly pointer
    if (strlen(buf) > 2 && buf[0] == '0' && (buf[1] == 'x' || buf[1] == 'X')) {
        errno = 0;
        unsigned long val = strtoul(buf, NULL, 16);
        if (errno == 0) {
            if (strlen(buf) >= 6) { // heuristic pointer
                result.dtype = TYPE_POINTER;
                result.dval.POINTER = (void*)val;
                return result;
            }
            result.dtype = TYPE_HEX;
            result.dval.HEX = val;
            return result;
        }
    }

    // Binary literal
    if (is_binary_literal(buf)) {
        unsigned long val = binary_to_ulong(buf);
        result.dtype = TYPE_BINARY;
        result.dval.BINARY = val;
        return result;
    }

    // Octal literal, 0o or 0O prefix
    if (strlen(buf) > 2 && buf[0] == '0' && (buf[1] == 'o' || buf[1] == 'O')) {
        errno = 0;
        unsigned long val = strtoul(buf + 2, NULL, 8);
        if (errno == 0) {
            result.dtype = TYPE_OCTAL;
            result.dval.OCTAL = val;
            return result;
        }
    }

    // Integer with suffixes
    size_t len = strlen(buf);
    bool has_u = false, has_l = false;
    int suffix_len = parse_int_suffix(buf, len, &has_u, &has_l);

    if (suffix_len > 0) {
        char int_buf[256];
        if (len - suffix_len >= sizeof(int_buf)) goto parse_float_double; // too long
        memcpy(int_buf, buf, len - suffix_len);
        int_buf[len - suffix_len] = '\0';
        char* endptr;
        errno = 0;
        unsigned long val = strtoul(int_buf, &endptr, 10);
        if (*endptr == '\0' && errno == 0) {
            if (has_u && has_l) {
                result.dtype = TYPE_ULONG;
                result.dval.ULONG = val;
            } else if (has_u) {
                result.dtype = TYPE_UINT;
                result.dval.UINT = (unsigned int)val;
            } else if (has_l) {
                result.dtype = TYPE_LONG;
                result.dval.LONG = (long)val;
            } else {
                result.dtype = TYPE_INT;
                result.dval.INT = (int)val;
            }
            return result;
        }
    }

parse_float_double:
    // Try integer without suffix
    {
        char* endptr;
        errno = 0;
        long lval = strtol(buf, &endptr, 10);
        if (*endptr == '\0' && errno == 0) {
            if (lval >= -32768 && lval <= 32767) {
                result.dtype = TYPE_SHORT;
                result.dval.SHORT = (short)lval;
            } else {
                result.dtype = TYPE_INT;
                result.dval.INT = (int)lval;
            }
            return result;
        }
    }

    // Try float/double with optional suffix f or l
    {
        size_t len = strlen(buf);
        int float_suf_len = parse_float_suffix(buf, len);
        char float_buf[256];
        if (len - float_suf_len >= sizeof(float_buf)) goto string_fallback;
        memcpy(float_buf, buf, len - float_suf_len);
        float_buf[len - float_suf_len] = '\0';

        char* endptr;
        errno = 0;
        float fval = strtof(float_buf, &endptr);
        if (*endptr == '\0' && errno == 0) {
            if (float_suf_len == 1 && (tolower(buf[len-1]) == 'f')) {
                result.dtype = TYPE_FLOAT;
                result.dval.FLOAT = fval;
                return result;
            }
            errno = 0;
            double dval = strtod(float_buf, &endptr);
            if (*endptr == '\0' && errno == 0) {
                result.dtype = TYPE_DOUBLE;
                result.dval.DOUBLE = dval;
                return result;
            }
        }
    }

string_fallback:
    result.dtype = TYPE_STRING;
    result.dval.STRING = strdup(buf);
    return result;
}

const char* dtype_to_string(Dtype dtype) {
    switch (dtype) {
        case TYPE_INT: return "int";
        case TYPE_UINT: return "unsigned int";
        case TYPE_LONG: return "long";
        case TYPE_ULONG: return "unsigned long";
        case TYPE_SHORT: return "short";
        case TYPE_USHORT: return "unsigned short";
        case TYPE_FLOAT: return "float";
        case TYPE_DOUBLE: return "double";
        case TYPE_BOOL: return "bool";
        case TYPE_CHAR: return "char";
        case TYPE_STRING: return "string";
        case TYPE_HEX: return "hex";
        case TYPE_OCTAL: return "octal";
        case TYPE_BINARY: return "binary";
        case TYPE_POINTER: return "pointer";
        case TYPE_NAN: return "NaN";
        case TYPE_INF: return "Infinity";
        case TYPE_NEG_INF: return "-Infinity";
        default: return "unknown";
    }
}

void print_value(const Data* data) {
    switch (data->dtype) {
        case TYPE_INT: printf("Value (int): %d\n", data->dval.INT); break;
        case TYPE_UINT: printf("Value (unsigned int): %u\n", data->dval.UINT); break;
        case TYPE_LONG: printf("Value (long): %ld\n", data->dval.LONG); break;
        case TYPE_ULONG: printf("Value (unsigned long): %lu\n", data->dval.ULONG); break;
        case TYPE_SHORT: printf("Value (short): %d\n", data->dval.SHORT); break;
        case TYPE_USHORT: printf("Value (unsigned short): %u\n", data->dval.USHORT); break;
        case TYPE_FLOAT: printf("Value (float): %.6g\n", data->dval.FLOAT); break;
        case TYPE_DOUBLE: printf("Value (double): %.10g\n", data->dval.DOUBLE); break;
        case TYPE_BOOL: printf("Value (bool): %s\n", data->dval.BOOL ? "true" : "false"); break;
        case TYPE_CHAR: printf("Value (char): %c\n", data->dval.CHAR); break;
        case TYPE_STRING: printf("Value (string): %s\n", data->dval.STRING); break;
        case TYPE_HEX: printf("Value (hex): 0x%lX (%lu)\n", data->dval.HEX, data->dval.HEX); break;
        case TYPE_OCTAL: printf("Value (octal): 0o%lo (%lu)\n", data->dval.OCTAL, data->dval.OCTAL); break;
        case TYPE_BINARY: {
            printf("Value (binary): 0b");
            bool started = false;
            int bits = sizeof(unsigned long) * 8;
            for (int i = bits - 1; i >= 0; i--) {
                bool bit = (data->dval.BINARY >> i) & 1;
                if (bit) started = true;
                if (started) printf("%d", bit);
            }
            if (!started) printf("0");
            printf(" (%lu)\n", data->dval.BINARY);
            break;
        }
        case TYPE_POINTER: printf("Value (pointer): %p\n", data->dval.POINTER); break;
        case TYPE_NAN: printf("Value: NaN\n"); break;
        case TYPE_INF: printf("Value: +Infinity\n"); break;
        case TYPE_NEG_INF: printf("Value: -Infinity\n"); break;
        default: printf("Value: unknown\n"); break;
    }
}

int main() {
    char input[256];
    printf("Enter value: ");
    if (!fgets(input, sizeof(input), stdin)) {
        fprintf(stderr, "Input error\n");
        return 1;
    }
    // Remove newline
    input[strcspn(input, "\n")] = '\0';

    Data d = type(input);
    printf("Datatype: %s\n", dtype_to_string(d.dtype));
    print_value(&d);

    if (d.dtype == TYPE_STRING) {
        free(d.dval.STRING);
    }
    return 0;
}
