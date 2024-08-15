#include <string.h>
#include <stdbool.h>
#include "stdio.h"

#define MAX_STACK_SIZE 1000

typedef struct {
    const char* pattern;
    const char* text;
    const char* text_start;
    const char* text_end;
} StackEntry;

typedef struct {
    StackEntry entries[MAX_STACK_SIZE];
    int top;
} Stack;

void stack_init(Stack* s) {
    s->top = -1;
}

void stack_push(Stack* s, const char* pattern, const char* text, const char* text_start, const char* text_end) {
    if (s->top < MAX_STACK_SIZE - 1) {
        s->top++;
        s->entries[s->top].pattern = pattern;
        s->entries[s->top].text = text;
        s->entries[s->top].text_start = text_start;
        s->entries[s->top].text_end = text_end;
    }
}

bool stack_pop(Stack* s, const char** pattern, const char** text, const char** text_start, const char** text_end) {
    if (s->top >= 0) {
        *pattern = s->entries[s->top].pattern;
        *text = s->entries[s->top].text;
        *text_start = s->entries[s->top].text_start;
        *text_end = s->entries[s->top].text_end;
        s->top--;
        return true;
    }
    return false;
}

bool match_char(const char** pattern, const char** text, const char* text_start, const char* text_end) {
    if (**pattern == '^') {
        return *text == text_start;
    } else if (**pattern == '$') {
        return *text == text_end;
    } else if (**pattern == '.') {
        return **text != '\0' && *text != text_end;
    } else {
        return **pattern == **text;
    }
}

bool regex_match_helper(const char* pattern, const char* text, const char* text_start, const char* text_end) {
    Stack stack;
    stack_init(&stack);
    stack_push(&stack, pattern, text, text_start, text_end);

    while (stack_pop(&stack, &pattern, &text, &text_start, &text_end)) {
        if (*pattern == '\0') return true;


        if (*(pattern + 1) == '*') {
            stack_push(&stack, pattern + 2, text, text_start, text_end);
            if (match_char(&pattern, &text, text_start, text_end)) {
                stack_push(&stack, pattern, text + 1, text_start, text_end);
            }
        } else if (*(pattern + 1) == '+') {
            if (match_char(&pattern, &text, text_start, text_end)) {
                stack_push(&stack, pattern, text + 1, text_start, text_end);
                stack_push(&stack, pattern + 2, text + 1, text_start, text_end);
            }
        } else if (*(pattern + 1) == '?') {
            stack_push(&stack, pattern + 2, text, text_start, text_end);
            if (match_char(&pattern, &text, text_start, text_end)) {
                stack_push(&stack, pattern + 2, text + 1, text_start, text_end);
            }
        } else if (match_char(&pattern, &text, text_start, text_end)) {
            stack_push(&stack, pattern + 1, text + 1, text_start, text_end);
        }
    }

    return false;
}

bool regex_match(const char* pattern, const char* text, int* index) {
    int text_len = strlen(text);
    const char* text_end = text + strlen(text);

    if (pattern[0] == '^') {
        if (regex_match_helper(pattern + 1, text, text, text_end)) {
            *index = 0;
            return true;    
        }
    } else {
        for (int i = 0; i <= text_len; ++i) {
            if (regex_match_helper(pattern, text + i, text, text_end)) {
                *index = i;
                return true;
            }
        }
    }

    *index = -1;
    return false;
}