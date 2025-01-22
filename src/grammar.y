%{
#include "tree.h"  // Header file containing definitions for syntax tree structures and functions
#include <stdio.h> // Standard I/O library for input/output operations

/* 
 * Global tree pointers used for building different parts of the syntax tree during parsing. 
 */

/** 
 * Used to store the type of a variable or field declaration. 
 * Example: When declaring an `int` or `float` variable, `type_record` holds the type node.
 */
tree type_record;

/** 
 * Used to store the return type of a method during method declaration parsing. 
 * Example: In `int sum() { ... }`, `type_method` holds the `int` type node.
 */
tree type_method;

/** 
 * Used to build the argument type list for method parameters. 
 * Example: For `sum(int a, int b)`, `argtype` helps construct the parameter tree.
 */
tree argtype;

/**
 * External reference to the root of the syntax tree.
 * This will be set after parsing is complete and represents the entire program structure.
 */
 extern tree SyntaxTree;
%}

/* -------------------------- Token Declarations -------------------------- */
/**
 * Tokens represent the basic symbols or keywords of the language.
 * Each token is associated with an `intg` value to store its corresponding value in parsing.
 */
 /* 
 * %union Declaration
 * -----------------------------------------------
 * Purpose:
 * - The `%union` directive defines a data structure that can hold 
 *   multiple types of values. This structure is used for `yylval`, 
 *   the semantic value associated with tokens or grammar rules.
 *
 * - Each field in `%union` corresponds to a type that may be passed 
 *   between the lexer and parser, or between grammar rules.
 *
 * Usage:
 * - The `<field_name>` in `%token` or `%type` declarations references 
 *   a specific field of this union.
 *
 * Example:
 * - `%token <intg> ICONSTnum`: Tokens like `ICONSTnum` will use 
 *   the `intg` field to store integer values.
 * - `%type <tptr> Expression`: Grammar rules for `Expression` will 
 *   use the `tptr` field to store syntax tree pointers.
 */

/* Union definition for semantic values */
%union {
  int intg;   /* Stores integer values (e.g., numeric constants) */
  tree tptr;  /* Stores pointers to syntax tree nodes */
}

%token <intg> PROGRAMnum         // 'program' keyword
%token <intg> IDnum              // Identifier (variable/function/class names)
%token <intg> SEMInum            // ';' (semicolon)
%token <intg> CLASSnum           // 'class' keyword
%token <intg> DECLARATIONSnum    // 'declarations' keyword
%token <intg> ENDDECLARATIONSnum // 'enddeclarations' keyword

%token <intg> COMMAnum           // ',' (comma)
%token <intg> EQUALnum           // '=' (assignment or equality operator)
%token <intg> LBRACEnum          // '{' (left brace)
%token <intg> RBRACEnum          // '}' (right brace)
%token <intg> LBRACnum           // '[' (left bracket)
%token <intg> RBRACnum           // ']' (right bracket)
%token <intg> LPARENnum          // '(' (left parenthesis)
%token <intg> RPARENnum          // ')' (right parenthesis)
%token <intg> VOIDnum            // 'void' keyword

%token <intg> INTnum             // 'int' keyword
%token <intg> METHODnum          // 'method' keyword
%token <intg> VALnum             // 'val' keyword (for value parameters)
%token <intg> DOTnum             // '.' (dot operator for member access)
%token <intg> ASSGNnum           // ':=' (assignment operator)
%token <intg> RETURNnum          // 'return' keyword
%token <intg> IFnum              // 'if' keyword
%token <intg> ELSEnum            // 'else' keyword
%token <intg> WHILEnum           // 'while' keyword

%token <intg> LTnum              // '<' (less than)
%token <intg> LEnum              // '<=' (less than or equal)
%token <intg> EQnum              // '==' (equality operator)
%token <intg> NEnum              // '!=' (not equal)
%token <intg> GEnum              // '>=' (greater than or equal)
%token <intg> GTnum              // '>' (greater than)

%token <intg> PLUSnum            // '+' (addition)
%token <intg> MINUSnum           // '-' (subtraction)
%token <intg> ORnum              // '||' (logical OR)
%token <intg> TIMESnum           // '*' (multiplication)
%token <intg> DIVIDEnum          // '/' (division)
%token <intg> ANDnum             // '&&' (logical AND)

%token <intg> NOTnum             // '!' (logical NOT)
%token <intg> ICONSTnum          // Integer constant (e.g., 42)
%token <intg> SCONSTnum          // String constant (e.g., "Hello")

/* -------------------------- Non-terminal Declarations -------------------------- */
/**
 * Non-terminals are grammar rules that represent complex language constructs.
 * These are associated with a `tree` pointer (`tptr`) to build the syntax tree.
 */

%type <tptr> Program               // Entire program structure
%type <tptr> ClassDecl_rec         // Multiple class declarations
%type <tptr> ClassDecl             // A single class declaration
%type <tptr> ClassBody             // Body of a class (methods and fields)

%type <tptr> MethodDecl_z1        // Optional method declaration
%type <tptr> MethodDecl_rec       // One or more method declarations
%type <tptr> Decls                // Declarations block

%type <tptr> FieldDecl_rec        // Multiple field declarations
%type <tptr> FieldDecl            // A single field declaration
%type <tptr> Tail                 // Remaining variables in a declaration
%type <tptr> FieldDecl_body       // Body of field declarations

%type <tptr> VariableDeclId       // Identifier for variable declarations
%type <tptr> Bracket_rec1         // Bracket notation for arrays (e.g., [])
%type <tptr> Bracket_rec2         // Nested brackets for multidimensional arrays

%type <tptr> VariableInitializer  // Variable initialization (e.g., `= 5`)
%type <tptr> ArrayInitializer     // Array initialization (e.g., `{1, 2, 3}`)
%type <tptr> ArrayInitializer_body // Body of array initialization
%type <tptr> ArrayCreationExpression // `new` array creation
%type <tptr> ArrayCreationExpression_tail // Array size expressions

%type <tptr> MethodDecl           // Full method declaration
%type <tptr> FormalParameterList_z1 // Optional parameter list
%type <tptr> FormalParameterList  // Full parameter list
%type <tptr> FormalParameterList_rec // Recursive parameter list construction

%type <tptr> IDENTIFIER_rec       // List of identifiers

%type <tptr> Block                // A block of statements `{ ... }`
%type <tptr> Type                 // Type information (int, void, etc.)
%type <tptr> Type_front           // The basic type without arrays

%type <tptr> StatementList        // List of statements in a block
%type <tptr> Statement_rec        // Recursive statement list
%type <tptr> Statement            // A single statement

%type <tptr> AssignmentStatement  // Assignment operation (`x := 5`)
%type <tptr> MethodCallStatement  // Method call (`foo()`)
%type <tptr> MethodCallStatement_tail // Arguments of method calls

%type <tptr> Expression_rec       // Recursive expression handling
%type <tptr> ReturnStatement      // `return` statement
%type <tptr> IfStatement          // `if` statement
%type <tptr> If_rec               // Recursive `if-else` structure
%type <tptr> WhileStatement       // `while` loop

%type <tptr> Expression           // Full expression
%type <tptr> Comp_op              // Comparison operators
%type <tptr> SimpleExpression     // Addition, subtraction, etc.
%type <tptr> Term                 // Multiplication, division, etc.
%type <tptr> Factor               // Factors in expressions (constants, variables)

%type <tptr> Expression_rec2      // Recursive expression handling for arrays

%type <tptr> UnsignedConstant     // Numeric or string constant
%type <tptr> Variable             // Variable reference
%type <tptr> Variable_tail        // Array or field access (`a[0]`, `obj.field`)
%type <tptr> Variable_rec         // Recursive variable access
%type <tptr> Variable_1           // Single variable component (array index or field access)

%% /* YACC Specification */

/**
 * Grammar Rule: Program
 * 
 * A program begins with the keyword `PROGRAM`, followed by an identifier (program name),
 * a semicolon, and a series of class declarations.
 * 
 * Example: 
 *      program MyProgram;
 *      class MyClass { ... }
 * 
 * Action:
 * - Constructs the root of the syntax tree with `ProgramOp` as the operation.
 * - The left child represents the parsed class declarations.
 * - The right child is set to `NullExp()` since it isn't needed here.
 * - The `SyntaxTree` global variable is updated to store the constructed tree.
 * 
 * Example Syntax Tree Structure:
 * ProgramOp
 * ├── BodyOp (contains all class declarations)
 * │   ├── ClassDefOp (class definition for `MyClass`)
 * │   │   ├── BodyOp (class body: fields, methods)
 * │   │   │   ├── DeclOp (field declaration: `int x;`)
 * │   │   │   │   ├── NullExp()
 * │   │   │   │   └── CommaOp
 * │   │   │   │       ├── IDNode ("x")
 * │   │   │   │       └── TypeIdOp → INTEGERTNode ("int")
 * │   │   │   └── MethodOp (method definition: `void print() { ... }`)
 * │   │   │       ├── HeadOp (method signature: `void print()`)
 * │   │   │       │   ├── IDNode ("print")
 * │   │   │       │   └── SpecOp (parameter list and return type)
 * │   │   │       │       ├── NullExp() (no parameters)
 * │   │   │       │       └── NullExp() (void return type)
 * │   │   │       └── BodyOp (method body: statements)
 * │   │   │           └── StmtOp (e.g., `x = 5;`)
 * │   │   └── IDNode ("MyClass") (class name)
 * └── NullExp()
 */
Program : PROGRAMnum IDnum SEMInum ClassDecl_rec
          {  
              $$ = MakeTree(ProgramOp, $4, NullExp());  // Create the Program node in the syntax tree
              SyntaxTree = $$;                          // Store the syntax tree in the global variable
              // printtree($$, 0);                     // (Optional) Uncomment to print the tree
          }
        ;

/**
 * Grammar Rule: ClassDecl_rec
 * 
 * Handles **one or more** class declarations in the program.
 * 
 * Example:
 *      class A { }
 *      class B { }
 * 
 * Action:
 * - If there's only one class, wrap it with a `ClassOp` node. The left child is `NullExp()` 
 *   since there are no additional class declarations to chain.
 * - If multiple classes are declared, chain them together using `ClassOp`:
 *   - The left child represents the previously processed class declarations.
 *   - The right child represents the next class declaration being added.
 * 
 * Example Syntax Tree Structure:
 * ClassOp
 * ├── ClassOp (nested node for the first class)
 * │   ├── NullExp()
 * │   └── ClassDefOp (class definition for `A`)
 * │       ├── BodyOp (class body: fields, methods)
 * │       │   └── NullExp() (no fields or methods)
 * │       └── IDNode ("A") (class name)
 * └── ClassDefOp (class definition for `B`)
 *     ├── BodyOp (class body: fields, methods)
 *     │   └── NullExp() (no fields or methods)
 *     └── IDNode ("B") (class name)
 */
ClassDecl_rec : ClassDecl
                { $$ = MakeTree(ClassOp, NullExp(), $1); }  // Single class declaration
              | ClassDecl_rec ClassDecl
                { $$ = MakeTree(ClassOp, $1, $2); }         // Multiple classes are chained together
              ;

/**
 * Grammar Rule: ClassDecl
 * 
 * Defines a single class declaration with a name and body.
 * 
 * Example:
 *      class MyClass { 
 *          int x;
 *          void print() { ... }
 *      }
 * 
 * Action:
 * - Creates a `ClassDefOp` node for the class.
 * - The left child represents the class body (`$3`), which contains fields and methods.
 * - The right child represents the class name as an `IDNode` with `$2` as the identifier.
 * 
 * Example Syntax Tree Structure:
 * ClassDefOp
 * ├── BodyOp (class body)
 * │   ├── DeclOp (field declaration: `int x;`)
 * │   │   ├── NullExp()
 * │   │   └── CommaOp
 * │   │       ├── IDNode ("x")
 * │   │       └── TypeIdOp → INTEGERTNode ("int")
 * │   └── MethodOp (method definition: `void print() { ... }`)
 * │       ├── HeadOp (method signature: `void print()`)
 * │       │   ├── IDNode ("print") (method name)
 * │       │   └── SpecOp (parameter list and return type)
 * │       │       ├── NullExp() (no parameters)
 * │       │       └── NullExp() (void return type)
 * │       └── BodyOp (method body)
 * │           └── StmtOp (e.g., `x = 5;`)
 * └── IDNode ("MyClass") (class name)
 */
ClassDecl : CLASSnum IDnum ClassBody
            { $$ = MakeTree(ClassDefOp, $3, MakeLeaf(IDNode, $2)); }
          ;

/**
 * Grammar Rule: ClassBody
 * 
 * Defines the body of a class, which may contain variable declarations and method definitions.
 * 
 * Example 1 (Only Methods):
 *      class Person {
 *          method void speak() { }
 *          method void walk() { }
 *      }
 * 
 * Example 2 (Fields Only):
 *      class Animal {
 *          int legs;
 *          string species;
 *      }
 * 
 * Example 3 (Fields and Methods):
 *      class Vehicle {
 *          int wheels;
 *          method void drive() { }
 *      }
 * 
 * Action:
 * - If the class contains only method declarations, return those methods (`$2`).
 * - If the class has both fields and methods, attach field declarations (`$2`) 
 *   as the left child of method declarations (`$3`).
 * 
 * Example Syntax Tree Structures:
 * 
 * 1. **Only Methods (`Person`):**
 *    ClassBody
 *    └── MethodOp (method definition for `speak`)
 *        ├── HeadOp (method signature: `void speak()`)
 *        │   ├── IDNode ("speak")
 *        │   └── SpecOp (parameter list and return type)
 *        │       ├── NullExp() (no parameters)
 *        │       └── NullExp() (void return type)
 *        └── BodyOp (method body)
 *            └── NullExp() (empty body)
 * 
 * 2. **Fields Only (`Animal`):**
 *    ClassBody
 *    └── DeclOp (field declaration for `legs`)
 *        ├── NullExp()
 *        └── CommaOp
 *            ├── IDNode ("legs")
 *            └── TypeIdOp → INTEGERTNode ("int")
 * 
 * 3. **Fields and Methods (`Vehicle`):**
 *    ClassBody
 *    ├── DeclOp (field declaration for `wheels`)
 *    │   ├── NullExp()
 *    │   └── CommaOp
 *    │       ├── IDNode ("wheels")
 *    │       └── TypeIdOp → INTEGERTNode ("int")
 *    └── MethodOp (method definition for `drive`)
 *        ├── HeadOp (method signature: `void drive()`)
 *        │   ├── IDNode ("drive")
 *        │   └── SpecOp (parameter list and return type)
 *        │       ├── NullExp() (no parameters)
 *        │       └── NullExp() (void return type)
 *        └── BodyOp (method body)
 *            └── NullExp() (empty body)
 */
