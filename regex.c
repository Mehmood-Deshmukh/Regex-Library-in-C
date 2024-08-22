#include "regex.h"


// these are the helper functions declarations as mentioned by Brian and also similar to tinyregex
static int matchPattern(regex_token* compiledPattern, const char* inputText, int* matchedLength); // helper function for matching pattern
static int matchStar(regex_token token, regex_token* compiledPattern, const char* inputText, int* matchedLength); // helper function for matching star (*) regex operator which means match zero or more occurences
static int matchPlus(regex_token token, regex_token* compiledPattern, const char* inputText, int* matchedLength); // helper function for matching plus (+) regex operator which means match one or more occurences
static int matchQuestion(regex_token token, regex_token* compiledPattern, const char* inputText, int* matchedLength); // helper function for matching Question (?) regex operator which means match zero or one occurences
static int matchSingleCharacter(regex_token token, char character); // matches single character based on the given token


// This is the main function that users call to match a pattern with the text
int regex_match(const char* pattern, const char* text, int* matchLength) {
    regex_t compiledPattern = regex_compile(pattern); // Compile the pattern into a sequence of tokens
    return regex_match_compiled_pattern(compiledPattern, text, matchLength); // Match the compiled pattern against the text
}


// This function matches a compiled regex pattern against the provided text
int regex_match_compiled_pattern(regex_t compiledPattern, const char* text, int* matchLength) {
    *matchLength = 0;
    if (compiledPattern != NULL) {
        if (compiledPattern[0].type == BEGIN) { // Check if the pattern starts with a '^' (beginning anchor)
            return matchPattern(&compiledPattern[1], text, matchLength) ? 0 : -1;
        } else {
             // Try to match the pattern starting at each position in the text
            int textPosition = -1;
            do {
                textPosition++;
                if (matchPattern(compiledPattern, text, matchLength)) {
                    return text[0] == '\0' ? -1 : textPosition; // if match is found return the position
                }
            } while (*text++ != '\0');
        }
    }
    return -1;
}

// this function compiles a regex pattern string into a series of tokens for easier processing during matching
regex_t regex_compile(const char* pattern) {
    static regex_token compiledPattern[MAX_REGEXP_OBJECTS]; //this array stores the compiled pattern tokens
    int i = 0, j = 0;
    int inverted = 0; // to check if character class is inverted

    while (pattern[i] != '\0' && (j + 1 < MAX_REGEXP_OBJECTS)) {

        // handling classes and invert classes
        if (pattern[i] == '[') {
            i++;
            inverted = (pattern[i] == '^');
            if (inverted) i++;
            compiledPattern[j].type = inverted ? INV_CHAR_CLASS : CHAR_CLASS;
            int k = 0;
            while (pattern[i] != ']' && pattern[i] != '\0') {
                if (pattern[i] == '\\') {
                    if (k < MAX_CHAR_CLASS_LEN - 2) {
                        compiledPattern[j].u.char_class[k++] = pattern[i++];
                        compiledPattern[j].u.char_class[k++] = pattern[i++];
                    } else {
                        i += 2; 
                    }
                } else if (pattern[i] == '-' && k > 0 && pattern[i + 1] != ']' && pattern[i + 1] != '\0') {
                    if (k < MAX_CHAR_CLASS_LEN - 1) {
                        compiledPattern[j].u.char_class[k++] = pattern[i++];
                    } else {
                        i++; 
                    }
                } else {
                    if (k < MAX_CHAR_CLASS_LEN - 1) {
                        compiledPattern[j].u.char_class[k++] = pattern[i++];
                    } else {
                        i++; 
                    }
                }
            }
            if (pattern[i] == ']') {
                i++;
            }
            compiledPattern[j].u.char_class[k] = '\0';
            j++;
        } else if (pattern[i] == '^') { // handling beginning anchor
            compiledPattern[j].type = BEGIN;
            i++;
            j++;
        } else if (pattern[i] == '$') { // handling end anchor
            compiledPattern[j].type = END;
            i++;
            j++;
        } else if (pattern[i] == '.') { // handling dot operator
            compiledPattern[j].type = DOT;
            i++;
            j++;
        } else if (pattern[i] == '*') { //handling star operator
            compiledPattern[j].type = STAR;
            i++;
            j++;
        } else if (pattern[i] == '+') { //handling plus operator
            compiledPattern[j].type = PLUS;
            i++;
            j++;
        } else if (pattern[i] == '?') { // handling question mark operator
            compiledPattern[j].type = QUESTIONMARK;
            i++;
            j++;
        } else if (pattern[i] == '\\') { // handle escape sequences
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
            } else { // handle backslash
                compiledPattern[j].type = CHAR;
                compiledPattern[j].u.ch = '\\';
            }
            i++;
            j++;
        } else { // handle literal characters
            compiledPattern[j].type = CHAR;
            compiledPattern[j].u.ch = pattern[i];
            i++;
            j++;
        }
    }
    compiledPattern[j].type = UNUSED; // mark end of compiled pattern
    return (regex_t)compiledPattern;
}

