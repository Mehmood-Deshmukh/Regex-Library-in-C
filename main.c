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

    // Basic character matching (no quantifiers)
    run_test("abc", "123abc456", true);  // Pattern at index 3
    run_test("abc", "aabcdefabc", true); // Pattern at index 1
    run_test("abc", "xabcxabcx", true);  // Pattern at index 1
    run_test("abc", "abxcyz", false);    // Pattern does not match
    run_test("ab", "xababx", true);      // Pattern at index 2

    // Wildcard (no quantifiers)
    run_test("a.c", "xyzabcxyz", true);   // Pattern at index 3
    run_test("a.c", "a1c a2c a3c", true); // Pattern at index 0, 5, and 10
    run_test("a.c", "acacac", false);     // Pattern does not match
    run_test("a.c", "a-c", true);        // Pattern at index 0
    run_test("a.c", "zaxyc", false);      // Pattern does not match

    // One or more (+)
    run_test("ab+c", "aabbbbc", true);  // Pattern at index 1
    run_test("ab+c", "abcabc", true);   // Pattern at index 0 and 3
    run_test("ab+c", "abcd", true);    // Pattern at index 0
    run_test("a+b", "aaab", true);      // Pattern at index 0
    run_test("a+b", "baaa", false);      // Pattern does not match

    // Zero or more (*)
    run_test("ab*c", "abbbbc", true);   // Pattern at index 0
    run_test("ab*c", "abcabc", true);   // Pattern at index 0 and 3
    run_test("ab*c", "cabc", true);     // Pattern at index 0
    run_test("a*b", "aab", true);       // Pattern at index 0
    run_test("a*b", "b", true);         // Pattern at index 0

    // Zero or one (?)
    run_test("ab?c", "abcc", true);     // Pattern at index 0
    run_test("ab?c", "acac", true);     // Pattern at index 0 and 3
    run_test("ab?c", "abcabc", true);  // Pattern at index 0
    run_test("a?b", "aab", true);       // Pattern at index 0
    run_test("a?b", "b", true);         // Pattern at index 0

    // Combination of quantifiers
    run_test("a*b+c?", "aabbbc", true); // Pattern at index 0
    run_test("a*b+c?", "aaabbc", true); // Pattern at index 0
    run_test("a*b+c?", "aabc", true);   // Pattern at index 0
    run_test("a*b+c?", "abc", true);    // Pattern at index 0
    run_test("a*b*c?", "b", true);     // Pattern at index 0
    run_test("a*b+c?", "ac", false);    // Pattern does not match

    return 0;
}
