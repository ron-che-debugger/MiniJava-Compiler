#include "symbol_table.h"
#include "tree.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/********************************* Data Structures **********************/
/*
 * There are three arrays for symbol table operation.  "st" is the real
 * symbol table, it carries all the information of an id and will still be
 * used in code generation phase.  The attributes for an id are kept in a
 * link list pointed by its symbol table entry. It is easy to be extended in
 * the next phase to include more attributes, such like the size and the
 * location of a varialbe. "stack" is a temporary structure in which all the
 * id's in the current scoping context are visible.  "attrarray" is to store
 * all the attributes in symbol table.
 */

/* Definition of the stack array with a size of STACK_SIZE. */
StackEntry stack[STACK_SIZE];

/*
 * Symbol Table Array
 * ------------------
 * This array holds the main symbol table entries. Each entry in this array
 * points to the starting index of the linked list of attributes in `attrarray`
 * for a specific identifier.
 *
 * - Each index in `st` corresponds to a unique identifier (variable, function, etc.).
 * - The linked list of attributes holds detailed information about the identifier.
 */
int st[ST_SIZE]; /* Symbol table array with size defined by ST_SIZE. */

/*
 * Attribute Array
 * ---------------
 * This array stores all attributes associated with the identifiers in the symbol table.
 *
 * - Each symbol table entry in `st` points to its attributes in this array.
 * - Attributes are linked together via `next_attr`, forming a linked list.
 * - Attributes may include information such as type, value, nesting level, etc.
 *
 * This design avoids dynamic memory allocation, simplifying testing and debugging.
 */
attr_type attrarray[ATTR_SIZE]; /* Array of attributes for identifiers. */

/*
 * Global Counters
 * ---------------
 * These counters keep track of the current positions within the symbol table,
 * stack, nesting levels, and attribute array.
 */
int stack_top = 0; /* Tracks the top index of the stack used for managing scopes. */
int st_top = 0;    /* Tracks the top index of the symbol table (`st`). */
int nesting = 0;   /* Tracks the current nesting level (scope depth). */
int attr_top = 0;  /* Tracks the top index of the attribute array (`attrarray`). */

/*
 * External Variables
 * ------------------
 * These external variables are declared elsewhere but used here.
 */
extern int yyline;                /* Current line number in the source code (used for error reporting). */
extern char strg_tbl[];           /* String table storing identifier names, defined in table.c. */
extern int loc_str(char *string); // Declare the function

/************************ routines *****************************/

/*
 * STInit(): Initialize the Symbol Table.
 * -------------------------------------
 * This function initializes the symbol table by inserting predefined system identifiers
 * such as "system", "readln", and "println". These are considered built-in functions/classes
 * and must be available for semantic analysis during compilation.
 *
 * - "system": A predefined class used for system-level operations.
 * - "readln": A predefined procedure for reading input.
 * - "println": A predefined procedure for printing output.
 */
void STInit() {
    int nStrInd, nSymInd; /* nStrInd: index in the string table, nSymInd: index in the symbol table */

    /* Insert the predefined class "system" */
    nStrInd = loc_str("system");        /* Find or insert "system" in the string table */
    if (nStrInd != -1) {                /* Check if "system" was successfully added/found */
        nSymInd = InsertEntry(nStrInd); /* Insert "system" into the symbol table */

        /* Set predefined attributes for "system" */
        /* SetAttr(nSymInd, TREE_ATTR, NULL); */ // Optional: associate with a syntax tree (commented out)
        SetAttr(nSymInd, PREDE_ATTR, true);      /* Mark as a predefined identifier */
        SetAttr(nSymInd, KIND_ATTR, CLASS);      /* Specify that it's a class */
    }

    /* Insert the predefined procedure "readln" */
    nStrInd = loc_str("readln"); /* Find or insert "readln" in the string table */
    if (nStrInd != -1) {
        nSymInd = InsertEntry(nStrInd); /* Insert "readln" into the symbol table */

        /* Set attributes for "readln" */
        SetAttr(nSymInd, NEST_ATTR, nesting + 1); /* Set its nesting level (1 deeper than the current) */
        /* SetAttr(nSymInd, TREE_ATTR, NULL); */  // Optional: associate with a syntax tree (commented out)
        SetAttr(nSymInd, PREDE_ATTR, true);       /* Mark as a predefined identifier */
        SetAttr(nSymInd, KIND_ATTR, PROCE);       /* Specify that it's a procedure */
    }

    /* Insert the predefined procedure "println" */
    nStrInd = loc_str("println"); /* Find or insert "println" in the string table */
    if (nStrInd != -1) {
        nSymInd = InsertEntry(nStrInd); /* Insert "println" into the symbol table */

        /* Set attributes for "println" */
        SetAttr(nSymInd, NEST_ATTR, nesting + 1); /* Set its nesting level (1 deeper than the current) */
        /* SetAttr(nSymInd, TREE_ATTR, NULL); */  // Optional: associate with a syntax tree (commented out)
        SetAttr(nSymInd, PREDE_ATTR, true);       /* Mark as a predefined identifier */
        SetAttr(nSymInd, KIND_ATTR, PROCE);       /* Specify that it's a procedure */
    }
}

