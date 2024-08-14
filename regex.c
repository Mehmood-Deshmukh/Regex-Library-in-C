#include "regex.h"

bool match_helper(const char* pattern, const char* text) {
    while (*pattern != '\0') {
        if (*(pattern + 1) == '*') {
            char match_char = *pattern;
            pattern += 2;
            while (*text != '\0' && (match_char == '.' || match_char == *text)) {
                if (match_helper(pattern, text)) return true;
                text++;
            }
            continue;
        } else if (*(pattern + 1) == '+') {
            char match_char = *pattern;
            if (*text == '\0' || (match_char != '.' && match_char != *text)) return false;
            pattern += 2;
            do {
                text++;
                if (match_helper(pattern, text)) return true;
            } while (*text != '\0' && (match_char == '.' || match_char == *text));
            return false;
        } else if (*(pattern + 1) == '?') {
            char match_char = *pattern;
            pattern += 2;
            if (*text != '\0' && (match_char == '.' || match_char == *text)) {
                if (match_helper(pattern, text + 1)) return true;
            }
            if (match_helper(pattern, text)) return true;
            return false;
        }

        if (*text == '\0') return false;

        if (*pattern == '.' || *pattern == *text) {
            pattern++;
            text++;
        } else {
            return false;
        }
    }

    return *pattern == '\0';
}


bool regex_match(const char* pattern, const char* text, int* index) {
    int text_len = strlen(text);

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
