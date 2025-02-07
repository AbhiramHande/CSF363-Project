# CSF363-Project
This is a compiler built for a toy language as a part of the course CS F363 - Compiler Construction. The contributing members are
1. Abhiram H (2021B4A71134P)
2. Ankur Renduchintala (2021B5A71159P)
3. Avyakth Kumar (2021B3A71111P)
4. Suchit Chebolu (2021B1A72281P)
5. Vikram Hariharan (2022A7PS0013P)

## Table of Contents
- [File Structure](#file-structure)
- [Installation](#installation)
- [Usage](#usage)

## File structure:
The project directory structure is as follows:
```
CSF363-Project/
├── src/
│   ├── main.c
│   ├── lexer.c
│   ├── lexer.h
│   ├── parser.c
│   ├── parser.h
│   ├── debug.h
│   ├── tester.h
│   └── tester.c
├── obj/
│   ├── main.o
│   ├── lexer.o
│   ├── parser.o
│   ├── tester.o
├── tests/
│   ├── test_cases/
│   |   ├── test1.txt
│   |   ├── test2.txt
│   |   └── ...
│   └── expected_output/
│       ├── test1.txt
│       ├── test2.txt
│       └── ...
├── build/
│   └── exe
│   └── test
├── .gitignore
├── makefile
└── README.md
```

## Installation:
1. Clone the repository: `git clone https://github.com/username/CSF363-Project.git`
2. Install necessary dependencies like GCC, make and Linux.
3. Run `make` to build the project.
4. Run `./build/exe` to execute the project.
   
## Usage:
1. `make` to create the executable `exe`
2. `make test` to create and run the exectable `test`
3. `make clean` to remove all object files and exectuables
4. `make debug` to create the executable `exe` with debugging options
5. `make ltest` to create and run the executable `test` with debugging options