ClassBody : LBRACEnum MethodDecl_z1 RBRACEnum
            { $$ = $2; }  // Only method declarations are present

          | LBRACEnum Decls MethodDecl_z1 RBRACEnum
            {  
                // If there are no method declarations, just return the declarations
                if (NodeKind($3) == DUMMYNode)  // Check if methods are empty
                    $$ = $2;                   // Only field declarations exist
                else
                    // Attach declarations ($2) as the left child of methods ($3)
                    $$ = MkLeftC($2, $3);
            }
          ;

/**
 * Grammar Rule: MethodDecl_z1
 * 
 * Handles the optional presence of method declarations in a class body.
 * 
 * Example 1 (No Methods):
 *      class Person { }
 * 
 * Syntax Tree for Example 1:
 * BodyOp
 * ├── NullExp() (No method declarations)
 * 
 * Example 2 (With Methods):
 *      class Person {
 *          method void speak() { }
 *      }
 * 
 * Syntax Tree for Example 2:
 * BodyOp
 * ├── MethodOp (method declaration: `void speak()`)
 * │   ├── HeadOp (method signature)
 * │   │   ├── IDNode ("speak") (method name)
 * │   │   └── SpecOp (parameter list and return type)
 * │   │       ├── NullExp() (no parameters)
 * │   │       └── NullExp() (void return type)
 * │   └── BodyOp (method body: statements)
 * │       └── NullExp() (empty method body)
 * 
 * Action:
 * - If no methods exist, return a **null expression** (`NullExp()`).
 * - If methods exist, delegate to **`MethodDecl_rec`** to construct the syntax tree
 *   for all method declarations.
 */
MethodDecl_z1 :  
    /* No methods present */    
    { $$ = NullExp(); }  

    | MethodDecl_rec
      { $$ = $1; }  // Pass the method declarations up the tree
    ;

/**
 * Grammar Rule: MethodDecl_rec
 * 
 * Handles **one or more** method declarations in a class body.
 * 
 * Example:
 *      method void speak() { }
 *      method void walk() { }
 * 
 * Action:
 * - If only one method exists, create a **`BodyOp`** node with the method as its right child.
 * - If multiple methods exist, chain them under a single **`BodyOp`** node.
 * - The left child of each `BodyOp` represents previously declared methods, while the right child
 *   represents the current method being added to the chain.
 * 
 * Example Syntax Tree Structure:
 * BodyOp
 * ├── BodyOp
 * │   ├── NullExp() (no previous method at the start of the chain)
 * │   └── MethodOp (method definition: `void speak() { }`)
 * └── MethodOp (method definition: `void walk() { }`)
 * 
 * Expanded Example:
 * For:
 *      method void speak() { }
 *      method void walk() { }
 *      method int jump() { return 1; }
 * 
 * The syntax tree:
 * BodyOp
 * ├── BodyOp
 * │   ├── BodyOp
 * │   │   ├── NullExp()
 * │   │   └── MethodOp (method definition: `void speak() { }`)
 * │   └── MethodOp (method definition: `void walk() { }`)
 * └── MethodOp (method definition: `int jump() { return 1; }`)
 */
MethodDecl_rec : 
    MethodDecl
      { $$ = MakeTree(BodyOp, NullExp(), $1); }  // Single method

    | MethodDecl_rec MethodDecl
      { $$ = MakeTree(BodyOp, $1, $2); }        // Chain multiple methods
    ;

/**
 * Grammar Rule: Decls
 * 
 * Handles **field declarations** within the `DECLARATIONS` block.
 * 
 * Example 1 (Empty Declarations):
 *      declarations
 *      enddeclarations
 * 
 * Example 2 (With Declarations):
 *      declarations
 *          int age;
 *          string name;
 *      enddeclarations
 * 
 * Action:
 * - If no fields exist, create an empty **`BodyOp`** node.
 *   - The left and right children of `BodyOp` are set to `NullExp()`.
 * - If field declarations are provided, process them through **`FieldDecl_rec`**
 *   and pass the resulting syntax tree node up the tree.
 * 
 * Example Syntax Tree Structure:
 * Decls (Example 1: Empty Declarations)
 * BodyOp
 * ├── NullExp()  // No declarations
 * └── NullExp()  // No declarations
 * 
 * Decls (Example 2: With Declarations)
 * BodyOp
 * ├── DeclOp (int age)
 * │   ├── NullExp()  // No initializer
 * │   └── CommaOp
 * │       ├── IDNode ("age")  // Field name
 * │       └── TypeIdOp → INTEGERTNode ("int")  // Field type
 * └── DeclOp (string name)
 *     ├── NullExp()  // No initializer
 *     └── CommaOp
 *         ├── IDNode ("name")  // Field name
 *         └── TypeIdOp → STRINGNode ("string")  // Field type
 */
Decls : 
    DECLARATIONSnum ENDDECLARATIONSnum
      { $$ = MakeTree(BodyOp, NullExp(), NullExp()); }  // Empty declarations

    | DECLARATIONSnum FieldDecl_rec ENDDECLARATIONSnum
      { $$ = $2; }  // Pass field declarations up the tree
    ;

/**
 * Grammar Rule: FieldDecl_rec
 * 
 * Handles **multiple field declarations** within a class or a declarations block.
 * 
 * Example 1 (Single Field Declaration):
 *      int age;
 * 
 * Example 2 (Multiple Field Declarations):
 *      int age;
 *      string name;
 * 
 * Action:
 * - If there is only one field declaration, create a **`BodyOp`** node with:
 *   - Left child as `NullExp()`.
 *   - Right child as the single field declaration node (`FieldDecl`).
 * - If multiple fields exist, chain them using **`BodyOp`** nodes:
 *   - The left child represents the previously parsed fields (`FieldDecl_rec`).
 *   - The right child represents the current field declaration (`FieldDecl`).
 * 
 * Example Syntax Tree Structure:
 * 
 * FieldDecl_rec (Example 1: Single Field Declaration)
 * BodyOp
 * ├── NullExp()           // No previous fields
 * └── DeclOp              // Field declaration
 *     ├── NullExp()       // No initializer
 *     └── CommaOp
 *         ├── IDNode ("age")          // Field name
 *         └── TypeIdOp → INTEGERTNode ("int")  // Field type
 * 
 * FieldDecl_rec (Example 2: Multiple Field Declarations)
 * BodyOp
 * ├── BodyOp              // First field
 * │   ├── NullExp()       // No previous fields
 * │   └── DeclOp
 * │       ├── NullExp()   // No initializer
 * │       └── CommaOp
 * │           ├── IDNode ("age")          // Field name
 * │           └── TypeIdOp → INTEGERTNode ("int")  // Field type
 * └── DeclOp              // Second field
 *     ├── NullExp()       // No initializer
 *     └── CommaOp
 *         ├── IDNode ("name")          // Field name
 *         └── TypeIdOp → STRINGNode ("string")  // Field type
 */
FieldDecl_rec : 
    FieldDecl
      { $$ = MakeTree(BodyOp, NullExp(), $1); }  // Single field

    | FieldDecl_rec FieldDecl
      { $$ = MakeTree(BodyOp, $1, $2); }        // Chain multiple fields
    ;

/**
 * Grammar Rule: FieldDecl
 * 
 * Handles a **single field declaration**, including its type and initialization.
 * 
 * Example 1 (Without Initialization):
 *      int age;
 * 
 * Example 2 (With Initialization):
 *      int age = 30;
 * 
 * Action:
 * - Stores the field's type (`Type`) in the global variable **`type_record`** for reuse.
 * - Delegates the actual variable processing and initialization to **`FieldDecl_body`**.
 * - The resulting node from **`FieldDecl_body`** is passed up the tree.
 * 
 * Example Syntax Tree Structure:
 * 
 * FieldDecl (Example 1: Without Initialization)
 * DeclOp
 * ├── NullExp()  // No initializer
 * └── CommaOp
 *     ├── IDNode ("age")  // Field name
 *     └── TypeIdOp → INTEGERTNode ("int")  // Field type
 * 
 * FieldDecl (Example 2: With Initialization)
 * DeclOp
 * ├── AssignOp
 * │   ├── IDNode ("age")  // Field name
 * │   └── ICONSTNode (30)  // Initializer value
 * └── CommaOp
 *     ├── TypeIdOp → INTEGERTNode ("int")  // Field type
 *     └── NullExp()
 */
FieldDecl : 
    Type { type_record = $1; } FieldDecl_body SEMInum
      { $$ = $3; }  // Pass the processed field declaration
    ;

/**
 * Grammar Rule: Tail
 * 
 * Handles variable declarations, with or without initialization.
 * 
 * Example 1 (Without Initialization):
 *      int x;
 * 
 * Example 2 (With Initialization):
 *      int x = 10;
 * 
 * Action:
 * - If there’s no initializer:
 *   - Create a **`CommaOp`** node linking the variable name to the type information.
 * - If an initializer is provided:
 *   - Attach the initializer to the **`CommaOp`** node to represent the full declaration.
 * 
 * Example Syntax Tree Structures:
 * 
 * **Without Initialization:**
 * CommaOp
 * ├── IDNode ("x")  // Variable name
 * └── CommaOp
 *     ├── TypeIdOp → INTEGERTNode ("int")  // Variable type
 *     └── NullExp()  // No initializer
 * 
 * **With Initialization:**
 * CommaOp
 * ├── IDNode ("x")  // Variable name
 * └── CommaOp
 *     ├── TypeIdOp → INTEGERTNode ("int")  // Variable type
 *     └── ICONSTNode (10)  // Initializer value
 */
Tail : 
    VariableDeclId
      { 
          tree commat;
          commat = MakeTree(CommaOp, type_record, NullExp());  // Link type with variable
          $$ = MakeTree(CommaOp, $1, commat);                 // Attach variable to type
      }

    | VariableDeclId EQUALnum VariableInitializer
      {
          tree commat;
          commat = MakeTree(CommaOp, type_record, $3);  // Link type with initializer
          $$ = MakeTree(CommaOp, $1, commat);          // Attach variable to initializer
      }
    ;

/**
 * Grammar Rule: FieldDecl_body
 * 
 * Handles the body of a field declaration, allowing for **single** or **multiple** variables
 * to be declared in one statement.
 * 
 * Example 1 (Single Declaration):
 *      int x;
 * 
 * Example 2 (Multiple Declarations):
 *      int x, y = 5, z;
 * 
 * Action:
 * - For a **single variable**, it creates a `DeclOp` node to represent the declaration.
 * - For **multiple variables**, it chains them together using `DeclOp` nodes.
 * 
 * Example Syntax Tree Structures:
 * 
 * **Single Variable Declaration:**
 * DeclOp
 * ├── NullExp()  // Left child (no previous declarations)
 * └── CommaOp
 *     ├── IDNode ("x")  // Variable name
 *     └── CommaOp
 *         ├── TypeIdOp → INTEGERTNode ("int")  // Variable type
 *         └── NullExp()  // No initializer
 * 
 * **Multiple Variable Declarations:**
 * DeclOp
 * ├── DeclOp
 * │   ├── DeclOp
 * │   │   ├── NullExp()  // No previous declarations
 * │   │   └── CommaOp
 * │   │       ├── IDNode ("x")  // First variable name
 * │   │       └── CommaOp
 * │   │           ├── TypeIdOp → INTEGERTNode ("int")  // Type of first variable
 * │   │           └── NullExp()  // No initializer
 * │   └── CommaOp
 * │       ├── IDNode ("y")  // Second variable name
 * │       └── CommaOp
 * │           ├── TypeIdOp → INTEGERTNode ("int")  // Type of second variable
 * │           └── ICONSTNode (5)  // Initializer for `y`
 * └── CommaOp
 *     ├── IDNode ("z")  // Third variable name
 *     └── CommaOp
 *         ├── TypeIdOp → INTEGERTNode ("int")  // Type of third variable
 *         └── NullExp()  // No initializer for `z`
 */
FieldDecl_body : 
    Tail
    /* Case 1: Single Variable Declaration */
    {  
        // Create a DeclOp node with only one variable (`$1` from Tail)
        // The left child is `NullExp()` (empty), and the right child is the variable.
        $$ = MakeTree(DeclOp, NullExp(), $1);  
    }

  | FieldDecl_body COMMAnum Tail
    /* Case 2: Multiple Variable Declarations (chained with commas) */
    {  
        // Combine multiple variable declarations into a single DeclOp tree.
        // The left child (`$1`) holds previously parsed variables.
        // The right child (`$3`) adds the newly parsed variable from Tail.
        $$ = MakeTree(DeclOp, $1, $3);  
    }
;

/**
 * Grammar Rule: VariableDeclId
 * 
 * Handles **variable identifiers** in declarations, supporting both **simple variables** 
 * and **array declarations** (e.g., `x`, `arr[]`).
 * 
 * Example 1 (Simple Variable):
 *      int x;
 * 
 * Example 2 (Array Declaration):
 *      int arr[];
 * 
 * Action:
 * - For a **simple identifier** (`IDnum`), creates a **leaf node** with `IDNode`.
 * - For an **array identifier** (`IDnum` followed by brackets), still creates a **leaf node**.
 *   The array brackets (`[]`) are handled elsewhere and do not modify the node here.
 * 
 * Example Syntax Tree Structures:
 * 
 * **Simple Variable:**
 * CommaOp
 * ├── IDNode ("x")  // Variable name
 * └── CommaOp
 *     ├── TypeIdOp → INTEGERTNode ("int")  // Type information
 *     └── NullExp()  // No initializer
 * 
 * **Array Declaration:**
 * CommaOp
 * ├── IDNode ("arr")  // Array name
 * └── CommaOp
 *     ├── TypeIdOp → INTEGERTNode ("int")  // Type information
 *     └── NullExp()  // Array bounds and dimensions handled elsewhere
 */
VariableDeclId : 
      IDnum
      {  
          // Create a leaf node for the identifier (simple variable).
          $$ = MakeLeaf(IDNode, $1);  
      }

    | IDnum Bracket_rec1
      {  
          // Create a leaf node for the identifier, even with array brackets.
          // Array brackets are parsed but not attached here.
          $$ = MakeLeaf(IDNode, $1);  
      }
