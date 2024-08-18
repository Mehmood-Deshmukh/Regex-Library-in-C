#include "../regex.h"
#include <stdio.h>

#define OK ((char *)1)
#define NOK ((char *)0)

#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_RESET "\033[0m"

char *test_vector[][4] = {
        /* Dot (.) - Matches any character */
        {OK, ".", "a", (char *)1},
        {OK, "a.c", "abc", (char *)3},
        {OK, "a.c", "a_c", (char *)3},

        /* Start anchor (^) - Matches beginning of string */
        {OK, "^abc", "abc", (char *)3},
        {NOK, "^abc", "zabc", (char *)0},

        /* End anchor ($) - Matches end of string */
        {OK, "abc$", "zabc", (char *)3},
        {NOK, "abc$", "abcz", (char *)0},

        /* Asterisk (*) - Matches zero or more (greedy) */
        {OK, "a*", "aaa", (char *)3},
        {OK, "a*", "b", (char *)0},
        {OK, "a*b", "aaab", (char *)4},

        /* Plus (+) - Matches one or more (greedy) */
        {OK, "a+", "aaa", (char *)3},
        {NOK, "a+", "b", (char *)0},
        {OK, "a+b", "aaab", (char *)4},

        /* Question mark (?) - Matches zero or one (non-greedy) */
        {OK, "a?b", "abaab", (char *)2},
        {OK, "a?", "b", (char *)0},
        {OK, "a?b", "ab", (char *)2},
        {OK, "a?b", "b", (char *)1},

        /* Combining multiple operators */
        {OK, ".*c", "abcabc", (char *)6},
        {OK, ".+c", "abcabc", (char *)6},
        {OK, ".?bar", "real_bar", (char *)4},
        {NOK, ".?bar", "real_foo", (char *)0},
        {NOK, "X?Y", "Z", (char *)0},

        /* Digit (\d) - Matches any digit [0-9] */
        {OK, "\\d+", "12345", (char *)5},
        {NOK, "\\d+", "abc", (char *)0},
        {OK, "\\d+", "123abc", (char *)3},
        {OK, "\\d+", "abc123", (char *)3},

        /* Non-digit (\D) - Matches any non-digit */
        {OK, "\\D+", "abc", (char *)3},
        {NOK, "\\D+", "123", (char *)0},
        {OK, "\\D+", "abc123", (char *)3},
        {OK, "\\D+", "123abc", (char *)3},

        /* Alphanumeric (\w) - Matches any alphanumeric character [a-zA-Z0-9_] */
        {OK, "\\w+", "abc_123", (char *)7},
        {NOK, "\\w+", "!!@", (char *)0},
        {OK, "\\w+", "abc", (char *)3},
        {NOK, "\\w+", " !!", (char *)0},

        /* Non-alphanumeric (\W) - Matches any non-alphanumeric character */
        {OK, "\\W+", "!@#", (char *)3},
        {NOK, "\\W+", "abc", (char *)0},
        {OK, "\\W+", "abc!@#", (char *)3},
        {OK, "\\W+", "mehm!@#ood", (char *)3},

        /* Whitespace (\s) - Matches any whitespace character [ \t\n\r\f\v] */
        {OK, "\\s+", " \t\n", (char *)3},
        {NOK, "\\s+", "abc", (char *)0},
        {OK, "\\s+", "abc \t", (char *)2},
        {NOK, "\\s+", "abc", (char *)0},

        /* Non-whitespace (\S) - Matches any non-whitespace character */
        {OK, "\\S+", "abc123", (char *)6},
        {NOK, "\\S+", " \t\n", (char *)0},
        {OK, "\\S+", "abc 123", (char *)3},
        {NOK, "\\S+", " \t ", (char *)0},

        /* Combining multiple operators with \d, \D, \w, \W, \s, \S */
        {OK, "\\d+\\W+\\w+", "123!_abc", (char *)8},
        {NOK, "\\d+\\W+\\w+", "123abc", (char *)0},
        {OK, "\\S+\\s+\\S+", "abc 123def", (char *)10},
        {OK, "\\S+\\s+\\S+", "abc 123", (char *)7},

        //   final test for 16/08/2024
        {OK, "\\d+\\D*\\d+.*\\d+", "12ab34__56zz789", (char *)15},
        {OK, "\\S+\\s+\\w+\\W+\\d+", "Hello World! 123", (char *)16},
        {OK, "^\\w+\\s+\\d+:\\d+\\s+\\S+$", "hello 12:30 world!", (char *)18},

        // tests for classes
        {OK, "[abc]", "a", (char *)1},
        {OK, "[abc]", "b", (char *)1},
        {OK, "[abc]", "c", (char *)1},
        {NOK, "[abc]", "d", (char *)0},
        {OK, "[meh]+[mood]+", "mehmood", (char *)7},
        {NOK, "[abc]+", "d", (char *)0},

        {OK, "[^abc]+", "aaamehmaaabbccc", (char *)4},
        {OK, "[^abc]", "d", (char *)1},
        {OK, "[^aeiou]+", "bcdfgh", (char *)6},
        {OK, "[^abc]+def", "xyzdef", (char *)6},
        {OK, "[^abc]+b", "xyzb", (char *)4},
        {OK, "[d-z]+b", "xyzb", (char *)4},
        // complex
        {OK, "\\w+@\\w+\\.com", "my email is mehmood@email.com", (char *)17},
        /* Complex pattern: \s+\w+@\d+ */
        {OK, "\\s+\\w+@\\d+", "My email is test123@456", (char *)12},
        {OK, "\\s+\\w+@\\d+", "Contact me at john_doe@12345", (char *)15},
        {NOK, "\\s+\\w+@\\d+", "No email here", (char *)0},
        {OK, "\\s+\\w+@\\d+", "Please reach out to alice@42", (char *)9},
        {NOK, "\\s+\\w+@\\d+", "hello world!", (char *)0},

        /* More complex pattern with multiple matches */
        {OK, "\\s+\\w+@\\d+", "Emails: jane_doe@99 and bob_smith@2020", (char *)12},
        {OK, "\\s+\\w+@\\d+", "First contact: alice@1; second: charlie@123", (char *)8},

        /* Edge case with surrounding text */
        {OK, "\\s+\\w+@\\d+", "Here is a number john_doe@789 in the middle of the text.", (char *)13},
        {NOK, "\\s+\\w+@\\d+", "john_doe@789 without spaces before it", (char *)0},
        {OK, "\\d", "5", (char *)1},
        {OK, "\\w+", "hej", (char *)3},
        {OK, "\\s", "\t \n", (char *)1},
        {NOK, "\\S", "\t \n", (char *)0},
        {OK, "[\\s]", "\t \n", (char *)1},
        {NOK, "[\\S]", "\t \n", (char *)0},
        {NOK, "\\D", "5", (char *)0},
        {NOK, "\\W+", "hej", (char *)0},
        {OK, "[0-9]+", "12345", (char *)5},
        {OK, "\\D", "hej", (char *)1},
        {NOK, "\\d", "hej", (char *)0},
        {OK, "[^\\w]", "\\", (char *)1},
        {OK, "[\\W]", "\\", (char *)1},
        {NOK, "[\\w]", "\\", (char *)0},
        {OK, "[^\\d]", "d", (char *)1},
        {NOK, "[\\d]", "d", (char *)0},
        {NOK, "[^\\D]", "d", (char *)0},
        {OK, "[\\D]", "d", (char *)1},
        {OK, "^.*\\\\.*$", "c:\\Tools", (char *)8},
        {OK, "^[\\+-]*[\\d]+$", "+27", (char *)3},
        {OK, "[abc]", "1c2", (char *)1},
        {NOK, "[abc]", "1C2", (char *)0},
        {OK, "[1-5]+", "0123456789", (char *)5},
        {OK, "[.2]", "1C2", (char *)1},
        {OK, "a*$", "Xaa", (char *)2},
        {OK, "a*$", "Xaa", (char *)2},
        {OK, "[a-h]+", "abcdefghxxx", (char *)8},
        {NOK, "[a-h]+", "ABCDEFGH", (char *)0},
        {OK, "[A-H]+", "ABCDEFGH", (char *)8},
        {NOK, "[A-H]+", "abcdefgh", (char *)0},
        {OK, "[^\\s]+", "abc def", (char *)3},
        {OK, "[^fc]+", "abc def", (char *)2},
        {OK, "[^d\\sf]+", "abc def", (char *)3},
        {OK, "\n", "abc\ndef", (char *)1},
        {OK, "b.\\s*\n", "aa\r\nbb\r\ncc\r\n\r\n", (char *)4},
        {OK, ".*c", "abcabc", (char *)6},
        {OK, ".+c", "abcabc", (char *)6},
        {OK, "[b-z].*", "ab", (char *)1},
        {OK, "b[k-z]*", "ab", (char *)1},
        {NOK, "[0-9]", "  - ", (char *)0},
        {OK, "[^0-9]", "  - ", (char *)1},
        {OK, "0|", "0|", (char *)2},
        {NOK, "\\d\\d:\\d\\d:\\d\\d", "0s:00:00", (char *)0},
        {NOK, "\\d\\d:\\d\\d:\\d\\d", "000:00", (char *)0},
        {NOK, "\\d\\d:\\d\\d:\\d\\d", "00:0000", (char *)0},
        {NOK, "\\d\\d:\\d\\d:\\d\\d", "100:0:00", (char *)0},
        {NOK, "\\d\\d:\\d\\d:\\d\\d", "00:100:00", (char *)0},
        {NOK, "\\d\\d:\\d\\d:\\d\\d", "0:00:100", (char *)0},
        {OK, "\\d\\d?:\\d\\d?:\\d\\d?", "0:0:0", (char *)5},
        {OK, "\\d\\d?:\\d\\d?:\\d\\d?", "0:00:0", (char *)6},
        {OK, "\\d\\d?:\\d\\d?:\\d\\d?", "0:0:00", (char *)5},
        {OK, "\\d\\d?:\\d\\d?:\\d\\d?", "00:0:0", (char *)6},
        {OK, "\\d\\d?:\\d\\d?:\\d\\d?", "00:00:0", (char *)7},
        {OK, "\\d\\d?:\\d\\d?:\\d\\d?", "00:0:00", (char *)6},
        {OK, "\\d\\d?:\\d\\d?:\\d\\d?", "0:00:00", (char *)6},
        {OK, "\\d\\d?:\\d\\d?:\\d\\d?", "00:00:00", (char *)7},
        {OK, "[Hh]ello [Ww]orld\\s*[!]?", "Hello world !", (char *)12},
        {OK, "[Hh]ello [Ww]orld\\s*[!]?", "hello world !", (char *)12},
        {OK, "[Hh]ello [Ww]orld\\s*[!]?", "Hello World !", (char *)12},
        {OK, "[Hh]ello [Ww]orld\\s*[!]?", "Hello world!   ", (char *)11},
        {OK, "[Hh]ello [Ww]orld\\s*[!]?", "Hello world  !", (char *)13},
        {OK, "[Hh]ello [Ww]orld\\s*[!]?", "hello World    !", (char *)15},
        {NOK, "\\d\\d?:\\d\\d?:\\d\\d?", "a:0", (char *)0}, 

        {OK, "[^\\w][^-1-4]", ")T", (char *)2},
        {OK, "[^\\w][^-1-4]", ")^", (char *)2},
        {OK, "[^\\w][^-1-4]", "*)", (char *)2},
        {OK, "[^\\w][^-1-4]", "!.", (char *)2},
        {OK, "[^\\w][^-1-4]", " x", (char *)2},
        {OK, "[^\\w][^-1-4]", "$b", (char *)2},

        {OK, ".?bar", "real_bar", (char *)4},
        {NOK, ".?bar", "real_foo", (char *)0},
        {NOK, "X?Y", "Z", (char *)0},
        {OK, "[a-z]+\nbreak", "blahblah\nbreak", (char *)14},
        {OK, "[a-z\\s]+\nbreak", "bla bla \nbreak", (char *)14},
};

