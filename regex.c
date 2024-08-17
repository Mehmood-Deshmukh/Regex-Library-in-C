#include "regex.h"


static int matchPattern(regex_token* compiledPattern, const char* inputText, int* matchedLength);
static int matchStar(regex_token token, regex_token* compiledPattern, const char* inputText, int* matchedLength);
static int matchPlus(regex_token token, regex_token* compiledPattern, const char* inputText, int* matchedLength);
static int matchQuestion(regex_token token, regex_token* compiledPattern, const char* inputText, int* matchedLength);
static int matchSingleCharacter(regex_token token, char character);

int regex_match(const char* pattern, const char* text, int* matchLength) {
    regex_t compiledPattern = regex_compile(pattern);
    return regex_match_compiled_pattern(compiledPattern, text, matchLength);
}

int regex_match_compiled_pattern(regex_t compiledPattern, const char* text, int* matchLength) {
    *matchLength = 0;
    if (compiledPattern != NULL) {
        if (compiledPattern[0].type == BEGIN) {
            return matchPattern(&compiledPattern[1], text, matchLength) ? 0 : -1;
        } else {
            int textPosition = -1;
            do {
                textPosition++;
                if (matchPattern(compiledPattern, text, matchLength)) {
                    return text[0] == '\0' ? -1 : textPosition;
                }
            } while (*text++ != '\0');
        }
    }
    return -1;
}

regex_t regex_compile(const char* pattern) {
    static regex_token compiledPattern[MAX_REGEXP_OBJECTS];
    int i = 0, j = 0;
    int inCharClass = 0;

    while (pattern[i] != '\0' && (j + 1 < MAX_REGEXP_OBJECTS)) {
        if (pattern[i] == '[') {
            inCharClass = 1;
            i++;
            compiledPattern[j].type = CHAR_CLASS;
            int k = 0;
            while (pattern[i] != ']' && pattern[i] != '\0') {
                if (k < MAX_CHAR_CLASS_LEN - 1) {
                    compiledPattern[j].u.char_class[k++] = pattern[i++];
                }
            }
            if (pattern[i] == ']') {
                i++;
            }
            compiledPattern[j].u.char_class[k] = '\0';
            j++;
            inCharClass = 0;
        } else if (pattern[i] == '^') {
            compiledPattern[j].type = BEGIN;
            i++;
            j++;
        } else if (pattern[i] == '$') {
            compiledPattern[j].type = END;
            i++;
            j++;
        } else if (pattern[i] == '.') {
            compiledPattern[j].type = DOT;
            i++;
            j++;
        } else if (pattern[i] == '*') {
            compiledPattern[j].type = STAR;
            i++;
            j++;
        } else if (pattern[i] == '+') {
            compiledPattern[j].type = PLUS;
            i++;
            j++;
        } else if (pattern[i] == '?') {
            compiledPattern[j].type = QUESTIONMARK;
            i++;
            j++;
        } else if (pattern[i] == '\\') {
            if (pattern[i + 1] != '\0') {
                i++;
                switch (pattern[i]) {
                    case 'd': compiledPattern[j].type = DIGIT; break;
                    case 'D': compiledPattern[j].type = NOT_DIGIT; break;
                    case 'w': compiledPattern[j].type = ALPHA; break;
                    case 'W': compiledPattern[j].type = NOT_ALPHA; break;
                    case 's': compiledPattern[j].type = WHITESPACE; break;
                    case 'S': compiledPattern[j].type = NOT_WHITESPACE; break;
                    default:
                        compiledPattern[j].type = CHAR;
                        compiledPattern[j].u.ch = pattern[i];
                        break;
                }
            } else {
                compiledPattern[j].type = CHAR;
                compiledPattern[j].u.ch = '\\';
            }
            i++;
            j++;
        } else {
            compiledPattern[j].type = CHAR;
            compiledPattern[j].u.ch = pattern[i];
            i++;
            j++;
        }
    }
    compiledPattern[j].type = UNUSED;
    return (regex_t)compiledPattern;
}