;

/**
 * Grammar Rule: Bracket_rec1
 * 
 * Handles **array bracket notation** for declaring arrays (e.g., `arr[]`, `matrix[][]`).
 * 
 * Example 1 (Single-Dimensional Array):
 *      int arr[];
 * 
 * Example 2 (Multi-Dimensional Array):
 *      int matrix[][][];
 * 
 * Action:
 * - This rule **counts array dimensions** but does not attach them to the syntax tree.
 * - Returns **`NullExp()`** as a placeholder, meaning no syntax tree nodes are created here.
 * - The actual array dimensions are stored in the **symbol table** during semantic analysis.
 * 
 * Syntax Tree Context:
 * - The syntax tree for array declarations focuses on the variable name and type.
 * - Brackets (`[]`) influence **attributes** like dimensions but are not represented in the syntax tree.
 * 
 * Example Syntax Tree Structures:
 * 
 * **Single-Dimensional Array:**
 * DeclOp
 * ├── NullExp()
 * └── CommaOp
 *     ├── IDNode ("arr")          // Array name
 *     └── CommaOp
 *         ├── TypeIdOp → INTEGERTNode ("int")  // Type information
 *         └── NullExp()           // No initializer
 * 
 * **Multi-Dimensional Array:**
 * DeclOp
 * ├── NullExp()
 * └── CommaOp
 *     ├── IDNode ("matrix")       // Array name
 *     └── CommaOp
 *         ├── TypeIdOp → INTEGERTNode ("int")  // Type information
 *         └── NullExp()           // No initializer
 * 
 * Note:
 * - The **dimensions** (`[]`, `[][]`) are tracked in attributes (e.g., `DIMEN_ATTR`) 
 *   and resolved during semantic analysis.
 */
Bracket_rec1 : 
      LBRACnum RBRACnum
      { 
          // Single pair of brackets: `[]`
          $$ = NullExp();  
      }

    | Bracket_rec1 LBRACnum RBRACnum
      { 
          // Multiple pairs of brackets: `[][]...`
          $$ = NullExp();  
      }
;

/**
 * Grammar Rule: VariableInitializer
 * 
 * Handles **variable initialization** during declarations. This supports:
 * - Simple expressions (`x = 5`)
 * - Array initialization (`x = {1, 2, 3}`)
 * - Array creation (`x = new int[5]`)
 * 
 * Example 1 (Expression Initialization):
 *      int x = 5 + 3;
 * 
 * Example 2 (Array Initialization):
 *      int arr[] = {1, 2, 3};
 * 
 * Example 3 (Array Creation):
 *      int arr[] = new int[5];
 * 
 * Action:
 * - If initialized with a **normal expression**, return the expression node.
 * - If initialized with an **array initializer** (`{}`), return the array initializer node.
 * - If initialized with an **array creation expression** (`new`), return that node.
 * 
 * Syntax Tree Context:
 * - This rule determines the right child of the `CommaOp` node in the declaration tree.
 * - Each initialization type (`Expression`, `ArrayInitializer`, `ArrayCreationExpression`) corresponds to a subtree.
 * 
 * Example Syntax Tree Structures:
 * 
 * **1. Expression Initialization:**
 * DeclOp
 * ├── NullExp()                      // No preceding variables
 * └── CommaOp
 *     ├── IDNode ("x")               // Variable name
 *     └── CommaOp
 *         ├── TypeIdOp → INTEGERTNode ("int")  // Variable type
 *         └── AddOp                  // Expression
 *             ├── NUMNode (5)
 *             └── NUMNode (3)
 * 
 * **2. Array Initialization:**
 * DeclOp
 * ├── NullExp()
 * └── CommaOp
 *     ├── IDNode ("arr")
 *     └── CommaOp
 *         ├── TypeIdOp → INTEGERTNode ("int")  // Array type
 *         └── ArrayInitializer                 // Array literal
 *             ├── NUMNode (1)
 *             ├── NUMNode (2)
 *             └── NUMNode (3)
 * 
 * **3. Array Creation:**
 * DeclOp
 * ├── NullExp()
 * └── CommaOp
 *     ├── IDNode ("arr")
 *     └── CommaOp
 *         ├── TypeIdOp → INTEGERTNode ("int")  // Array type
 *         └── ArrayCreationExpression
 *             └── NUMNode (5)                  // Array size
 * 
 */
VariableInitializer : 
      Expression
      {  
          // Standard initialization, e.g., `x = 5 + 3;`
          $$ = $1;  
      }

    | ArrayInitializer
      {  
          // Array literal initialization, e.g., `x = {1, 2, 3};`
          $$ = $1;  
      }

    | ArrayCreationExpression
      {  
          // Dynamic array creation, e.g., `x = new int[5];`
          $$ = $1;  
      }
;

/**
 * Grammar Rule: ArrayInitializer
 * 
 * Handles array initialization using **curly braces** `{}` for explicit values.
 * 
 * Example 1 (Single-Dimensional Array):
 *      int arr[] = {1, 2, 3};
 * 
 * Example 2 (Multi-Dimensional Array):
 *      int matrix[][] = {{1, 2}, {3, 4}};
 * 
 * Action:
 * - The rule creates an **`ArrayTypeOp`** node to represent the array initialization.
 * - **`$2`** holds the parsed array elements from **`ArrayInitializer_body`**.
 * - **`type_record`** is used to ensure the type consistency of the array (e.g., `int`).
 * 
 * Syntax Tree Context:
 * - The `ArrayTypeOp` node serves as the **right child** of the `CommaOp` in the declaration tree.
 * - Each dimension or nested array creates a subtree under `ArrayTypeOp`.
 * 
 * Example Syntax Tree Structures:
 * 
 * **1. Single-Dimensional Array:**
 * DeclOp
 * ├── NullExp()
 * └── CommaOp
 *     ├── IDNode ("arr")                 // Variable name
 *     └── CommaOp
 *         ├── TypeIdOp → INTEGERTNode    // Variable type
 *         └── ArrayTypeOp                // Array initialization
 *             ├── NUMNode (1)
 *             ├── NUMNode (2)
 *             └── NUMNode (3)
 * 
 * **2. Multi-Dimensional Array:**
 * DeclOp
 * ├── NullExp()
 * └── CommaOp
 *     ├── IDNode ("matrix")              // Variable name
 *     └── CommaOp
 *         ├── TypeIdOp → INTEGERTNode    // Variable type
 *         └── ArrayTypeOp
 *             ├── ArrayTypeOp            // First dimension
 *             │   ├── NUMNode (1)
 *             │   └── NUMNode (2)
 *             └── ArrayTypeOp            // Second dimension
 *                 ├── NUMNode (3)
 *                 └── NUMNode (4)
 * 
 */
ArrayInitializer : 
      LBRACEnum ArrayInitializer_body RBRACEnum
      {  
          // Create an ArrayTypeOp node linking the array elements to the type.
          $$ = MakeTree(ArrayTypeOp, $2, type_record);  
      }
;

/**
 * Grammar Rule: ArrayInitializer_body
 * 
 * Handles the **contents** of the array initializer, allowing for multiple comma-separated elements.
 * 
 * Example 1 (Simple Array):
 *      int arr[] = {1, 2, 3};
 * 
 * Example 2 (Nested Array):
 *      int matrix[][] = {{1, 2}, {3, 4}};
 * 
 * Action:
 * - Wraps each **array element** (`VariableInitializer`) in a **`CommaOp`** node.
 * - Chains multiple elements using recursive **`CommaOp`** nodes.
 * 
 * Syntax Tree Context:
 * - This rule builds the subtree for the **`ArrayInitializer`**, which serves as the right child of the `CommaOp` node 
 *   in the variable declaration tree.
 * - Each element is represented by its own subtree (e.g., expressions or nested initializers).
 * - Multiple elements are combined into a recursive **`CommaOp`** chain.
 * 
 * Example Syntax Tree Structures:
 * 
 * **1. Simple Array:**
 * DeclOp
 * ├── NullExp()
 * └── CommaOp
 *     ├── IDNode ("arr")                   // Variable name
 *     └── CommaOp
 *         ├── TypeIdOp → INTEGERTNode      // Variable type
 *         └── ArrayInitializer             // Array literal
 *             └── CommaOp
 *                 ├── NUMNode (1)          // First element
 *                 └── CommaOp
 *                     ├── NUMNode (2)      // Second element
 *                     └── NUMNode (3)      // Third element
 * 
 * **2. Nested Array:**
 * DeclOp
 * ├── NullExp()
 * └── CommaOp
 *     ├── IDNode ("matrix")
 *     └── CommaOp
 *         ├── TypeIdOp → INTEGERTNode      // Variable type
 *         └── ArrayInitializer
 *             └── CommaOp
 *                 ├── ArrayInitializer     // First row
 *                 │   └── CommaOp
 *                 │       ├── NUMNode (1)  // Row 1, Element 1
 *                 │       └── NUMNode (2)  // Row 1, Element 2
 *                 └── ArrayInitializer     // Second row
 *                     └── CommaOp
 *                         ├── NUMNode (3)  // Row 2, Element 1
 *                         └── NUMNode (4)  // Row 2, Element 2
 */
ArrayInitializer_body : 
      VariableInitializer
      {  
          // Wrap a single element in a CommaOp node.
          $$ = MakeTree(CommaOp, NullExp(), $1);  
      }

    | ArrayInitializer_body COMMAnum VariableInitializer
      {  
          // Chain multiple array elements using CommaOp.
          $$ = MakeTree(CommaOp, $1, $3);  
      }
;

/**
 * Grammar Rule: ArrayCreationExpression
 * 
 * Handles the **dynamic creation** of arrays using the `new` keyword and size expressions.
 * 
 * Example 1 (Single-Dimensional Array Creation):
 *      new int[5]
 * 
 * Example 2 (Multi-Dimensional Array Creation):
 *      new int[3][4]
 * 
 * Action:
 * - Creates an **`ArrayTypeOp`** node.
 * - **`$2`** represents the dimension sizes, parsed by `ArrayCreationExpression_tail`.
 * - **`$1`** holds the array's base type (`int`), represented as an **`INTEGERTNode`**.
 * - The syntax tree links the type to its dimensions, forming a structured representation of the array.
 * 
 * Syntax Tree Context:
 * - This rule constructs the right child of a `CommaOp` in declarations where dynamic array creation occurs.
 * - The **`ArrayTypeOp`** serves as the parent node, linking type and dimensions.
 * 
 * Example Syntax Tree Structures:
 * 
 * **1. Single-Dimensional Array Creation:**
 * DeclOp
 * ├── NullExp()                              // No preceding variables
 * └── CommaOp
 *     ├── IDNode ("arr")                     // Array name
 *     └── CommaOp
 *         ├── TypeIdOp → INTEGERTNode ("int")  // Base type
 *         └── ArrayTypeOp                     // Array creation
 *             └── NUMNode (5)                // Array size
 * 
 * **2. Multi-Dimensional Array Creation:**
 * DeclOp
 * ├── NullExp()
 * └── CommaOp
 *     ├── IDNode ("matrix")
 *     └── CommaOp
 *         ├── TypeIdOp → INTEGERTNode ("int")
 *         └── ArrayTypeOp
 *             ├── NUMNode (3)                // Outer dimension
 *             └── NUMNode (4)                // Inner dimension
 * 
 * Detailed Breakdown:
 * - `ArrayTypeOp` is the parent node, encapsulating the dimensions and base type.
 * - Each dimension is represented as a `NUMNode` in the syntax tree, parsed from `ArrayCreationExpression_tail`.
 * 
 * Rule Definition:
 */
ArrayCreationExpression : 
      INTnum ArrayCreationExpression_tail
      {  
          // Create an ArrayTypeOp node linking the type and dimensions.
          $$ = MakeTree(ArrayTypeOp, $2, MakeLeaf(INTEGERTNode, $1));  
      }
;

/**
 * Grammar Rule: ArrayCreationExpression_tail
 * 
 * Handles the **dimension sizes** in array creation expressions, such as `[5]`, `[3][4]`.
 * This rule allows arrays of **any dimension** to be created.
 * 
 * Example 1 (Single-Dimensional Array):
 *      int[5]
 * 
 * Example 2 (Multi-Dimensional Array):
 *      int[3][4]
 * 
 * Action:
 * - For a **single dimension**, it creates a **`BoundOp`** node to represent the size.
 * - For **multi-dimensional arrays**, it **recursively** chains the sizes using **`BoundOp`** nodes.
 * 
 * Syntax Tree Context:
 * - This rule builds the right subtree of an `ArrayTypeOp` node.
 * - Each dimension adds a new `BoundOp` node to the tree, with the sizes represented as expression nodes.
 * 
 * Example Syntax Tree Structures:
 * 
 * **1. Single-Dimensional Array:**
 * ArrayTypeOp
 * ├── TypeIdOp → INTEGERTNode ("int")  // Base type
 * └── BoundOp                          // Array size
 *     ├── NullExp()                   // No previous dimensions
 *     └── NUMNode (5)                 // Dimension size
 * 
 * **2. Multi-Dimensional Array:**
 * ArrayTypeOp
 * ├── TypeIdOp → INTEGERTNode ("int")  // Base type
 * └── BoundOp                          // Outer dimension
 *     ├── BoundOp                      // Inner dimension
 *     │   ├── NullExp()               // No more previous dimensions
 *     │   └── NUMNode (3)             // Inner dimension size
 *     └── NUMNode (4)                 // Outer dimension size
 * 
 * **Key Points:**
 * - Each `BoundOp` node represents a dimension in the array, with sizes as its right child.
 * - Multi-dimensional arrays chain `BoundOp` nodes to represent multiple dimensions.
 */
ArrayCreationExpression_tail : 
      LBRACnum Expression RBRACnum
      {  
          // Single dimension: Create a BoundOp node for `[size]`
          $$ = MakeTree(BoundOp, NullExp(), $2);  // $2 holds the size expression
      }

    | ArrayCreationExpression_tail LBRACnum Expression RBRACnum
      {  
          // Multiple dimensions: Recursively create BoundOp nodes for `[size][size]...`
          $$ = MakeTree(BoundOp, $1, $3);  // $1 holds previous dimensions, $3 is the new size
      }
;

