#include "regex.h"
#include <stdio.h>

#define OK    ((char*) 1)
#define NOK   ((char*) 0)

#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_RESET "\033[0m"

char* test_vector[][4] =
{
  /* Dot (.) - Matches any character */
  { OK,  ".",                         "a",               (char*) 1      },
  { OK,  "a.c",                       "abc",             (char*) 3      },
  { OK,  "a.c",                       "a_c",             (char*) 3      },

  /* Start anchor (^) - Matches beginning of string */
  { OK,  "^abc",                      "abc",             (char*) 3      },
  { NOK, "^abc",                      "zabc",            (char*) 0      },
  
  /* End anchor ($) - Matches end of string */
  { OK,  "abc$",                      "zabc",            (char*) 3      },
  { NOK, "abc$",                      "abcz",            (char*) 0      },

  /* Asterisk (*) - Matches zero or more (greedy) */
  { OK,  "a*",                        "aaa",             (char*) 3      },
  { OK,  "a*",                        "b",               (char*) 0      },
  { OK,  "a*b",                       "aaab",            (char*) 4      },
  
  /* Plus (+) - Matches one or more (greedy) */
  { OK,  "a+",                        "aaa",             (char*) 3      },
  { NOK, "a+",                        "b",               (char*) 0      },
  { OK,  "a+b",                       "aaab",            (char*) 4      },

  /* Question mark (?) - Matches zero or one (non-greedy) */
  { OK,  "a?b",                        "abaab",             (char*) 2      },
  { OK,  "a?",                        "b",               (char*) 0      },
  { OK,  "a?b",                       "ab",              (char*) 2      },
  { OK,  "a?b",                       "b",               (char*) 1      },
  
  /* Combining multiple operators */
  { OK,  ".*c",                       "abcabc",          (char*) 6      },
  { OK,  ".+c",                       "abcabc",          (char*) 6      },
  { OK,  ".?bar",                     "real_bar",        (char*) 4      },
  { NOK, ".?bar",                     "real_foo",        (char*) 0      },
  { NOK, "X?Y",                       "Z",               (char*) 0      },

    /* Digit (\d) - Matches any digit [0-9] */
  { OK,  "\\d+",                        "12345",          (char*) 5      },
  { NOK, "\\d+",                        "abc",            (char*) 0      },
  { OK,  "\\d+",                        "123abc",         (char*) 3      },
  { OK, "\\d+",                        "abc123",         (char*) 3      },

  /* Non-digit (\D) - Matches any non-digit */
  { OK,  "\\D+",                        "abc",            (char*) 3      },
  { NOK, "\\D+",                        "123",            (char*) 0      },
  { OK,  "\\D+",                        "abc123",         (char*) 3      },
  { OK, "\\D+",                        "123abc",         (char*) 3      },

  /* Alphanumeric (\w) - Matches any alphanumeric character [a-zA-Z0-9_] */
  { OK,  "\\w+",                        "abc_123",        (char*) 7      },
  { NOK, "\\w+",                        "!!@",            (char*) 0      },
  { OK,  "\\w+",                        "abc",            (char*) 3      },
  { NOK, "\\w+",                        " !!",            (char*) 0      },

  /* Non-alphanumeric (\W) - Matches any non-alphanumeric character */
  { OK,  "\\W+",                        "!@#",            (char*) 3      },
  { NOK, "\\W+",                        "abc",            (char*) 0      },
  { OK,  "\\W+",                        "abc!@#",         (char*) 3      },
  { OK, "\\W+",                        "mehm!@#ood",         (char*) 3      },

  /* Whitespace (\s) - Matches any whitespace character [ \t\n\r\f\v] */
  { OK,  "\\s+",                        " \t\n",          (char*) 3      },
  { NOK, "\\s+",                        "abc",            (char*) 0      },
  { OK,  "\\s+",                        "abc \t",         (char*) 2      },
  { NOK, "\\s+",                        "abc",          (char*) 0      },

  /* Non-whitespace (\S) - Matches any non-whitespace character */
  { OK,  "\\S+",                        "abc123",         (char*) 6      },
  { NOK, "\\S+",                        " \t\n",          (char*) 0      },
  { OK,  "\\S+",                        "abc 123",        (char*) 3      },
  { NOK, "\\S+",                        " \t ",           (char*) 0      },

  /* Combining multiple operators with \d, \D, \w, \W, \s, \S */
  { OK,  "\\d+\\W+\\w+",                "123!_abc",       (char*) 8      },
  { NOK, "\\d+\\W+\\w+",                "123abc",         (char*) 0      },
  { OK,  "\\S+\\s+\\S+",                "abc 123def",     (char*) 10      },
  { OK, "\\S+\\s+\\S+",                "abc 123",         (char*) 7      },

    //   final test for 16/08/2024
  { OK, "\\d+\\D*\\d+.*\\d+", "12ab34__56zz789", (char*) 15 },
  { OK, "\\S+\\s+\\w+\\W+\\d+", "Hello World! 123", (char*) 16 },
  { OK, "^\\w+\\s+\\d+:\\d+\\s+\\S+$", "hello 12:30 world!", (char*) 18 },

  // tests for classes
    { OK,  "[abc]",                       "a",              (char*) 1      },
    { OK,  "[abc]",                       "b",              (char*) 1      },
    { OK,  "[abc]",                       "c",              (char*) 1      },
    { NOK, "[abc]",                       "d",              (char*) 0      },
    { OK,  "[meh]+[mood]+",                     "mehmood",          (char*) 7      },
    { NOK, "[abc]+",                     "d",              (char*) 0      },

    //complex 
    { OK,  "\\w+@\\w+\\.com",                       "my email is mehmood@email.com",              (char*) 17      },
};

