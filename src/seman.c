/**************************************************************************************************
 * File: semantic_analyzer.c
 * ------------------------------------------------------------------------------------------------
 * Description:
 *    This file implements the **Semantic Analyzer** for the compiler. Its primary responsibility
 *    is to traverse the Abstract Syntax Tree (AST) and perform semantic checks, ensuring that the
 *    program adheres to the language's semantic rules. It verifies correct usage of variables,
 *    functions, arrays, classes, and expressions by cross-referencing the **Symbol Table**.
 *
 *    The semantic analyzer performs the following key operations:
 *
 *    1. **Symbol Table Linking:**
 *       - Resolves identifiers in the AST by looking them up in the **Symbol Table**.
 *       - Replaces `IDNode` leaves in the AST with `STNode` leaves pointing to symbol table entries.
 *       - Ensures efficient access to type, kind, and scope information for variables, functions, etc.
 *
 *    2. **Type Checking:**
 *       - Ensures operations and assignments use **compatible types**.
 *       - Detects and reports **type mismatches** (e.g., assigning a string to an integer).
 *
 *    3. **Scope and Binding Resolution:**
 *       - Manages **scopes** for variables, functions, and classes.
 *       - Opens and closes scopes to ensure **proper visibility** of identifiers.
 *       - Prevents **out-of-scope** or **undeclared** variable usage.
 *
 *    4. **Function and Method Verification:**
 *       - Validates function and method **declarations** and **calls**.
 *       - Checks for **correct argument types** and **return types**.
 *       - Detects **redeclarations**, especially for the `main` method.
 *
 *    5. **Array and Indexing Validation:**
 *       - Ensures that arrays are accessed with the **correct number of indices**.
 *       - Verifies that index expressions are **valid** and within **declared dimensions**.
 *       - Handles array properties like `.length` with proper validation.
 *
 *    6. **Object and Field Access Handling:**
 *       - Validates that **fields** exist in objects and are **accessible**.
 *       - Resolves **nested field accesses** (e.g., `obj.field1.field2`).
 *
 *    7. **Error Detection and Reporting:**
 *       - Detects and reports **semantic errors** such as:
 *           - **Undeclared variables**
 *           - **Redeclarations**
 *           - **Invalid field/method access**
 *           - **Type mismatches**
 *           - **Invalid array indexing**
 *
 * ------------------------------------------------------------------------------------------------
 * Major Functions:
 *
 *    1. **void MkST(tree node):**
 *       - Recursively traverses the AST and delegates semantic checks to specialized functions.
 *       - Handles nodes like class definitions, method declarations, variable declarations, etc.
 *
 *    2. **void declop(tree node):**
 *       - Processes **variable** and **array declarations**.
 *       - Inserts variables into the symbol table and links AST nodes to symbol table entries.
 *       - Validates **initializers** and assigns correct **type attributes**.
 *
 *    3. **void varop(tree node, int declop):**
 *       - Handles **variable usage**, **assignments**, **field access**, and **array indexing**.
 *       - Ensures correct **type** and **scope** resolution for variables and objects.
 *       - Replaces identifiers with **symbol table references** (`STNode`).
 *
 *    4. **void typeidop(tree node):**
 *       - Resolves **type identifiers** for primitive and user-defined types.
 *       - Handles **array types** and processes nested type structures.
 *
 *    5. **void classdefop(tree node):**
 *       - Processes **class definitions** by inserting them into the symbol table.
 *       - Opens a **new scope** for class members and processes fields and methods.
 *
 *    6. **void methodop(tree node):**
 *       - Handles **method/function declarations**.
 *       - Inserts methods into the symbol table and validates parameters and return types.
 *       - Opens and closes the **local scope** for methods.
 *
 *    7. **void specop(tree node):**
 *       - Processes **method/function parameter declarations**.
 *       - Differentiates between **value arguments** and **reference arguments**.
 *
 *    8. **void routinecallop(tree node):**
 *       - Handles **routine calls** (functions/methods).
 *       - Validates routine existence and processes **argument expressions**.
 *
 * ------------------------------------------------------------------------------------------------
 * Example Workflow:
 *
 *    Given the following program:
 *
 *    ```c
 *    class Person {
 *        int age;
 *        void greet() {
 *            print("Hello");
 *        }
 *    }
 *
 *    Person p;
 *    p.greet();
 *    ```
 *
 *    **Step-by-Step Semantic Analysis:**
 *
 *    1. **Class Declaration:**
 *       - `Person` is added to the symbol table (`CLASS` kind).
 *       - `age` is inserted as a field in `Person`'s scope.
 *       - `greet()` is inserted as a `PROCE` (void method) in `Person`'s scope.
 *
 *    2. **Variable Declaration:**
 *       - `p` is added to the symbol table as a `VAR` of type `Person`.
 *
 *    3. **Method Call (`p.greet()`):**
 *       - `p` is resolved to the `Person` object.
 *       - The analyzer confirms `greet()` exists within the `Person` class.
 *
 *    4. **Error Detection:**
 *       - If `greet` was misspelled (`p.gret();`), the analyzer would report an **undeclared method error**.
 *
 * ------------------------------------------------------------------------------------------------
 * Error Reporting Examples:
 *
 *    - **Undeclared Variable:**
 *      ```c
 *      x = 5;  // Error: 'x' is undeclared.
 *      ```
 *
 *    - **Invalid Field Access:**
 *      ```c
 *      int a;
 *      a.length;  // Error: 'int' has no field 'length'.
 *      ```
 *
 *    - **Type Mismatch:**
 *      ```c
 *      int x;
 *      x = "hello";  // Error: assigning a string to an integer.
 *      ```
 *
 **************************************************************************************************/

#include "symbol_table.h"
#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * External Function Declarations
 */

/**
 * loc_str(): Searches for or inserts a string into the string table.
 * -----------------------------------------------------------------
 * This function locates a string in the string table and returns its index.
 * If the string doesn't exist, it inserts the string and returns its new index.
 *
 * Defined in: `string_hash_table.c`
 */
extern int loc_str();

/**
 * yyparse(): Starts the parsing process.
 * -------------------------------------
 * This function is generated by **Yacc/Bison** and is responsible for parsing
 * the source code according to the grammar rules.
 *
 * Defined in: `y.tab.c` (generated file)
 */
extern void yyparse();

/**
 * st_top: Tracks the top index of the symbol table.
 * -------------------------------------------------
 * This variable keeps track of the highest index used in the symbol table (`st[]`).
 *
 * Defined in: `symbol_table.c` or similar.
 */
extern int st_top;

/*
 * Function Declarations
 * ---------------------
 * These functions handle various operations on the syntax tree.
 */

/**
 * MkST(): Builds the Syntax Tree (AST) after parsing.
 * ---------------------------------------------------
 * This function is responsible for constructing the syntax tree (`SyntaxTree`)
 * based on the parsed tokens and grammar rules.
 *
 * Parameters:
 *   - tree: The root of the constructed syntax tree.
 */
void MkST(tree);

/**
 * typeidop(): Handles type identifier operations in the syntax tree.
 * -------------------------------------------------------------------
 * This function processes **type identifiers** (e.g., `int`, `float`) within
 * the syntax tree during semantic analysis.
 *
 * Parameters:
 *   - tree: The subtree where type identifiers are processed.
 */
void typeidop(tree);

/**
 * varop(): Handles variable operations in the syntax tree.
 * --------------------------------------------------------
 * This function processes variable-related operations in the syntax tree, such as:
 *   - Variable declarations
 *   - Assignments
 *   - Usage checks
 *
 * Parameters:
 *   - tree: The subtree representing a variable operation.
 *   - int:  Additional information (e.g., scope level or flags).
 */
void varop(tree, int);