/*
 * error_msg(): Reports semantic errors during compilation.
 * --------------------------------------------------------
 * This function handles different types of semantic errors detected during compilation.
 * It prints descriptive error messages, including the source line number and context-specific
 * details, to help developers debug their code.
 *
 * Parameters:
 *   - type   : The type of semantic error (based on predefined error codes).
 *   - action : Determines whether to continue or abort after reporting the error (CONTINUE/ABORT).
 *   - id     : The identifier (symbol) related to the error, used to fetch its name.
 *   - seq    : Additional context (e.g., sequence of parameters) for specific errors.
 *
 * Behavior:
 *   - Prints the corresponding error message for the given `type`.
 *   - If `action` is `ABORT`, the program terminates after reporting the error.
 */
void error_msg(int type, int action, int id, int seq) {
    /* Print the line number where the semantic error occurred */
    printf("Semantic Error--line: %d, ", yyline);

    /* Handle specific error types */
    switch (type) {
    case STACK_OVERFLOW:
        printf("stack overflow.\n");
        break;

    case REDECLARATION:
        printf("symbol %s: redeclared.\n", getname(id)); // Identifier redeclared in the same scope
        break;

    case ST_OVERFLOW:
        printf("symbol table overflow.\n"); // Symbol table has exceeded its capacity
        break;

    case UNDECLARATION:
        printf("symbol %s: undeclared.\n", getname(id)); // Identifier used without declaration
        break;

    case ATTR_OVERFLOW:
        printf("attribute array overflowed.\n"); // Attribute array exceeded its capacity
        break;

    case BOUND:
        printf("symbol %s: not declared as a constant, can't be used as subrange bound.\n", getname(id));
        break;

    case ARGUMENTS_NUM1:
        printf("routine %s: argument number in definition is different from the previous forward declaration.\n", getname(id));
        break;

    case ARGUMENTS_NUM2:
        printf("routine %s: argument number is different from the previous declaration.\n", getname(id));
        break;

    case FORW_REDECLARE:
        printf("routine %s: forward redeclaration.\n", getname(id)); // Function/procedure redeclared after forward declaration
        break;

    case PROCE_MISMATCH:
        printf("symbol %s: can't act as a procedure call.\n", getname(id)); // Using a non-procedure as a procedure
        break;

    case FUNC_MISMATCH:
        printf("symbol %s: can't act as a function call.\n", getname(id)); // Using a non-function as a function
        break;

    case VAR_VAL:
        printf("routine %s: reference/value type of the ", getname(id));
        printf("%s parameter is different from the previous forward declaration.\n", seq_str(seq));
        break;

    case CONSTANT_VAR:
        printf("routine %s: the ", getname(id));
        printf("%s parameter is a reference argument, can't be a constant.\n", seq_str(seq));
        break;

    case EXPR_VAR:
        printf("routine %s: reference argument of the ", getname(id));
        printf("%s parameter can't be an expression.\n", seq_str(seq)); // Expression passed instead of a variable
        break;

    case CONSTANT_ASSIGN:
        printf("symbol %s: declared as a constant, can't be assigned a new value.\n", getname(id));
        break;

    case ARR_TYPE_MIS:
        if (seq == 0)
            printf("symbol %s: isn't defined as an array.\n", getname(id)); // Using a non-array as an array
        else {
            printf("symbol %s: the ", getname(id));
            printf("%s index isn't defined as an array.\n", seq_str(seq)); // Incorrect array indexing
        }
        break;

    case ARR_DIME_MIS:
        printf("symbol %s: inappropriate usage of array element.\n", getname(id)); // Wrong array access
        break;

    case REC_TYPE_MIS:
        printf("symbol %s: illegal usage of a field name.\n", getname(id)); // Misuse of a record/struct field
        break;

    case INDX_MIS:
        printf("symbol %s: has incorrect number of dimensions.\n", getname(id)); // Array indexing dimension mismatch
        break;

    case FIELD_MIS:
        printf("symbol %s: is an undeclared field name.\n", getname(id)); // Accessing an undeclared struct field
        break;

    case VARIABLE_MIS:
        printf("symbol %s: can't be used as a variable.\n", getname(id)); // Invalid use of a non-variable
        break;

    case NOT_TYPE:
        printf("symbol %s: is not declared as a type.\n", getname(id)); // Identifier is not a type
        break;

    case TYPE_MIS:
        printf("symbol %s: incorrect type usage.\n", getname(id)); // Incorrect type usage
        break;

    case MULTI_MAIN:
        printf("main() method already declared.\n"); // Multiple declarations of the main function
        break;

    default:
        printf("error type: %d.\n", type); // Catch-all for unknown error types
    }

    /* If the error is fatal, terminate the program */
    if (action == ABORT)
        exit(0);
}

