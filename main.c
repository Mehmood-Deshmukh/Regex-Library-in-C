#include "regex.h"

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

void run_test(const char* pattern, const char* text, bool expected) {
    int index;
    bool result = regex_match(pattern, text, &index);
    printf("Pattern: %-20s Text: %-20s Expected: %-5s Result: %-5s Index: %d %s\n",
           pattern, text, expected ? "true" : "false", result ? "true" : "false",
           index, (result == expected) ? GREEN "PASS" RESET : RED "FAIL" RESET);
}

int main() {
    printf("Running regex matcher tests:\n\n");

    run_test("abc", "abc", true);
    run_test("abc", "abcd", true);
    run_test("abc", "ab", false);
    run_test("a", "a", true);
    run_test("a", "b", false);
    run_test("a", "aa", true);
    run_test("hello", "hello", true);
    run_test("hello", "helloo", true);
    run_test("hello", "hell", false);

    run_test("a.c", "abc", true);
    run_test("a.c", "acc", true);
    run_test("a.c", "adc", true);
    run_test("a.c", "abcc", true);
    run_test("a.c", "a", false);
    run_test("a.c", "ab", false);
    run_test("a.c", "aabbc", false);
    run_test("a.c", "aCc", true);
    run_test("a.c", "a c", true);
    run_test("a.c", "a.c", true);

    run_test("ab+c", "abc", true);
    run_test("ab+c", "abbc", true);
    run_test("ab+c", "ac", false);
    run_test("a+b", "ab", true);
    run_test("a+b", "aab", true);
    run_test("a+b", "aaab", true);
    run_test("a+b", "b", false);
    run_test("a+b", "a", false);
    run_test("a+b", "abb", true);
    run_test("a+b", "aabb", true);
    run_test("a+b", "aaabb", true);
    run_test("a+b", "abbb", true);
    run_test("a+b", "bba", false);

    return 0;
}