#include "regex.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define OK ((char *)1)
#define NOK ((char *)0)

#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_RESET "\033[0m"

char *test_replace[][5] =
{
    /* Simple replacement */
    {OK, "quick", "slow", "The quick brown fox jumps over the lazy dog", "The slow brown fox jumps over the lazy dog"},
    {OK, "lazy", "energetic", "The quick brown fox jumps over the lazy dog", "The quick brown fox jumps over the energetic dog"},
    {OK, "fox", "cat", "The quick brown fox jumps over the lazy dog", "The quick brown cat jumps over the lazy dog"},

    /* Dot (.) - Wildcard replacement */
    {OK, "b.*n", "bird", "The quick brown fox jumps over the lazy dog", "The quick bird fox jumps over the lazy dog"},
    {OK, "o.e", "X", "One of the odes was often overplayed", "One of the Xs was often Xrplayed"},

    /* Anchors (^) and ($) - Start and end replacements */
    {OK, "^The", "A", "The quick brown fox jumps over the lazy dog", "A quick brown fox jumps over the lazy dog"},
    {OK, "dog$", "cat", "The quick brown fox jumps over the lazy dog", "The quick brown fox jumps over the lazy cat"},


    /* Replacements with multiple occurrences */
    {OK, "over", "under", "The quick brown fox jumps over the lazy dog over the hill", "The quick brown fox jumps under the lazy dog under the hill"},

    {NOK, "elephant", "giraffe", "The quick brown fox jumps over the lazy dog", "The quick brown fox jumps over the lazy dog"},
    {NOK, "^fox", "cat", "The quick brown fox jumps over the lazy dog", "The quick brown fox jumps over the lazy dog"},

    {OK, "\\[\\w*\\]", "", "This is a [test] string with [brackets].", "This is a  string with ."},
    {OK, "https?://\\S+", "", "Visit https://example.com", "Visit "},
    {OK, "www\\.\\S+\\.com", "", "Visit www.example.com", "Visit "},

    {OK, "</?\\w*>", "", "<html><body>This is a test</body></html>", "This is a test"},
    {OK, "[,\\.\"]", "", "Remove punctuation, e.g., commas, periods.", "Remove punctuation eg commas periods"},
    {OK, "\\w*\\d\\w*", "", "Test 123 and 456", "Test  and "},
};

void regex_print(regex_t);

int main()
{
    char *text;
    char *pattern;
    char *replacement;
    char *result;
    size_t ntests = sizeof(test_replace) / sizeof(*test_replace);
    size_t npassed = 0;
    size_t nfailed = 0;
    size_t i;

    for (i = 0; i < ntests; ++i)
    {
        pattern = test_replace[i][1];
        replacement = test_replace[i][2];
        text = test_replace[i][3];
        char *expected = test_replace[i][4];
        char *text_copy = strdup(text);
        result = regex_replace(pattern, text_copy, replacement);

        if (result == NULL)
        {
            printf(COLOR_RED);
            printf("Pattern: '%s', Replacement: '%s'\nOriginal: '%s'\nResult: No replacement made\nExpected: '%s'\n\n", pattern, replacement, text, expected);
            nfailed++;
            free(text_copy);
        }
        else
        {
            if (strcmp(result, expected) == 0)
            {
                printf(COLOR_GREEN);
                printf("Pattern: '%s', Replacement: '%s'\nOriginal: '%s'\nResult: '%s'\nExpected: '%s'\n\n", pattern, replacement, text, result, expected);
                npassed++;
            }
            else
            {
                printf(COLOR_RED);
                printf("Pattern: '%s', Replacement: '%s'\nOriginal: '%s'\nResult: '%s'\nExpected: '%s'\n\n", pattern, replacement, text, result, expected);
                nfailed++;
            }
            free(result);
            free(text_copy);
        }
    }

    printf(COLOR_RESET);
    printf(COLOR_GREEN "%zu/%zu test cases passed.\n" COLOR_RESET, npassed, ntests);
    if(nfailed)
        printf(COLOR_RED "%zu/%zu test cases failed.\n" COLOR_RESET, nfailed, ntests);

    return nfailed;
}
