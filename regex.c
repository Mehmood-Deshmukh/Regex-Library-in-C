#include <string.h>
#include <stdbool.h>

#define MAX_STACK_SIZE 1000

typedef struct {
    const char* pattern;
    const char* text;
} StackEntry;

typedef struct {
    StackEntry entries[MAX_STACK_SIZE];
    int top;
} Stack;

void stack_init(Stack* s) {
    s->top = -1;
}

void stack_push(Stack* s, const char* pattern, const char* text) {
    if (s->top < MAX_STACK_SIZE - 1) {
        s->top++;
        s->entries[s->top].pattern = pattern;
        s->entries[s->top].text = text;
    }
}

bool stack_pop(Stack* s, const char** pattern, const char** text) {
    if (s->top >= 0) {
        *pattern = s->entries[s->top].pattern;
        *text = s->entries[s->top].text;
        s->top--;
        return true;
    }
    return false;
}

bool match_char(const char** pattern, const char** text) {
    if (**pattern == '.') {
        return **text != '\0';
    } else {
        return **pattern == **text;
    }
}

bool regex_match_helper(const char* pattern, const char* text) {
    Stack stack;
    stack_init(&stack);
    stack_push(&stack, pattern, text);

    while (stack_pop(&stack, &pattern, &text)) {
        if (*pattern == '\0') return true;
        if (*text == '\0') continue;

        if (*(pattern + 1) == '*') {
            stack_push(&stack, pattern + 2, text);
            if (match_char(&pattern, &text)) {
                stack_push(&stack, pattern, text + 1);
            }
        } else if (*(pattern + 1) == '+') {
            if (match_char(&pattern, &text)) {
                stack_push(&stack, pattern, text + 1);
                stack_push(&stack, pattern + 2, text + 1);
            }
        } else if (*(pattern + 1) == '?') {
            stack_push(&stack, pattern + 2, text);
            if (match_char(&pattern, &text)) {
                stack_push(&stack, pattern + 2, text + 1);
            }
        } else if (match_char(&pattern, &text)) {
            stack_push(&stack, pattern + 1, text + 1);
        }
    }

    return false;
}

bool regex_match(const char* pattern, const char* text, int* index) {
    int text_len = strlen(text);

    for (int i = 0; i <= text_len; ++i) {
        if (regex_match_helper(pattern, text + i)) {
            *index = i;
            return true;
        }
    }

    *index = -1;
    return false;
}