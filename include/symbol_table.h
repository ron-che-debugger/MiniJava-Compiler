#ifndef __SYMBOL_TABLE_H
#define __SYMBOL_TABLE_H

#define false 0
#define true 1
#define FALSE 0   // Alternative boolean representation for false
#define TRUE 1    // Alternative boolean representation for true
#define bool char // Custom boolean type defined as a char

#define STACK_SIZE 100 // Maximum size of the symbol table stack
#define ST_SIZE 500    // Maximum number of entries in the symbol table
#define ATTR_SIZE 2000 // Maximum size for the attributes table

/*
 * Error type definitions for the error reporting routine.
 * These are used to identify and handle different semantic errors during compilation.
 */
#define STACK_OVERFLOW 100  // Stack overflow error in the symbol table stack
#define REDECLARATION 101   // Error for redeclaration of a variable or function
#define ST_OVERFLOW 102     // Symbol table overflow error
#define UNDECLARATION 103   // Use of an undeclared identifier
#define ATTR_OVERFLOW 104   // Overflow in the attribute list
#define NOT_USED 105        // Warning for declared but unused variables or functions
#define ARGUMENTS_NUM1 106  // Incorrect number of arguments in function call (type 1)
#define ARGUMENTS_NUM2 107  // Incorrect number of arguments in function call (type 2)
#define BOUND 108           // Array index out-of-bounds error
#define PROCE_MISMATCH 109  // Procedure call mismatch error
#define VAR_VAL 110         // Error in variable value assignment
#define CONSTANT_VAR 111    // Attempt to modify a constant variable
#define EXPR_VAR 112        // Invalid variable usage in an expression
#define CONSTANT_ASSIGN 113 // Illegal assignment to a constant
#define INDX_MIS 114        // Array index type mismatch
#define FIELD_MIS 115       // Incorrect field access in a structure
#define FORW_REDECLARE 116  // Forward declaration redeclaration error
#define REC_TYPE_MIS 117    // Record type mismatch error
#define ARR_TYPE_MIS 118    // Array type mismatch error
#define VARIABLE_MIS 119    // General variable misuse error
#define FUNC_MISMATCH 120   // Function type mismatch error
#define TYPE_MIS 121        // General type mismatch error
#define NOT_TYPE 122        // Invalid type usage error
#define ARR_DIME_MIS 123    // Array dimension mismatch error
#define MULTI_MAIN 124      // Multiple 'main' function declarations error

/*
 * Processing instructions for the error reporting routine.
 * These directives control how the compiler handles errors during semantic analysis.
 */
#define CONTINUE 0 /* Print the error message and continue execution (non-fatal error). */
#define ABORT 1    /* Print a fatal error message and terminate program execution. */

/*
 * Stack element field definitions.
 * These constants correspond to the fields in a stack element used for scope management.
 */
#define MARKER 1 /* Marks the beginning of a new block scope in the stack. */
#define NAME 2   /* Points to the identifier's lexeme in the string table. */
#define ST_PTR 3 /* Points to the identifier's symbol table entry. */
#define DUMMY 4  /* Placeholder for undeclared identifiers or incomplete entries. */
#define USED 5   /* Boolean flag indicating whether the identifier has been used. */

/*
 * Possible attributes for symbol table entries.
 * These constants define various attributes that can be associated with identifiers.
 * Common attributes have smaller constant values to optimize sorting in linked lists.
 */
#define NAME_ATTR 1   /* Pointer to the identifier's lexeme, set by InsertEntry. */
#define NEST_ATTR 2   /* Nesting level of the identifier, set by InsertEntry. */
#define TREE_ATTR 3   /* Pointer back to the syntax subtree (AST) where the identifier appears. */
#define PREDE_ATTR 4  /* Boolean flag indicating if the identifier is predefined (e.g., built-in functions). */
#define KIND_ATTR 5   /* Classifies the kind of identifier (e.g., variable, function, constant). */
#define TYPE_ATTR 6   /* Pointer to the type tree for the identifier (variable, constant, or function). */
#define VALUE_ATTR 7  /* Value of a constant identifier (integer, character, or string pointer). */
#define OFFSET_ATTR 8 /* Memory offset for the identifier (useful for code generation and stack frames). */

