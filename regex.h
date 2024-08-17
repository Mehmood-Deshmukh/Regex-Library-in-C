/*
 *
 * A regex library inspired by kokke/tiny-regex-c and Rob Pike's regex code with Exegesis by Brian Kernighan described in:
 *
 * http://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html
 *
 *
 * Currently, the library supports:
 * ---------
 *   '.'        Dot, matches any character
 *   '^'        Start anchor, matches beginning of string
 *   '$'        End anchor, matches end of string
 *   '*'        Asterisk, match zero or more (greedy)
 *   '+'        Plus, match one or more (greedy)
 *   '?'        Question, match zero or one (non-greedy)
 *   '\d'       Digit, [0-9]
 *   '\D'       Non-digit
 *   '\w'       Alphanumeric, [a-zA-Z0-9_]
 *   '\W'       Non-alphanumeric
 *   '\s'       Whitespace, [ \t\n\r\f\v]
 *   '\S'       Non-whitespace
 *   '[abc]'    Character class, match if one of {'a', 'b', 'c'}
 *  '[^abc]'   Inverted class, match if NOT one of {'a', 'b', 'c'}
 * '[a-zA-Z]' Character ranges, the character set of the ranges { a-z | A-Z }
 *  ---------
 *
 */
#ifndef REGEX_C
#define REGEX_C

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_REGEXP_OBJECTS 30
#define MAX_CHAR_CLASS_LEN 40

enum {
    UNUSED, DOT, BEGIN, END, QUESTIONMARK, STAR, PLUS, CHAR,
    DIGIT, NOT_DIGIT, ALPHA, NOT_ALPHA, WHITESPACE, NOT_WHITESPACE, CHAR_CLASS, INV_CHAR_CLASS
};

typedef struct regex_token {
    unsigned char type;
    union {
        unsigned char ch;
        unsigned char char_class[MAX_CHAR_CLASS_LEN];
    } u;
} regex_token;



typedef struct regex_token* regex_t;

regex_t regex_compile(const char* pattern);
int regex_match_compiled_pattern(regex_t pattern, const char* text, int* matchlength);
int regex_match(const char* pattern, const char* text, int* matchlength);
char* regex_replace(const char* pattern, const char* text, const char* replacement);

#endif