/**
 * Grammar Rule: MethodDecl
 * 
 * Handles **method declarations** with or without a return type.
 * 
 * Example 1 (Method with a Return Type):
 *      method int sum(int a; int b) { ... }
 * 
 * Example 2 (Void Method):
 *      method void print() { ... }
 * 
 * Action:
 * - For a **typed method**:
 *   - Stores the return type in `type_method` for later use.
 * - For a **void method**:
 *   - Sets `type_method` to `NullExp()`, representing no return type.
 * - Constructs a **`MethodOp`** node:
 *   - **Left child**: `HeadOp` node containing the method name and formal parameters.
 *   - **Right child**: Block representing the method's body.
 * 
 * Syntax Tree Context:
 * - `MethodOp` serves as the root node for a method declaration.
 * - The `HeadOp` node contains the method's signature, including its name and parameters.
 * - The block representing the method body is attached as the right child.
 * 
 * Example Syntax Tree Structures:
 * 
 * **1. Method with a Return Type:**
 * 
 * MethodOp
 * ├── HeadOp                           // Method signature
 * │   ├── IDNode ("sum")               // Method name
 * │   └── FormalParameterList          // Parameters
 * │       ├── RArgTypeOp               // First parameter
 * │       │   ├── IDNode ("a")         // Parameter name
 * │       │   └── TypeIdOp → INTNode   // Parameter type
 * │       └── RArgTypeOp               // Second parameter
 * │           ├── IDNode ("b")
 * │           └── TypeIdOp → INTNode
 * └── Block                            // Method body
 *     ├── StmtOp                       // Statements inside the block
 *     │   └── ...
 *     └── ...
 * 
 * **2. Void Method:**
 * 
 * MethodOp
 * ├── HeadOp                           // Method signature
 * │   ├── IDNode ("print")             // Method name
 * │   └── NullExp()                    // No parameters
 * └── Block                            // Method body
 *     ├── StmtOp                       // Statements inside the block
 *     │   └── RoutineCallOp            // Routine call inside the body
 *     │       ├── IDNode ("print")     // Called routine
 *     │       └── ...
 *     └── ...
 */
MethodDecl :    
      METHODnum Type { type_method = $2; } IDnum LPARENnum FormalParameterList_z1 RPARENnum Block
      {  
          // Create the method head with the method name and parameters
          tree head = MakeTree(HeadOp, MakeLeaf(IDNode, $4), $6);  
          
          // Create the MethodOp node with the head and body of the method
          $$ = MakeTree(MethodOp, head, $8);  
      }

    | METHODnum VOIDnum { type_method = NullExp(); } IDnum LPARENnum FormalParameterList_z1 RPARENnum Block
      {  
          // Create the method head for a void method
          tree head = MakeTree(HeadOp, MakeLeaf(IDNode, $4), $6);  

          // Create the MethodOp node with the head and body of the void method
          $$ = MakeTree(MethodOp, head, $8);  
      }
;

/**
 * Grammar Rule: FormalParameterList_z1
 * 
 * Handles **optional parameter lists** in method declarations.
 * 
 * Example 1 (No Parameters):
 *      method int sum() { ... }
 * 
 * Example 2 (With Parameters):
 *      method int sum(int a, int b) { ... }
 * 
 * Action:
 * - If **no parameters** are provided, create a `SpecOp` node with only the return type.
 * - If parameters **exist**, attach the parameter list as the left child of `SpecOp`.
 * 
 * Syntax Tree Context:
 * - This rule produces the **`SpecOp` node** in the syntax tree, which specifies:
 *   - **Left Child**: The list of parameters (`FormalParameterList`) or `NullExp()` if no parameters.
 *   - **Right Child**: The method's return type (`type_method`).
 * 
 * Example Syntax Tree Structures:
 * 
 * **1. No Parameters:**
 * MethodOp
 * ├── HeadOp
 * │   ├── IDNode ("sum")                 // Method name
 * │   └── SpecOp                        // Method signature
 * │       ├── NullExp()                 // No parameters
 * │       └── TypeIdOp → INTEGERTNode   // Return type: int
 * └── BodyOp                            // Method body
 * 
 * **2. With Parameters:**
 * MethodOp
 * ├── HeadOp
 * │   ├── IDNode ("sum")                 // Method name
 * │   └── SpecOp                        // Method signature
 * │       ├── CommaOp                   // Parameter list
 * │       │   ├── VArgTypeOp            // First parameter
 * │       │   │   ├── IDNode ("a")      // Parameter name
 * │       │   │   └── TypeIdOp → INTEGERTNode  // Parameter type: int
 * │       │   └── VArgTypeOp            // Second parameter
 * │       │       ├── IDNode ("b")
 * │       │       └── TypeIdOp → INTEGERTNode
 * │       └── TypeIdOp → INTEGERTNode   // Return type: int
 * └── BodyOp                            // Method body
 * 
 * Explanation of Syntax Tree:
 * - For **no parameters**, the `SpecOp` node has a `NullExp()` as its left child.
 * - For **parameterized methods**, the `SpecOp` node includes a `CommaOp` subtree that lists all parameters.
 * - In both cases, the right child of `SpecOp` holds the return type.
 * 
 */
FormalParameterList_z1 : 
      /* No parameters provided */
      {  
          // Create a SpecOp node with only the return type (`type_method`)
          $$ = MakeTree(SpecOp, NullExp(), type_method);  
      }

    | FormalParameterList
      {  
          // Create a SpecOp node with the parameter list and return type
          $$ = MakeTree(SpecOp, $1, type_method);  
      }
;

/**
 * Grammar Rule: FormalParameterList
 * 
 * Handles a **list of formal parameters** for a method. 
 * This rule parses and structures parameter lists, ensuring they are properly 
 * organized in the syntax tree for further semantic analysis or code generation.
 * 
 * Example 1 (Single Parameter):
 *      method int sum(int a) { ... }
 * 
 * Example 2 (Multiple Parameters):
 *      method int sum(int a, int b) { ... }
 * 
 * Action:
 * - If there's a **single parameter**, the rule directly returns it without modification.
 * - If there are **multiple parameters**, they are chained together using `MkRightC`.
 * 
 * Syntax Tree Context:
 * - This rule constructs the left child of the `SpecOp` node in the syntax tree.
 * - Each parameter is represented as a `VArgTypeOp` (value argument) or `RArgTypeOp` 
 *   (reference argument) node.
 * - Parameters are connected in a linked structure using `CommaOp`.
 * 
 * Example Syntax Tree Structures:
 * 
 * **1. Single Parameter:**
 * SpecOp
 * ├── VArgTypeOp                    // Parameter
 * │   ├── IDNode ("a")              // Parameter name
 * │   └── TypeIdOp → INTEGERTNode   // Parameter type
 * └── TypeIdOp → INTEGERTNode       // Return type
 * 
 * **2. Multiple Parameters:**
 * SpecOp
 * ├── CommaOp                       // Parameter list
 * │   ├── VArgTypeOp                // First parameter
 * │   │   ├── IDNode ("a")          // Parameter name
 * │   │   └── TypeIdOp → INTEGERTNode  // Parameter type
 * │   └── VArgTypeOp                // Second parameter
 * │       ├── IDNode ("b")
 * │       └── TypeIdOp → INTEGERTNode
 * └── TypeIdOp → INTEGERTNode       // Return type
 * 
 * Explanation of Syntax Tree:
 * - For a **single parameter**, no additional `CommaOp` nodes are created; the parameter node is passed up as-is.
 * - For **multiple parameters**, they are chained with `CommaOp`, forming a tree structure where:
 *   - The left child of `CommaOp` holds the current parameter.
 *   - The right child connects subsequent parameters or `NullExp()`.
 * 
 */
FormalParameterList : 
      FormalParameterList_rec
      {  
          // Single parameter: pass it up directly
          $$ = $1;  
      }

    | FormalParameterList SEMInum FormalParameterList_rec
      {  
          // Multiple parameters: chain them with MkRightC
          $$ = MkRightC($3, $1);  
      }
;

/**
 * Grammar Rule: FormalParameterList_rec
 * 
 * Handles **formal parameter declarations** within a method. Supports both:
 * 1. **Regular parameters** (e.g., `int a`) passed by reference.
 * 2. **Value parameters** (e.g., `val int a`) passed by value.
 * 
 * Example 1 (Regular parameter):
 *      method int sum(int a) { ... }
 * 
 * Example 2 (Value parameter):
 *      method int sum(val int a) { ... }
 * 
 * Action:
 * - For regular parameters, creates a **`RArgTypeOp`** node.
 * - For value parameters, creates a **`VArgTypeOp`** node.
 * - Attaches the parameter name (`IDENTIFIER_rec`) and type to the syntax tree.
 * - Ensures the argument nodes are correctly linked for chaining in a parameter list.
 * 
 * Syntax Tree Context:
 * - Parameters are represented as `RArgTypeOp` or `VArgTypeOp` nodes.
 * - Each parameter node includes the parameter name (`IDNode`) and type (`TypeIdOp`).
 * - Multiple parameters are chained using the `CommaOp` node (handled in parent rules).
 * 
 * Example Syntax Tree:
 * 
 * **1. Single Regular Parameter:**
 * RArgTypeOp
 * ├── IDNode ("a")            // Parameter name
 * └── TypeIdOp → INTEGERTNode // Parameter type
 * 
 * **2. Single Value Parameter:**
 * VArgTypeOp
 * ├── IDNode ("a")            // Parameter name
 * └── TypeIdOp → INTEGERTNode // Parameter type
 * 
 * **3. Multiple Parameters:**
 * CommaOp
 * ├── RArgTypeOp              // First parameter (regular)
 * │   ├── IDNode ("a")
 * │   └── TypeIdOp → INTEGERTNode
 * └── VArgTypeOp              // Second parameter (value)
 *     ├── IDNode ("b")
 *     └── TypeIdOp → INTEGERTNode
 * 
 */
FormalParameterList_rec : 
      INTnum
      /* Case 1: Regular parameter (e.g., `int a`) */
      {  
          // Create a node for a regular (reference) argument type.
          argtype = MakeTree(RArgTypeOp, NullExp(), NullExp());  

          // Initialize the regular argument node.
          $$ = MakeTree(RArgTypeOp, NullExp(), argtype);  
      }

    IDENTIFIER_rec
      /* Attach the parameter identifier to the regular argument node */
      {  
          SetLeftChild($2, $3);  // Attach parameter identifier (`$3`) to the node.

          if (NodeKind(LeftChild(RightChild($2))) == DUMMYNode) {
              // If no additional parameters, set right child to `NullExp`.
              SetRightChild($2, NullExp());
          } else {
              // Traverse to the last argument and clean up dangling nodes.
              tree t = $2;
              while (t->RightC != argtype)
                  t = t->RightC;

              t->RightC = NullExp();  // Clear dangling nodes.
              free(argtype);          // Free temporary node memory.
          }

          $$ = $2;  // Return the constructed regular argument node.
      }

    | VALnum INTnum
      /* Case 2: Value parameter (e.g., `val int a`) */
      {  
          // Create a node for a value argument type.
          argtype = MakeTree(VArgTypeOp, NullExp(), NullExp());  

          // Initialize the value argument node.
          $$ = MakeTree(VArgTypeOp, NullExp(), argtype);  
      }

    IDENTIFIER_rec
      /* Attach the parameter identifier to the value argument node */
      {  
          SetLeftChild($3, $4);  // Attach parameter identifier (`$4`) to the node.

          if (NodeKind(LeftChild(RightChild($3))) == DUMMYNode) {
              // If no additional parameters, set right child to `NullExp`.
              SetRightChild($3, NullExp());
          } else {
              // Traverse to the last argument and clean up dangling nodes.
              tree t = $3;
              while (t->RightC != argtype)
                  t = t->RightC;

              t->RightC = NullExp();  // Clear dangling nodes.
              free(argtype);          // Free temporary node memory.
          }

          $$ = $3;  // Return the constructed value argument node.
      }
;

/**
 * Grammar Rule: IDENTIFIER_rec
 * 
 * Handles **one or more identifiers** in parameter declarations.  
 * Supports both **single** and **multiple identifiers** separated by commas.
 * 
 * Example 1 (Single Parameter):
 *      int a
 * 
 * Example 2 (Multiple Parameters):
 *      int a, b, c
 * 
 * Action:
 * - For a **single identifier**, creates a **`CommaOp`** node linking the identifier with a default integer value (`0`).
 * - For **multiple identifiers**, recursively chains them using **`CommaOp`**, extending the tree structure.
 * 
 * Syntax Tree Context:
 * - Each identifier is represented by an **`IDNode`**.
 * - The default value (`0`) is represented by an **`INTEGERTNode`**.
 * - Multiple identifiers are chained using **`CommaOp`** nodes.
 * 
 * Example Syntax Tree:
 * 
 * **1. Single Identifier (int a):**
 * CommaOp
 * ├── IDNode ("a")         // Identifier name
 * └── INTEGERTNode (0)     // Default value
 * 
 * **2. Multiple Identifiers (int a, b, c):**
 * CommaOp
 * ├── IDNode ("a")         // First identifier
 * └── CommaOp              // Chain for additional identifiers
 *     ├── IDNode ("b")     // Second identifier
 *     └── CommaOp
 *         ├── IDNode ("c") // Third identifier
 *         └── INTEGERTNode (0) // Default value for all
 * 
 */
IDENTIFIER_rec : 
      IDnum
      /* Case 1: Single Identifier (e.g., int a) */
      {  
          // Create a `CommaOp` node linking the identifier (`IDnum`) with a default value (`0`).
          $$ = MakeTree(CommaOp, MakeLeaf(IDNode, $1), MakeLeaf(INTEGERTNode, 0));  
      }

    | IDENTIFIER_rec COMMAnum IDnum
      /* Case 2: Multiple Identifiers (e.g., int a, b, c) */
      {  
          tree comma, temp;

          // Keep the existing chain of identifiers (e.g., 'a, b').
          $$ = $1;

          // Create a new `CommaOp` node for the newly parsed identifier (`IDnum`).
          // Left child: IDNode (identifier name).
          // Right child: INTEGERTNode (default value 0).
          comma = MakeTree(CommaOp, MakeLeaf(IDNode, $3), MakeLeaf(INTEGERTNode, 0));

          // Attach the new identifier (`comma`) as the leftmost child of `argtype`.
          MkLeftC(comma, argtype);

          // Create a temporary node to extend the argument type tree.
          temp = MakeTree(NodeOp(argtype), NullExp(), NullExp());

          // Set the temporary node as the right child of `argtype` to maintain the structure.
          SetRightChild(argtype, temp);

          // Update `argtype` to point to the newly created node for future additions.
          argtype = temp;
      }