/**
 * classdefop - Handles class definitions during semantic analysis.
 * ---------------------------------------------------------------------------
 *
 * @param node The syntax tree node representing the class definition (`ClassDefOp`).
 *
 * This function processes a class definition node in the syntax tree by:
 *   1. **Symbol Table Entry:** Extracts the class name and adds it to the symbol table.
 *   2. **Class Marking:** Marks the symbol as a `CLASS` kind in the symbol table.
 *   3. **Scope Management:** Opens a new scope for class members (fields and methods).
 *   4. **Syntax Tree Update:** Links the class node to its symbol table entry (`STNode`).
 *   5. **Recursive Processing:** Processes the class body to handle member declarations.
 *   6. **Scope Closure:** Closes the block to finalize the class's scope.
 *
 * **Examples of Syntax Trees Handled by `classdefop`:**
 *
 * 1. **Simple Class with Fields:**
 *
 *      class Person {
 *          int age;
 *          int height;
 *      }
 *
 *      ClassDefOp
 *       ├── BodyOp                   // Class body (fields and methods)
 *       │     ├── DeclOp             // Field: int age
 *       │     │     ├── NullExp()
 *       │     │     └── CommaOp
 *       │     │          ├── IDNode ("age")
 *       │     │          └── CommaOp
 *       │     │               ├── TypeIdOp → INTEGERTNode ("int")
 *       │     │               └── NullExp()
 *       │     └── DeclOp      // Field: int height
 *       │           ├── NullExp()
 *       │           └── CommaOp
 *       │                ├── IDNode ("height")
 *       │                └── CommaOp
 *       │                     ├── TypeIdOp → INTEGERTNode ("int")
 *       │                     └── NullExp()
 *       └── IDNode ("Person")       // Class name
 *
 * 2. **Class with a Method:**
 *
 *      class Calculator {
 *          method int add(int a, int b) {
 *              return a + b;
 *          }
 *      }
 *
 *      ClassDefOp
 *       ├── BodyOp                       // Class body
 *       │     └── MethodOp               // Method declaration
 *       │           ├── HeadOp          // Method signature
 *       │           │     ├── IDNode ("add")     // Method name
 *       │           │     └── SpecOp             // Parameters and return type
 *       │           │           ├── CommaOp     // Parameter list
 *       │           │           │     ├── IDNode ("a")
 *       │           │           │     └── IDNode ("b")
 *       │           │           └── TypeIdOp → INTEGERTNode ("int")  // Return type
 *       │           └── BodyOp         // Method body
 *       │                 └── StmtOp
 *       │                       └── ReturnOp
 *       │                             └── AddOp
 *       │                                   ├── VarOp → IDNode ("a")
 *       │                                   └── VarOp → IDNode ("b")
 *       └── IDNode ("Calculator")           // Class name
 *
 * 3. **Class with Self-Referencing Field:**
 *
 *      class Node {
 *          Node next;
 *      }
 *
 *      ClassDefOp
 *       ├── BodyOp                  // Class body
 *       │     └── DeclOp           // Field declaration
 *       │           ├── NullExp()
 *       │           └── CommaOp
 *       │                ├── IDNode ("next")
 *       │                └── CommaOp
 *       │                     ├── TypeIdOp → IDNode ("Node")  // Self-reference
 *       │                     └── NullExp()
 *       └── IDNode ("Node")       // Class name
 *
 * **Explanation:**
 * - The class name (`Node`) must be inserted into the symbol table before processing
 *   its body, allowing self-referencing fields like `Node next;`.
 * - The function ensures that the class body is scoped correctly and linked to the
 *   symbol table for future symbol resolution.
 */
void classdefop(tree node) {
    int nStrInd; /* String Table index for the class name */
    int nSymInd; /* Symbol Table index for the class entry */
    tree child;

    /**
     * Step 1: Extract the class name and insert it into the symbol table.
     * -------------------------------------------------------------------
     * - The **right child** holds the class identifier (`IDnum` in the syntax tree).
     * - The class name must be inserted into the symbol table **before** processing
     *   the class body to allow the class members to reference the class itself.
     */
    child = (tree)RightChild(node); // Get the right child (class name)
    nStrInd = IntVal(child);        // Get the string table index of the class name
    nSymInd = InsertEntry(nStrInd); // Insert the class into the symbol table

    /**
     * Step 2: Mark the entry as a CLASS in the symbol table.
     * ------------------------------------------------------
     * - This distinguishes the class from other kinds of symbols (variables, functions).
     */
    SetAttr(nSymInd, KIND_ATTR, CLASS);

    /**
     * Step 3: Open a new block for the class body.
     * -------------------------------------------
     * - This creates a **new scope** for the class members (fields and methods).
     * - Members declared inside the class will be correctly scoped.
     */
    OpenBlock();

    /**
     * Step 4: Replace the right child with a symbol table node.
     * --------------------------------------------------------
     * - The original **class name node** is freed to prevent memory leaks.
     * - The **right child** is updated to point to the symbol table entry (`STNode`),
     *   linking the syntax tree to the symbol table for future reference.
     */
    free(child);
    SetRightChild(node, MakeLeaf(STNode, nSymInd)); // Link class to symbol table

    /**
     * Step 5: Recursively process the class body (left child).
     * --------------------------------------------------------
     * - The **left child** contains the class body (fields and methods).
     * - Recursively process it using `MkST()` to handle nested declarations.
     */
    MkST((tree)LeftChild(node)); // Process class members (fields, methods)

    /**
     * Step 6: Close the class scope.
     * ------------------------------
     * - Once all class members have been processed, the scope is closed.
     * - This prevents class members from being accessible outside the class.
     */
    CloseBlock();
}

/**
 * methodop - Handles method definitions during semantic analysis.
 * ---------------------------------------------------------------------------
 *
 * @param node The syntax tree node representing the method definition (`MethodOp`).
 *
 * This function processes a method definition by:
 *   1. **Symbol Table Entry:** Inserts the method name into the symbol table.
 *   2. **Main Method Check:** Ensures that the `main` method isn't redeclared.
 *   3. **Method Type Assignment:** Marks the method as a `FUNC` (with return type) or `PROCE` (void).
 *   4. **Syntax Tree Update:** Replaces the method name node with a symbol table node (`STNode`).
 *   5. **Recursive Processing:** Processes the method's parameters and body.
 *   6. **Scope Closure:** Closes the method's local scope after processing.
 *
 * **Examples of Syntax Trees Handled by `methodop`:**
 *
 * 1. **Method with Return Type and Parameters:**
 *
 *      method int sum(int a, int b) {
 *          return a + b;
 *      }
 *
 *      MethodOp
 *       ├── HeadOp                         // Method signature
 *       │     ├── STNode ("sum")           // Linked to symbol table entry for "sum"
 *       │     └── SpecOp                   // Parameters and return type
 *       │           ├── CommaOp            // Parameters list
 *       │           │     ├── IDNode ("a")
 *       │           │     └── IDNode ("b")
 *       │           └── TypeIdOp → INTEGERTNode ("int")  // Return type
 *       └── BodyOp                         // Method body
 *             └── StmtOp
 *                   └── ReturnOp
 *                         └── AddOp
 *                               ├── VarOp → IDNode ("a")
 *                               └── VarOp → IDNode ("b")
 *
 * 2. **Void Method (Procedure) Without Parameters:**
 *
 *      method void greet() {
 *          print("Hello, World!");
 *      }
 *
 *      MethodOp
 *       ├── HeadOp                       // Method signature
 *       │     ├── STNode ("greet")       // Linked to symbol table entry for "greet"
 *       │     └── SpecOp                 // No parameters, return type is void
 *       │           ├── NullExp()       // No parameters
 *       │           └── NullExp()       // Void return type
 *       └── BodyOp                       // Method body
 *             └── StmtOp
 *                   └── RoutineCallOp
 *                         ├── VarOp → IDNode ("print")
 *                         └── CommaOp
 *                               └── STRINGNode ("Hello, World!")
 *
 * 3. **Redeclaration Check for `main`:**
 *
 *      method int main() { return 0; }
 *      method int main() { return 1; }  // Error: Redeclaration of 'main'
 *
 * **Explanation:**
 * - The first `main` method is inserted into the symbol table.
 * - When the second `main` is encountered, `methodop` detects the redeclaration
 *   and triggers an error message.
 *
 * **Detailed Workflow of `methodop`:**
 *
 * 1. **Extract Method Name:**
 *    - The method name is located in the **left child** of the `HeadOp` node.
 *    - This name is inserted into the symbol table for symbol tracking.
 *
 * 2. **Check for Redeclaration:**
 *    - If the method name is `"main"`, the function checks the symbol table to avoid multiple declarations.
 *    - If a duplicate is found, an error is reported, and processing is stopped.
 *
 * 3. **Insert and Open Scope:**
 *    - The method is added to the symbol table.
 *    - A **new block** is opened for scoping the method's parameters and body.
 *
 * 4. **Method Type Assignment:**
 *    - If the method has a return type, it is marked as a `FUNC` and the return type is stored.
 *    - If the method has no return type (`void`), it is marked as a `PROCE`.
 *
 * 5. **Link Syntax Tree to Symbol Table:**
 *    - The original method name node is replaced with a **`STNode`** to link to the symbol table entry.
 *
 * 6. **Recursive Processing:**
 *    - The **parameters** (right child of `HeadOp`) and the **body** (right child of `MethodOp`) are recursively processed.
 *
 * 7. **Close Scope:**
 *    - The method’s scope is closed to restrict local variables and parameters to the method.
 */

