/*
Group Number : 14
Name : Avyakth Krishna Kumar   ID : 2021B3A71111P
Name : Suchit Chebolu          ID : 2021B1A72281P
Name : Abhiram H               ID : 2021B4A71134P
Name : Ankur Renduchintala     ID : 2021B5A71159P
Name : Vikram Hariharan        ID : 2022A7PS0013P
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int prompt_user(const char *message) {
    char response;
    printf("%s [y/N]: ", message);
    scanf(" %c", &response);  // The space before %c consumes any leading whitespace
    if (response == 'y' || response == 'Y') {
        return 1;  // Continue
    }
    return 0;  // Terminate
}

void compare_files(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "r");
    FILE *f2 = fopen(file2, "r");

    if (f1 == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", file1);
        exit(1);
    }

    if (f2 == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", file2);
        exit(1);
    }

    char line1[1024], line2[1024];
    int line_number = 1;

    while (fgets(line1, sizeof(line1), f1) != NULL && fgets(line2, sizeof(line2), f2) != NULL) {
        // Compare lines
        if (strcmp(line1, line2) != 0) {
            printf("Mismatch at line %d:\n", line_number);
            printf("Expected: %s", line1);
            printf("Received: %s", line2);

            // Ask user whether the mismatch is acceptable
            if (!prompt_user("This is an error. Do you accept this mismatch?")) {
                fclose(f1);
                fclose(f2);
                exit(1);  // Terminate if user doesn't accept the error
            }
        }
        line_number++;
    }

    // If one file has more lines than the other
    if (fgets(line1, sizeof(line1), f1) != NULL || fgets(line2, sizeof(line2), f2) != NULL) {
        printf("Files have different lengths!\n");

        // Ask user whether the length difference is acceptable
        if (!prompt_user("This is an error. Do you accept the file length mismatch?")) {
            fclose(f1);
            fclose(f2);
            exit(1);  // Terminate if user doesn't accept the length difference
        }
    }

    printf("Files match.\n");

    fclose(f1);
    fclose(f2);
}

int main(int argc, char** argv) {
    if(argc != 3)
        exit(EXIT_FAILURE);

    compare_files(argv[1], argv[2]);

    return 0;
}