static int matchSingleCharacter(regex_token token, char character) {
    if (token.type == CHAR_CLASS) {
        for (int i = 0; token.u.char_class[i] != '\0'; i++) {
            if (token.u.char_class[i] == character) {
                return 1;
            }
        }
        return 0;
    }
    
    switch (token.type) {
        case DOT:
            return (character != '\n' && character != '\r');
        case DIGIT:
            return isdigit(character);
        case NOT_DIGIT:
            return !isdigit(character);
        case ALPHA:
            return isalnum(character) || (character == '_');
        case NOT_ALPHA:
            return !isalnum(character) && (character != '_');
        case WHITESPACE:
            return isspace(character);
        case NOT_WHITESPACE:
            return !isspace(character);
        default:
            return token.u.ch == character;
    }
}

static int matchStar(regex_token token, regex_token* compiledPattern, const char* inputText, int* matchedLength) {
    int initialMatchLength = *matchedLength;
    const char* initialText = inputText;

    while (*inputText != '\0' && matchSingleCharacter(token, *inputText)) {
        inputText++;
        (*matchedLength)++;
    }

    while (inputText >= initialText) {
        if (matchPattern(compiledPattern, inputText--, matchedLength)) return 1;
        (*matchedLength)--;
    }
    *matchedLength = initialMatchLength;
    return 0;
}

static int matchPlus(regex_token token, regex_token* compiledPattern, const char* inputText, int* matchedLength) {
    const char* initialText = inputText;

    while (*inputText != '\0' && matchSingleCharacter(token, *inputText)) {
        inputText++;
        (*matchedLength)++;
    }

    while (inputText > initialText) {
        if (matchPattern(compiledPattern, inputText--, matchedLength)) return 1;
        (*matchedLength)--;
    }
    return 0;
}

static int matchQuestion(regex_token token, regex_token* compiledPattern, const char* inputText, int* matchedLength) {
    if (token.type == UNUSED) return 1;
    if (matchPattern(compiledPattern, inputText, matchedLength)) return 1;
    if (*inputText && matchSingleCharacter(token, *inputText++)) {
        if (matchPattern(compiledPattern, inputText, matchedLength)) {
            (*matchedLength)++;
            return 1;
        }
    }
    return 0;
}

static int matchPattern(regex_token* compiledPattern, const char* inputText, int* matchedLength) {
    int initialMatchLength = *matchedLength;

    do {
        if (compiledPattern[0].type == UNUSED || compiledPattern[1].type == QUESTIONMARK) {
            return matchQuestion(compiledPattern[0], &compiledPattern[2], inputText, matchedLength);
        } else if (compiledPattern[1].type == STAR) {
            return matchStar(compiledPattern[0], &compiledPattern[2], inputText, matchedLength);
        } else if (compiledPattern[1].type == PLUS) {
            return matchPlus(compiledPattern[0], &compiledPattern[2], inputText, matchedLength);
        } else if (compiledPattern[0].type == END && compiledPattern[1].type == UNUSED) {
            return (inputText[0] == '\0');
        }
        (*matchedLength)++;
    } while (*inputText != '\0' && matchSingleCharacter(*compiledPattern++, *inputText++));

    *matchedLength = initialMatchLength;
    return 0;
}

void regex_print(regex_token* compiledPattern) {
    const char* tokenTypes[] = {
        "UNUSED", "DOT", "BEGIN", "END", "QUESTIONMARK", "STAR", "PLUS", 
        "CHAR", "DIGIT", "NOT_DIGIT", "ALPHA", "NOT_ALPHA", "WHITESPACE", 
        "NOT_WHITESPACE", "CHAR_CLASS"
    };

    for (int i = 0; i < MAX_REGEXP_OBJECTS; ++i) {
        if (compiledPattern[i].type == UNUSED) {
            break;
        }

        printf("Type: %s", tokenTypes[compiledPattern[i].type]);

        if (compiledPattern[i].type == CHAR) {
            printf(" '%c'", compiledPattern[i].u.ch);
        } else if (compiledPattern[i].type == CHAR_CLASS) {
            printf(" [");
            for (int j = 0; compiledPattern[i].u.char_class[j] != '\0'; j++) {
                printf("%c", compiledPattern[i].u.char_class[j]);
            }
            printf("]");
        }
        printf("\n");
    }
}
