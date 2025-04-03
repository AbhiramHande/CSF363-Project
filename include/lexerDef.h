#ifndef LEXERDEF_H
#define LEXERDEF_H

#define BUFFER_SIZE     2048        /**< Defines the capacity of each buffer in the twin buffer.*/
#define TERMINAL_COUNT  60          /**< Defines the number of tokens.*/
#define KEYWORD_END     TK_ELSE     /**< @depracated Defines the start of keyword.*/
#define KEYWORD_START   TK_WITH     /**< @depracated Defines the end of keyword.*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * @brief Enumerates different types of tokens recognized by the lexer.
 * 
 * @details 
 * - Used to classify lexemes into meaningful categories such as keywords, operators, identifiers, 
 *   and literals during lexical analysis.
 * - Helps in distinguishing between different token types efficiently in the parsing phase.
 * - Each token type corresponds to a unique identifier in the `TokenType` enumeration.
 */
typedef enum TokenType token_type;

/**
 * @brief A structure representing a twin buffer for lexical analysis.
 * 
 * @details
 * - The twin buffer is used to efficiently handle input buffering for the lexer.
 * - It consists of two alternating buffers, allowing seamless processing of lexemes.
 * - Helps in implementing lookahead and efficient file reading.
 */
typedef struct TwinBuffer twin_buffer;

/**
 * @brief A structure representing metadata associated with a token.
 * 
 * @details
 * - Stores information about a token, including its type, lexeme, and position in the source code.
 * - Includes additional fields to store integer or floating-point values when applicable.
 */
typedef struct Token token;