void methodop(tree node) {
    int nStrInd; /* String Table index for the method name */
    int nSymInd; /* Symbol Table index for the method entry */
    tree child, child1;

    /**
     * Step 1: Extract the method name from the left child of the node.
     * ----------------------------------------------------------------
     * - The **left child** of the `MethodOp` node holds the method signature (`HeadOp`).
     * - The **left child** of `HeadOp` contains the method name.
     */
    child = (tree)LeftChild(node);      // Access `HeadOp` node (method signature)
    nStrInd = IntVal(LeftChild(child)); // Extract method name (string table index)

    /**
     * Step 2: Check for multiple `main` function declarations.
     * --------------------------------------------------------
     * - If the method is `main`, verify that it hasn't been declared before.
     * - If redeclared, report an error.
     */
    if (nStrInd == loc_str("main")) {
        int i;
        for (i = 0; i <= st_top; i++) {
            if (IsAttr(i, NAME_ATTR)) {                             // Check if the symbol has a name attribute
                if (GetAttr(i, NAME_ATTR) == nStrInd) {             // Check if the method name is already used
                    error_msg(REDECLARATION, CONTINUE, nStrInd, 0); // Report redeclaration error
                    return;
                }
            }
        }
    }

    /**
     * Step 3: Insert the method into the symbol table and open a new scope.
     * ---------------------------------------------------------------------
     * - Insert the method name as a new entry in the symbol table.
     * - Open a new block to handle the method's local scope (parameters, variables).
     */
    nSymInd = InsertEntry(nStrInd); // Insert method into the symbol table
    OpenBlock();                    // Open a new scope for the method body

    /**
     * Step 4: Set the method kind and return type.
     * --------------------------------------------
     * - If the method has a return type, mark it as a `FUNC`.
     * - If no return type is provided, mark it as a `PROCE` (procedure/void).
     */
    child1 = (tree)RightChild(RightChild(child)); // Access the return type (if any)
    if (!IsNull(child1)) {
        SetAttr(nSymInd, KIND_ATTR, FUNC);        // Mark as a function
        SetAttr(nSymInd, TYPE_ATTR, (int)child1); // Safely cast the pointer to an integer
    } else {
        SetAttr(nSymInd, KIND_ATTR, PROCE); // Mark as a procedure (void)
    }

    /**
     * Step 5: Replace the method name with a symbol table reference.
     * --------------------------------------------------------------
     * - Free the original method name node.
     * - Replace it with a symbol table node (`STNode`) that links to the method's symbol table entry.
     */
    free(LeftChild(child));                         // Free the old name node
    SetLeftChild(child, MakeLeaf(STNode, nSymInd)); // Link to the symbol table entry

    /**
     * Step 6: Recursively process method parameters and the method body.
     * ------------------------------------------------------------------
     * - Process the method's parameter list (right child of `HeadOp`).
     * - Then process the method body (right child of the `MethodOp` node).
     */
    MkST((tree)RightChild(child)); // Process method parameters
    MkST((tree)RightChild(node));  // Process method body

    /**
     * Step 7: Close the method's scope after processing.
     * --------------------------------------------------
     * - This ensures that method-specific variables and parameters are scoped correctly.
     */
    CloseBlock();
}

/**
 * declop - Processes variable and array declarations during semantic analysis.
 * ---------------------------------------------------------------------------
 *
 * @param node The syntax tree node representing a declaration operation (`DeclOp`).
 *
 * This function performs the following steps for each declared variable:
 *   1. **Symbol Table Entry:** Inserts the variable into the symbol table.
 *   2. **Type Assignment:** Assigns the type attribute to the variable.
 *   3. **Array Handling:** If the variable is an array, it records its dimensions.
 *   4. **Initialization:** Processes any initializer provided for the variable.
 *   5. **Syntax Tree Update:** Replaces `IDNode` with `STNode` to link the variable to the symbol table.
 *
 * **Examples of Syntax Trees Handled by `declop`:**
 *
 * 1. **Scalar Variable Declaration (No Initialization):**
 *
 *      int x;
 *
 *      DeclOp
 *       ├── NullExp()
 *       └── CommaOp
 *            ├── IDNode ("x")               // Variable name
 *            └── CommaOp
 *                 ├── TypeIdOp              // Type information
 *                 │     ├── INTEGERTNode   // Base type: int
 *                 │     └── NullExp()      // No array dimensions
 *                 └── NullExp()            // No initializer
 *
 * 2. **Scalar Variable Declaration with Initialization:**
 *
 *      int y = 10;
 *
 *      DeclOp
 *       ├── NullExp()
 *       └── CommaOp
 *            ├── IDNode ("y")               // Variable name
 *            └── CommaOp
 *                 ├── TypeIdOp              // Type information
 *                 │     ├── INTEGERTNode   // Base type: int
 *                 │     └── NullExp()
 *                 └── ICONSTNode (10)      // Initializer (value 10)
 *
 * 3. **Array Declaration (Without Initialization):**
 *
 *      int arr[5][3];
 *
 *      DeclOp
 *       ├── NullExp()
 *       └── CommaOp
 *            ├── IDNode ("arr")             // Array name
 *            └── CommaOp
 *                 ├── TypeIdOp              // Array type
 *                 │     ├── INTEGERTNode   // Base type: int
 *                 │     └── IndexOp        // First dimension [5]
 *                 │          ├── ICONSTNode (5)
 *                 │          └── IndexOp   // Second dimension [3]
 *                 │               ├── ICONSTNode (3)
 *                 │               └── NullExp()
 *                 └── NullExp()            // No initializer
 *
 * 4. **Array Declaration with Initialization:**
 *
 *      int nums[3] = {1, 2, 3};
 *
 *      DeclOp
 *       ├── NullExp()
 *       └── CommaOp
 *            ├── IDNode ("nums")            // Array name
 *            └── CommaOp
 *                 ├── TypeIdOp             // Array type
 *                 │     ├── INTEGERTNode  // Base type: int
 *                 │     └── IndexOp       // Single dimension [3]
 *                 │          ├── ICONSTNode (3)
 *                 │          └── NullExp()
 *                 └── ArrayTypeOp         // Array initializer
 *                      ├── CommaOp
 *                      │     ├── ICONSTNode (1)
 *                      │     └── CommaOp
 *                      │           ├── ICONSTNode (2)
 *                      │           └── ICONSTNode (3)
 *                      └── INTEGERTNode
 *
 * 5. **Multiple Variable Declarations:**
 *
 *      int a, b = 20, c;
 *
 *      DeclOp
 *       ├── DeclOp
 *       │     ├── DeclOp
 *       │     │     ├── NullExp()
 *       │     │     └── CommaOp
 *       │     │          ├── IDNode ("a")
 *       │     │          └── CommaOp
 *       │     │               ├── TypeIdOp → INTEGERTNode ("int")
 *       │     │               └── NullExp()
 *       │     └── CommaOp
 *       │          ├── IDNode ("b")
 *       │          └── CommaOp
 *       │               ├── TypeIdOp → INTEGERTNode ("int")
 *       │               └── ICONSTNode (20)
 *       └── CommaOp
 *            ├── IDNode ("c")
 *            └── CommaOp
 *                 ├── TypeIdOp → INTEGERTNode ("int")
 *                 └── NullExp()
 *
 * **Note:** Redeclaration checks are not fully implemented yet.
 */

void declop(tree node) {
    int nStrInd; /* String Table index for the variable name */
    int nSymInd; /* Symbol Table index for the declared symbol */
    tree child, child1, child2, typenode;

    /* Step 1: Iterate through all declarations in the CommaOp chain */
    child = node;
    while (!IsNull(child)) {
        /* Step 2: Extract the variable's name and type */
        child1 = (tree)RightChild(child);    /* Access CommaOp node */
        nStrInd = IntVal(LeftChild(child1)); /* Get the IDNode's string index (variable name) */

        /* Step 3: Insert the variable into the symbol table */
        if (!(nSymInd = InsertEntry(nStrInd))) /* If redeclared, exit */
            return;

        /* Step 4: Assign the type attribute */
        typenode = (tree)LeftChild(RightChild(child1)); /* Access the type node */
        SetAttr(nSymInd, TYPE_ATTR, (int)typenode);     /* Set type attribute */

        /* Step 5: Replace the IDNode with an STNode in the syntax tree */
        free(LeftChild(child1));                         /* Free the old IDNode */
        SetLeftChild(child1, MakeLeaf(STNode, nSymInd)); /* Link to the symbol table */

        /* Step 6: Handle the type (scalar or array) */
        typeidop(typenode); /* Analyze the type node */

        if (IsNull(RightChild(typenode))) {
            /* Scalar variable: mark as VAR */
            SetAttr(nSymInd, KIND_ATTR, VAR);
        } else if (NodeOp(RightChild(typenode)) == IndexOp) {
            /* Array: mark as ARR and set dimensions */
            int n = 0;
            tree temp = (tree)RightChild(typenode);

            /* Count the number of IndexOp nodes to determine array dimensions */
            while (!IsNull(temp)) {
                n++;
                temp = (tree)RightChild(temp);
            }

            SetAttr(nSymInd, DIMEN_ATTR, n);  /* Set array dimension attribute */
            SetAttr(nSymInd, KIND_ATTR, ARR); /* Mark as an array */
        }

        /* Step 7: Handle initialization (if any) */
        child2 = (tree)RightChild(RightChild(child1)); /* Check for initializer */

        if (NodeOp(child2) == VarOp) {
            /* If it's a variable initializer, process it */
            varop(child2, 1);
        } else {
            /* Otherwise, recursively process the initializer */
            MkST(child2);
        }

        /* Step 8: Move to the next declaration in the chain */
        child = (tree)LeftChild(child);
    }
}

