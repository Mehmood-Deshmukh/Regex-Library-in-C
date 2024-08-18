# Regex Library in C

## Overview

This C library provides a lightweight implementation of regular expressions (regex), focusing on efficiency and ease of use. It provides the users with functionality to compile regex patterns, match them against text, and perform replacements.

## Features

The library supports the following regex operators:

- **Literal Characters**: Matches exact characters in the text.
- **Dot (.)**: Matches any single character except newline characters (`\n` and `\r`).
- **Star (*)**: Matches zero or more occurrences of the preceding element.
- **Plus (+)**: Matches one or more occurrences of the preceding element.
- **Question (?)**: Matches zero or one occurrence of the preceding element.
- **Character Classes ([...])**: Matches any single character contained within the brackets. Supports:
  - **Character Ranges**: Matches a range of characters (e.g., `[a-z]`).
  - **Special Classes**: Matches specific types of characters:
    - `\d`: Digit (0-9)
    - `\w`: Word character (alphanumeric and underscore)
    - `\s`: Whitespace character (space, tab, newline, etc.)
    - `\D`: Non-digit
    - `\W`: Non-word character
    - `\S`: Non-whitespace character
  - **Inverted Character Classes**: Matches any character not listed in the brackets (e.g., `[^a-z]`).
- **Anchors**:
  - **Caret (^)**: Matches the beginning of a line or string.
  - **Dollar ($)**: Matches the end of a line or string.


## Usage

### Compilation

To compile the library, use the following command:

```sh
gcc -o regex_test regex.c 
```

## Example
Here's a simple example of how to use the library to match a pattern against a text:

```c
#include "regex.h"

int main() {
    const char* pattern = "a*b";
    const char* text = "aaab";
    int matchLength;

    int result = regex_match(pattern, text, &matchLength);
    if (result >= 0) {
        printf("Match found at position %d with length %d\n", result, matchLength);
    } else {
        printf("No match found\n");
    }

    return 0;
}
```

## Functions

### `int regex_match(const char* pattern, const char* text, int* matchLength)`

Matches the `pattern` against the `text` and stores the length of the match in `matchLength`. Returns the position of the match or `-1` if no match is found.

### `char* regex_replace(const char* pattern, const char* text, const char* replacement)`

Replaces matches of `pattern` in `text` with the `replacement` string. Returns a new string with replacements.


## Inspirations and References

- **tiny-regex-c**: provided a solid foundation for building this library.

- **Brian Kernighan's A regular expression matcher**: The comprehensive explanations and implementations of regex and related algorithms with reference to **Rob Pike's** work on regex implementation and related concepts

## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.

- If you have suggestions for adding or removing features, feel free to [open an issue](https://github.com/Mehmood-Deshmukh/Regex-Library-in-C) to discuss it, or directly create a pull request with necessary changes.
- Create individual PR for each suggestion.

### Creating A Pull Request

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request


## Ideas for contributions

### Extended Regex Features

- **Grouping `()`**: Allow for capturing groups to enable grouping of patterns and extraction of matched subexpressions.
- **Alternation `|`**: Support the alternation operator to match one of several alternatives (e.g., `a|b` matches either `a` or `b`).
- **Quantifiers `{}`**: Add support for specifying exact or range-based occurrences of the preceding element (e.g., `a{2,5}` matches `a` between 2 and 5 times).

### Performance Optimization

- Enhance the efficiency of the matching algorithm for large texts and complex patterns.

### Unicode Support

- Add support for Unicode characters and regex patterns.

### Error Handling

- Improve error handling and reporting mechanisms.


### Connect with me

- LinkedIn: [Mehmood Deshmukh](https://www.linkedin.com/in/mehmood-deshmukh-93533a2a7/)
- GitHub: [Mehmood Deshmukh](https://github.com/Mehmood-Deshmukh)

Feel free to reach out for collaboration, feedback, or just to say hi!