enum TokenType {
    TK_ASSIGNOP,    /**< Represents the assignment token `<--` used to assign values to variables */
    TK_COMMENT,     /**< Represents comment tokens that follow a percent symbol (`%`) */
    TK_FIELDID,     /**< Represents field identifiers, matching the pattern `[a-z]+`. */
    TK_ID,          /**< Represents identifiers, which match the regular expression `[b-d][2-7][b-d]*[2-7]*` */
    TK_NUM,         /**< Represents numeric tokens that match the regex pattern `[0-9]+` for integers.*/
    TK_RNUM,        /**< Represents numeric tokens that match the regex pattern `[0-9]+\.[0-9][0-9]E[+|-|eps][0-9][0-9]` for real numbers.*/
    TK_FUNID,       /**< Represents function identifiers, matching the regex `_[a-zA-Z][a-zA-Z0-9]*`.*/
    TK_RUID,        /**< Represents record or union type identifiers, matching the regex `#[a-z]+`.*/
    TK_LT,          /**< Represents the less-than operator (`<`) */
    TK_LE,          /**< Represents the less-than or equal-to operator (`<=`) */
    TK_GT,          /**< Represents the greater-than operator (`>`) */
    TK_GE,          /**< Represents the greater-than or equal-to operator (`>=`) */
    EPSILON,        /**< Represents an epsilon (@f$ \epsilon @f$) in grammar rules -- **NOT A TOKEN** */
    DOLLAR,         /**< @deprecated Represents the stack bottom marker (`$`) -- **NOT A TOKEN* **/
    TK_SQL,         /**< Represents an opening square bracket (`[`) */
    TK_SQR,         /**< Represents a closing square bracket (`]`) */
    TK_COMMA,       /**< Represents a comma (`,`) */
    TK_SEM,         /**< Represents a semicolon (`;`) */
    TK_COLON,       /**< Represents a colon (`:`) */
    TK_DOT,         /**< Represents a dot (`.`) */
    TK_OP,          /**< Represents an opening round bracket (`(`) */
    TK_CL,          /**< Represents a closing round bracket (`)`) */
    TK_PLUS,        /**< Represents the addition operator (`+`) */
    TK_MINUS,       /**< Represents the subtraction operator (`-`) */
    TK_MUL,         /**< Represents the multiplication operator (`*`) */
    TK_DIV,         /**< Represents the division operator (`/`) */
    TK_AND,         /**< Represents the logical AND operator (`&&&`) */
    TK_OR,          /**< Represents the logical OR operator (`@@@`) */
    TK_NOT,         /**< Represents the logical NOT operator (`~`) */
    TK_NE,          /**< Represents the not-equal operator (`!=`) */
    TK_EQ,          /**< Represents the equality operator (`==`) */
    TK_WITH,        /**< Represents the keyword `with`, typically used for specifying the parameters in a function call. */
    TK_PARAMETERS,  /**< Represents the keyword `parameters`, used to define function or procedure parameters. */
    TK_END,         /**< Represents the keyword `end`, marking the termination of a block or scope. */
    TK_WHILE,       /**< Represents the keyword `while`, used for loop constructs. */
    TK_UNION,       /**< Represents the keyword `union`, used for defining union types. */
    TK_ENDUNION,    /**< Represents the keyword `endunion`, marking the termination of a union definition. */
    TK_DEFINETYPE,  /**< Represents the keyword `definetype`, used for defining new data types. */
    TK_AS,          /**< Represents the keyword `as`, used in type definitions or aliases. */
    TK_TYPE,        /**< Represents the keyword `type`, used for specifying data types. */
    TK_MAIN,        /**< Represents the keyword `_main`, denoting the program’s main function. */
    TK_GLOBAL,      /**< Represents the keyword `global`, used for defining globally accessible variables or functions. */
    TK_PARAMETER,   /**< Represents the keyword `parameter`, used in function definitions. */
    TK_LIST,        /**< Represents the keyword `list`, used to define a collection or list. */
    TK_INPUT,       /**< Represents the keyword `input`, used to specify the input parameters and its datatypes. */
    TK_OUTPUT,      /**< Represents the keyword `output`, used to specify the output parameters and its datatypes. */
    TK_INT,         /**< Represents the keyword `int`, used for declaring integer data types. */
    TK_REAL,        /**< Represents the keyword `real`, used for declaring real (floating-point) data types. */
    TK_ENDWHILE,    /**< Represents the keyword `endwhile`, marking the end of a `while` loop. */
    TK_IF,          /**< Represents the keyword `if`, used for conditional statements. */
    TK_THEN,        /**< Represents the keyword `then`, used in `if` statements to indicate the consequence block. */
    TK_ENDIF,       /**< Represents the keyword `endif`, marking the end of an `if` block. */
    TK_READ,        /**< Represents the keyword `read`, used for reading input values from the terminal. */
    TK_WRITE,       /**< Represents the keyword `write`, used for printing output values on the terminal. */
    TK_RETURN,      /**< Represents the keyword `return`, used to return values from functions. */
    TK_CALL,        /**< Represents the keyword `call`, used for invoking functions or procedures. */
    TK_RECORD,      /**< Represents the keyword `record`, used for defining records. */
    TK_ENDRECORD,   /**< Represents the keyword `endrecord`, marking the termination of a record definition. */
    TK_ELSE,        /**< Represents the keyword `else`, used in conditional statements to specify alternative execution paths. */
    TK_ERROR,       /**< Represents an unrecognized token, indicating a lexical error. */
    TK_EOF          /**< Represents the end-of-file (EOF) marker, signaling the end of the source code. */
};

struct TwinBuffer {
    char* active_buffer;    /**< Pointer to the chunk of code currently being lexed. */
    char* load_buffer;      /**< Pointer to the chunk of code that is either about to be lexed or has already been lexed. */
    int forward_ptr;        /**< Index of the current character in the `active_buffer`. */
    int begin_ptr;          /**< Index of the start of the lexeme.
                                 @note 
                                 - If negative, the lexeme begins in the `load_buffer` (from the end). 
                                 - If positive, the lexeme begins in the `active_buffer`. */
    bool buffer_loaded;     /**< Indicates whether the `load_buffer` should be lexed or discarded. */
};

struct Token {
    token_type name;        /**< Represents the type of token. */
    char* lexeme;           /**< Pointer to the lexeme (string representation of the token). */
    int line_num;           /**< Line number where the lexeme was encountered. */    
    bool is_value_int;      /**< Indicates whether the token represents an integer (`true`) or a floating-point number (`false`). */

    union {
        long long num;      /**< Stores the integer value (if applicable). */
        long double r_num;  /**< Stores the floating-point value (if applicable). */
    } value;                /**< Union for storing the token’s numeric value (if any). */
};

#endif