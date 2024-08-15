/*
 *
 *A regex library which is inspired by kokke /tiny-regex-c Rob Pike's regex code with Exegesis by Brian Kernighan described in:
 *
 * http://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html
 *
 *
 * Currently the library Supports:
 * ---------
 *   '.'        Dot, matches any character
 *   '^'        Start anchor, matches beginning of string
 *   '$'        End anchor, matches end of string
 *   '*'        Asterisk, match zero or more (greedy)
 *   '+'        Plus, match one or more (greedy)
 *   '?'        Question, match zero or one (non-greedy)
 *  ---------
 *
 */

#ifndef REGEX_C
#define REGEX_C

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_REGEXP_OBJECTS 30
#define MAX_CHAR_CLASS_LEN 40

enum { UNUSED, DOT, BEGIN, END, QUESTIONMARK, STAR, PLUS, CHAR };

typedef struct regex_token {
    unsigned char type;
    union {
        unsigned char ch;
        unsigned char* ccl;
    } u;
} regex_token;


typedef struct regex_token* regex_t;

regex_t regex_compile(const char* pattern);

int regex_match_compiled_pattern(regex_t pattern, const char* text, int* matchlength);


int regex_match(const char* pattern, const char* text, int* matchlength);

#endif