// Helper function to match a single character based on the token type
static int matchSingleCharacter(regex_token token, char character) {
    switch (token.type) {
        // we will just use logic of char class and in the end check if it is a inverted class or not
        // if it is an inverted class we will just return inverse of match.
        case CHAR_CLASS:
        case INV_CHAR_CLASS: {
            int match = 0;
            for (int i = 0; token.u.char_class[i] != '\0'; i++) {
                if (token.u.char_class[i] == '\\') {
                    i++;
                    switch (token.u.char_class[i]) {
                        case 's': match |= isspace((unsigned char)character); break;
                        case 'd': match |= isdigit((unsigned char)character); break;
                        case 'w': match |= isalnum((unsigned char)character) || character == '_'; break;
                        case 'W': match |= !(isalnum((unsigned char)character) || character == '_'); break;
                        case 'D': match |= !isdigit((unsigned char)character); break;
                        case 'S': match |= !isspace((unsigned char)character); break;
                        default: match |= (token.u.char_class[i] == character);
                    }
                } else if (token.u.char_class[i] == '-' && i > 0 && token.u.char_class[i+1] != '\0') {
                    match |= (character >= token.u.char_class[i-1] && character <= token.u.char_class[i+1]);
                    i++;
                } else {
                    match |= (token.u.char_class[i] == character);
                }
            }
            return (token.type == CHAR_CLASS) ? match : !match;
        }
        case DOT: // for dot we will accept everything except newline and carriage return
            return (character != '\n' && character != '\r');
        case DIGIT: // check if it is a digit
            return isdigit((unsigned char)character);
        case NOT_DIGIT: //check if it is not a digit
            return !isdigit((unsigned char)character);
        case ALPHA: // check if it is alpha num or '_'
            return isalnum((unsigned char)character) || (character == '_');
        case NOT_ALPHA: // check if it is not alpha num or '_'
            return !(isalnum((unsigned char)character) || (character == '_'));
        case WHITESPACE: // check if it is whitespace
            return isspace((unsigned char)character);
        case NOT_WHITESPACE: //check if it is not a whitespace
            return !isspace((unsigned char)character);
        case CHAR: // match literal character
            return token.u.ch == character;
        default:
            return 0;
    }
}

// Helper function to match the star (*) operator, which matches zero or more occurrences
static int matchStar(regex_token token, regex_token* compiledPattern, const char* inputText, int* matchedLength) {
    int initialMatchLength = *matchedLength; //save the initial matchlength
    const char* initialText = inputText; // save the initial position of input text

    //try matching as many character as possible
    while (*inputText != '\0' && matchSingleCharacter(token, *inputText)) {
        inputText++;
        (*matchedLength)++;
    }

    //backtracking step 
    while (inputText >= initialText) {
        if (matchPattern(compiledPattern, inputText--, matchedLength)) return 1;
        (*matchedLength)--;
    }
    *matchedLength = initialMatchLength;// Restore initial match length if no match found
    return 0;
}