/*
 * InsertEntry(): Creates a new symbol table entry for a given identifier.
 * ----------------------------------------------------------------------
 * This function inserts a new identifier into the symbol table while checking for redeclarations.
 * It sets the identifier's attributes, such as its name and nesting level, and pushes it onto the stack
 * for proper scope management.
 *
 * Parameters:
 *   - id: The index of the identifier's name in the string table (used to fetch the identifier's name).
 *
 * Returns:
 *   - The symbol table index where the new entry was added.
 *   - Returns 0 if a redeclaration error occurred.
 */
int InsertEntry(int id) {

    /* Step 1: Check for redeclaration within the current block */
    if (LookUpHere(id)) {
        // If the identifier already exists in the current scope, report a redeclaration error.
        error_msg(REDECLARATION, CONTINUE, id, 0);
        return 0; // Return 0 to indicate failure to insert due to redeclaration.
    }

    /* Step 2: Check for symbol table overflow */
    if (st_top >= ST_SIZE - 1) {
        // If the symbol table is full, report an overflow error and abort the program.
        error_msg(ST_OVERFLOW, ABORT, 0, 0);
    }

    /* Step 3: Insert the new entry into the symbol table */
    st_top++;       // Move to the next free slot in the symbol table.
    st[st_top] = 0; // Initialize the new symbol table entry.

    /* Step 4: Set basic attributes for the identifier */
    SetAttr(st_top, NAME_ATTR, id);      // Set the identifier's name attribute.
    SetAttr(st_top, NEST_ATTR, nesting); // Set the current nesting level (scope depth).

    /* Step 5: Push the new entry onto the stack for scope tracking */
    Push(false, id, st_top, false); // Push to the stack: not a marker, not a dummy, not yet used.

    /* Step 6: Return the symbol table index of the new entry */
    return st_top;
}

/*
 * LookUp(): Searches for an identifier across all visible scopes.
 * ---------------------------------------------------------------
 * This function searches the stack from the top down for a given identifier (`id`).
 * If found, it returns the symbol table entry pointer (`st_ptr`) associated with the identifier.
 * If not found, it reports an undeclared error and pushes a dummy entry onto the stack to
 * prevent repeated error reports for the same identifier.
 *
 * Parameters:
 *   - id: The identifier to search for (index in the string table).
 *
 * Returns:
 *   - The symbol table entry pointer if the identifier is found.
 *   - 0 if the identifier is undeclared.
 */
int LookUp(int id) {
    int i;

    /* Step 1: Search the stack for the identifier from top to bottom */
    for (i = stack_top; i > 0; i--) {
        if (!stack[i].marker && stack[i].name == id) {
            // If the identifier is found and it's not a block marker:
            stack[i].used = true;   // Mark the identifier as used.
            return stack[i].st_ptr; // Return the symbol table entry pointer.
        }
    }

    /* Step 2: If the identifier is not found, handle undeclared variable */
    error_msg(UNDECLARATION, CONTINUE, id, 0); // Report undeclared error.

    /* Step 3: Push a dummy entry to prevent repeated undeclared errors */
    Push(false, id, 0, true); // Push as a dummy entry with no symbol table reference.

    return 0; // Return 0 to indicate the identifier was not found.
}

