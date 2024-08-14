#ifndef REGEX_H
#define REGEX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


bool regex_match(const char* pattern, const char* text, int* index);

#endif