/**
 * specop - Handles method/function parameter declarations during semantic analysis.
 * ---------------------------------------------------------------------------------
 *
 * @param node The syntax tree node representing the parameter specification (`SpecOp`).
 *
 * This function processes formal parameters of methods or functions by:
 *   1. **Symbol Table Entry:** Inserts each parameter into the symbol table.
 *   2. **Type Assignment:** Assigns the correct type to each parameter.
 *   3. **Argument Kind Handling:** Distinguishes between **value arguments** (`VALUE_ARG`)
 *      and **reference arguments** (`REF_ARG`).
 *   4. **Syntax Tree Update:** Replaces `IDNode` with `STNode` to link parameters to the symbol table.
 *
 * **Examples of Syntax Trees Handled by `specop`:**
 *
 * 1. **Single Value Parameter:**
 *
 *      method void print(val int x) { ... }
 *
 *      SpecOp
 *       └── VArgTypeOp                    // Value argument
 *             └── CommaOp
 *                   ├── IDNode ("x")     // Parameter name
 *                   └── INTEGERTNode     // Type: int
 *
 *    **Action:**
 *    - `IDNode("x")` is inserted into the symbol table.
 *    - Marked as a **value argument** (`VALUE_ARG`).
 *    - Replaced with `STNode` pointing to the symbol table entry.
 *
 *    **After `specop`:**
 *
 *      SpecOp
 *       └── VArgTypeOp
 *             └── CommaOp
 *                   ├── STNode (symbol table entry for "x")
 *                   └── INTEGERTNode
 *
 * 2. **Single Reference Parameter:**
 *
 *      method void update(int y) { ... }
 *
 *      SpecOp
 *       └── RArgTypeOp                    // Reference argument (default)
 *             └── CommaOp
 *                   ├── IDNode ("y")     // Parameter name
 *                   └── INTEGERTNode     // Type: int
 *
 *    **Action:**
 *    - `IDNode("y")` is inserted into the symbol table.
 *    - Marked as a **reference argument** (`REF_ARG`).
 *    - Replaced with `STNode` pointing to the symbol table entry.
 *
 *    **After `specop`:**
 *
 *      SpecOp
 *       └── RArgTypeOp
 *             └── CommaOp
 *                   ├── STNode (symbol table entry for "y")
 *                   └── INTEGERTNode
 *
 * 3. **Multiple Mixed Parameters (Value and Reference):**
 *
 *      method void sum(val int a, int b) { ... }
 *
 *      SpecOp
 *       ├── VArgTypeOp                    // Value argument
 *       │     └── CommaOp
 *       │           ├── IDNode ("a")     // Parameter name
 *       │           └── INTEGERTNode     // Type: int
 *       └── RArgTypeOp                    // Reference argument
 *             └── CommaOp
 *                   ├── IDNode ("b")     // Parameter name
 *                   └── INTEGERTNode     // Type: int
 *
 *    **Action:**
 *    - `a` is inserted as a **value argument** (`VALUE_ARG`).
 *    - `b` is inserted as a **reference argument** (`REF_ARG`).
 *    - Both `IDNode`s are replaced with `STNode`s linking to their symbol table entries.
 *
 *    **After `specop`:**
 *
 *      SpecOp
 *       ├── VArgTypeOp
 *       │     └── CommaOp
 *       │           ├── STNode (symbol table entry for "a")
 *       │           └── INTEGERTNode
 *       └── RArgTypeOp
 *             └── CommaOp
 *                   ├── STNode (symbol table entry for "b")
 *                   └── INTEGERTNode
 *
 * 4. **Array Parameter (Reference by Default):**
 *
 *      method void process(int data[5]) { ... }
 *
 *      SpecOp
 *       └── RArgTypeOp                    // Reference argument
 *             └── CommaOp
 *                   ├── IDNode ("data")  // Parameter name
 *                   └── TypeIdOp
 *                         ├── INTEGERTNode  // Element type
 *                         └── IndexOp       // Array size
 *                               └── ICONSTNode (5)
 *
 *    **Action:**
 *    - `data` is inserted as a **reference argument** (`REF_ARG`).
 *    - The array structure (`IndexOp`) is preserved.
 *    - `IDNode("data")` is replaced with an `STNode`.
 *
 *    **After `specop`:**
 *
 *      SpecOp
 *       └── RArgTypeOp
 *             └── CommaOp
 *                   ├── STNode (symbol table entry for "data")
 *                   └── TypeIdOp
 *                         ├── INTEGERTNode
 *                         └── IndexOp
 *                               └── ICONSTNode (5)
 *
 * 5. **User-Defined Type as Parameter (Class Object):**
 *
 *      method void configure(Device dev) { ... }
 *
 *      SpecOp
 *       └── RArgTypeOp                    // Reference argument
 *             └── CommaOp
 *                   ├── IDNode ("dev")   // Parameter name
 *                   └── IDNode ("Device") // User-defined type
 *
 *    **Action:**
 *    - `dev` is inserted as a **reference argument** (`REF_ARG`).
 *    - `IDNode("Device")` is looked up in the symbol table and replaced with an `STNode`.
 *    - `IDNode("dev")` is replaced with an `STNode`.
 *
 *    **After `specop`:**
 *
 *      SpecOp
 *       └── RArgTypeOp
 *             └── CommaOp
 *                   ├── STNode (symbol table entry for "dev")
 *                   └── STNode (symbol table entry for "Device")
 *
 * **Step-by-Step Workflow of `specop`:**
 *
 * 1. **Initialize Traversal:**
 *    - Starts by checking the **left child** (`VArgTypeOp` or `RArgTypeOp`) for each parameter.
 *
 * 2. **Symbol Table Entry:**
 *    - Inserts each parameter into the symbol table.
 *
 * 3. **Type Assignment:**
 *    - Assigns the type (`TYPE_ATTR`) from the **right child** of the `CommaOp` node.
 *
 * 4. **Argument Kind Handling:**
 *    - Marks the parameter as **value** (`VALUE_ARG`) or **reference** (`REF_ARG`).
 *
 * 5. **Syntax Tree Update:**
 *    - Replaces the `IDNode` with an `STNode` that links to the symbol table.
 *
 * 6. **Move to Next Parameter:**
 *    - Traverses through all parameters via the **right child** of each node.
 */

void specop(tree node) {
    int nStrInd; /* String Table index for the parameter name */
    int nSymInd; /* Symbol Table index for the parameter entry */
    tree child, child1;

    /**
     * Step 1: Iterate through all parameters in the specification list.
     * -----------------------------------------------------------------
     * - The **left child** of the `SpecOp` node is either `VArgTypeOp` (value argument)
     *   or `RArgTypeOp` (reference argument).
     * - We traverse through each parameter declaration in the list.
     */
    child = (tree)LeftChild(node); /* Start with the first parameter (`VArgTypeOp` or `RArgTypeOp`) */
    while (!IsNull(child)) {

        /**
         * Step 2: Extract the parameter name and its type.
         * -----------------------------------------------
         * - The **left child** of the argument node (`VArgTypeOp` or `RArgTypeOp`) is a `CommaOp`.
         * - The **left child** of `CommaOp` is the parameter's **IDNode** (name).
         * - The **right child** of `CommaOp` holds the parameter's **type**.
         */
        child1 = (tree)LeftChild(child);     /* Access `CommaOp` node */
        nStrInd = IntVal(LeftChild(child1)); /* Extract the parameter name (string table index) */

        /**
         * Step 3: Insert the parameter into the symbol table.
         * ---------------------------------------------------
         * - Insert the parameter name as a new entry in the symbol table.
         * - This ensures that the parameter is correctly scoped in the method.
         */
        nSymInd = InsertEntry(nStrInd);

        /**
         * Step 4: Assign the type of the parameter.
         * -----------------------------------------
         * - Set the **type attribute** (`TYPE_ATTR`) for the parameter.
         * - The type is stored in the **right child** of the `CommaOp` node.
         */
        SetAttr(nSymInd, TYPE_ATTR, (int)RightChild(child1));

        /**
         * Step 5: Set the argument kind (Value or Reference).
         * ---------------------------------------------------
         * - If the parameter is declared with `val`, it is a **value argument** (`VALUE_ARG`).
         * - Otherwise, it defaults to a **reference argument** (`REF_ARG`).
         */
        if (NodeOp(child) == VArgTypeOp) {
            SetAttr(nSymInd, KIND_ATTR, VALUE_ARG); /* Pass-by-value argument */
        } else if (NodeOp(child) == RArgTypeOp) {
            SetAttr(nSymInd, KIND_ATTR, REF_ARG); /* Pass-by-reference argument */
        }

        /**
         * Step 6: Update the syntax tree to link the parameter to the symbol table.
         * ------------------------------------------------------------------------
         * - Free the original **IDNode** to prevent memory leaks.
         * - Replace it with an **STNode** that links to the parameter's symbol table entry.
         */
        free(LeftChild(child1));                         /* Free the old parameter name node */
        SetLeftChild(child1, MakeLeaf(STNode, nSymInd)); /* Link parameter to the symbol table */

        /**
         * Step 7: Move to the next parameter in the list.
         * -----------------------------------------------
         * - Traverse to the next parameter (if any) via the **right child**.
         */
        child = (tree)RightChild(child); /* Move to the next parameter */
    }
}