/*
 * LookUpHere(): Searches for an identifier within the current block (scope).
 * --------------------------------------------------------------------------
 * This function searches the stack for a given identifier (`id`) but limits the search
 * to the current block. It stops searching once a scope marker (`marker`) is encountered.
 * This function is useful for checking redeclarations or forward declarations within a scope.
 *
 * Parameters:
 *   - id: The identifier to search for (index in the string table).
 *
 * Returns:
 *   - The symbol table entry pointer if the identifier is found in the current block.
 *   - 0 if the identifier is not found within the current block.
 */
int LookUpHere(int id) {
    int i;

    /* Step 1: Search only within the current block (scope) */
    for (i = stack_top; !stack[i].marker; i--) {
        if (stack[i].name == id && !stack[i].dummy) {
            // If the identifier is found and it's not a dummy entry:
            return stack[i].st_ptr; // Return the symbol table entry pointer.
        }
    }

    /* Step 2: Identifier was not found in the current block */
    return 0; // Return 0 to indicate the identifier was not found in the current scope.
}

/*
 * OpenBlock(): Marks the beginning of a new block/scope in the program.
 * ---------------------------------------------------------------------
 * This function is called when entering a new block, such as when the compiler encounters
 * the start of a program, procedure, function, or record definition.
 *
 * Behavior:
 *   - Increments the nesting level to represent entering a deeper scope.
 *   - Pushes a marker onto the stack to signal the start of a new block.
 *
 * Note:
 *   - The function or procedure name must be inserted into the symbol table
 *     *before* calling this routine.
 */
void OpenBlock() {
    nesting++;               /* Increase the nesting level to represent entering a new block. */
    Push(true, 0, 0, false); /* Push a marker onto the stack to mark the start of the new block. */
}

/*
 * CloseBlock(): Marks the end of the current block/scope in the program.
 * ----------------------------------------------------------------------
 * This function is called when exiting a block, such as the end of a program, procedure,
 * function, or record definition.
 *
 * Behavior:
 *   - Iterates through the stack to check if all declared variables in the current block
 *     were used. (Currently commented out but could be enabled for warnings).
 *   - Removes all entries from the stack related to the current block by popping
 *     until the last marker is found.
 *   - Decreases the nesting level to represent exiting the current scope.
 */
void CloseBlock() {
    int i;

    /* Step 1: Traverse the stack backwards to check for unused variables in the current block */
    for (i = stack_top; !stack[i].marker; i--) {
        if (!stack[i].used && !stack[i].dummy) {
            /* Uncomment to warn about unused variables in the block:
             * error_msg(NOT_USED, CONTINUE, stack[i].name, 0);
             */
        }
    }

    /* Step 2: Exit the current block by decrementing the nesting level */
    nesting--;

    /* Step 3: Remove all identifiers declared in the current block by trimming the stack */
    stack_top = i - 1; /* Move the stack top to just before the block marker */
}

/*
 * IsAttr(): Checks if a symbol table entry has a specific attribute.
 * -------------------------------------------------------------------
 * This function searches for a given attribute (`attr_num`) in the attribute list
 * associated with a symbol table entry (`st_ptr`).
 *
 * Parameters:
 *   - st_ptr: The index of the symbol in the symbol table (`st` array).
 *   - attr_num: The attribute number to search for (e.g., NAME_ATTR, TYPE_ATTR).
 *
 * Returns:
 *   - The index of the attribute in the `attrarray` if found.
 *   - 0 if the attribute is not found.
 */
int IsAttr(int st_ptr, int attr_num) {
    int i;

    /* Step 1: Get the starting index of the attribute list for the symbol */
    i = st[st_ptr]; // Points to the first attribute for this symbol.

    /* Step 2: Traverse the linked list of attributes */
    while (i) {
        if (attrarray[i].attr_num == attr_num) {
            // If the current attribute matches the desired attribute, return its index.
            return i;
        }

        if (attrarray[i].attr_num > attr_num) {
            // Attributes are sorted by `attr_num`. If the current attribute exceeds `attr_num`, stop.
            break;
        }

        i = attrarray[i].next_attr; // Move to the next attribute in the list.
    }

    /* Step 3: Attribute not found */
    return 0;
}

