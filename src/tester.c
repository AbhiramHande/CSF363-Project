#define _POSIX_C_SOURCE 200809L
#define BUFFER_SIZE 1024

#include <pty.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h> 
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

int process_line(const char *line, FILE* file_ptr) {
    char* expected_output = NULL;
    size_t buf_size = 0;
    int chars_read = getline(&expected_output, &buf_size, file_ptr);
    if (expected_output == NULL) {
        fprintf(stderr, "\033[31mError: Unexpected line:\033[0m \"%s\"\n", line);
        return -1;
    }

    if(expected_output[chars_read - 1] == '\n')
        expected_output[chars_read - 1] = '\0';
    if (strcmp(line, expected_output)) {
        fprintf(stderr, "\033[32mExpected:\033[0m \"%s\"\n", expected_output);
        fprintf(stderr, "\033[31mReceived:\033[0m \"%s\"\n", line);     
        fflush(stderr);   
        return -1;
    }
    #ifdef LTEST
        printf("\033[32m\u2713 Received line match: \"%s\"\033[0m\n", line);
    #endif
    return 0;
}



int main(int argc, char *argv[]) {
    if(argc != 4){
        fprintf(stderr, "Error: Usage: test_exe <program> <test_case_dir> <expected_oputput_dir>.\n");
        exit(EXIT_FAILURE);
    }
    else{
        #ifdef DEBUG
            fprintf(stdout, "Testing executable: %s\n", argv[1]);
            fprintf(stdout, "Directory with testcases: %s\n", argv[2]);
            fprintf(stdout, "Directory with expected outputs: %s\n", argv[3]);
        #endif
    }

    FILE* file_ptr = fopen("./tests/test.txt", "r");
    if(file_ptr == NULL){
        perror("Invalid file");
        exit(EXIT_FAILURE);
    }

    int master_fd = posix_openpt(O_RDWR | O_NOCTTY);
    if (master_fd == -1) {
        perror("posix_openpt failed");
        exit(EXIT_FAILURE);
    }

    if (grantpt(master_fd) == -1) {
        perror("grantpt failed");
        close(master_fd);
        exit(EXIT_FAILURE);
    }

    if (unlockpt(master_fd) == -1) {
        perror("unlockpt failed");
        close(master_fd);
        exit(EXIT_FAILURE);
    }

    char *slave_name = ptsname(master_fd);
    if (!slave_name) {
        perror("ptsname failed");
        close(master_fd);
        exit(EXIT_FAILURE);
    }

    struct termios ts;
    if (tcgetattr(master_fd, &ts) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    ts.c_oflag &= ~ONLCR;  // Disable newline conversion
    if (tcsetattr(master_fd, TCSANOW, &ts) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("fork failed");
        close(master_fd);
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        close(master_fd);

        if (setsid() == -1) {
            perror("setsid failed");
            exit(EXIT_FAILURE);
        }

        int slave_fd = open(slave_name, O_RDWR);
        if (slave_fd == -1) {
            perror("open slave pty failed");
            exit(EXIT_FAILURE);
        }

        dup2(slave_fd, STDOUT_FILENO);
        dup2(slave_fd, STDERR_FILENO);
        close(slave_fd);

        execvp(argv[1], &argv[1]);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } 
    else {
        char buffer[BUFFER_SIZE];
        char line_buffer[BUFFER_SIZE];
        size_t line_pos = 0;
        int line_number = 0;

        while (1) {
            ssize_t bytes_read = read(master_fd, buffer, sizeof(buffer));
            if (bytes_read == -1) {
                if (errno == EINTR) continue;
                if (errno == EIO){
                    // Check if all expected lines were processed
                    char* buf = NULL;
                    size_t buf_size = 0;
                    getline(&buf, &buf_size, file_ptr);
                    if(buf != NULL && strlen(buf) != 0) {
                        fprintf(stderr, "\033[31m\033[1mError:\033[0m Missing expected output starting at line %d.\n", line_number + 1);
                        kill(child_pid, SIGKILL);
                        waitpid(child_pid, NULL, 0);
                        close(master_fd);
                        exit(EXIT_FAILURE);
                    }
                    break;
                }
                perror("read failed");
                break;
            } else if (bytes_read == 0) { // EOF
                break;
            }

            for (ssize_t i = 0; i < bytes_read; ++i) {
                if (buffer[i] == '\n') {
                    line_buffer[line_pos] = '\0';
                    if (process_line(line_buffer, file_ptr) != 0) {
                        kill(child_pid, SIGKILL);
                        waitpid(child_pid, NULL, 0);
                        close(master_fd);
                        exit(EXIT_FAILURE);
                    }
                    line_number++;
                    line_pos = 0;
                } 
                else {
                    if (line_pos < sizeof(line_buffer) - 1)
                        line_buffer[line_pos++] = buffer[i];
                    else {
                        line_buffer[line_pos] = '\0';
                        fprintf(stderr, "Line too long: \"%s\"\n", line_buffer);
                        line_pos = 0;
                    }
                }
            }
        }

        // Wait for child to exit normally
        int status;
        waitpid(child_pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("\033[32mAll outputs matched.\033[0m\n");
            printf("Program exited with status %d\n", WEXITSTATUS(status));
        } 
        else 
            printf("\033[31mProgram terminated abnormally\033[0m\n");

        close(master_fd);
    }

    return 0;
}