void regex_print(regex_t);

int main(){
    char *text;
    char *pattern;
    int should_fail;
    int length;
    int correctlen;
    size_t ntests = sizeof(test_vector) / sizeof(*test_vector);
    size_t nfailed = 0;
    size_t i;

    for (i = 0; i < ntests; ++i){
        pattern = test_vector[i][1];
        text = test_vector[i][2];
        should_fail = (test_vector[i][0] == NOK);
        correctlen = (int)(test_vector[i][3]);

        int m = regex_match(pattern, text, &length);

        if (should_fail){
            if (m != (-1)){
                printf(COLOR_RED);
                printf("\n");
                regex_print(regex_compile(pattern));
                fprintf(stderr, "[%lu/%lu]: pattern '%s' matched '%s' unexpectedly, matched %i chars. \n", (i + 1), ntests, pattern, text, length);
                printf(COLOR_RESET);
                nfailed += 1;
            }
        }
        else {
            if (m == (-1)) {
                printf(COLOR_RED);
                printf("\n");
                regex_print(regex_compile(pattern));
                fprintf(stderr, "[%lu/%lu]: pattern '%s' didn't match '%s' as expected. \n", (i + 1), ntests, pattern, text);
                printf(COLOR_RESET);
                nfailed += 1;
            }
            else if (length != correctlen){
                printf(COLOR_RED);
                fprintf(stderr, "[%lu/%lu]: pattern '%s' matched '%i' chars of '%s'; expected '%i'. \n", (i + 1), ntests, pattern, length, text, correctlen);
                printf(COLOR_RESET);
                nfailed += 1;
            }
            else {
                printf(COLOR_GREEN);
                printf("[%lu/%lu]: pattern '%s' matched '%s' as expected. \n", (i + 1), ntests, pattern, text);
                printf(COLOR_RESET);
            }
        }
    }

    printf("\n");
    printf(COLOR_GREEN "%lu/%lu tests succeeded.\n" COLOR_RESET, ntests - nfailed, ntests);
    printf("\n");

    return nfailed;
}