;

/**
 * Grammar Rule: Block
 * 
 * Handles **blocks of code** enclosed in braces `{}`.  
 * A block can consist of **only statements** or **declarations followed by statements**.
 * 
 * Example 1 (Statements only):
 * ```c
 * {
 *     x = 5;
 *     print(x);
 * }
 * ```
 * 
 * Example 2 (Declarations and Statements):
 * ```c
 * {
 *     int a;
 *     a = 10;
 * }
 * ```
 * 
 * Action:
 * - Wraps the entire block in a **`BodyOp`** node to represent the block's body.
 * - If there are **only statements**, the left child is **empty** (`NullExp()`).
 * - If there are **declarations and statements**, both are attached as children.
 * 
 * Syntax Tree Context:
 * - The `BodyOp` node encapsulates the block.
 * - Left child: **Declarations** (`Decls`) or `NullExp()` if no declarations exist.
 * - Right child: **Statements** (`StatementList`).
 * 
 * Example Syntax Trees:
 * 
 * **1. Statements Only (e.g., `{ x = 5; print(x); }`):**
 * ```
 * BodyOp
 * ├── NullExp()           // No declarations
 * └── StmtOp              // StatementList (e.g., x = 5; print(x);)
 *     ├── AssignOp
 *     │   ├── VarOp (x)
 *     │   └── NUMNode (5)
 *     └── RoutineCallOp
 *         ├── VarOp (print)
 *         └── VarOp (x)
 * ```
 * 
 * **2. Declarations and Statements (e.g., `{ int a; a = 10; }`):**
 * ```
 * BodyOp
 * ├── DeclOp              // Declarations
 * │   ├── NullExp()       // Empty left child (start of declaration chain)
 * │   └── CommaOp
 * │       ├── IDNode ("a") // Variable `a`
 * │       └── INTEGERTNode (int) // Type
 * └── StmtOp              // StatementList (e.g., a = 10;)
 *     └── AssignOp
 *         ├── VarOp (a)
 *         └── NUMNode (10)
 * ```
 */
Block : 
      StatementList
      {  
          // Only statements are present, so create a BodyOp node.  
          // Left child: NullExp() (no declarations)  
          // Right child: StatementList ($1)
          $$ = MakeTree(BodyOp, NullExp(), $1);  
      }

    | Decls StatementList
      {  
          // Both declarations and statements are present.  
          // Left child: Decls ($1)  
          // Right child: StatementList ($2)
          $$ = MakeTree(BodyOp, $1, $2);  
      }
;

/**
 * Grammar Rule: Type
 * 
 * Handles **type declarations** for variables and arrays.  
 * Supports **simple types** (e.g., `int`) and **array types** (e.g., `int[]` or `int[][]`).  
 * Additionally, supports **nested types** with field access (e.g., `int[].fieldType`).
 * 
 * Example 1 (Simple type):
 * ```c
 * int a;
 * ```
 * 
 * Example 2 (Array type):
 * ```c
 * int arr[];
 * int matrix[][];
 * ```
 * 
 * Example 3 (Nested type):
 * ```c
 * int[].fieldType a;
 * ```
 * 
 * Action:
 * - **Simple type:** Creates a **`TypeIdOp`** node with the type.
 * - **Array type:** Links the type with **array brackets** using **`Bracket_rec2`**.
 * - **Nested types:** Handles complex types like **arrays of arrays** or types with field access.
 * 
 * Syntax Tree Context:
 * - The `TypeIdOp` node represents the type declaration.
 * - Left child: **Type name** (e.g., `int`).
 * - Right child: **Array brackets** (if present) or **nested types**.
 * 
 * Example Syntax Trees:
 * 
 * **1. Simple Type (e.g., `int a;`):**
 * ```
 * TypeIdOp
 * ├── INTEGERTNode ("int")  // Type name
 * └── NullExp()             // No additional brackets or nesting
 * ```
 * 
 * **2. Array Type (e.g., `int[][] matrix;`):**
 * ```
 * TypeIdOp
 * ├── INTEGERTNode ("int")  // Base type
 * └── IndexOp               // Array dimensions
 *     ├── NullExp()
 *     └── IndexOp
 *         ├── NullExp()
 *         └── NullExp()
 * ```
 * 
 * **3. Nested Type (e.g., `int[].fieldType a;`):**
 * ```
 * TypeIdOp
 * ├── INTEGERTNode ("int")      // Base type
 * └── TypeIdOp                 // Nested type
 *     ├── IndexOp              // Array dimensions
 *     │   ├── NullExp()
 *     │   └── NullExp()
 *     └── FieldOp              // Field access
 *         ├── IDNode ("fieldType")
 *         └── NullExp()
 * ```
 */
Type : 
      Type_front
      {  
          // Simple type declaration (e.g., `int`).  
          // Creates a TypeIdOp node with no array brackets.  
          $$ = MakeTree(TypeIdOp, $1, NullExp());  
      }

    | Type_front Bracket_rec2
      {  
          // Array type declaration (e.g., `int[]`).  
          // Creates a TypeIdOp node linking the type to its dimensions.  
          $$ = MakeTree(TypeIdOp, $1, $2);  
      }

    | Type_front Bracket_rec2 DOTnum Type
      {  
          // Complex/nested type (e.g., `int[].fieldType`).  
          // First, link the type and brackets.  
          tree temp = MakeTree(TypeIdOp, $1, $2);

          // Attach the more complex type on the right.  
          $$ = MkRightC($4, temp);  
      }
;

/**
 * Grammar Rule: Type_front
 * 
 * Handles the **base type** of a variable, which could be a **primitive type** like `int` 
 * or a **user-defined type** (identifier).
 * 
 * Example 1 (Primitive type):
 * ```c
 * int a;
 * ```
 * 
 * Example 2 (User-defined type):
 * ```c
 * Person p;
 * ```
 * 
 * Action:
 * - If the type is **`INTnum`**, create a **`INTEGERTNode`** to represent the primitive type.
 * - If the type is a **custom identifier**, create an **`IDNode`** to represent the user-defined type.
 * 
 * Syntax Tree Context:
 * - This rule creates a **leaf node** in the syntax tree to represent the type.
 * - For a **primitive type**, the node is an **`INTEGERTNode`**.
 * - For a **user-defined type**, the node is an **`IDNode`**.
 * 
 * Example Syntax Trees:
 * 
 * **1. Primitive Type (e.g., `int a;`):**
 * ```
 * INTEGERTNode ("int")
 * ```
 * 
 * **2. User-Defined Type (e.g., `Person p;`):**
 * ```
 * IDNode ("Person")
 * ```
 */
Type_front : 
      IDnum
      {  
          // User-defined type (e.g., `Person`).  
          $$ = MakeLeaf(IDNode, $1);  
      }

    | INTnum
      {  
          // Primitive type (e.g., `int`).  
          $$ = MakeLeaf(INTEGERTNode, $1);  
      }
;

/**
 * Grammar Rule: Bracket_rec2
 * 
 * Handles **array type dimensions** (e.g., `int[]`, `int[][]`) by recursively parsing
 * bracket pairs and constructing the correct array dimension structure.
 * 
 * Example 1 (Single Dimension Array):
 * ```c
 * int arr[];
 * ```
 * Example 2 (Multi-Dimensional Array):
 * ```c
 * int matrix[][][];
 * ```
 * 
 * Action:
 * - For a **single pair** of brackets `[]`, create an **`IndexOp`** node.
 * - For **multiple pairs** of brackets, recursively chain **`IndexOp`** nodes to represent
 *   the nested structure of dimensions.
 * 
 * Syntax Tree Context:
 * - Each `IndexOp` node represents a single pair of brackets.
 * - For multi-dimensional arrays, `IndexOp` nodes are recursively linked via the **right child**.
 * 
 * Example Syntax Trees:
 * 
 * **1. Single-Dimensional Array (e.g., `int arr[];`):**
 * ```
 * IndexOp
 * ├── NullExp()  // Placeholder for the left child
 * └── NullExp()  // Placeholder for the right child
 * ```
 * 
 * **2. Multi-Dimensional Array (e.g., `int matrix[][][];`):**
 * ```
 * IndexOp
 * ├── NullExp()
 * └── IndexOp
 *     ├── NullExp()
 *     └── IndexOp
 *         ├── NullExp()
 *         └── NullExp()
 * ```
 */
Bracket_rec2 : 
      LBRACnum RBRACnum
      {  
          // Single dimension: Create a single IndexOp node for `[]`.
          $$ = MakeTree(IndexOp, NullExp(), NullExp());  
      }

    | LBRACnum RBRACnum Bracket_rec2
      {  
          // Multiple dimensions: Recursively create nested IndexOp nodes.
          $$ = MakeTree(IndexOp, NullExp(), $3);  
      }
;

/**
 * Grammar Rule: StatementList
 * 
 * Handles a **block of statements** enclosed in `{}` braces. 
 * A statement list groups multiple statements into a single block, 
 * often used in control flow constructs (e.g., loops, conditionals) 
 * or method bodies.
 * 
 * Example:
 * 
 * **1. Empty Block:**
 *      {
 *      }
 * 
 * **2. Block with Statements:**
 *      {
 *          x = 5;         // Assignment
 *          print(x);      // Function call
 *      }
 * 
 * Action:
 * - Parses the statements within the block (`Statement_rec`) and directly passes
 *   the resulting subtree (`$2`) up the syntax tree for further processing.
 * 
 * Syntax Tree Context:
 * - The `Statement_rec` subtree represents the sequence of statements within the block.
 * - Each statement is linked via `StmtOp` or other operation nodes, forming a chain.
 * 
 * Example Syntax Tree:
 * 
 * **1. Single Statement Block:**
 * StmtOp
 * └── AssignOp
 *      ├── IDNode ("x")            // Variable being assigned
 *      └── NUMNode (5)             // Value being assigned
 * 
 * **2. Multi-Statement Block:**
 * StmtOp
 * ├── StmtOp
 * │   └── AssignOp
 * │       ├── IDNode ("x")
 * │       └── NUMNode (5)
 * └── RoutineCallOp
 *     ├── IDNode ("print")         // Function being called
 *     └── IDNode ("x")             // Argument passed
 */
StatementList : 
      LBRACEnum Statement_rec RBRACEnum
      {  
          // Pass the parsed statement block directly.
          $$ = $2;  
      }
;

/**
 * Grammar Rule: Statement_rec
 * 
 * Handles **one or more statements** within a block, separated by semicolons (`;`).
 * 
 * **Purpose**:
 * - To process and construct the syntax tree for one or more statements.
 * - Chains multiple statements using **`StmtOp`** nodes to represent sequential execution.
 * 
 * **Examples**:
 * 
 * **1. Single Statement:**
 *      x = 5;
 * 
 * **2. Multiple Statements:**
 *      x = 5;
 *      y = x + 2;
 * 
 * **Action**:
 * - For a **single statement**, it creates a `StmtOp` node, with the statement as the right child.
 * - For **multiple statements**, it chains them using `StmtOp`, where:
 *   - The left child represents the previously processed statements.
 *   - The right child represents the newly parsed statement.
 * - Ignores empty statements (e.g., multiple consecutive semicolons).
 * 
 * **Syntax Tree Context**:
 * - **Single Statement Block:**
 *   ```
 *   StmtOp
 *   └── AssignOp
 *        ├── IDNode ("x")     // Variable being assigned
 *        └── NUMNode (5)      // Value being assigned
 *   ```
 * 
 * - **Multiple Statement Block:**
 *   ```
 *   StmtOp
 *   ├── StmtOp
 *   │   └── AssignOp
 *   │       ├── IDNode ("x")
 *   │       └── NUMNode (5)
 *   └── AssignOp
 *        ├── IDNode ("y")
 *        └── AddOp
 *             ├── IDNode ("x")
 *             └── NUMNode (2)
 *   ```
 * 
 * **Grammar**:
 */
Statement_rec : 
      Statement
      {  
          // Case 1: Single statement - wrap it in a `StmtOp` node.
          $$ = MakeTree(StmtOp, NullExp(), $1);  
      }

    | Statement_rec SEMInum Statement
      {  
          // Case 2: Multiple statements - chain them using `StmtOp`.
          if (NodeKind($3) == DUMMYNode)
              // Ignore empty statements (e.g., `;;`).
              $$ = $1;  
          else
              // Create a `StmtOp` node to chain the current and new statement.
              $$ = MakeTree(StmtOp, $1, $3);  
      }
;

/**
 * Grammar Rule: Statement
 * 
 * **Purpose**:
 * - Processes all types of **statements** supported by the language. This includes:
 *   - Assignments
 *   - Method calls
 *   - Return statements
 *   - Conditional (`if`) statements
 *   - Loops (`while`)
 *   - Empty statements
 * 
 * **Examples**:
 * 
 * **1. Empty Statement:**
 *      ;
 *   - Represented as `NullExp()`.
 * 
 * **2. Assignment Statement:**
 *      x = 5;
 *   - Processed and passed as an assignment syntax tree.
 * 
 * **3. Method Call:**
 *      print(x);
 *   - Processed and passed as a method call syntax tree.
 * 
 * **4. Return Statement:**
 *      return x + 1;
 *   - Processed and passed as a return statement syntax tree.
 * 
 * **5. Conditional Statement (`if`):**
 *      if (x > 0) { ... } else { ... }
 *   - Processed and passed as a conditional syntax tree.
 * 
 * **6. Loop Statement (`while`):**
 *      while (x < 10) { x++; }
 *   - Processed and passed as a loop syntax tree.
 * 
 * **Action**:
 * - For **empty statements**, return `NullExp()` as a placeholder.
 * - For specific statement types (e.g., assignments, method calls), pass their syntax tree nodes directly.
 * 
 * **Syntax Tree Context**:
 * - **Assignment Statement**:
 *   ```
 *   AssignOp
 *   ├── IDNode ("x")     // Variable being assigned
 *   └── NUMNode (5)      // Value being assigned
 *   ```
 * 
 * - **Conditional Statement**:
 *   ```
 *   IfElseOp
 *   ├── (x > 0) - condition
 *   ├── BodyOp - if block
 *   └── BodyOp - else block
 *   ```
 * 
 * - **Loop Statement**:
 *   ```
 *   LoopOp
 *   ├── (x < 10) - condition
 *   └── BodyOp - loop body
 *   ```
 * 
 * **Grammar**:
 */
