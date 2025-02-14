# CSF363-Project
This is a compiler built for a toy language as a part of the course **CS F363 - Compiler Construction**. The contributing members are
1. Abhiram H (2021B4A71134P)
2. Ankur Renduchintala (2021B5A71159P)
3. Avyakth Kumar (2021B3A71111P)
4. Suchit Chebolu (2021B1A72281P)
5. Vikram Hariharan (2022A7PS0013P)

## Table of Contents:
- [CSF363-Project](#csf363-project)
  - [Table of Contents:](#table-of-contents)
  - [File structure:](#file-structure)
  - [Installation:](#installation)
  - [Usage:](#usage)
  - [Contribution:](#contribution)
    - [How to Contribute](#how-to-contribute)
    - [Reporting Issues](#reporting-issues)

## File structure:
The project directory structure is as follows:
```
CSF363-Project/
├── src/
│   ├── lexer.c
│   ├── main.c
│   ├── parser.c
│   └── tester.c
├── include/
│   ├── debug.h
│   ├── lexer.h
│   ├── parser.h
├── obj/
│   ├── lexer.o
│   ├── main.o
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
│   └── prog.exe
│   └── test.exe
├── .gitignore
├── Language specifications.pdf
├── makefile
└── README.md
```

## Installation:
1. Clone the repository: `git clone https://github.com/AbhiramHande/CSF363-Project.git`
2. Install necessary dependencies like GCC, make and Linux.
3. Run `make` to build the project.
4. Run `./build/exe` to execute the project.
   
## Usage:
1. `make` to create the executable `exe`
2. `make test` to create and run the exectable `test`
3. `make clean` to remove all object files and exectuables
4. `make debug` to create the executable `exe` with debugging options
5. `make ltest` to create and run the executable `test` with debugging options

## Contribution:
I have listed the following instructions for Linux based machines (find alternatives if it is a Mac or Windows system)

### How to Contribute
1. **Generate a SSH Keys**(if you don’t already have one): You can generate an SSH key pair (public and private keys) on your local machine by running the following command in your terminal:
```bash
ssh-keygen -t ed25519 -C "your_email@gmail.com"
```
2. **Add the SSH Key**: After generating the key, you need to add the public SSH key (id_rsa.pub) to your GitHub account. You can do so by copying the contents of your public key by running:
```bash
cat ~/.ssh/id_ed25519.pub
```
Then, go to **GitHub** > **Settings** > **SSH and GPG keys** > **New SSH key** and paste the key in there.
3. **Clone**: Once the SSH key is added to GitHub, you can clone the repository to your local machine
```bash
git clone git@github.com:AbhiramHande/CSF363-Project.git
```
4. **New Branch**: For each feature or fix, create a new branch
```bash
git checkout -b feature/your-feature-name
```
5. **Make your changes**: Implement the changes, and test them to make sure everything works as expected.
6. **Commit**: After making changes, commit them with a descriptive message
```bash
git commit -m "Added new feature or fixed bug"
```
7. **Push**: Push your changes to the remote repository:
```bash
git push origin feature/your-feature-name
```
1. **Pull Request**: Submit a pull request (PR) to the main branch from your feature branch. Describe the changes in detail.

### Reporting Issues
If you encounter any issues or have suggestions for improvements, please open an issue in the repository. When reporting bugs, provide as much detail as possible to help quickly resolve the problem.