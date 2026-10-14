#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEXEME 129

static int is_keyword(const char *word) {
    const char *keywords[] = {"int", "if", "else", "while", "return"};
    size_t count = sizeof(keywords) / sizeof(keywords[0]);

    for (size_t i = 0; i < count; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

static void print_token(const char *lexeme, const char *category) {
    printf("%s,%s\n", lexeme, category);
}

static int is_delimiter(int c) {
    return c == '(' || c == ')' || c == '{' || c == '}' ||
           c == '[' || c == ']' || c == ';' || c == ',';
}

int main(int argc, char *argv[]) {
    FILE *file;
    int c;


    file = stdin;

    printf("Enter your code in terminal (Press Ctrl+Z then Enter to finish):\n");
    printf("------------------------------------------------------------------\n");

    while ((c = fgetc(file)) != EOF) {
        char lexeme[MAX_LEXEME];
        size_t length = 0;

        if (isspace((unsigned char)c)) {
            continue;
        }

        if (isalpha((unsigned char)c) || c == '_') {
            do {
                if (length < MAX_LEXEME - 1) {
                    lexeme[length++] = (char)c;
                }
                c = fgetc(file);
            } while (c != EOF &&
                     (isalnum((unsigned char)c) || c == '_'));

            lexeme[length] = '\0';
            if (c != EOF) {
                ungetc(c, file);
            }

            print_token(lexeme, is_keyword(lexeme) ? "KEYWORD" : "IDENTIFIER");
            continue;
        }

        if (isdigit((unsigned char)c)) {
            do {
                if (length < MAX_LEXEME - 1) {
                    lexeme[length++] = (char)c;
                }
                c = fgetc(file);
            } while (c != EOF && isdigit((unsigned char)c));

            lexeme[length] = '\0';
            if (c != EOF) {
                ungetc(c, file);
            }

            print_token(lexeme, "INTEGER");
            continue;
        }

        if (c == '/') {
            int next = fgetc(file);

            if (next == '/') {
                while ((c = fgetc(file)) != EOF && c != '\n') {
                    /* Skip a line comment. */
                }
                continue;
            }

            if (next == '*') {
                int previous = 0;
                int closed = 0;

                while ((c = fgetc(file)) != EOF) {
                    if (previous == '*' && c == '/') {
                        closed = 1;
                        break;
                    }
                    previous = c;
                }

                if (!closed) {
                    fprintf(stderr, "Error: unclosed block comment\n");
                    return 1;
                }
                continue;
            }

            if (next != EOF) {
                ungetc(next, file);
            }
            print_token("/", "ARITHMETIC_OPERATOR");
            continue;
        }

        if (c == '=' || c == '!' || c == '<' || c == '>' ||
            c == '&' || c == '|') {
            int next = fgetc(file);

            if ((c == '=' && next == '=') ||
                (c == '!' && next == '=') ||
                (c == '<' && next == '=') ||
                (c == '>' && next == '=') ||
                (c == '&' && next == '&') ||
                (c == '|' && next == '|')) {
                lexeme[0] = (char)c;
                lexeme[1] = (char)next;
                lexeme[2] = '\0';

                if (c == '&' || c == '|') {
                    print_token(lexeme, "LOGICAL_OPERATOR");
                } else {
                    print_token(lexeme, "RELATIONAL_OPERATOR");
                }
                continue;
            }

            if (next != EOF) {
                ungetc(next, file);
            }

            lexeme[0] = (char)c;
            lexeme[1] = '\0';

            if (c == '=') {
                print_token(lexeme, "ASSIGNMENT_OPERATOR");
            } else if (c == '!') {
                print_token(lexeme, "LOGICAL_OPERATOR");
            } else if (c == '<' || c == '>') {
                print_token(lexeme, "RELATIONAL_OPERATOR");
            } else {
                print_token(lexeme, "UNKNOWN");
            }
            continue;
        }

        lexeme[0] = (char)c;
        lexeme[1] = '\0';

        if (c == '+' || c == '-' || c == '*' || c == '%') {
            print_token(lexeme, "ARITHMETIC_OPERATOR");
        } else if (is_delimiter(c)) {
            print_token(lexeme, "DELIMITER");
        } else if ((unsigned char)c < 128) {
            print_token(lexeme, "UNKNOWN");
        } else {
            print_token(lexeme, "UNKNOWN");
        }
    }

    return 0;
}