Statement : 
      /* Empty statement (e.g., just a semicolon `;`) */
      {  
          // Represents an empty statement as `NullExp()`
          $$ = NullExp();  
      }

    | AssignmentStatement
      {  
          // Pass the result of the parsed assignment statement
          $$ = $1;  
      }

    | MethodCallStatement
      {  
          // Pass the result of the parsed method call statement
          $$ = $1;  
      }

    | ReturnStatement
      {  
          // Pass the result of the parsed return statement
          $$ = $1;  
      }

    | IfStatement
      {  
          // Pass the result of the parsed if-else statement
          $$ = $1;  
      }

    | WhileStatement
      {  
          // Pass the result of the parsed while loop
          $$ = $1;  
      }
;

/**
 * Grammar Rule: AssignmentStatement
 * 
 * **Purpose**:
 * - Processes **variable assignment** statements, including:
 *   - Simple assignments
 *   - Indexed (array) assignments
 * 
 * **Examples**:
 * 
 * **1. Simple Assignment:**
 *      x = 5;
 *   - The variable `x` is assigned the value `5`.
 * 
 * **2. Indexed Assignment:**
 *      arr[2] = 10;
 *   - The array `arr` at index `2` is assigned the value `10`.
 * 
 * **Action**:
 * - Constructs an **`AssignOp`** node for the assignment operation.
 * - **Left Child**: Contains the variable being assigned, wrapped in an additional `AssignOp` for consistency.
 * - **Right Child**: Contains the expression or value being assigned.
 * 
 * **Syntax Tree Context**:
 * - **Simple Assignment**:
 *   ```
 *   AssignOp
 *   ├── AssignOp
 *   │   ├── NullExp()       // Placeholder for consistency
 *   │   └── IDNode ("x")    // Variable being assigned
 *   └── NUMNode (5)         // Value being assigned
 *   ```
 * 
 * - **Indexed Assignment**:
 *   ```
 *   AssignOp
 *   ├── AssignOp
 *   │   ├── NullExp()       // Placeholder for consistency
 *   │   └── IndexOp
 *   │       ├── IDNode ("arr")  // Array name
 *   │       └── NUMNode (2)     // Index
 *   └── NUMNode (10)        // Value being assigned
 *   ```
 * 
 * **Grammar**:
 */
AssignmentStatement : 
      Variable ASSGNnum Expression
      {  
          // Create an AssignOp node for the assignment operation.
          // Left child: Variable being assigned (e.g., `x` or `arr[2]`)
          // Right child: Expression or value (e.g., `5`, `10`)
          $$ = MakeTree(AssignOp, MakeTree(AssignOp, NullExp(), $1), $3);  
      }
;

/**
 * Grammar Rule: MethodCallStatement
 * 
 * **Purpose**:
 * - Processes **method call statements**, supporting:
 *   - Simple method calls.
 *   - Method calls on objects.
 * 
 * **Examples**:
 * 
 * **1. Simple Method Call:**
 *      print(x);
 *   - The method `print` is called with the argument `x`.
 * 
 * **2. Method Call on an Object:**
 *      obj.method(a, b);
 *   - The method `method` of the object `obj` is called with arguments `a` and `b`.
 * 
 * **Action**:
 * - Constructs a **`RoutineCallOp`** node for the method call.
 * - **Left Child**: Contains the method or object being called.
 * - **Right Child**: Contains the arguments of the method, processed by `MethodCallStatement_tail`.
 * 
 * **Syntax Tree Context**:
 * - **Simple Method Call**:
 *   ```
 *   RoutineCallOp
 *   ├── IDNode ("print")      // Method being called
 *   └── CommaOp               // Argument list
 *       ├── IDNode ("x")      // First argument
 *       └── NullExp()         // End of argument list
 *   ```
 * 
 * - **Method Call on an Object**:
 *   ```
 *   RoutineCallOp
 *   ├── SelectOp              // Object method access
 *   │   ├── IDNode ("obj")    // Object
 *   │   └── IDNode ("method") // Method of the object
 *   └── CommaOp               // Argument list
 *       ├── IDNode ("a")      // First argument
 *       └── IDNode ("b")      // Second argument
 *   ```
 * 
 * **Grammar**:
 */
MethodCallStatement : 
      Variable MethodCallStatement_tail
      {  
          // Create a RoutineCallOp node for the method call.
          // Left child: Method or variable being called (e.g., `print` or `obj.method`)
          // Right child: Arguments (processed by MethodCallStatement_tail)
          $$ = MakeTree(RoutineCallOp, $1, $2);  
      }
;

/**
 * Grammar Rule: MethodCallStatement_tail
 * 
 * **Purpose**:
 * - Processes the **arguments** passed to method calls, handling cases where:
 *   - Arguments are provided in a comma-separated list.
 *   - No arguments are provided.
 * 
 * **Examples**:
 * 
 * **1. Method Call With Arguments**:
 *      print(x, y);
 *   - The method `print` is called with arguments `x` and `y`.
 * 
 * **2. Method Call Without Arguments**:
 *      print();
 *   - The method `print` is called with no arguments.
 * 
 * **Action**:
 * - Constructs the **right child** of the `RoutineCallOp` node, based on the provided arguments.
 * - If arguments exist, returns a **`CommaOp`** node containing the argument list.
 * - If no arguments exist, returns a **`NullExp()`** as a placeholder.
 * 
 * **Syntax Tree Context**:
 * 
 * **1. Method Call With Arguments**:
 *   ```
 *   RoutineCallOp
 *   ├── IDNode ("print")    // Method being called
 *   └── CommaOp             // Argument list
 *       ├── IDNode ("x")    // First argument
 *       └── IDNode ("y")    // Second argument
 *   ```
 * 
 * **2. Method Call Without Arguments**:
 *   ```
 *   RoutineCallOp
 *   ├── IDNode ("print")    // Method being called
 *   └── NullExp()           // No arguments
 *   ```
 * 
 * **Grammar**:
 */
MethodCallStatement_tail : 
      LPARENnum Expression_rec RPARENnum
      {  
          // If arguments are present, return the argument list as a CommaOp.
          $$ = $2;  // Expression_rec constructs the CommaOp for arguments
      }

    | LPARENnum RPARENnum
      {  
          // No arguments, return NullExp to indicate absence of arguments.
          $$ = NullExp();  
      }
;

/**
 * Grammar Rule: Expression_rec
 * 
 * **Purpose**:
 * - Processes **comma-separated expressions**, commonly used for function or method arguments.
 * - Builds a syntax tree structure that represents argument lists as **`CommaOp`** nodes.
 * 
 * **Examples**:
 * 
 * **1. Single Argument**:
 *      print(x);
 *   - A single argument `x` is passed to the `print` function.
 * 
 * **2. Multiple Arguments**:
 *      print(x, y, z);
 *   - Multiple arguments `x`, `y`, and `z` are passed, separated by commas.
 * 
 * **Action**:
 * - For a **single expression**, create a `CommaOp` node with the expression on the left and a `NullExp()` placeholder on the right.
 * - For **multiple expressions**, recursively chain them using `CommaOp` nodes, where:
 *   - The **left child** holds the current expression.
 *   - The **right child** holds the remaining expressions.
 * 
 * **Syntax Tree Context**:
 * 
 * **1. Single Argument**:
 *   ```
 *   CommaOp
 *   ├── IDNode ("x")    // Argument
 *   └── NullExp()       // Placeholder
 *   ```
 * 
 * **2. Multiple Arguments**:
 *   ```
 *   CommaOp
 *   ├── IDNode ("x")         // First argument
 *   └── CommaOp
 *       ├── IDNode ("y")     // Second argument
 *       └── CommaOp
 *           ├── IDNode ("z") // Third argument
 *           └── NullExp()    // Placeholder
 *   ```
 * 
 * **Grammar**:
 */
Expression_rec : 
      Expression
      {  
          // Single expression: wrap it in a CommaOp node.
          // Left child: the expression itself.
          // Right child: NullExp() as a placeholder.
          $$ = MakeTree(CommaOp, $1, NullExp());  
      }

    | Expression COMMAnum Expression_rec
      {  
          // Multiple expressions: recursively chain them with CommaOp.
          // Left child: current expression ($1).
          // Right child: the rest of the expression list ($3).
          $$ = MakeTree(CommaOp, $1, $3);  
      }
;

/**
 * Grammar Rule: ReturnStatement
 * 
 * **Purpose**:
 * - Handles **`return` statements** used in methods or functions.
 * - Supports both **void returns** and **returns with values**.
 * 
 * **Examples**:
 * 
 * **1. Void Return**:
 *      return;
 *   - No value is returned.
 * 
 * **2. Return with a Value**:
 *      return x + 5;
 *   - Returns the result of the expression `x + 5`.
 * 
 * **Action**:
 * - For a **void return**:
 *   - Create a `ReturnOp` node with no children (`NullExp()` for both).
 * - For a **return with a value**:
 *   - Attach the expression to the **left child** of the `ReturnOp` node.
 *   - The **right child** remains `NullExp()` as it's unused in this context.
 * 
 * **Syntax Tree Context**:
 * 
 * **1. Void Return**:
 *   ```
 *   ReturnOp
 *   ├── NullExp()  // No returned value
 *   └── NullExp()  // Placeholder
 *   ```
 * 
 * **2. Return with Value**:
 *   ```
 *   ReturnOp
 *   ├── AddOp       // Expression: x + 5
 *   │   ├── IDNode ("x")
 *   │   └── NUMNode (5)
 *   └── NullExp()   // Placeholder
 *   ```
 * 
 * **Grammar**:
 */
ReturnStatement : 
      RETURNnum
      {  
          // Void return: no value is returned.
          // Both left and right children are placeholders.
          $$ = MakeTree(ReturnOp, NullExp(), NullExp());  
      }

    | RETURNnum Expression
      {  
          // Return with a value: attach the expression to the left child.
          $$ = MakeTree(ReturnOp, $2, NullExp());  
      }
;

/**
 * Grammar Rule: IfStatement
 * 
 * **Purpose**:
 * - Handles all variations of conditional statements:
 *   - **Simple `if`**: Executes a block if the condition is true.
 *   - **`if-else`**: Adds an alternate block if the condition is false.
 *   - **`else-if` chains**: Adds cascading conditions and blocks for evaluation.
 * 
 * **Examples**:
 * 
 * **1. Simple `if`**:
 *      if (x > 0) { print(x); }
 * 
 * **2. `if-else`**:
 *      if (x > 0) { print(x); } else { print(0); }
 * 
 * **3. `else-if` Chain**:
 *      if (x > 0) { print("Positive"); }
 *      else if (x < 0) { print("Negative"); }
 *      else { print("Zero"); }
 * 
 * **Action**:
 * - Constructs an `IfElseOp` node for all conditional structures.
 * - Combines **conditions** and **statement blocks** using `CommaOp`.
 * - For **nested conditions** (`else-if`), recursively attach new `IfElseOp` nodes to the tree.
 * 
 * **Syntax Tree Context**:
 * 
 * **1. Simple `if`**:
 * ```
 * IfElseOp
 * ├── NullExp()        // No `else` branch
 * └── CommaOp          // Combines condition and body
 *     ├── LTOp         // Condition: x > 0
 *     │   ├── IDNode ("x")
 *     │   └── NUMNode (0)
 *     └── StmtOp       // Body: print(x);
 *         └── RoutineCallOp
 *             ├── IDNode ("print")
 *             └── IDNode ("x")
 * ```
 * 
 * **2. `if-else`**:
 * ```
 * IfElseOp
 * ├── StmtOp            // Else branch: print(0);
 * │   └── RoutineCallOp
 * │       ├── IDNode ("print")
 * │       └── NUMNode (0)
 * └── CommaOp           // If branch
 *     ├── LTOp          // Condition: x > 0
 *     │   ├── IDNode ("x")
 *     │   └── NUMNode (0)
 *     └── StmtOp        // Body: print(x);
 *         └── RoutineCallOp
 *             ├── IDNode ("print")
 *             └── IDNode ("x")
 * ```
 * 
 * **3. `else-if` Chain**:
 * ```
 * IfElseOp
 * ├── IfElseOp          // Nested else-if
 * │   ├── StmtOp        // Else branch: print("Zero");
 * │   │   └── RoutineCallOp
 * │   │       ├── IDNode ("print")
 * │   │       └── STRINGNode ("Zero")
 * │   └── CommaOp       // Condition: x < 0
 * │       ├── LTOp
 * │       │   ├── IDNode ("x")
 * │       │   └── NUMNode (0)
 * │       └── StmtOp    // Body: print("Negative");
 * │           └── RoutineCallOp
 * │               ├── IDNode ("print")
 * │               └── STRINGNode ("Negative")
 * └── CommaOp           // Main condition: x > 0
 *     ├── LTOp
 *     │   ├── IDNode ("x")
 *     │   └── NUMNode (0)
 *     └── StmtOp        // Body: print("Positive");
 *         └── RoutineCallOp
 *             ├── IDNode ("print")
 *             └── STRINGNode ("Positive")
 * ```
 * 
 * **Grammar**:
 */
IfStatement : 
      IFnum Expression StatementList  /* Simple `if` */
      {  
          // Simple `if`: Combine condition ($2) and body ($3) into a CommaOp.
          $$ = MakeTree(IfElseOp, NullExp(), MakeTree(CommaOp, $2, $3));  
      }

    | If_rec StatementList  /* `if-else` */
      {  
          // Handle the `else` branch by attaching it to the IfElseOp tree.
          $$ = MakeTree(IfElseOp, $1, $2);  
      }

    | If_rec IFnum Expression StatementList  /* `else-if` chain */
      {  
          tree temp;

          // Create a new IfElseOp for the `else-if` condition and its body.
          temp = MakeTree(IfElseOp, NullExp(), MakeTree(CommaOp, $3, $4));  

          // Attach this new `else-if` to the existing IfElseOp tree.
          $$ = MkLeftC($1, temp);  
      }
;

