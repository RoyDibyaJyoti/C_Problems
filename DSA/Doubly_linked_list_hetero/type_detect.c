#include "type_detect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

static void trim(char* str) {
    char* start = str;
    while (isspace((unsigned char)*start)) start++;
    if (start != str) memmove(str, start, strlen(start) + 1);
    char* end = str + strlen(str) - 1;
    while (end >= str && isspace((unsigned char)*end)) *end-- = '\0';
}

static bool str_casecmp(const char* a, const char* b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return false;
        a++; b++;
    }
    return *a == '\0' && *b == '\0';
}

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

static Dtype is_special_float(const char* s) {
    if (str_casecmp(s, "nan")) return TYPE_NAN;
    if (str_casecmp(s, "inf") || str_casecmp(s, "+inf") || str_casecmp(s, "infinity") || str_casecmp(s, "+infinity")) return TYPE_INF;
    if (str_casecmp(s, "-inf") || str_casecmp(s, "-infinity")) return TYPE_NEG_INF;
    return -1;
}

static bool is_binary_literal(const char* s) {
    if (strlen(s) > 2 && s[0] == '0' && (s[1] == 'b' || s[1] == 'B')) {
        for (size_t i = 2; s[i]; i++) {
            if (s[i] != '0' && s[i] != '1') return false;
        }
        return true;
    }
    return false;
}

static unsigned long binary_to_ulong(const char* s) {
    unsigned long val = 0;
    for (size_t i = 2; s[i]; i++) {
        val <<= 1;
        if (s[i] == '1') val |= 1;
    }
    return val;
}

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

static int parse_float_suffix(const char* s, size_t len) {
    if (len == 0) return 0;
    char last = tolower(s[len - 1]);
    if (last == 'f' || last == 'l') return 1;
    return 0;
}

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

    if (strlen(buf) > 2 && buf[0] == '0' && (buf[1] == 'x' || buf[1] == 'X')) {
        errno = 0;
        unsigned long val = strtoul(buf, NULL, 16);
        if (errno == 0) {
            if (strlen(buf) >= 6) {
                result.dtype = TYPE_POINTER;
                result.dval.POINTER = (void*)val;
                return result;
            }
            result.dtype = TYPE_HEX;
            result.dval.HEX = val;
            return result;
        }
    }

    if (is_binary_literal(buf)) {
        unsigned long val = binary_to_ulong(buf);
        result.dtype = TYPE_BINARY;
        result.dval.BINARY = val;
        return result;
    }

    if (strlen(buf) > 2 && buf[0] == '0' && (buf[1] == 'o' || buf[1] == 'O')) {
        errno = 0;
        unsigned long val = strtoul(buf + 2, NULL, 8);
        if (errno == 0) {
            result.dtype = TYPE_OCTAL;
            result.dval.OCTAL = val;
            return result;
        }
    }

    size_t len = strlen(buf);
    bool has_u = false, has_l = false;
    int suffix_len = parse_int_suffix(buf, len, &has_u, &has_l);

    if (suffix_len > 0) {
        char int_buf[256];
        if (len - suffix_len >= sizeof(int_buf)) goto parse_float_double;
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