/**
 * typeidop - Processes type identifiers during semantic analysis.
 * ---------------------------------------------------------------------------
 *
 * @param node The syntax tree node representing a type or array type.
 *
 * This function ensures that types used in declarations are correctly resolved to either:
 *   1. **Primitive Types** (specifically `int`, represented by `INTEGERTNode`).
 *   2. **User-Defined Types** (classes), which are looked up in the symbol table to validate their declaration.
 *   3. **Array Types**, by recursively processing nested type structures (e.g., multi-dimensional arrays).
 *
 * **Detailed Workflow:**
 *
 *   - If the type is a user-defined class, it is replaced with a reference (`STNode`) to its symbol table entry.
 *   - If the type is an array, the function traverses through nested dimensions and processes each level.
 *
 * **Examples of Syntax Trees Handled by `typeidop`:**
 *
 * 1. **Simple Primitive Type Declaration (Integer):**
 *
 *      int x;
 *
 *      DeclOp
 *       └── CommaOp
 *             ├── IDNode ("x")              // Variable name
 *             └── TypeIdOp
 *                   └── INTEGERTNode ("int") // Primitive type
 *
 *    **Action:** No change is required since `int` is a primitive type.
 *
 * 2. **User-Defined Type Declaration (Class):**
 *
 *      Person p;
 *
 *      DeclOp
 *       └── CommaOp
 *             ├── IDNode ("p")                  // Variable name
 *             └── TypeIdOp
 *                   └── IDNode ("Person")       // User-defined class type
 *
 *    **Action:**
 *    - `IDNode("Person")` is looked up in the symbol table.
 *    - If found, it is replaced by `STNode` linked to the class entry.
 *
 *    **After `typeidop`:**
 *
 *      DeclOp
 *       └── CommaOp
 *             ├── IDNode ("p")
 *             └── TypeIdOp
 *                   └── STNode (symbol table entry for "Person")
 *
 * 3. **Single-Dimensional Array Declaration:**
 *
 *      int arr[10];
 *
 *      DeclOp
 *       └── CommaOp
 *             ├── IDNode ("arr")              // Array name
 *             └── TypeIdOp
 *                   ├── INTEGERTNode ("int")  // Element type
 *                   └── IndexOp               // Array dimension
 *                         └── ICONSTNode (10) // Array size
 *
 *    **Action:**
 *    - No symbol lookup is needed since the base type is `int`.
 *    - The `IndexOp` is preserved as the array dimension.
 *
 * 4. **Multi-Dimensional Array of a Class Type:**
 *
 *      Matrix m[5][10];
 *
 *      DeclOp
 *       └── CommaOp
 *             ├── IDNode ("m")                 // Array variable
 *             └── TypeIdOp
 *                   ├── IDNode ("Matrix")      // User-defined type
 *                   └── IndexOp                // Outer dimension
 *                         ├── ICONSTNode (5)  // Outer size
 *                         └── IndexOp         // Inner dimension
 *                               └── ICONSTNode (10) // Inner size
 *
 *    **Action:**
 *    - `IDNode("Matrix")` is looked up in the symbol table.
 *    - It is replaced by `STNode` referencing the class.
 *    - Array dimensions (`IndexOp`) are preserved.
 *
 *    **After `typeidop`:**
 *
 *      DeclOp
 *       └── CommaOp
 *             ├── IDNode ("m")
 *             └── TypeIdOp
 *                   ├── STNode (symbol table entry for "Matrix")  // Replaced type
 *                   └── IndexOp
 *                         ├── ICONSTNode (5)
 *                         └── IndexOp
 *                               └── ICONSTNode (10)
 *
 * **Step-by-Step Workflow of `typeidop`:**
 *
 * 1. **Initialize Traversal:**
 *    - Starts by checking the **left child** (`lchild`) for the base type.
 *    - The **right child** (`rchild`) is used for moving through nested types (e.g., array dimensions).
 *
 * 2. **Type Resolution:**
 *    - If `lchild` is **not null** and **not an integer** (`INTEGERTNode`):
 *      - Looks up the type in the **symbol table** to check if it's a valid class.
 *      - If found, replaces the `IDNode` with an `STNode` linked to the symbol table entry.
 *
 * 3. **Array Handling:**
 *    - If the type is an array (indicated by nested `IndexOp` nodes), the function **recursively traverses**
 *      the **right child** to process deeper levels of the array.
 *
 * 4. **Update the Syntax Tree:**
 *    - After verifying the type, updates the **syntax tree** by replacing type nodes with **symbol table references**.
 */
void typeidop(tree node) {
    tree lchild, rchild; /* Pointers for left and right children of the current node */
    int nSymInd;         /* Symbol Table index for the type (if user-defined) */

    /* Initialize traversal:
     * - `lchild` starts at the left child of the node (the base type).
     * - `rchild` is the current node (used for traversal through possible nested types).
     */
    lchild = (tree)LeftChild(node);
    rchild = node;

    /* Traverse through the right children (to handle array dimensions or nested types) */
    while (!IsNull(rchild)) {

        /* Check if the left child is not null and is NOT a primitive integer type */
        if (!IsNull(lchild) && NodeKind(lchild) != INTEGERTNode) {

            /* Look up the type in the symbol table to verify it's a declared class */
            nSymInd = LookUp(IntVal(lchild));

            /* Free the original left child (IDNode) after lookup */
            free(lchild);

            /* Replace the left child with a symbol table node (STNode) referencing the type */
            SetLeftChild(rchild, MakeLeaf(STNode, nSymInd));
        }

        /* Move to the next level in the type tree (for multi-dimensional arrays) */
        rchild = (tree)RightChild(rchild); /* Proceed to the next array dimension, if any */
        lchild = (tree)LeftChild(rchild);  /* Update the left child for the next check */
    }
}

/**
 * varop - Handles variable and field access during semantic analysis.
 * --------------------------------------------------------------------
 *
 * @param node The syntax tree node representing a variable or field access (`VarOp`).
 * @param declop Flag indicating the context of the call:
 *        - `1`: Called by `declop` for variable declarations.
 *        - `2`: Called by `routineop` for routine processing.
 *        - `0`: General case (variable or field access).
 *
 * This function resolves variable references, array accesses, and object field accesses by:
 *   1. **Symbol Lookup:** Verifying if the variable is declared in the symbol table.
 *   2. **Type Checking:** Distinguishing between variables, arrays, methods, and classes.
 *   3. **Field Access Handling:** Resolving chained field accesses (e.g., `obj.field1.field2`).
 *   4. **Array Index Validation:** Ensuring correct dimensionality for array accesses.
 *   5. **Syntax Tree Update:** Replacing `IDNode` with `STNode` to link to the symbol table.
 *   6. **Error Handling:** Reporting undeclared variables, incorrect indexing, or invalid field access.
 *
 * **Examples of Syntax Trees Handled by `varop`:**
 *
 * 1. **Simple Variable Access:**
 *
 *      x;
 *
 *      VarOp
 *       └── IDNode ("x")
 *
 *    **Action:**
 *    - Looks up `"x"` in the symbol table.
 *    - Replaces `IDNode("x")` with `STNode` linking to its symbol table entry.
 *
 *    **After `varop`:**
 *
 *      VarOp
 *       └── STNode (symbol table entry for "x")
 *
 * 2. **Array Access with Correct Indexing:**
 *
 *      arr[5];
 *
 *      VarOp
 *       ├── IDNode ("arr")
 *       └── IndexOp
 *             └── ICONSTNode (5)
 *
 *    **Action:**
 *    - Verifies that `"arr"` is declared as an array.
 *    - Checks that the number of indices matches the array's dimension.
 *    - Replaces `IDNode("arr")` with `STNode`.
 *
 *    **After `varop`:**
 *
 *      VarOp
 *       ├── STNode (symbol table entry for "arr")
 *       └── IndexOp
 *             └── ICONSTNode (5)
 *
 * 3. **Multi-Dimensional Array Access:**
 *
 *      matrix[3][4];
 *
 *      VarOp
 *       ├── IDNode ("matrix")
 *       └── IndexOp
 *             ├── ICONSTNode (3)
 *             └── IndexOp
 *                   └── ICONSTNode (4)
 *
 *    **Action:**
 *    - Checks that `"matrix"` is declared with **2 dimensions**.
 *    - Replaces `IDNode("matrix")` with `STNode`.
 *
 *    **After `varop`:**
 *
 *      VarOp
 *       ├── STNode (symbol table entry for "matrix")
 *       └── IndexOp
 *             ├── ICONSTNode (3)
 *             └── IndexOp
 *                   └── ICONSTNode (4)
 *
 * 4. **Field Access (Object Member):**
 *
 *      obj.field;
 *
 *      VarOp
 *       ├── IDNode ("obj")
 *       └── FieldOp
 *             └── IDNode ("field")
 *
 *    **Action:**
 *    - Verifies `"obj"` is declared and is of a class type.
 *    - Searches for `"field"` within `"obj"`'s class.
 *    - Replaces both `"obj"` and `"field"` with their respective `STNode`s.
 *
 *    **After `varop`:**
 *
 *      VarOp
 *       ├── STNode (symbol table entry for "obj")
 *       └── FieldOp
 *             └── STNode (symbol table entry for "field")
 *
 * 5. **Invalid Field Access on a Method:**
 *
 *      myMethod.member;
 *
 *    **Action:**
 *    - Detects that `"myMethod"` is a method, not an object.
 *    - Reports an error: *"method members cannot be accessed"*.
 *
 * 6. **Array Access with Incorrect Dimensions:**
 *
 *      arr[3][4];  // But `arr` is a 1D array
 *
 *    **Action:**
 *    - Detects the mismatch between provided indices and declared dimensions.
 *    - Reports an error: *"Incorrect indexing of array `arr`"*.
 *
 * **Step-by-Step Workflow of `varop`:**
 *
 * 1. **Symbol Lookup:**
 *    - Starts by looking up the variable (`IDNode`) in the symbol table.
 *    - If not found, reports an *undeclared variable* error.
 *
 * 2. **Node Replacement:**
 *    - Replaces `IDNode` with `STNode` if the variable is found.
 *
 * 3. **Type Handling:**
 *    - **Variable (`VAR`)**: Passes if no field/indexing follows.
 *    - **Function/Procedure (`FUNC`/`PROCE`)**: Disallows further access.
 *    - **Class (`CLASS`)**: Allows access to fields/methods via `FieldOp`.
 *    - **Array (`ARR`)**: Verifies correct indexing via `IndexOp`.
 *
 * 4. **Field Access Resolution:**
 *    - Traverses chained `FieldOp` nodes to resolve nested fields.
 *    - Checks scoping and class hierarchy for member validity.
 *
 * 5. **Array Index Validation:**
 *    - Confirms that the number of indices matches the declared dimensions.
 *    - Recursively processes expressions in index positions.
 *
 * 6. **Error Handling:**
 *    - Reports errors for undeclared variables, invalid field access, or incorrect indexing.
 *
 * 7. **Syntax Tree Update:**
 *    - Ensures the syntax tree accurately reflects symbol table entries by replacing nodes.
 */