// Helper function to match the plus (+) operator, which matches one or more occurrences
static int matchPlus(regex_token token, regex_token* compiledPattern, const char* inputText, int* matchedLength) {
    const char* initialText = inputText; //save initial position of input text

    //match as many characters as possible
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

// Helper function to match the question mark (?) operator, which matches zero or one occurrence
static int matchQuestion(regex_token token, regex_token* compiledPattern, const char* inputText, int* matchedLength) {
    // If the token is UNUSED, the question mark matches zero occurrences
    if (token.type == UNUSED) return 1; //try matching without current character
    if (matchPattern(compiledPattern, inputText, matchedLength)) return 1;
    if (*inputText && matchSingleCharacter(token, *inputText++)) {
        if (matchPattern(compiledPattern, inputText, matchedLength)) {
            (*matchedLength)++;
            return 1;
        }
    }
    return 0;
}

// Helper function to match a pattern against the input text
static int matchPattern(regex_token* compiledPattern, const char* inputText, int* matchedLength) {
    int initialMatchLength = *matchedLength;

    do {
        if (compiledPattern[0].type == UNUSED) {
            return 1;
        } else if (compiledPattern[0].type == END) {
            return (*inputText == '\0' || *inputText == '\n');
        } else if (compiledPattern[1].type == QUESTIONMARK) {
            return matchQuestion(compiledPattern[0], &compiledPattern[2], inputText, matchedLength);
        } else if (compiledPattern[1].type == STAR) {
            return matchStar(compiledPattern[0], &compiledPattern[2], inputText, matchedLength);
        } else if (compiledPattern[1].type == PLUS) {
            return matchPlus(compiledPattern[0], &compiledPattern[2], inputText, matchedLength);
        }
        
        if (*inputText == '\0') {
            *matchedLength = initialMatchLength;
            return 0;
        }
        
        if (!matchSingleCharacter(*compiledPattern, *inputText)) {
            *matchedLength = initialMatchLength;
            return 0;
        }
        
        compiledPattern++;
        inputText++;
        (*matchedLength)++;
    } while (1);
}

// Function to print the compiled regex pattern inspired from tinyregex
void regex_print(regex_token* compiledPattern) {
    const char* tokenTypes[] = {
        "UNUSED", "DOT", "BEGIN", "END", "QUESTIONMARK", "STAR", "PLUS", 
        "CHAR", "DIGIT", "NOT_DIGIT", "ALPHA", "NOT_ALPHA", "WHITESPACE", 
        "NOT_WHITESPACE", "CHAR_CLASS", "INV_CHAR_CLASS"
    };

    for (int i = 0; i < MAX_REGEXP_OBJECTS; ++i) {
        if (compiledPattern[i].type == UNUSED) {
            break;
        }

        printf("Type: %s", tokenTypes[compiledPattern[i].type]); // type of the token

        // Print additional information based on the token type
        if (compiledPattern[i].type == CHAR) {
            printf(" '%c'", compiledPattern[i].u.ch); // Print literal character
        } else if (compiledPattern[i].type == CHAR_CLASS || compiledPattern[i].type == INV_CHAR_CLASS) {
            printf(" [");
            for (int j = 0; compiledPattern[i].u.char_class[j] != '\0'; j++) {
                printf("%c", compiledPattern[i].u.char_class[j]); // Print character class
            }
            printf("]");
        }
        printf("\n");
    }
}

// Function to replace matches of a pattern in the text with a replacement string
char* regex_replace(const char* pattern, const char* text, const char* replacement) {
    if (!pattern || !text || !replacement) {
        return NULL;
    }

    int matchLength = 0;
    const char* searchText = text;
    size_t resultBufferSize = strlen(text) + 1;
    char* result = (char*)malloc(resultBufferSize);
    if (!result) {
        perror("Failed to allocate memory");
        return NULL;
    }
    result[0] = '\0';

    while (*searchText != '\0') {
        int matchPosition = regex_match(pattern, searchText, &matchLength);

        if (matchPosition >= 0) {
            resultBufferSize += strlen(replacement) - matchLength;
            char* newResult = (char*)realloc(result, resultBufferSize);
            if (!newResult) {
                perror("Failed to reallocate memory");
                free(result);
                return NULL;
            }
            result = newResult;

            strncat(result, text, matchPosition);
            strcat(result, replacement);
            searchText += matchPosition + matchLength;
            text = searchText;
        } else {
            strcat(result, searchText);
            break;
        }
    }

    return result;
}