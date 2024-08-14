#include "regex.h"

bool match_helper(const char* pattern, const char* text) {
    while (*pattern != '\0' && *text != '\0') {
        if (*(pattern + 1) == '+') {
            char match_char = *pattern;
            if (match_char != '.' && match_char != *text) {
                return false;
            }
            while (*text != '\0' && (match_char == '.' || match_char == *text)) {
                text++;
            }
            pattern += 2;
        } else if (*pattern == '.' || *pattern == *text) {
            pattern++;
            text++;
        } else {
            return false;
        }
    }
    
    while (*pattern != '\0' && *(pattern + 1) == '+') {
        pattern += 2;
    }

    return *pattern == '\0';
}

bool regex_match(const char* pattern, const char* text, int* index) {
    int text_len = strlen(text);
    int pattern_len = strlen(pattern);

    for (int i = 0; i <= text_len; ++i) {
        const char* sub_text = text + i;
        if (match_helper(pattern, sub_text)) {
            *index = i;
            return true;
        }
    }

    *index = -1;
    return false;
}