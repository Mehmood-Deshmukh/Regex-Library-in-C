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
};

void re_print(regex_t);

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
                re_print(regex_compile(pattern));
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
                re_print(regex_compile(pattern));
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