void varop(tree node, int declop) {
    int nSymInd, tempind;     /* Symbol table indices for identifier lookup */
    int st_ind0, nest0;       /* Current symbol table index and nesting level */
    int dimension, d;         /* Expected and provided array dimensions */
    int i;                    /* Loop counter for symbol table search */
    tree lchild, rchild;      /* Left and right children for traversal */
    tree typeidop, temp;      /* Type node and temporary node for type checks */
    tree selectop, fld_indop; /* Nodes for field and index operations */
    bool found;               /* Flag for field/method lookup success */

    /* Step 1: Initialization */
    st_ind0 = nest0 = -1;
    dimension = d = 0;

    /* Extract variable identifier and its access pattern (field or array) */
    lchild = (tree)LeftChild(node);  /* IDNode (variable name) */
    rchild = (tree)RightChild(node); /* SelectOp (field/index access) */

    /* Step 2: Symbol Table Lookup */
    if ((nSymInd = LookUp(IntVal(lchild)))) {
        /* Found in the symbol table, replace IDNode with STNode */
        // free(lchild);
        SetLeftChild(node, MakeLeaf(STNode, nSymInd));
    } else {
        /* Not found, return (undeclared variable) */
        error_msg(UNDECLARATION, CONTINUE, IntVal(lchild), 0);
        return;
    }

    st_ind0 = nSymInd;

    /* Step 3: Process Access Pattern (Field or Array Access) */
    do {
        switch (GetAttr(st_ind0, KIND_ATTR)) {

        /* Case 1: Variable Access */
        case VAR:
            /**
             * Step 1: Retrieve the variable's type.
             * -------------------------------------
             * - `TYPE_ATTR` holds the type information of the variable.
             * - `typeidop` points to the **full type** of the variable.
             *   - If it's a scalar (`int`), it's just `INTEGERTNode`.
             *   - If it's an object, it's an `IDNode` referencing a class.
             */
            typeidop = (tree)GetAttr(st_ind0, TYPE_ATTR); /* Retrieve the variable's type */
            temp = (tree)LeftChild(typeidop);             /* Base type of the variable */

            /**
             * Step 2: Handle scalar (primitive) variables.
             * --------------------------------------------
             * - If the base type is `INTEGERTNode`, it's a scalar (`int`).
             * - Scalars **cannot** have fields or methods accessed (e.g., `x.age` is invalid).
             */
            if (NodeKind(temp) == INTEGERTNode) {
                /* Scalar variable: no further access allowed */
                if (IsNull(rchild)) {
                    return; /* No further access, correct usage */
                } else {
                    /**
                     * Error: Attempting to access a field or method of a scalar variable.
                     * Example of invalid code:
                     *      int x;
                     *      x.age;  // Error: 'x' is an integer, cannot access 'age'
                     */
                    printf("%s :", getname(GetAttr(st_ind0, NAME_ATTR)));
                    error_msg(FIELD_MIS, CONTINUE, IntVal(LeftChild(LeftChild(rchild))), 0);
                    return;
                }
            }
            /**
             * Step 3: Handle object variables (user-defined types like classes).
             * ------------------------------------------------------------------
             * - If the base type (`temp`) is an **IDNode**, it refers to a class type.
             * - `IntVal(temp)` retrieves the symbol table index of the class.
             * - **Update the nesting level (`nest0`)** to reflect the scope of the class.
             *   - This ensures that field/method lookups happen **inside the class scope**.
             *
             * Example:
             *      class Person {
             *          int age;
             *      }
             *      Person p;
             *      p.age;  // Correct: 'age' is accessed within the 'Person' class scope
             */
            else if ((st_ind0 = IntVal(temp))) {
                /* Object of a class, update nesting level */
                nest0 = GetAttr(st_ind0, NEST_ATTR);
            }
            break;

        /* Case 2: Function or Procedure Access */
        case PROCE:
        case FUNC:
            /**
             * Step 1: Handle Direct Access to a Function or Procedure.
             * -------------------------------------------------------
             * - If the **right child** (`rchild`) is `NULL`, it means the function/procedure is being used **correctly**.
             * - Valid Example (No field access):
             *      sum();       // Correct: Calling the function directly
             *      display();   // Correct: Calling the procedure directly
             */
            if (IsNull(rchild)) {
                return; /* Correct if no fields are accessed */
            } else {
                /**
                 * Step 2: Error Handling for Invalid Field Access on Functions/Procedures.
                 * -----------------------------------------------------------------------
                 * - **Functions** and **procedures** cannot have fields or methods accessed.
                 * - Trying to access a member of a function/procedure is a semantic error.
                 *
                 * **Invalid Examples:**
                 *
                 *      sum.result;         // Error: Cannot access a field of a function
                 *      display.output;     // Error: Cannot access a field of a procedure
                 *      sum[2];             // Error: Cannot index a function
                 *
                 * **Correct Usage:**
                 *
                 *      int result = sum();  // Correct: Function is called, result is stored
                 *      display();           // Correct: Procedure is called
                 *
                 * Action:
                 * - Print an error message indicating that the method cannot be accessed in this way.
                 * - Terminate the program with `exit(1)` to stop further processing.
                 */
                printf("method %s members cannot be accessed\n", getname(IntVal(lchild)));
                free(lchild);
                exit(1);
            }
            break;

        /* Case 3: Class Field Access */
        case CLASS:
            /**
             * Step 1: Initialize Field Access in a Class.
             * ------------------------------------------
             * - Retrieve the **nesting level** of the class to ensure proper scoping.
             * - Initialize pointers to traverse through **field accesses** (`.` operator).
             *
             * Example:
             *      class Person {
             *          int age;
             *          Address addr;
             *      }
             *
             *      person.age;   // Correct: Direct field access
             *      person.addr.city;  // Correct: Nested field access
             */
            nest0 = GetAttr(st_ind0, NEST_ATTR);   /* Get class's nesting level */
            selectop = rchild;                     /* Start at the first field access */
            fld_indop = (tree)LeftChild(selectop); /* Field being accessed */

            /**
             * Step 2: Check if the Current Access is a Field Access.
             *-----------------------------------------------------
             * - Verify that the next operation is a **field access** (`FieldOp`).
             * - If so, begin searching for the field within the class.
             */
            if (NodeOp(fld_indop) == FieldOp) {
                /**
                 * Step 3: Search for the Field in the Class Scope.
                 * -----------------------------------------------
                 * - Start searching from the **next symbol table index** after the class.
                 * - Ensure the field is declared at the **correct nesting level** (`nest0 + 1`).
                 *
                 * Example:
                 *      person.addr.city;
                 *
                 *      Declared as:
                 *          - `Person` (nesting level 0)
                 *          - `addr` inside `Person` (nesting level 1)
                 *          - `city` inside `Address` (nesting level 2)
                 */
                i = st_ind0 + 1;
                do {
                    if (GetAttr(i, NAME_ATTR) == IntVal(LeftChild(fld_indop)) &&
                        GetAttr(i, NEST_ATTR) == (nest0 + 1)) {
                        /**
                         * Step 4: Field Found - Update Syntax Tree.
                         * ----------------------------------------
                         * - Replace the **IDNode** of the field with a **STNode** linked to the symbol table entry.
                         * - This allows the compiler to reference the field's declaration directly.
                         */
                        free(LeftChild(fld_indop));
                        SetLeftChild(fld_indop, MakeLeaf(STNode, i));
                        found = true;

                        /**
                         * Step 5: Move to the Next Field Access (for Chained Access).
                         * ---------------------------------------------------------
                         * - If the field is an object, continue processing deeper field accesses.
                         * - Update the symbol table index and nesting level to reflect the next object.
                         *
                         * Example:
                         *      person.addr.city;  // Move from `addr` to `city`
                         */
                        st_ind0 = i;
                        nest0++;
                        rchild = (tree)RightChild(rchild);
                        break;
                    }
                    i++;
                } while (i <= st_top && GetAttr(i, NEST_ATTR) > nest0);

                /**
                 * Step 6: Error Handling - Field Not Found.
                 * ----------------------------------------
                 * - If the field is **not declared** within the class, report an **undeclared field error**.
                 *
                 *  **Invalid Example:**
                 *      person.height;  // Error: `height` is not a field in `Person`
                 */
                if (!found) {
                    error_msg(UNDECLARATION, CONTINUE, IntVal(LeftChild(fld_indop)), 0);
                    return;
                }
            }

            /**
             * Step 7: Handle Array Access on Class Objects (Edge Case).
             * ---------------------------------------------------------
             * - If the access is **not a field** but an **array index** (`IndexOp`), verify if this usage is valid.
             * - Arrays of class objects are valid **only during declarations** (`declop == 1`).
             * - Any other usage results in a **type mismatch error**.
             *
             * Example (Valid in Declaration):
             *      Person people[5];  // Valid if used in a declaration
             *
             * Example (Invalid Outside Declaration):
             *      people[5].name;    // Error if used improperly
             */
            else if (NodeOp(fld_indop) == IndexOp) {
                if (declop != 1) {
                    /* Error: Array access on a class object outside of a declaration is invalid */
                    error_msg(TYPE_MIS, CONTINUE, GetAttr(st_ind0, NAME_ATTR), 0);
                    return;
                } else {
                    /* Validate multiple array dimensions */
                    int di = 1; // Initialize dimension counter
                    rchild = (tree)RightChild(rchild);

                    while (!IsNull(rchild)) {
                        di++;
                        rchild = (tree)RightChild(rchild);
                        fld_indop = (tree)LeftChild(rchild);

                        /* Error: Invalid non-array access during multi-dimensional indexing */
                        if (NodeOp(fld_indop) != IndexOp) {
                            error_msg(FIELD_MIS, CONTINUE, IntVal(LeftChild(fld_indop)), 0);
                            return;
                        }
                    }
                }
            }
            break;

        /* Case 4: Array Access */
        case ARR:
            /**
             * Step 1: Ensure Proper Array Access
             * -----------------------------------
             * - If the array is accessed without any index (e.g., `arr` instead of `arr[0]`),
             *   this is invalid usage for arrays. Arrays must be accessed with an index.
             *
             * Example of incorrect usage:
             *      int arr[5];
             *      x = arr;       // Error: Accessing array without indexing.
             */
            if (IsNull(rchild)) {
                error_msg(INDX_MIS, CONTINUE, GetAttr(st_ind0, NAME_ATTR), 0);
                return;
            }

            /**
             * Step 2: Retrieve Array Type and Dimension Information
             * -----------------------------------------------------
             * - `typeidop`: Retrieves the type information of the array.
             * - `temp`: Extracts the base type (e.g., `int`, `class`) of the array.
             * - `dimension`: Fetches how many dimensions the array is declared with.
             *
             * Example:
             *      int arr[5][10];  →  dimension = 2
             */
            typeidop = (tree)GetAttr(st_ind0, TYPE_ATTR); // Access array's type information
            temp = (tree)LeftChild(typeidop);             // Extract base type (e.g., int)
            selectop = rchild;                            // Start processing the array's access pattern
            fld_indop = (tree)LeftChild(rchild);          // The first index or field being accessed
            dimension = GetAttr(st_ind0, DIMEN_ATTR);     // Total dimensions of the array

            /**
             * Step 3: Validate Array Indexing
             * -------------------------------
             * - Iterates through the provided indices in the access pattern.
             * - Ensures that the number of indices does **not exceed** the declared dimension.
             * - Each index must be a valid expression (handled via `MkST`).
             *
             * Correct Usage:
             *      int arr[3][4];
             *      arr[2][1];   // Correct (2 dimensions used)
             *
             * Incorrect Usage:
             *      arr[2][1][0];  // Error: Too many indices for a 2D array
             */
            if (NodeKind(temp) == INTEGERTNode) {
                d = 0; /* Initialize index depth counter */

                while (!IsNull(rchild) && NodeOp(fld_indop) != FieldOp) {
                    d++; /* Count each index access */

                    /* Error: Too many indices for the declared dimensions */
                    if (d > dimension) {
                        error_msg(INDX_MIS, CONTINUE, GetAttr(st_ind0, NAME_ATTR), 0);
                        return;
                    }

                    /* Validate that each index is a valid expression */
                    if (NodeKind(LeftChild(fld_indop)) == EXPRNode) {
                        // Example: arr[i * 2 - 1];
                        MkST((tree)LeftChild(fld_indop)); /* Process the index expression */
                    }

                    /* Move to the next index */
                    selectop = rchild = (tree)RightChild(rchild);
                    fld_indop = (tree)LeftChild(selectop);
                }

                /* Error: Not enough indices provided for the array */
                if (IsNull(rchild) && d < dimension) {
                    error_msg(INDX_MIS, CONTINUE, GetAttr(st_ind0, NAME_ATTR), 0);
                    return;
                }

                /**
                 * Step 4: Special Case - Accessing Array `.length`
                 * ------------------------------------------------
                 * - Valid access: `arr.length`
                 * - No further access is allowed after `.length`.
                 */
                else if (!IsNull(rchild)) {
                    if (IntVal(LeftChild(fld_indop)) != loc_str("length")) {
                        error_msg(TYPE_MIS, CONTINUE, GetAttr(st_ind0, NAME_ATTR), 0);
                        return;
                    } else {
                        /* Error if there's anything beyond `.length` */
                        if (!IsNull(RightChild(selectop))) {
                            error_msg(TYPE_MIS, CONTINUE, GetAttr(st_ind0, NAME_ATTR), 0);
                            return;
                        }

                        /* Move past `.length` */
                        rchild = (tree)RightChild(rchild);
                        fld_indop = (tree)LeftChild(selectop);
                    }
                }
            }

            /**
             * Step 5: Validate Array Indexing for Class-Type Arrays
             * -----------------------------------------------------
             * - If the array is of a **user-defined type** (class), validate access accordingly.
             * - Example:
             *      class Person { int age; };
             *      Person people[5];
             *      people[2].age;  // Valid
             */
            else if ((tempind = IntVal(temp))) { /* Array of a class type */
                d = 0;                           /* Reset index counter */

                /* Validate multi-dimensional indexing */
                while (!IsNull(rchild) && NodeOp(fld_indop) != FieldOp) {
                    d++;

                    /* Error: Too many indices */
                    if (d > dimension) {
                        error_msg(INDX_MIS, CONTINUE, GetAttr(st_ind0, NAME_ATTR), 0);
                        return;
                    }

                    /* Validate index expressions */
                    if (NodeKind(LeftChild(fld_indop)) == EXPRNode) {
                        MkST((tree)LeftChild(fld_indop));
                    }

                    selectop = rchild = (tree)RightChild(rchild);
                    fld_indop = (tree)LeftChild(selectop);
                }

                /* Error: Not enough indices */
                if (IsNull(rchild) && d < dimension) {
                    error_msg(INDX_MIS, CONTINUE, GetAttr(st_ind0, NAME_ATTR), 0);
                    return;
                }

                /**
                 * Step 6: Accessing Fields or Properties of Class Arrays
                 * ------------------------------------------------------
                 * - Handles cases where a **class array** is accessed using an index, followed by either:
                 *   1. **Accessing a field** of the class object.
                 *   2. **Accessing the `.length` property** to retrieve the array size.
                 *
                 * **Valid Example 1 (Accessing Object Field):**
                 *      Person people[5];
                 *      int age = people[2].age;  // Accessing the 'age' field of the Person object at index 2.
                 *
                 * **Valid Example 2 (Accessing Array Length):**
                 *      int size = people.length;  // Correct: Accessing the size of the array.
                 *
                 * **Invalid Example (Chaining After `.length`):**
                 *      int x = people.length.value;  // Error: `.length` is not an object and can't have fields.
                 */
                else if (!IsNull(rchild)) {
                    /**
                     * Step 6.1: Check if the accessed property is **not** `.length`.
                     * --------------------------------------------------------------
                     * - If it's **not** `.length`, it assumes the access is a **field** of the class object.
                     * - In this case, it **updates the context** to the class type to continue checking deeper fields.
                     *
                     * Example:
                     *      people[2].address.city;
                     *      // `people` → array of `Person`
                     *      // `address` → field in `Person`
                     *      // `city` → field in `Address`
                     */
                    if (IntVal(LeftChild(fld_indop)) != loc_str("length")) {
                        /* Update context to the class type */
                        st_ind0 = tempind;                   // Update the current symbol to the class type.
                        nest0 = GetAttr(st_ind0, NEST_ATTR); // Update nesting level for proper scoping.
                    } else {
                        /**
                         * Step 6.2: Handle `.length` Property Access
                         * ------------------------------------------
                         * - If the accessed property **is** `.length`, ensure that **no further fields** are accessed.
                         * - **`.length`** should **only** be accessed directly without any chaining.
                         *
                         * **Valid Usage:**
                         *      people.length;   // Correct: Accessing the array size.
                         *
                         * **Invalid Usage:**
                         *      people.length.value;  // Error: `.length` is not an object, cannot access `.value`.
                         */
                        if (!IsNull(RightChild(selectop))) {
                            /* Error: Further access after `.length` is invalid. */
                            error_msg(TYPE_MIS, CONTINUE, GetAttr(st_ind0, NAME_ATTR), 0);
                            return;
                        }

                        /**
                         * Step 6.3: Move Past `.length` Access
                         * ------------------------------------
                         * - Since `.length` access is valid and standalone, move to the **next field** or **index** (if any).
                         */
                        rchild = (tree)RightChild(rchild);
                        fld_indop = (tree)LeftChild(selectop);
                    }
                }
            }
            break;

        default:
            break;
        }

    } while (!IsNull(rchild)); /* Continue until no more fields/indices */
}