/*
 * GetAttr(): Retrieves the value of a specific attribute for a symbol table entry.
 * --------------------------------------------------------------------------------
 * This function retrieves the value of a specific attribute (`attr_num`) for a symbol
 * table entry (`st_ptr`). If the attribute does not exist, it reports a debug error.
 *
 * Parameters:
 *   - st_ptr: The index of the symbol in the symbol table (`st` array).
 *   - attr_num: The attribute number to retrieve.
 *
 * Returns:
 *   - The value of the attribute if it exists.
 *   - 0 if the attribute is not found.
 */
int GetAttr(int st_ptr, int attr_num) {
    int i;

    /* Step 1: Check if the attribute exists using IsAttr() */
    i = IsAttr(st_ptr, attr_num);

    if (!i) {
        /* Step 2: Attribute not found — print debug information */
        printf("DEBUG--The wanted attribute number %d does not exist\n", attr_num);
        return 0; // Return 0 to indicate the attribute is missing.
    }

    /* Step 3: Return the attribute value if found */
    return attrarray[i].attr_val;
}

/*
 * SetAttr(): Assigns or updates an attribute for a symbol table entry.
 * --------------------------------------------------------------------
 * This function sets a specific attribute (`attr_num`) with a given value (`attr_val`)
 * for a symbol table entry (`st_ptr`). If the attribute already exists, it updates its value.
 * If not, it inserts the attribute into the symbol's attribute list in sorted order.
 *
 * Parameters:
 *   - st_ptr  : The index of the symbol in the symbol table (`st` array).
 *   - attr_num: The attribute number to set (e.g., NAME_ATTR, TYPE_ATTR).
 *   - attr_val: The value to assign to the attribute.
 */
void SetAttr(int st_ptr, int attr_num, int attr_val) {
    int *p, next;
    int i;

    /* Step 1: Check if the attribute already exists */
    if ((i = IsAttr(st_ptr, attr_num))) {
        /*
        printf("DEBUG--The attribute number %d to be added already exists\n", attr_num);
        */
        attrarray[i].attr_val = attr_val; // Update the existing attribute value.
        return;                           // No need to insert a new attribute.
    }

    /* Step 2: Prepare to insert a new attribute */
    p = &st[st_ptr];   // Pointer to the first attribute of the symbol.
    next = st[st_ptr]; // Start traversing the linked list of attributes.

    /* Step 3: Find the correct insertion point in the sorted attribute list */
    while (next) {
        if (attrarray[next].attr_num < attr_num) {
            p = &(attrarray[next].next_attr); // Move to the next attribute.
            next = attrarray[next].next_attr; // Advance to the next attribute.
        } else {
            break; // Found the right place to insert.
        }
    }

    /* Step 4: Check for attribute array overflow */
    if (attr_top >= ATTR_SIZE - 1) {
        error_msg(ATTR_OVERFLOW, ABORT, 0, 0); // Stop execution if out of space.
    }

    /* Step 5: Insert the new attribute */
    attr_top++; // Move to the next free slot in the attribute array.

    attrarray[attr_top].attr_num = attr_num; // Set the attribute number.
    attrarray[attr_top].attr_val = attr_val; // Assign the value.
    attrarray[attr_top].next_attr = next;    // Link to the next attribute.

    *p = attr_top; // Update the previous link to point to the new attribute.
}

/*
 * External File Pointer for Output
 * --------------------------------
 * 'table' is an external file pointer used for outputting the symbol table or other
 * compiler-related information.
 *
 * This variable is used in functions like `STPrint()` to direct output to either the console
 * (stdout) or a file.
 *
 * Example Usage:
 *   - If `table = stdout;` → Output will be printed to the console.
 *   - If `table = fopen("symbol_table.txt", "w");` → Output will be written to a file.
 */
extern FILE *table;