void regex_print(regex_t);

int main()
{
    char* text;
    char* pattern;
    int should_fail;
    int length;
    int correctlen;
    size_t ntests = sizeof(test_vector) / sizeof(*test_vector);
    size_t nfailed = 0;
    size_t i;

    for (i = 0; i < ntests; ++i)
    {
        pattern = test_vector[i][1];
        text = test_vector[i][2];
        should_fail = (test_vector[i][0] == NOK);
        correctlen = (int)(test_vector[i][3]);

        int m = regex_match(pattern, text, &length);

        if (should_fail)
        {
            if (m != (-1))
            {
                printf(COLOR_RED);
                printf("\n");
                regex_print(regex_compile(pattern));
                fprintf(stderr, "[%lu/%lu]: pattern '%s' matched '%s' unexpectedly, matched %i chars. \n", (i+1), ntests, pattern, text, length);
                printf(COLOR_RESET);
                nfailed += 1;
            }
        }
        else
        {
            if (m == (-1))
            {
                printf(COLOR_RED);
                printf("\n");
                regex_print(regex_compile(pattern));
                fprintf(stderr, "[%lu/%lu]: pattern '%s' didn't match '%s' as expected. \n", (i+1), ntests, pattern, text);
                printf(COLOR_RESET);
                nfailed += 1;
            }
            else if (length != correctlen)
            {
                printf(COLOR_RED);
                fprintf(stderr, "[%lu/%lu]: pattern '%s' matched '%i' chars of '%s'; expected '%i'. \n", (i+1), ntests, pattern, length, text, correctlen);
                printf(COLOR_RESET);
                nfailed += 1;
            }
            else
            {
                printf(COLOR_GREEN);
                printf("[%lu/%lu]: pattern '%s' matched '%s' as expected. \n", (i+1), ntests, pattern, text);
                printf(COLOR_RESET);
            }
        }
    }

    printf("\n");
    printf(COLOR_GREEN "%lu/%lu tests succeeded.\n" COLOR_RESET, ntests - nfailed, ntests);
    printf("\n");

    return nfailed;
}