/**
 * routinecallop - Handles routine (function/method) calls during semantic analysis.
 * ---------------------------------------------------------------------------------
 *
 * @param node The syntax tree node representing a routine call (`RoutineCallOp`).
 *
 * This function performs semantic analysis for routine calls by:
 *
 *   1. **Resolving the Called Routine:**
 *      - Verifies the routine (function/method) being called exists in the symbol table.
 *      - Ensures the correct type of entity is being called (i.e., not a variable or class).
 *      - Handles any object or method chaining using `varop()`.
 *
 *   2. **Processing Call Arguments:**
 *      - If the call has arguments, it recursively processes each parameter expression.
 *      - Validates argument types and expressions for compatibility with the function signature.
 *
 * **Detailed Workflow:**
 *
 *   - **Left Child:** Represents the routine (method or function) being called.
 *   - **Right Child:** Represents the list of argument expressions passed to the routine.
 *
 * **Examples of Syntax Trees Handled by `routinecallop`:**
 *
 * 1. **Function Call Without Arguments:**
 *
 *      foo();
 *
 *      RoutineCallOp
 *       ├── IDNode ("foo")    // Function to call
 *       └── NullExp()         // No parameters
 *
 *    **Processing Steps:**
 *    - Calls `varop()` on `"foo"` to ensure it's declared and callable.
 *    - Since there are no parameters, `MkST()` is called with a `NullExp()`.
 *
 * ---
 *
 * 2. **Function Call With Arguments:**
 *
 *      sum(a, b + 1);
 *
 *      RoutineCallOp
 *       ├── IDNode ("sum")            // Function to call
 *       └── CommaOp                   // Argument list
 *            ├── IDNode ("a")        // First argument
 *            └── AddOp               // Second argument (b + 1)
 *                 ├── IDNode ("b")
 *                 └── ICONSTNode (1)
 *
 *    **Processing Steps:**
 *    - Calls `varop()` on `"sum"` to verify the function exists.
 *    - Calls `MkST()` on the `CommaOp` node to process each argument expression.
 *
 * ---
 *
 * 3. **Method Call on an Object Without Arguments:**
 *
 *      obj.print();
 *
 *      RoutineCallOp
 *       ├── SelectOp                // Object access
 *       │    ├── IDNode ("obj")     // Object
 *       │    └── FieldOp            // Method call
 *       │         └── IDNode ("print")
 *       └── NullExp()               // No parameters
 *
 *    **Processing Steps:**
 *    - `varop()` resolves `obj.print` ensuring `print` exists in `obj`'s class.
 *    - No parameters, so `MkST()` is called on `NullExp()`.
 *
 * ---
 *
 * **Step-by-Step Workflow of `routinecallop`:**
 *
 * 1. **Extract the Routine Name and Arguments:**
 *    - `lchild`: Points to the routine being called (function/method).
 *    - `rchild`: Points to the list of argument expressions (or `NullExp()` if none).
 *
 * 2. **Resolve the Routine Identifier (`varop`):**
 *    - Calls `varop(lchild, 2)` to verify the routine exists in the symbol table.
 *    - The `2` indicates the context is a routine call, ensuring proper checks.
 *
 * 3. **Process Argument Expressions (`MkST`):**
 *    - If `rchild` is **not null**, calls `MkST(rchild)` to recursively process and validate all arguments.
 *    - Handles complex expressions and ensures type correctness.
 *
 * 4. **Error Handling:**
 *    - If the routine is undeclared or the wrong type, `varop()` will handle reporting the error.
 *    - If argument types are incorrect, `MkST()` will flag mismatches.
 */