/*
 * Array of Identifier Kinds (String Representations)
 * --------------------------------------------------
 * 'kind_name' is an array of string literals used to represent the kinds of identifiers
 * stored in the symbol table. Each index in this array corresponds to a specific kind of
 * symbol, making it easier to print or debug the symbol table.
 *
 * Example Kinds:
 *   - "constant"  → Constant values (e.g., `const int x = 5;`)
 *   - "variable"  → Regular variables (e.g., `int y;`)
 *   - "funcforw"  → Forward-declared functions
 *   - "function"  → Fully declared/defined functions
 *   - "ref_arg"   → Reference arguments in functions (e.g., `void func(int &x)`)
 *   - "val_arg"   → Value arguments in functions (e.g., `void func(int x)`)
 *   - "field"     → Fields within a structure or class
 *   - "typedef"   → Type definitions created using `typedef`
 *   - "procforw"  → Forward-declared procedures (functions without return values)
 *   - "procedure" → Defined procedures
 *   - "class"     → Class definitions in object-oriented languages
 *   - "array"     → Array types (e.g., `int arr[10];`)
 *
 * This array is typically used when printing the symbol table to convert numeric
 * identifiers (like `KIND_ATTR`) into human-readable strings.
 *
 * Example Usage:
 *   fprintf(table, "%s", kind_name[kind_attr - 1]);
 */
char *kind_name[] = {
    "constant",  // 1: Constant value
    "variable",  // 2: Variable
    "funcforw",  // 3: Forward-declared function
    "function",  // 4: Fully defined function
    "ref_arg",   // 5: Reference argument (passed by reference)
    "val_arg",   // 6: Value argument (passed by value)
    "field",     // 7: Field in a structure or class
    "typedef",   // 8: User-defined type using typedef
    "procforw",  // 9: Forward-declared procedure
    "procedure", // 10: Fully defined procedure (function without return value)
    "class",     // 11: Class definition
    "array"      // 12: Array type
};

/*
 * STPrint(): Prints the current state of the symbol table.
 * --------------------------------------------------------
 * This function iterates through the symbol table (`st`) and prints all relevant attributes
 * for each symbol in a well-formatted table. It displays information such as:
 *   - Identifier name
 *   - Nesting level
 *   - Kind (variable, function, etc.)
 *   - Type
 *   - Value (if it's a constant)
 *   - Memory offset
 *   - Array dimensions
 *   - Argument count (for functions/procedures)
 *
 * This function is mainly used for **debugging** and **verification** of the symbol table's state.
 */
void STPrint() {
    // FILE *table;
    int i, attr_num, attr_val, attr_ptr;
    // int treeval;
    tree ptrTree;

    /* Step 1: Set the output stream to the console */
    table = stdout;

    /* Step 2: Print the header of the symbol table */
    fprintf(table,
            "********************************Symbol Table************************************\n\n");
    fprintf(table,
            "          Name Nest-Level  Tree-Node Predefined        Kind       Type      Value  Offset Dimension   Argnum\n\n");

    /* Step 3: Iterate through each entry in the symbol table */
    for (i = 1; i <= st_top; i++) {
        // treeval = 0;  // Reset the tree node value for each symbol.

        /* Print the symbol table index */
        fprintf(table, "%3d", i);

        /* Step 4: Iterate through all possible attributes for each symbol */
        for (attr_num = NAME_ATTR; attr_num <= ARGNUM_ATTR; attr_num++) {
            /* Check if the current attribute exists for the symbol */
            if ((attr_ptr = IsAttr(i, attr_num))) {
                attr_val = attrarray[attr_ptr].attr_val; // Retrieve the attribute's value.

                /* Step 5: Print attribute values based on their type */
                switch (attr_num) {
                case NAME_ATTR:
                    fprintf(table, "%11s", getname(attr_val)); // Print the identifier's name.
                    break;

                case OFFSET_ATTR:
                case NEST_ATTR:
                case TREE_ATTR:
                    fprintf(table, "%11d", attr_val); // Print integer values for offset, nesting, or tree node.
                    break;

                case TYPE_ATTR:
                    // treeval = attr_val;  // Store the type attribute for potential later use.
                    fprintf(table, "%11d", attr_val); // Print the type information.
                    break;

                case PREDE_ATTR:
                    // Indicate if the symbol is predefined (like built-in functions).
                    fprintf(table, "%11s", attr_val == true ? "yes" : "no");
                    break;

                case KIND_ATTR:
                    // Print the kind of identifier (e.g., variable, function).
                    fprintf(table, "%11s", kind_name[attr_val - 1]);
                    break;

                case VALUE_ATTR:
                    // Print the value if the symbol is a constant.
                    if (GetAttr(i, KIND_ATTR) == CONST) {
                        ptrTree = (tree)attr_val; // Cast to tree
                        switch (NodeKind(ptrTree)) {
                        case NUMNode:
                            fprintf(table, "%11d", IntVal(ptrTree)); // Print integer constant.
                            break;
                        case CHARNode:
                            if (isprint(IntVal(ptrTree)))
                                fprintf(table, "%11c", IntVal(ptrTree)); // Print character constant.
                            break;
                        case STRINGNode:
                            fprintf(table, "%11s", getstring(IntVal(ptrTree))); // Print string constant.
                            break;
                        }
                    }
                    break;

                case DIMEN_ATTR:
                    fprintf(table, "%11d", attr_val); // Print array dimensions.
                    break;

                case ARGNUM_ATTR:
                    fprintf(table, "%11d", attr_val); // Print the number of arguments for functions/procedures.
                    break;
                }
            } else {
                /* If the attribute doesn't exist for the symbol, print empty space */
                fprintf(table, "%11s", " ");
            }
        }

        /* Step 6: Move to the next line for the next symbol */
        fprintf(table, "\n");

        /* Optional: Print the associated syntax tree (currently commented out) */
        /*
        if (treeval != 0)
          printtree((tree)treeval, 0);
        */
    }
}

