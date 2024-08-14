#include "regex.h"

static bool matchChar(char regexChar, char textChar) {
    return regexChar == textChar || regexChar == '.';
}

static bool matchPlusQuantifier(const char *pattern, const char *text) {
    const char *plusPos = strchr(pattern, '+');
    if (plusPos == NULL) {
        return false; 
    }

    size_t prefixLength = plusPos - pattern;
    const char *prefix = pattern;
    const char *suffix = plusPos + 1;

    while (strncmp(text, prefix, prefixLength) == 0) {
        const char *t = text + prefixLength;


        while (*t) {
            const char *p = suffix;
            while (*p && matchChar(*p, *t)) {
                p++;
                t++;
            }
            if (*p == '\0') {
                return true;
            }
            t++;
        }


        text++;
    }

    return false;
}


bool regex_match(const char *pattern, const char *text) {
    if (pattern == NULL || text == NULL) return false;

    if (strchr(pattern, '+')) {
        return matchPlusQuantifier(pattern, text);
    }

    while (*text) {
        const char *p = pattern;
        const char *t = text;

        while (*p && *t && matchChar(*p, *t)) {
            p++;
            t++;
        }

        if (*p == '\0') {
            return true;
        }
        text++;
    }
    return false;
}
