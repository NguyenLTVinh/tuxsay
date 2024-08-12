#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define QUOTE_FILE "quotes.txt"
#define MAX_QUOTES 1000
#define MAX_QUOTE_LEN 1000
#define MAX_LINES 100
#define MAX_LINE_LENGTH 256 

void print_help() {
    printf("Usage: tuxsay [--help | --fortune | <message>] [<character>]\n");
    printf("  --help         Display this help message.\n");
    printf("  --fortune      Display a random fortune with the specified character.\n");
    printf("  <message>      The message to be displayed in the message box.\n");
    printf("  <character>    The name of the ASCII art file to use (without .txt extension).\n");
}

void print_message_box(const char *message, int include_attribution) {
    char *columns_env = getenv("COLUMNS");
    int terminal_width = columns_env ? atoi(columns_env) : 80;
    int max_box_width = terminal_width / 2;
    int len = strlen(message);
    int box_width;
    
    const char *attribution = "- Linus Torvalds";
    int attribution_len = strlen(attribution);
    
    box_width = len + 4 < max_box_width ? len + 4 : max_box_width;

    printf("  ");
    for (int i = 0; i < box_width; i++) {
        printf("_");
    }
    printf("\n");
    
    if (len + 4 < max_box_width) {
        printf("  | %s", message);
        for (int i = len; i < box_width - 4; i++) {
            printf(" ");
        }
        printf(" |\n");
    } else {
        int start = 0, end;
        while (start < len) {
            int line_length = (len - start) < (box_width - 4) ? (len - start) : (box_width - 4);
            end = start + line_length;
            if (end < len && message[end] != ' ') {
                while (end > start && message[end] != ' ') {
                    end--;
                }
            }
            if (end == start) {
                end = start + line_length;
            }
            printf("  | ");
            for (int i = start; i < end; i++) {
                printf("%c", message[i]);
            }
            for (int i = end; i < start + (box_width - 4); i++) {
                printf(" ");
            }
            printf(" |\n");
            start = end;
            while (start < len && message[start] == ' ') {
                start++;
            }
        }
    }

    if (include_attribution) {
        printf("  |");
        for (int i = 0; i < box_width - 4 - attribution_len; i++) {
            printf(" ");
        }
        printf("%s  |\n", attribution);
    }

    printf("  ");
    for (int i = 0; i < box_width; i++) {
        printf("_");
    }
    printf("\n");
    printf("    \\\n");
    printf("     \\\n");
}

char **load_ascii_art(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    char **ascii_art = malloc(MAX_LINES * sizeof(char *));
    if (!ascii_art) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    char buffer[MAX_LINE_LENGTH];
    int i = 0;

    while (i < MAX_LINES && fgets(buffer, sizeof(buffer), file)) {
        ascii_art[i] = strdup(buffer);
        if (!ascii_art[i]) {
            perror("Memory allocation failed");
            break;
        }
        i++;
    }
    ascii_art[i] = NULL;

    fclose(file);
    return ascii_art;
}

void free_ascii_art(char **ascii_art) {
    if (!ascii_art) return;

    for (int i = 0; ascii_art[i] != NULL; i++) {
        free(ascii_art[i]);
    }
    free(ascii_art);
}

void print_tux(const char *message, const char *character, int include_attribution) {
    char filename[256];
    snprintf(filename, sizeof(filename), "./characters/%s.txt", character);
    
    char **ascii_art = load_ascii_art(filename);
    if (!ascii_art) {
        return;
    }

    print_message_box(message, include_attribution);

    for (int i = 0; ascii_art[i] != NULL; i++) {
        printf("%s", ascii_art[i]);
    }
    printf("\n");

    free_ascii_art(ascii_art);
}

void load_quotes(char quotes[][MAX_QUOTE_LEN], int *quote_count) {
    FILE *file = fopen(QUOTE_FILE, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open quotes file.\n");
        exit(1);
    }

    char buffer[MAX_QUOTE_LEN * MAX_QUOTES];
    fread(buffer, sizeof(char), sizeof(buffer), file);
    fclose(file);

    char *quote = strtok(buffer, "#");
    while (quote != NULL && *quote_count < MAX_QUOTES) {
        strncpy(quotes[*quote_count], quote, MAX_QUOTE_LEN);
        quotes[*quote_count][MAX_QUOTE_LEN - 1] = '\0';
        (*quote_count)++;
        quote = strtok(NULL, "#");
    }
}

void print_random_fortune(const char *character) {
    char quotes[MAX_QUOTES][MAX_QUOTE_LEN];
    int quote_count = 0;

    load_quotes(quotes, &quote_count);

    if (quote_count == 0) {
        fprintf(stderr, "Error: No quotes found in the file.\n");
        return;
    }

    srand(time(NULL));
    int random_index = rand() % quote_count;
    print_tux(quotes[random_index], character, 1);
}

int main(int argc, char *argv[]) {
    const char *default_character = "tux";

    if (argc < 2) {
        fprintf(stderr, "Error: Missing argument.\n");
        print_help();
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0) {
        print_help();
    } else if (strcmp(argv[1], "--fortune") == 0) {
        const char *character = argc >= 3 ? argv[2] : default_character;
        print_random_fortune(character);
    } else {
        const char *character = argc >= 3 ? argv[2] : default_character;
        print_tux(argv[1], character, 0);
    }

    return 0;
}