/*
 * Push(): Pushes an element onto the stack for scope management.
 * ---------------------------------------------------------------
 * This function is used to push a new entry onto the symbol table stack.
 * It is primarily used for managing variable declarations and scope boundaries.
 *
 * Parameters:
 *   - marker : Marks the start of a new block/scope if set to true.
 *   - name   : The identifier's name (index in the string table).
 *   - st_ptr : Pointer to the symbol table entry for this identifier.
 *   - dummy  : If true, marks the entry as a dummy (for undeclared identifiers).
 *
 * Behavior:
 *   - Checks for stack overflow.
 *   - Pushes the provided details onto the stack.
 *   - Sets the 'used' flag to false by default (indicating the variable hasn't been used yet).
 */
void Push(int marker, int name, int st_ptr, int dummy) {

    /* Step 1: Check for stack overflow */
    if (stack_top >= STACK_SIZE - 1) {
        error_msg(STACK_OVERFLOW, ABORT, 0, 0); // Abort if the stack is full.
    }

    /* Step 2: Move to the next available position on the stack */
    stack_top++;

    /* Step 3: Push the new entry onto the stack */
    stack[stack_top].marker = (bool)marker; // Marks the start of a new block if true.
    stack[stack_top].name = name;           // Stores the identifier's name.
    stack[stack_top].st_ptr = st_ptr;       // Links to the symbol table entry.
    stack[stack_top].dummy = (bool)dummy;   // Flags this entry as a dummy if true.
    stack[stack_top].used = false;          // Marks the identifier as unused by default.
}

/*
 * seq_str(): Converts a numeric sequence to its ordinal string representation.
 * ----------------------------------------------------------------------------
 * This function returns the ordinal form (e.g., "1st", "2nd") of a given integer.
 * It is mainly used for error messages that reference specific arguments or parameters.
 *
 * Parameters:
 *   - seq : The numeric sequence to convert.
 *
 * Returns:
 *   - A string representing the ordinal form of the input sequence.
 *
 * Example:
 *   seq_str(0) → "0th"
 *   seq_str(1) → "1st"
 *   seq_str(2) → "2nd"
 *   seq_str(3) → "3rd"
 *   seq_str(4) → "4th"
 */
char *seq_str(int seq) {
    static char s[10]; // Buffer to store the generated ordinal string.

    /* Handle special cases for 0, 1, 2, and 3 */
    switch (seq) {
    case 0:
        return "0th";
    case 1:
        return "1st";
    case 2:
        return "2nd";
    case 3:
        return "3rd";
    default:
        /* Default case: Append "th" to numbers 4 and above */
        sprintf(s, "%dth", seq); // Converts the number to a string with the "th" suffix.
        return s;
    }
}