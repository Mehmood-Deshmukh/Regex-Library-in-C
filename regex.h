// This is a tiny yet powerful regex library written in C.
// it is inspired from kokke/tiny-regex-c and Brian Kernighan's 'A regular expression Matcher' from the book Beautiful Code


// currently this library supports the following regex operators
// .: Matches any character except a newline.
// ^: Matches the start of the string.
// $: Matches the end of the string.
// *: Matches zero or more of the preceding element.
// +: Matches one or more of the preceding element.
// ?: Matches zero or one of the preceding element.
// \d: Matches any digit [0-9].
// \D: Matches any non-digit.
// \w: Matches any alphanumeric character [a-zA-Z0-9_].
// \W: Matches any non-alphanumeric character.
// \s: Matches any whitespace character.
// \S: Matches any non-whitespace character.
// [abc]: Matches any one of the characters 'a', 'b', or 'c'.
// [^abc]: Matches any character except 'a', 'b', or 'c'.
// [a-zA-Z]: Matches any letter, either lowercase or uppercase.


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

// instead of Brian's struct I used the one similar to that of tinyregex for more convinience
typedef struct regex_token {
    unsigned char type;  // The type of regex token 
    union {
        unsigned char ch; // Used when the token is a single character.
        unsigned char char_class[MAX_CHAR_CLASS_LEN];  // Used when the token is a character class.
    } u;
} regex_token;



typedef struct regex_token* regex_t; // pointer to a regex token

regex_t regex_compile(const char* pattern); // Compiles a regular expression pattern into a sequence of regex tokens.
int regex_match_compiled_pattern(regex_t pattern, const char* text, int* matchlength); //Matches a regex pattern against a given text string.
int regex_match(const char* pattern, const char* text, int* matchlength); // this function will be used by the user. it will call the regex_compile and regex_match_compiled_pattern 
char* regex_replace(const char* pattern, const char* text, const char* replacement); // replaces a pattern with a given string

#endif