#define DIMEN_ATTR 9   /* Dimension information for arrays (number of dimensions). */
#define ARGNUM_ATTR 10 /* Number of arguments for function or procedure declarations. */

/*
 * Possible values of the attribute KIND_ATTR.
 * These constants classify the kind of identifier stored in the symbol table.
 * They help in distinguishing between different types of declarations and uses.
 */
#define CONST 1       /* Constant: A fixed value that cannot be changed (e.g., const int x = 5). */
#define VAR 2         /* Variable: A mutable storage location for data (e.g., int x). */
#define FUNCFORWARD 3 /* Forward-declared function: A function declared but not yet defined. */
#define FUNC 4        /* Function: A fully defined function with an implementation. */
#define REF_ARG 5     /* Reference argument: A function argument passed by reference. */
#define VALUE_ARG 6   /* Value argument: A function argument passed by value. */
#define FIELD 7       /* Field: A member of a structure or class (e.g., obj.field). */
#define TYPEDEF 8     /* Type definition: A user-defined data type using typedef. */
#define PROCFORWARD 9 /* Forward-declared procedure: A procedure declared but not yet defined. */
#define PROCE 10      /* Procedure: A subroutine that performs actions but does not return a value. */
#define CLASS 11      /* Class: A user-defined type encapsulating data and methods (object-oriented). */
#define ARR 12        /* Array: A collection of elements of the same type, accessed by index. */

/********************************* Data Structures **********************/
/*
 * All the data structures and variables defined below are private to the symbol table operating routines.
 */

/*
 * Structure for the symbol table stack.
 * This stack is used for managing scope and symbol entries in the compiler's semantic analysis phase.
 */
typedef struct {
    bool marker; /* Marks the beginning of a new block (scope) in the symbol table stack. */
    int name;    /* Pointer to the lexeme of the identifier (index in the string table). */
    int st_ptr;  /* Pointer to the identifier's entry in the symbol table. */
    bool dummy;  /* Dummy flag to indicate an undeclared identifier. */
    bool used;   /* Boolean flag indicating if the identifier has been used. */
} StackEntry;    /* Stack array with a maximum size defined by STACK_SIZE. */

/*
 * Structure for the attribute list of symbol table entries.
 * Each symbol can have multiple attributes (e.g., type, value, nesting level).
 */
typedef struct {
    char attr_num; /* Attribute number (< 256), uniquely identifies the attribute type. */
    int attr_val;  /* Attribute value */
    int next_attr; /* Pointer (index) to the next attribute in the linked list, or -1 if none. */
} attr_type;       /* Defines the attribute structure used for symbol table entries. */

/*
 * Procedure Declarations
 * These functions provide the core operations for managing the symbol table and performing semantic analysis.
 */

/* Initializes the symbol table and related data structures. */
void STInit();

/* Reports an error with detailed information (error type, line number, column number, severity). */
void error_msg(int error_type, int line, int column, int severity);

/* Inserts a new identifier into the symbol table and returns its index. */
int InsertEntry(int id);

/* Looks up an identifier in the symbol table across all scopes. Returns its index or 0 if not found. */
int LookUp(int id);

/* Looks up an identifier only in the current scope. Returns its index or 0 if not found. */
int LookUpHere(int id);

/* Opens a new block/scope by pushing a marker onto the stack. */
void OpenBlock();

/* Closes the current block/scope by popping entries from the stack until a marker is found. */
void CloseBlock();

/* Checks if a symbol table entry has a specific attribute. Returns true (1) if present, false (0) otherwise. */
int IsAttr(int st_index, int attr_num);

/* Retrieves the value of a specific attribute from a symbol table entry. */
int GetAttr(int st_ptr, int attr_num);

/* Sets the value of a specific attribute for a symbol table entry. */
void SetAttr(int st_ptr, int attr_num, int attr_val);

/* Prints the contents of the symbol table for debugging and verification. */
void STPrint();

/* Pushes a new entry onto the stack with the given marker, name, symbol table pointer, and usage flag. */
void Push(int marker, int name, int st_ptr, int used);

/* Returns the string representation of a sequence number (used for retrieving lexemes). */
char *seq_str(int seq_num);

#endif