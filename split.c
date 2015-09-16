#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void split(char* str, char* delimiters, char*** tokens_ptr, int* tokens_count) {
    int str_num = 30;
    *tokens_ptr = (char**) calloc(str_num, sizeof(char*));
    char** tokens = *tokens_ptr;
    char* cur_token = NULL;

    cur_token = strtok(str, delimiters);
    while (cur_token) {
        tokens[*tokens_count] = cur_token;
        (*tokensCount)++;
        cur_token = strtok(NULL, delimiters);
    }
    tokens[*tokens_count] = cur_token;
}

void print_tokens (char** tokens, int tokens_count) {
    int i = 0;
    for (; i < tokens_count; i++)
        printf("%s\n", tokens[i]);
}

int main() {
    char* str = (char*)malloc(sizeof(* str) * 256);
    char* delimiters = (char*)malloc(sizeof(* delimiters) * 256);
    char **tokens = NULL;
    int tokensCount = 0;
    int i = 0;

    scanf("%s", str);
    scanf("%s", delimiters);
    split(str, delimiters, &tokens, &tokensCount);
    print_tokens(tokens, tokensCount);
    printf("%d\n", tokensCount);

    for (; tokensCount >= 0; tokensCount--)
        free(tokens[tokensCount]);
    free(tokens);
    return 0;
}