/**
 * Grammar Rule: If_rec
 * 
 * **Purpose**:
 * - Handles recursive structures for `if-else` and `else-if` chains in conditional statements.
 * - Builds a nested tree of **`IfElseOp`** nodes to represent these structures.
 * 
 * **Examples**:
 * 
 * **1. `if-else`**:
 *      if (x > 0) { print("Positive"); } else { print("Non-Positive"); }
 * 
 * **2. `else-if` Chain**:
 *      if (x > 0) { print("Positive"); }
 *      else if (x < 0) { print("Negative"); }
 *      else { print("Zero"); }
 * 
 * **Action**:
 * - Constructs **nested `IfElseOp` nodes** for `else-if` chains.
 * - Combines conditions and statement blocks using **`CommaOp`**.
 * - Recursively attaches each `else-if` branch to the existing `IfElseOp` tree.
 * 
 * **Syntax Tree Context**:
 * 
 * **1. `if-else`**:
 * ```
 * IfElseOp
 * ├── StmtOp                  // Else branch: print("Non-Positive")
 * │   └── RoutineCallOp
 * │       ├── IDNode ("print")
 * │       └── STRINGNode ("Non-Positive")
 * └── CommaOp                 // If branch
 *     ├── LTOp               // Condition: x > 0
 *     │   ├── IDNode ("x")
 *     │   └── NUMNode (0)
 *     └── StmtOp            // Body: print("Positive")
 *         └── RoutineCallOp
 *             ├── IDNode ("print")
 *             └── STRINGNode ("Positive")
 * ```
 * 
 * **2. `else-if` Chain**:
 * ```
 * IfElseOp
 * ├── IfElseOp                // Nested else-if
 * │   ├── StmtOp              // Else branch: print("Zero")
 * │   │   └── RoutineCallOp
 * │   │       ├── IDNode ("print")
 * │   │       └── STRINGNode ("Zero")
 * │   └── CommaOp             // Else-if branch: x < 0
 * │       ├── LTOp
 * │       │   ├── IDNode ("x")
 * │       │   └── NUMNode (0)
 * │       └── StmtOp          // Body: print("Negative")
 * │           └── RoutineCallOp
 * │               ├── IDNode ("print")
 * │               └── STRINGNode ("Negative")
 * └── CommaOp                 // Main if branch: x > 0
 *     ├── LTOp
 *     │   ├── IDNode ("x")
 *     │   └── NUMNode (0)
 *     └── StmtOp              // Body: print("Positive")
 *         └── RoutineCallOp
 *             ├── IDNode ("print")
 *             └── STRINGNode ("Positive")
 * ```
 * 
 * **Grammar**:
 */
If_rec : 
      IFnum Expression StatementList ELSEnum  /* `if-else` */
      {  
          // Simple `if-else`: Combine condition ($2) and body ($3).
          $$ = MakeTree(IfElseOp, NullExp(), MakeTree(CommaOp, $2, $3));  
      }

    | If_rec IFnum Expression StatementList ELSEnum  /* `else-if` */
      {  
          tree temp;

          // Create a new IfElseOp for the `else-if` condition and its body.
          temp = MakeTree(IfElseOp, NullExp(), MakeTree(CommaOp, $3, $4));  

          // Attach this `else-if` to the existing IfElseOp tree.
          $$ = MkLeftC($1, temp);  
      }
;

/**
 * Grammar Rule: WhileStatement
 * 
 * **Purpose**:
 * - Handles the construction of **`while` loops** in the syntax tree.
 * 
 * **Examples**:
 * 
 * **1. Basic `while` Loop**:
 *      while (x < 10) {
 *          x = x + 1;
 *      }
 * 
 * **2. `while` Loop with Multiple Statements**:
 *      while (x < 10) {
 *          x = x + 1;
 *          print(x);
 *      }
 * 
 * **Action**:
 * - Constructs a **`LoopOp`** node to represent the `while` loop.
 * - The **left child** stores the loop condition (`Expression`).
 * - The **right child** stores the loop body (`StatementList`).
 * 
 * **Syntax Tree Context**:
 * 
 * **Example 1**:
 * ```
 * LoopOp
 * ├── LTOp                  // Loop condition: x < 10
 * │   ├── IDNode ("x")      // Variable x
 * │   └── NUMNode (10)      // Constant 10
 * └── StmtOp                // Loop body
 *     └── AssignOp          // Assignment: x = x + 1
 *         ├── IDNode ("x")  // Variable x
 *         └── AddOp         // Addition: x + 1
 *             ├── IDNode ("x")
 *             └── NUMNode (1)
 * ```
 * 
 * **Example 2**:
 * ```
 * LoopOp
 * ├── LTOp                  // Loop condition: x < 10
 * │   ├── IDNode ("x")      // Variable x
 * │   └── NUMNode (10)      // Constant 10
 * └── StmtOp                // Loop body
 *     ├── AssignOp          // Assignment: x = x + 1
 *     │   ├── IDNode ("x")
 *     │   └── AddOp
 *     │       ├── IDNode ("x")
 *     │       └── NUMNode (1)
 *     └── RoutineCallOp     // Function call: print(x)
 *         ├── IDNode ("print")
 *         └── CommaOp
 *             ├── IDNode ("x")
 *             └── NullExp()
 * ```
 * 
 * **Grammar**:
 */
WhileStatement : 
      WHILEnum Expression StatementList
      {  
          // Create a LoopOp node for the while loop.  
          // Left child: loop condition ($2)  
          // Right child: loop body ($3)
          $$ = MakeTree(LoopOp, $2, $3);  
      }
;

/**
 * Grammar Rule: Expression
 * 
 * **Purpose**:
 * - Handles **arithmetic expressions** and **comparisons** between expressions.
 * 
 * **Examples**:
 * 
 * **1. Simple Arithmetic Expression**:
 *      x + 5
 * 
 * **2. Comparison Expression**:
 *      x < 10
 * 
 * **Action**:
 * - For a **simple arithmetic expression**, directly pass the result.
 * - For a **comparison**, create a comparison operator node (e.g., `<`, `==`).
 * - The **comparison operator** (`Comp_op`) connects the left-hand and right-hand sides.
 * 
 * **Syntax Tree Context**:
 * 
 * **Example 1** (`x + 5`):
 * ```
 * AddOp                   // Arithmetic addition operation
 * ├── IDNode ("x")        // Left operand: variable x
 * └── NUMNode (5)         // Right operand: constant 5
 * ```
 * 
 * **Example 2** (`x < 10`):
 * ```
 * LTOp                    // Less-than comparison operation
 * ├── IDNode ("x")        // Left operand: variable x
 * └── NUMNode (10)        // Right operand: constant 10
 * ```
 * 
 * **Grammar**:
 */
Expression : 
      SimpleExpression
      {  
          // Pass through simple expressions (e.g., x + 5).
          $$ = $1;  
      }

    | SimpleExpression Comp_op SimpleExpression
      {  
          // Complex expression with a comparison operator (e.g., x < 10).

          // Attach the left-hand side to the comparison operator.
          MkLeftC($1, $2);

          // Attach the right-hand side to the comparison operator.
          $$ = MkRightC($3, $2);  
      }
;

/**
 * Grammar Rule: Comp_op
 * 
 * **Purpose**:
 * - Handles **comparison operators** used in conditional and arithmetic expressions.
 * 
 * **Examples**:
 * 
 * **1. Simple Comparisons**:
 *      x < 5
 *      y >= 10
 *      a != b
 * 
 * **2. Usage in Complex Expressions**:
 *      (x + y) < (a * b)
 * 
 * **Action**:
 * - Creates a **comparison operation node** for operators like `<`, `<=`, `==`, etc.
 * - Each node is initialized with two empty children (**`NullExp()`**).
 * - Operands are attached to these nodes later in the **`Expression`** rule.
 * 
 * **Syntax Tree Context**:
 * 
 * **Example 1** (`x < 5`):
 * ```
 * LTOp                    // Less-than comparison operation
 * ├── IDNode ("x")        // Left operand: variable x
 * └── NUMNode (5)         // Right operand: constant 5
 * ```
 * 
 * **Example 2** (`a != b`):
 * ```
 * NEOp                    // Not-equal comparison operation
 * ├── IDNode ("a")        // Left operand: variable a
 * └── IDNode ("b")        // Right operand: variable b
 * ```
 * 
 * **Grammar**:
 */
Comp_op : 
      LTnum   /* `<` (Less Than) */
      {  
          // Create a less-than node with empty children.
          $$ = MakeTree(LTOp, NullExp(), NullExp());  
      }

    | LEnum   /* `<=` (Less Than or Equal) */
      {  
          // Create a less-than-or-equal node with empty children.
          $$ = MakeTree(LEOp, NullExp(), NullExp());  
      }

    | EQnum   /* `==` (Equal) */
      {  
          // Create an equality node with empty children.
          $$ = MakeTree(EQOp, NullExp(), NullExp());  
      }

    | NEnum   /* `!=` (Not Equal) */
      {  
          // Create a not-equal node with empty children.
          $$ = MakeTree(NEOp, NullExp(), NullExp());  
      }

    | GEnum   /* `>=` (Greater Than or Equal) */
      {  
          // Create a greater-than-or-equal node with empty children.
          $$ = MakeTree(GEOp, NullExp(), NullExp());  
      }

    | GTnum   /* `>` (Greater Than) */
      {  
          // Create a greater-than node with empty children.
          $$ = MakeTree(GTOp, NullExp(), NullExp());  
      }
;

/**
 * Grammar Rule: SimpleExpression
 * 
 * **Purpose**:
 * - Handles **arithmetic** and **logical OR** operations, including:
 *   - Binary operations like addition (`+`), subtraction (`-`), and logical OR (`||`).
 *   - Unary operations like negation (`-x`) and unary plus (`+x`).
 * 
 * **Examples**:
 * 
 * **1. Arithmetic Expressions**:
 *      x + 5
 *      y - 2
 *      -a
 * 
 * **2. Logical OR**:
 *      a || b
 * 
 * **Action**:
 * - Builds the syntax tree for arithmetic operations (`+`, `-`) and logical OR (`||`).
 * - Handles **unary negation** by creating a `UnaryNegOp` node.
 * 
 * **Syntax Tree Context**:
 * 
 * **Example 1** (`x + 5`):
 * ```
 * AddOp                   // Addition operation
 * ├── IDNode ("x")        // Left operand: variable x
 * └── NUMNode (5)         // Right operand: constant 5
 * ```
 * 
 * **Example 2** (`-a`):
 * ```
 * UnaryNegOp              // Unary negation operation
 * ├── IDNode ("a")        // Operand: variable a
 * └── NullExp()           // No right child
 * ```
 * 
 * **Example 3** (`a || b`):
 * ```
 * OrOp                    // Logical OR operation
 * ├── IDNode ("a")        // Left operand: variable a
 * └── IDNode ("b")        // Right operand: variable b
 * ```
 * 
 * **Grammar**:
 */
SimpleExpression : 
      Term
      /* Case 1: Simple expression (e.g., a number or variable) */
      {  
          // Pass through a single term.
          $$ = $1;  
      }

    | PLUSnum Term
      /* Case 2: Unary plus (e.g., `+x`) */
      {  
          // Treat unary plus as the value itself.
          $$ = $2;  
      }

    | MINUSnum Term
      /* Case 3: Unary negation (e.g., `-x`) */
      {  
          // Create a UnaryNegOp node for the negated term.
          $$ = MakeTree(UnaryNegOp, $2, NullExp());  
      }

    | SimpleExpression PLUSnum Term
      /* Case 4: Binary addition (e.g., `x + 5`) */
      {  
          // Create an AddOp node for the addition operation.
          $$ = MakeTree(AddOp, $1, $3);  
      }

    | SimpleExpression MINUSnum Term
      /* Case 5: Binary subtraction (e.g., `x - 2`) */
      {  
          // Create a SubOp node for the subtraction operation.
          $$ = MakeTree(SubOp, $1, $3);  
      }

    | SimpleExpression ORnum Term
      /* Case 6: Logical OR operation (e.g., `a || b`) */
      {  
          // Create an OrOp node for the logical OR operation.
          $$ = MakeTree(OrOp, $1, $3);  
      }
;

/**
 * Grammar Rule: Term
 * 
 * **Purpose**:
 * - Handles **multiplication**, **division**, and **logical AND** operations.
 * - These operators have **higher precedence** than addition and subtraction, ensuring they are evaluated first.
 * 
 * **Examples**:
 * 
 * **1. Arithmetic Operations**:
 *      x * y
 *      a / b
 * 
 * **2. Logical Operations**:
 *      a && b
 * 
 * **3. Complex Mixed Expressions**:
 *      x * y / z
 *      (a && b) * c
 * 
 * **Action**:
 * - Constructs a **syntax tree** for `*`, `/`, and `&&` operations.
 * - **Left associativity** is maintained by recursively chaining **`Term`** nodes.
 * - Each operation creates a **binary operation node** (e.g., `MultOp`, `DivOp`, `AndOp`) with:
 *   - The **left operand** as the **left child**.
 *   - The **right operand** as the **right child**.
 * 
 * **Syntax Tree Context**:
 * 
 * **Example 1** (`x * y`):
 * ```
 * MultOp                 // Multiplication operation
 * ├── IDNode ("x")       // Left operand: variable x
 * └── IDNode ("y")       // Right operand: variable y
 * ```
 * 
 * **Example 2** (`a && b`):
 * ```
 * AndOp                  // Logical AND operation
 * ├── IDNode ("a")       // Left operand: variable a
 * └── IDNode ("b")       // Right operand: variable b
 * ```
 * 
 * **Example 3** (`x * y / z`):
 * ```
 * DivOp                  // Division operation
 * ├── MultOp             // Nested multiplication
 * │   ├── IDNode ("x")   // Left operand: variable x
 * │   └── IDNode ("y")   // Right operand: variable y
 * └── IDNode ("z")       // Right operand: variable z
 * ```
 * 
 * **Grammar**:
 */
Term : 
      Factor
      {  
          // Single factor (number, variable, or parenthesized expression).
          $$ = $1;  
      }

    | Term TIMESnum Factor
      {  
          // Multiplication operation (e.g., `x * y`).
          // Left child: Term (accumulated product so far).
          // Right child: Factor (current operand).
          $$ = MakeTree(MultOp, $1, $3);  
      }

    | Term DIVIDEnum Factor
      {  
          // Division operation (e.g., `x / y`).
          // Left child: Term (accumulated division so far).
          // Right child: Factor (current operand).
          $$ = MakeTree(DivOp, $1, $3);  
      }

    | Term ANDnum Factor
      {  
          // Logical AND operation (e.g., `a && b`).
          // Left child: Term (accumulated logical operation so far).
          // Right child: Factor (current operand).
          $$ = MakeTree(AndOp, $1, $3);  
      }
;

