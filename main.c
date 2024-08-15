#include "regex.h"

#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define RESET "\033[0m"

void run_test(const char* pattern, const char* text, bool expected) {
    int index;
    bool result = regex_match(pattern, text, &index);
    printf("Pattern: %-20s Text: %-40s Expected: %-5s Result: %-5s Index: %d %s\n",
           pattern, text, expected ? "true" : "false", result ? "true" : "false",
           index, (result == expected) ? GREEN "PASS" RESET : RED "FAIL" RESET);
}

int main() {
    printf("Running regex matcher tests:\n\n");

    // Basic character matching (no quantifiers)
    run_test("abc", "Lorem ipsum abc dolor sit amet", true);  // Pattern at index 12
    run_test("abc", "abcdefghijklmnopqrstuvwxyzabc", true);  // Pattern at index 0 and 26
    run_test("xyz", "The quick brown fox jumps over the lazy dog", false);  // Pattern does not match
    run_test("jump", "The quick brown fox jumps over the lazy dog", true);  // Pattern at index 20

    // Wildcard (no quantifiers)
    run_test("qu.ck", "The quick brown fox jumps over the quick dog", true);  // Pattern at index 4 and 36
    run_test("f.x", "The quick brown fox jumps over the lazy dog", true);  // Pattern at index 16
    run_test("d.g", "The quick brown fox jumps over the lazy dog", true);  // Pattern at index 40
    run_test("c.t", "The quick brown fox jumps over the lazy dog", false);  // Pattern does not match

    // One or more (+)
    run_test("a+b", "caaabxaaabxaab", true);  // Pattern at index 1 and 6
    run_test("fo+x", "The quick brown foox jumps over the lazy dog", true);  // Pattern at index 16
    run_test("ju+mps", "The quick brown fox juuumps over the lazy dog", true);  // Pattern at index 20
    run_test("z+", "The quick brown fox jumps over the lazy dog", true);  // Pattern at index 37

    // Zero or more (*)
    run_test("ab*c", "The quick brown fox jumps over the abbbbc lazy dog", true);  // Pattern at index 35
    run_test("qu.*k", "The quick brown fox jumps over the quick dog", true);  // Pattern at index 4 and 36
    run_test("la*zy", "The quick brown fox jumps over the laaazy dog", true);  // Pattern at index 35
    run_test("x*y*z", "The quick brown fox jumps over the lazy dog", true);  // Pattern at index 40

    // Zero or one (?)
    run_test("colo?r", "The quick brown color fox jumps over the colour dog", true);  // Pattern at index 16 and 42
    run_test("jum?ps", "The quick brown fox jumps over the jups lazy dog", true);  // Pattern at index 20 and 35
    run_test("qu?ick", "The qick brown fox jumps over the quick dog", true);  // Pattern at index 4 and 36
    run_test("dog?s", "The quick brown fox jumps over the lazy dogs", true);  // Pattern at index 40

    // Combination of quantifiers
    run_test("a*b+c?", "The quick brown aaabbc fox jumps over the abc lazy dog", true);  // Pattern at index 10, 16 and 37
    run_test("fo?x*y+", "The quick brown foxy jumps over the foxxxy lazy dog", true);  // Pattern at index 16 and 36
    run_test("ju?m*p+s?", "The quick brown fox jumps over the jmmmppps lazy dog", true);  // Pattern at index 20 and 35
    run_test("qu+i?c*k", "The quuuick brown fox jumps over the qck lazy dog", true);  // Pattern at index 4 and 36

    // Complex patterns
    run_test("a+b*c?d+", "The quick abbbcd brown acd fox jumps over the abcd lazy dog", true);  // Pattern at index 10, 21, and 41
    run_test("w?o*r+l?d", "Hello world! The world is a beautiful place.", true);  // Pattern at index 6 and 18
    run_test("T?h*e", "The quick brown fox jumps over thee lazy dog", true);  // Pattern at index 0 and 31
    run_test("f+o?x*y?", "The quick brown foxy jumps over the foxx lazy dog", true);  // Pattern at index 16 and 36

        // New test cases for ^ (start of string)
    run_test("^The", "The quick brown fox", true);  // Pattern at index 0
    run_test("^The", "A quick brown fox", false);  // Pattern does not match at start
    run_test("^qu.ck", "quick brown fox", true);  // Pattern at index 0
    run_test("^qu.ck", "The quick brown fox", false);  // Pattern does not match at start
    run_test("^a+b", "aaab quick brown fox", true);  // Pattern at index 0
    run_test("^a+b", "baab quick brown fox", false);  // Pattern does not match at start

    // New test cases for $ (end of string)
    run_test("dog$", "The quick brown fox jumps over the lazy dog", true);  // Pattern at end
    run_test("dog$", "The quick brown dog jumps over the lazy fox", false);  // Pattern does not match at end
    run_test("lazy.$", "The quick brown fox jumps over the lazy ", true);  // Pattern at end
    run_test("lazy.$", "The quick brown fox jumps over the lazy dog", false);  // Pattern does not match at end
    run_test("x+$", "The quick brown foxx", true);  // Pattern at end
    run_test("x+$", "The quick brown foxxy", false);  // Pattern does not match at end

    // Combination of ^ and $
    run_test("^The.*dog$", "The quick brown fox jumps over the lazy dog", true);  // Matches entire string
    run_test("^The.*dog$", "The quick brown fox jumps over the lazy cat", false);  // Does not match entire string
    run_test("^a+b+c+$", "aaabbbccc", true);  // Matches entire string
    run_test("^a+b+c+$", "aaabbbcccd", false);  // Does not match entire string
    run_test("^.$", "a", true);  // Matches single character string
    run_test("^.$", "ab", false);  // Does not match multi-character string

    // More complex patterns with ^ and $
    run_test("^The.*fox.*dog$", "The quick brown fox jumps over the lazy dog", true);
    run_test("^The.*fox.*dog$", "The quick brown fox jumps over the lazy cat", false);
    run_test("^a*b+c?d*", "bbbcdddd nice", true);
    run_test("a*b*cd*$", "i like to say aaabbbcdddd", true);
    run_test("^a*b+c?d*$", "aaabbbccdddde", false);

    return 0;
}