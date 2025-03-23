# CSF363-Project
This project is a custom-built compiler designed to process a toy programming language as a part of the course **CS F363 - Compiler Construction**. The compiler consists of two phases, including lexical analysis and syntax analysis. It takes source code as input, processes it through these stages, and outputs either a Parse tree or meaningful error messages. The project is implemented in **C**. 
  
## Table of Contents:
- [CSF363-Project](#csf363-project)
  - [Table of Contents:](#table-of-contents)
  - [Key Features](#key-features)
  - [Installation:](#installation)
  - [Usage:](#usage)
  - [Contributors:](#contributors)
  - [References:](#references)

## Key Features  
- **Lexical Analysis:** Converts source code into tokens using a lexer.     
- **Syntax Analysis:** Constructs a syntax tree from the tokenized input.  
- **Error Handling & Recovery:** Implements strategies to detect and recover from lexical and syntax errors. 

## Installation:
1. Clone the repository:  
```sh
   git clone https://github.com/AbhiramHande/CSF363-Project.git
```
2. Ensure you have **GCC**, **Make**, and a **Linux** environment installed.
3. Run `make` to build the project.
4. Run `./bin/prog.exe` to execute the project.
   

## Usage:
- `make` → Compiles the project, generating `prog.exe` in `./bin/`.  
- `make test` → Compiles and runs `tester.exe` with `test_target.exe`.  
- `make clean` → Removes all object files and executables.  
- `make debug` → Compiles `prog.exe` and runs it with `gdb` for debugging.  
- `make help` → Displays all available options.  


## Contributors:
The contributing members are
1. (2021B4A71134P) Abhiram H 
2. (2021B5A71159P) Ankur Renduchintala 
3. (2021B3A71111P) Avyakth Kumar 
4. (2021B1A72281P) Suchit Chebolu 
5. (2022A7PS0013P) Vikram Hariharan 

## References:
- Built using concepts from *Compilers: Principles, Techniques, and Tools* by Aho, Lam, Sethi, and Ullman.