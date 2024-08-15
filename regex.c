#include "regex.h"


static int matchpattern(regex_token* pattern, const char* text, int* matchlength);
static int matchstar(regex_token p, regex_token* pattern, const char* text, int* matchlength);
static int matchplus(regex_token p, regex_token* pattern, const char* text, int* matchlength);
static int matchone(regex_token p, char c);

int regex_match(const char* pattern, const char* text, int* matchlength) {
    return regex_match_compiled_pattern(regex_compile(pattern), text, matchlength);
}

int regex_match_compiled_pattern(regex_t pattern, const char* text, int* matchlength) {
    *matchlength = 0;
    if (pattern != 0) {
        if (pattern[0].type == BEGIN) {
            return (matchpattern(&pattern[1], text, matchlength)) ? 0 : -1;
        } else {
            int idx = -1;
            do {
                idx += 1;
                if (matchpattern(pattern, text, matchlength)) {
                    if (text[0] == '\0')
                        return -1;
                    return idx;
                }
            } while (*text++ != '\0');
        }
    }
    return -1;
}

regex_t regex_compile(const char* pattern) {
    static regex_token regex_compiled[MAX_REGEXP_OBJECTS];
    static unsigned char ccl_buf[MAX_CHAR_CLASS_LEN];
    int i = 0, j = 0;

    while (pattern[i] != '\0' && (j + 1 < MAX_REGEXP_OBJECTS)) {
        switch (pattern[i]) {
            case '^': regex_compiled[j].type = BEGIN; break;
            case '$': regex_compiled[j].type = END; break;
            case '.': regex_compiled[j].type = DOT; break;
            case '*': regex_compiled[j].type = STAR; break;
            case '+': regex_compiled[j].type = PLUS; break;
            case '?': regex_compiled[j].type = QUESTIONMARK; break;
            default:
                regex_compiled[j].type = CHAR;
                regex_compiled[j].u.ch = pattern[i];
                break;
        }
        if (pattern[i] == 0) return 0;
        i += 1;
        j += 1;
    }
    regex_compiled[j].type = UNUSED;
    return (regex_t)regex_compiled;
}

static int matchone(regex_token p, char c) {
    return (p.type == DOT) ? (c != '\n' && c != '\r') : (p.u.ch == c);
}

static int matchstar(regex_token p, regex_token* pattern, const char* text, int* matchlength) {
    int prelen = *matchlength;
    const char* prepoint = text;
    while ((text[0] != '\0') && matchone(p, *text)) {
        text++;
        (*matchlength)++;
    }
    while (text >= prepoint) {
        if (matchpattern(pattern, text--, matchlength)) return 1;
        (*matchlength)--;
    }
    *matchlength = prelen;
    return 0;
}

static int matchplus(regex_token p, regex_token* pattern, const char* text, int* matchlength) {
    const char* prepoint = text;
    while ((text[0] != '\0') && matchone(p, *text)) {
        text++;
        (*matchlength)++;
    }
    while (text > prepoint) {
        if (matchpattern(pattern, text--, matchlength)) return 1;
        (*matchlength)--;
    }
    return 0;
}

static int matchquestion(regex_token p, regex_token* pattern, const char* text, int* matchlength) {
    if (p.type == UNUSED) return 1;
    if (matchpattern(pattern, text, matchlength)) return 1;
    if (*text && matchone(p, *text++)) {
        if (matchpattern(pattern, text, matchlength)) {
            (*matchlength)++;
            return 1;
        }
    }
    return 0;
}

static int matchpattern(regex_token* pattern, const char* text, int* matchlength) {
    int pre = *matchlength;
    do {
        if ((pattern[0].type == UNUSED) || (pattern[1].type == QUESTIONMARK)) {
            return matchquestion(pattern[0], &pattern[2], text, matchlength);
        } else if (pattern[1].type == STAR) {
            return matchstar(pattern[0], &pattern[2], text, matchlength);
        } else if (pattern[1].type == PLUS) {
            return matchplus(pattern[0], &pattern[2], text, matchlength);
        } else if ((pattern[0].type == END) && pattern[1].type == UNUSED) {
            return (text[0] == '\0');
        }
        (*matchlength)++;
    } while ((text[0] != '\0') && matchone(*pattern++, *text++));
    *matchlength = pre;
    return 0;
}


void re_print(regex_token* pattern)
{
  const char* types[] = { "UNUSED", "DOT", "BEGIN", "END", "QUESTIONMARK", "STAR", "PLUS", "CHAR", "CHAR_CLASS", "INV_CHAR_CLASS", "DIGIT", "NOT_DIGIT", "ALPHA", "NOT_ALPHA", "WHITESPACE", "NOT_WHITESPACE", "BRANCH" };

  int i;
  int j;
  char c;
  for (i = 0; i < MAX_REGEXP_OBJECTS; ++i){
    if (pattern[i].type == UNUSED){
      break;
    }

    printf("type: %s", types[pattern[i].type]);

    if (pattern[i].type == CHAR){
      printf(" '%c'", pattern[i].u.ch);
    }
    printf("\n");
  }
}