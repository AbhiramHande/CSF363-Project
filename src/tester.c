#include "../include/tester.h"
#include <stdio.h>

int main(int argc, char** argv){
    if(argc != 4){
        fprintf(stderr, "Error: Usage: test_exe <program> <test_case_dir> <expected_oputput_dir>.\n");
        return -1;
    }
    else{
        fprintf(stdout, "Recieved 4 inputs as expected.\n");
        return 0;
    }
}