/**
 * Grammar Rule: Factor
 * 
 * **Purpose**:
 * - Handles **basic expressions** in the language, which include:
 *   - Constants
 *   - Variables
 *   - Method calls
 *   - Parenthesized expressions
 *   - Logical NOT (`!`) operations
 * 
 * **Examples**:
 * 
 * **1. Constants**:
 *      42, "hello", true
 * 
 * **2. Variables**:
 *      x, arr[5]
 * 
 * **3. Parenthesized Expressions**:
 *      (x + y)
 * 
 * **4. Logical NOT**:
 *      !flag
 * 
 * **Action**:
 * - Directly passes through constants, variables, and method calls.
 * - For parenthesized expressions, unwraps them to their inner expression.
 * - For logical NOT (`!`), creates a **`NotOp`** node with the negated expression as the left child.
 * 
 * **Syntax Tree Context**:
 * 
 * **Example 1** (`x`):
 * ```
 * VarOp          // Variable usage
 * └── IDNode ("x")  // Variable name
 * ```
 * 
 * **Example 2** (`(x + y)`):
 * ```
 * AddOp          // Addition operation
 * ├── VarOp      // Left operand
 * │   └── IDNode ("x")
 * └── VarOp      // Right operand
 *     └── IDNode ("y")
 * ```
 * 
 * **Example 3** (`!flag`):
 * ```
 * NotOp          // Logical NOT operation
 * └── VarOp      // Operand
 *     └── IDNode ("flag")
 * ```
 * 
 * **Grammar**:
 */
Factor : 
      UnsignedConstant
      {  
          // Handle constant values like numbers or strings.
          $$ = $1;  
      }

    | Variable
      {  
          // Handle variable usage (e.g., `x`, `array[i]`).
          $$ = $1;  
      }

    | MethodCallStatement
      {  
          // Handle method calls (e.g., `foo()`).
          $$ = $1;  
      }

    | LPARENnum Expression RPARENnum
      {  
          // Handle parenthesized expressions (e.g., `(x + y)`).
          // Simply pass through the inner expression.
          $$ = $2;  
      }

    | NOTnum Factor
      {  
          // Handle logical NOT operation (e.g., `!flag`).
          // Create a NotOp node with the negated expression as the left child.
          $$ = MakeTree(NotOp, $2, NullExp());  
      }
;

/**
 * Grammar Rule: UnsignedConstant
 * 
 * **Purpose**:
 * - Handles **constant values** in the language, specifically:
 *   - Integer constants (e.g., `5`, `42`)
 *   - String constants (e.g., `"hello"`, `"world"`)
 * 
 * **Examples**:
 * 
 * **1. Integer Constants**:
 *      42, 5, 100
 * 
 * **2. String Constants**:
 *      `"hello"`, `"example"`
 * 
 * **Action**:
 * - For integer constants, wraps them in a **`NUMNode`**.
 * - For string constants, wraps them in a **`STRINGNode`**.
 * 
 * **Syntax Tree Context**:
 * 
 * **Example 1** (`5`):
 * ```
 * NUMNode         // Represents an integer constant
 * └── (value: 5)
 * ```
 * 
 * **Example 2** (`"hello"`):
 * ```
 * STRINGNode      // Represents a string constant
 * └── (value: "hello")
 * ```
 * 
 * **Grammar**:
 */
UnsignedConstant : 
      ICONSTnum
      {  
          // Integer constant (e.g., `5`, `42`).
          $$ = MakeLeaf(NUMNode, $1);  
      }

    | SCONSTnum
      {  
          // String constant (e.g., `"hello"`).
          $$ = MakeLeaf(STRINGNode, $1);  
      }
;

/**
 * Grammar Rule: Variable
 * 
 * **Purpose**:
 * - Handles **variable access** in expressions, supporting:
 *   - Simple variable names (e.g., `x`)
 *   - Array indexing (e.g., `arr[2]`)
 *   - Object field access (e.g., `obj.field`)
 *   - Chained field or array accesses (e.g., `obj.field1.field2[3]`)
 * 
 * **Examples**:
 * 
 * **1. Simple Variable**:
 *      x
 * 
 * **Syntax Tree Example**:
 * ```
 * VarOp
 * ├── IDNode ("x")
 * └── NullExp()  // No additional tail
 * ```
 * 
 * **2. Array Access**:
 *      arr[2]
 * 
 * **Syntax Tree Example**:
 * ```
 * VarOp
 * ├── IDNode ("arr")
 * └── IndexOp
 *     └── NUMNode (2)
 * ```
 * 
 * **3. Field Access**:
 *      obj.field
 * 
 * **Syntax Tree Example**:
 * ```
 * VarOp
 * ├── IDNode ("obj")
 * └── FieldOp
 *     └── IDNode ("field")
 * ```
 * 
 * **4. Chained Access**:
 *      obj.field1.field2[3]
 * 
 * **Syntax Tree Example**:
 * ```
 * VarOp
 * ├── IDNode ("obj")
 * └── FieldOp
 *     ├── FieldOp
 *     │   ├── IDNode ("field1")
 *     │   └── IDNode ("field2")
 *     └── IndexOp
 *         └── NUMNode (3)
 * ```
 * 
 * **Action**:
 * - Creates a **`VarOp`** node to represent variable access.
 * - **Left child**: The **identifier** (`IDNode`) for the base variable or object.
 * - **Right child**: The **`Variable_tail`** node, which may represent array indexing or field selections.
 * 
 * **Grammar**:
 */
Variable : 
      IDnum Variable_tail
      {  
          // Create a VarOp node to represent the variable.
          // Left child: IDNode (variable name, e.g., `x` or `arr`)
          // Right child: Variable_tail (e.g., array index or field access)
          $$ = MakeTree(VarOp, MakeLeaf(IDNode, $1), $2);  
      }
;

/**
 * Grammar Rule: Variable_tail
 * 
 * **Purpose**:
 * - Handles **optional extensions** to a variable. These extensions may include:
 *   - No extension (e.g., `x`)
 *   - Array indexing (e.g., `arr[2]`)
 *   - Field access (e.g., `obj.field`)
 * 
 * **Examples**:
 * 
 * **1. No Extension** (Simple Variable):
 *      x
 * 
 * **Syntax Tree Example**:
 * ```
 * NullExp()
 * ```
 * 
 * **2. Array Access**:
 *      arr[2]
 * 
 * **Syntax Tree Example**:
 * ```
 * IndexOp
 * └── NUMNode (2)
 * ```
 * 
 * **3. Field Access**:
 *      obj.field
 * 
 * **Syntax Tree Example**:
 * ```
 * FieldOp
 * └── IDNode ("field")
 * ```
 * 
 * **4. Complex Chained Access**:
 *      obj.field1[3]
 * 
 * **Syntax Tree Example**:
 * ```
 * IndexOp
 * ├── FieldOp
 * │   └── IDNode ("field1")
 * └── NUMNode (3)
 * ```
 * 
 * **Action**:
 * - If there is **no further access**, returns **`NullExp()`**, indicating a simple variable.
 * - If there are extensions (e.g., array indexing or field access), processes them with **`Variable_rec`**.
 * 
 * **Grammar**:
 */
Variable_tail : 
      /* Simple variable (no further access) */
      {  
          $$ = NullExp();  // No additional access, simple variable
      }

    | Variable_rec
      {  
          // Process further accesses (e.g., array indexing, field access).
          $$ = $1;  
      }
;

/**
 * Grammar Rule: Variable_rec
 * 
 * **Purpose**:
 * - Handles **recursive variable extensions** for more complex access patterns, such as:
 *   - **Array indexing** (e.g., `arr[2]`)
 *   - **Field access** (e.g., `obj.field`)
 *   - **Chained accesses** (e.g., `obj.arr[1].field` or `matrix[2][3].x`)
 * 
 * **Examples**:
 * 
 * **1. Array Access**:
 *      arr[2]
 * 
 * **Syntax Tree Example**:
 * ```
 * SelectOp
 * └── IndexOp
 *     └── NUMNode (2)
 * ```
 * 
 * **2. Field Access**:
 *      obj.field
 * 
 * **Syntax Tree Example**:
 * ```
 * SelectOp
 * └── FieldOp
 *     └── IDNode ("field")
 * ```
 * 
 * **3. Chained Access**:
 *      obj.arr[1].field
 * 
 * **Syntax Tree Example**:
 * ```
 * SelectOp
 * ├── SelectOp
 * │   ├── FieldOp
 * │   │   └── IDNode ("arr")
 * │   └── IndexOp
 * │       └── NUMNode (1)
 * └── FieldOp
 *     └── IDNode ("field")
 * ```
 * 
 * **Action**:
 * - Creates a **`SelectOp`** node for each level of access:
 *   - **Left child**: The current access (`Variable_1`).
 *   - **Right child**: The next access (`Variable_rec`) or **`NullExp()`** if there are no further extensions.
 * - Combines multiple accesses recursively to form a full variable access chain.
 * 
 * **Grammar**:
 */
Variable_rec : 
      Variable_1
      {  
          // Single access (e.g., `arr[2]` or `obj.field`).  
          $$ = MakeTree(SelectOp, $1, NullExp());  
      }

    | Variable_1 Variable_rec
      {  
          // Chained access (e.g., `obj.field1.field2` or `arr[1][2]`).  
          // Left child: the current access (`Variable_1`)  
          // Right child: the next access (`Variable_rec`)
          $$ = MakeTree(SelectOp, $1, $2);  
      }
;

/**
 * Grammar Rule: Variable_1
 * 
 * **Purpose**:
 * - Handles **specific variable extensions**, including:
 *   - **Array indexing** (e.g., `arr[2]`, `matrix[1, 2]`)
 *   - **Field access** (e.g., `person.name`, `obj.data.value`)
 * 
 * **Examples**:
 * 
 * **1. Array Access**:
 *      arr[2]
 * 
 * **Syntax Tree Example**:
 * ```
 * IndexOp
 * └── NUMNode (2)  // Index value
 * ```
 * 
 * **2. Field Access**:
 *      obj.field
 * 
 * **Syntax Tree Example**:
 * ```
 * FieldOp
 * └── IDNode ("field")  // Field name
 * ```
 * 
 * **Action**:
 * - **For array indexing**:
 *   - Delegates to **`Expression_rec2`**, which processes the index expression(s).
 *   - Returns the resulting tree structure directly.
 * - **For field access**:
 *   - Creates a **`FieldOp`** node with:
 *     - **Left child**: An `IDNode` representing the field name.
 *     - **Right child**: `NullExp()`, as no further access is handled here.
 * 
 * **Grammar**:
 */
Variable_1 : 
      LBRACnum Expression_rec2 RBRACnum  // Array access (e.g., arr[2])
      {  
          // Array indexing is processed by Expression_rec2 and returned as-is.
          $$ = $2;  
      }

    | DOTnum IDnum  // Field access (e.g., obj.field)
      {  
          // Create a FieldOp node for field access.
          // Left child: The field name (IDNode).
          // Right child: NullExp (no additional access yet).
          $$ = MakeTree(FieldOp, MakeLeaf(IDNode, $2), NullExp());  
      }
;

/**
 * Grammar Rule: Expression_rec2
 * 
 * **Purpose**:
 * - Handles **multi-dimensional array indexing** or **comma-separated expressions**.
 * - Builds syntax tree structures for indexing operations or lists of expressions.
 * 
 * **Examples**:
 * 
 * **1. Single-dimensional Array**:
 *      arr[2]
 * 
 * **Syntax Tree**:
 * ```
 * IndexOp
 * └── NUMNode (2)  // Index value
 * ```
 * 
 * **2. Multi-dimensional Array**:
 *      matrix[1, 2]
 * 
 * **Syntax Tree**:
 * ```
 * IndexOp
 * ├── NUMNode (1)  // First dimension
 * └── IndexOp
 *     ├── NUMNode (2)  // Second dimension
 *     └── NullExp()
 * ```
 * 
 * **3. Function Call with Arguments**:
 *      print(a, b, c)
 * 
 * **Syntax Tree**:
 * ```
 * IndexOp
 * ├── IDNode ("a")  // First argument
 * └── IndexOp
 *     ├── IDNode ("b")  // Second argument
 *     └── IndexOp
 *         ├── IDNode ("c")  // Third argument
 *         └── NullExp()
 * ```
 * 
 * **Action**:
 * - For a **single index or expression**, create an **`IndexOp`** node with the index as the left child.
 * - For **multiple indices or expressions**, recursively chain **`IndexOp`** nodes using **`COMMAnum`**:
 *   - **Left child**: Current index or expression (`$1`).
 *   - **Right child**: Remaining indices or expressions (`$3`).
 * 
 * **Grammar**:
 */
Expression_rec2 : 
      Expression  // Single array index (e.g., arr[2])
      {  
          // Wrap the single index or expression in an IndexOp node.
          $$ = MakeTree(IndexOp, $1, NullExp());  
      }

    | Expression COMMAnum Expression_rec2  // Multi-dimensional array or comma-separated list
      {  
          // Recursively build nested IndexOp nodes for each index or expression.
          $$ = MakeTree(IndexOp, $1, $3);  
      }
;

%%
/**
 * Global Variables for Tracking Source Code Position
 * 
 * - `yycolumn`: Tracks the current **column** number in the source file.
 * - `yyline`: Tracks the current **line** number in the source file.
 * 
 * These variables are useful for **error reporting** and **debugging** to pinpoint the exact location
 * of syntax errors.
 */

int yycolumn, yyline;

/**
 * Output File for Syntax Tree or Debug Information - Already defined in codegen.c
 * 
 * - `treelst` is a file pointer used to print the **syntax tree** or other debug information.
 * - By default, this is set to **stdout** (console output), but it could be redirected to a file.
 */

// FILE *treelst;

/**
 * Main Function (Currently Commented Out)
 * 
 * - Initializes the `treelst` output to standard output (`stdout`).
 * - Calls `yyparse()` to start the parsing process.
 * 
 * **Note:** This is commented out, so the parser won't run unless uncommented.
 */

/*
main()
{
  treelst = stdout;  // Set the output file for the syntax tree to console.
  yyparse();         // Start parsing the input.
}
*/

/**
 * Error Reporting Function
 * 
 * - `yyerror()` is automatically called by **YACC/Bison** when a syntax error occurs.
 * - Displays the error message along with the **line number** where the error was detected.
 * 
 * @param str Error message generated by the parser.
 */

void yyerror(char *str)
{
  printf("yyerror: %s at line %d\n", str, yyline);
}

#include "lex.yy.c"