void routinecallop(tree node) {
    tree lchild, rchild;

    /* Step 1: Extract routine identifier and argument list */
    lchild = (tree)LeftChild(node);  /* Routine name or object.method */
    rchild = (tree)RightChild(node); /* Argument list or NullExp() */

    /**
     * Step 2: Resolve the routine being called.
     * -----------------------------------------
     * - `varop` verifies that the routine exists and is callable.
     * - `2` signals that this is a routine call context, ensuring proper validation.
     * - Handles method calls (`obj.method`) or function calls (`foo`).
     */
    varop(lchild, 2);

    /**
     * Step 3: Process argument expressions (if any).
     * ----------------------------------------------
     * - If arguments are provided, validate each one using `MkST`.
     * - Ensures correct types and expression evaluation for function parameters.
     */
    MkST(rchild);
}

/**
 * MkST(): Builds the Symbol Table by Traversing the Syntax Tree.
 * --------------------------------------------------------------
 * This function recursively traverses the syntax tree and performs semantic analysis
 * by handling different kinds of nodes. It processes declarations, class definitions,
 * methods, and other constructs, updating the symbol table as needed.
 *
 * Parameters:
 *   - treenode: The current node in the syntax tree to process.
 *
 * Behavior:
 *   1. Checks if the node is null.
 *   2. Determines the node's operation type using `NodeOp()`.
 *   3. Calls the corresponding handler function for each operation type.
 *   4. Recursively processes the left and right child nodes for non-special cases.
 */
void MkST(tree treenode) {
    /* Step 1: Check if the current node is null. If it is, do nothing. */
    if (!IsNull(treenode)) {

        /* Step 2: Identify the operation type of the current node */
        switch (NodeOp(treenode)) {

        /* Step 3: Handle class definitions */
        case ClassDefOp:
            classdefop(treenode); // Process a class definition
            break;

        /* Step 4: Handle method (function) definitions inside classes */
        case MethodOp:
            methodop(treenode); // Process a method definition
            break;

        /* Step 5: Handle variable and constant declarations */
        case DeclOp:
            declop(treenode); // Process declarations (variables/constants)
            break;

        /* Step 6: Handle specialized declarations (e.g., arrays, records) */
        case SpecOp:
            specop(treenode); // Process specialized declarations
            break;

        /* Step 7: Handle type identifiers (e.g., `int`, `float`) */
        case TypeIdOp:
            typeidop(treenode); // Process type identifiers
            break;

        /* Step 8: Handle variable usage or assignments */
        case VarOp:
            varop(treenode, 0); // Process variable operations (e.g., assignments)
            break;

        /* Step 9: Handle routine (function/procedure) calls */
        case RoutineCallOp:
            routinecallop(treenode); // Process function/procedure calls
            break;

        /* Step 10: Default case—recursively process child nodes */
        default:
            /* If the node doesn't match any specific case, recursively process its children */
            MkST((tree)LeftChild(treenode));  // Process the left child
            MkST((tree)RightChild(treenode)); // Process the right child
            break;
        }
    }
}