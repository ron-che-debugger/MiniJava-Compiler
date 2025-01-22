#ifndef __TREE_H
#define __TREE_H

// Define the syntax tree node structure
typedef struct treenode {
    int NodeKind, NodeOpType, IntVal;
    struct treenode *LeftC, *RightC;
} ILTree, *tree;

/* -------------------- Operator Node Types -------------------- */

/* Represents the root of the program (main entry point) */
// Example: The entire program structure
#define ProgramOp 100

/* Represents the body of a program, method, or class */
// Example: The code block inside a function
#define BodyOp 101

/* Represents a declaration statement */
// Example: `int x;`
#define DeclOp 102

/* Represents the comma operator used in variable lists */
// Example: `int a, b, c;`
#define CommaOp 103

/* Represents an array type declaration */
// Example: `int arr[10];`
#define ArrayTypeOp 104

/* Represents a type identifier (int, char, etc.) */
// Example: `int`, `char`
#define TypeIdOp 105

/* Represents array bounds in declarations */
// Example: The `10` in `int arr[10];`
#define BoundOp 106

/* Represents record (struct/class) component declaration */
// Example: `struct Point { int x; int y; };`
#define RecompOp 107

/* Represents a loop increment (for counting up) */
// Example: `for (i = 0; i <= n; i++)`
#define ToOp 108

/* Represents a loop decrement (for counting down) */
// Example: `for (i = n; i >= 0; i--)`
#define DownToOp 109

/* Represents constant identifier usage */
// Example: `const int PI = 3.14;`
#define ConstantIdOp 110

/* Represents a procedure call */
// Example: `print();`
#define ProceOp 111

/* Represents a function call */
// Example: `sum(1, 2);`
#define FuncOp 112

/* Represents the head of a function or procedure */
// Example: `int sum(int a, int b)`
#define HeadOp 113

/* Represents regular function arguments */
// Example: `sum(a, b)` where `a` and `b` are arguments
#define RArgTypeOp 114

/* Represents value-specific arguments */
// Example: `sum(val int a, val int b)`
#define VArgTypeOp 115

/* Represents general statements */
// Example: `x = 5;`
#define StmtOp 116

/* Represents an if-else conditional */
// Example: `if (x > 0) { ... } else { ... }`
#define IfElseOp 117

/* Represents loop structures */
// Example: `while (x < 10) { x++; }`
#define LoopOp 118

/* Represents specification of parameters or types */
// Example: `int x` in a function parameter list
#define SpecOp 119

/* Represents a function or method call */
// Example: `printf("Hello");`
#define RoutineCallOp 120

/* Represents an assignment operation */
// Example: `x = y + 2;`
#define AssignOp 121

/* Represents a return statement */
// Example: `return x;`
#define ReturnOp 122

/* Represents an addition operation */
// Example: `a + b`
#define AddOp 123

/* Represents a subtraction operation */
// Example: `a - b`
#define SubOp 124

/* Represents a multiplication operation */
// Example: `a * b`
#define MultOp 125

/* Represents a division operation */
// Example: `a / b`
#define DivOp 126

/* Represents a less-than comparison */
// Example: `a < b`
#define LTOp 127

/* Represents a greater-than comparison */
// Example: `a > b`
#define GTOp 128

/* Represents an equality comparison */
// Example: `a == b`
#define EQOp 129

/* Represents a not-equal comparison */
// Example: `a != b`
#define NEOp 130

/* Represents a less-than-or-equal comparison */
// Example: `a <= b`
#define LEOp 131

/* Represents a greater-than-or-equal comparison */
// Example: `a >= b`
#define GEOp 132

/* Represents a logical AND operation */
// Example: `a && b`
#define AndOp 133

/* Represents a logical OR operation */
// Example: `a || b`
#define OrOp 134

/* Represents a unary negation operation */
// Example: `-a`
#define UnaryNegOp 135

/* Represents a logical NOT operation */
// Example: `!a`
#define NotOp 136

/* Represents a variable or identifier */
// Example: `x`, `counter`
#define VarOp 137

/* Represents member access via a dot or arrow operator */
// Example: `object.property` or `ptr->property`
#define SelectOp 138

/* Represents array indexing */
// Example: `arr[5]`
#define IndexOp 139

/* Represents field access in a structure */
// Example: `struct Point { int x; int y; } p; p.x;`
#define FieldOp 140

/* Represents a subrange operation */
// Example: `for (i = 1 to 10)`
#define SubrangeOp 141

/* Represents an exit operation (break/continue) */
// Example: `break;`
#define ExitOp 142

/* Represents a class declaration */
// Example: `class MyClass { ... }`
#define ClassOp 143

/* Represents a method inside a class */
// Example: `void print() { ... }`
#define MethodOp 144

/* Represents a full class definition */
// Example: `class MyClass { void print(); };`
#define ClassDefOp 145

/* -------------------- Node Types -------------------- */

/* Represents an identifier node (variable/function names) */
// Example: `x`, `main`
#define IDNode 200

/* Represents a numeric constant */
// Example: `5`, `42`
#define NUMNode 201

/* Represents a character constant */
// Example: `'a'`, `'Z'`
#define CHARNode 202

/* Represents a string constant */
// Example: `"Hello, World!"`
#define STRINGNode 203

/* Represents a dummy (null) node */
// Used as a placeholder for empty children in the tree
#define DUMMYNode 204

/* Represents an expression node */
// Example: `a + b`
#define EXPRNode 205

/* Represents an integer type node */
// Example: `int`
#define INTEGERTNode 206

/* Represents a character type node */
// Example: `char`
#define CHARTNode 207

/* Represents a boolean type node */
// Example: `true`, `false`
#define BOOLEANTNode 208

/* Represents a symbol table node */
// Used for managing symbol table entries
#define STNode 209

tree NullExp();
tree MakeLeaf(int, int);
tree MakeTree(int, tree, tree);
tree LeftChild(tree);
tree RightChild(tree);
tree MkLeftC(tree, tree);
tree MkRightC(tree, tree);

void SetNode(tree, tree);
void SetNodeOp(tree, int);
void SetLeftTreeOp(tree, int);
void SetRightTreeOp(tree, int);
void SetLeftChild(tree, tree);
void SetRightChild(tree, tree);
int LeftDepth(tree);

int IsNull(tree);
int IntVal(tree);
int NodeOp(tree);
int NodeKind(tree);

char *getname(int i);
char *getstring(int i);
void printtree(tree nd, int depth);

#endif