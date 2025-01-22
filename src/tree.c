#include "tree.h"
#include "symbol_table.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Global dummy node used as a placeholder in the syntax tree.
 *
 * - Represents an **empty** or **null** node in the tree structure.
 * - Used in functions like `NullExp()` to return a standardized "null" node.
 * - Prevents the need to use NULL pointers for missing child nodes.
 *
 * Structure:
 *   - NodeKind: `DUMMYNode` → Marks the node as a dummy placeholder.
 *   - NodeOpType: `0` → No operation, as this is not an expression node.
 *   - IntVal: `0` → No value, since dummy nodes don't store data.
 *   - LeftC: `0` → No left child (could be explicitly set to `NULL`).
 *   - RightC: `0` → No right child (could be explicitly set to `NULL`).
 */
ILTree dummy = {DUMMYNode, 0, 0, 0, 0};

/**
 * Provides a placeholder for an empty or null tree node.
 */
tree NullExp() {
    return (&dummy); // Returns a pointer to the global dummy node
}

/**
 * Creates a new leaf node with the specified node type and integer value.
 *
 * @param Kind The type of the node (e.g., IDNode, NUMNode).
 * @param N    The integer value to store in the node.
 */
tree MakeLeaf(int Kind, int N) {
    tree p;

    p = (tree)malloc(sizeof(ILTree)); // Allocate memory for the new node
    p->NodeKind = Kind;               // Set the node type (e.g., identifier, number)
    p->IntVal = N;                    // Set the integer value for the node
    p->LeftC = NullExp();             // Initialize left child as a dummy node
    p->RightC = NullExp();            // Initialize right child as a dummy node

    return (p); // Return the newly created leaf node
}

/**
 * Creates an internal expression node with the specified operation and child nodes.
 *
 * @param NodeOp The operation type for the node (e.g., AddOp, SubOp).
 * @param Left   The left child subtree.
 * @param Right  The right child subtree.
 */
tree MakeTree(int NodeOp, tree Left, tree Right) {
    tree p;

    p = (tree)malloc(sizeof(ILTree)); // Allocate memory for the new expression node
    p->NodeKind = EXPRNode;           // Set the node type as an expression node
    p->NodeOpType = NodeOp;           // Set the specific operation type (e.g., addition, subtraction)
    p->LeftC = Left;                  // Attach the left child subtree
    p->RightC = Right;                // Attach the right child subtree

    return (p); // Return the newly created expression node
}

/**
 * Retrieves the left child of the given expression node.
 *
 * @param T The tree node whose left child is to be accessed.
 */
tree LeftChild(tree T) {
    if (NodeKind(T) != EXPRNode) // Check if the node is an expression node
        return (NullExp());      // Return a dummy node if it's not an expression node
    return (T->LeftC);           // Return the left child if valid
}

/**
 * Retrieves the right child of the given expression node.
 *
 * @param T The tree node from which to retrieve the right child.
 */
tree RightChild(tree T) {
    if (NodeKind(T) != EXPRNode) // Check if the node is an expression node
        return (NullExp());      // Return a dummy node if not an expression node

    return (T->RightC); // Return the right child of the node
}

/**
 * Attaches subtree T1 as the leftmost child of subtree T2.
 * If T2 is empty, T1 is returned directly.
 *
 * @param T1 The subtree to attach as the leftmost child.
 * @param T2 The target subtree where T1 will be attached.
 */
tree MkLeftC(tree T1, tree T2) {
    tree p, q;

    if (IsNull(T2)) // If T2 is empty, return T1 as the new subtree
        return (T1);

    p = T2;
    q = LeftChild(p);

    /* Traverse to the leftmost child of T2 */
    while (!IsNull(q)) {
        p = q;
        q = LeftChild(p);
    }

    p->LeftC = T1; // Attach T1 to the leftmost DUMMYNode in T2
    return (T2);   // Return the updated tree with T1 attached
}

/**
 * Attaches subtree T1 as the rightmost child of subtree T2.
 * If T2 is empty, T1 is returned directly.
 *
 * @param T1 The subtree to attach as the rightmost child.
 * @param T2 The target subtree where T1 will be attached.
 */
tree MkRightC(tree T1, tree T2) {
    tree p, q;

    if (IsNull(T2)) // If T2 is empty, return T1 as the new subtree
        return (T1);

    p = T2;
    q = RightChild(p);

    /* Traverse to the rightmost child of T2 */
    while (!IsNull(q)) {
        p = q;
        q = RightChild(p);
    }

    p->RightC = T1; // Attach T1 to the rightmost DUMMYNode in T2
    return (T2);    // Return the updated tree with T1 attached
}

/**
 * Retrieves the operation type of an expression node.
 *
 * @param T The tree node whose operation type is being accessed.
 */
int NodeOp(tree T) {
    if (NodeKind(T) != EXPRNode) { // Check if the node is an expression node
        // printf("NodeOP(): This node must be an EXPRNode!\n");
        return 0; // Return 0 if the node is not an expression node
    }
    return (T->NodeOpType); // Return the operation type of the expression node
}

/**
 * Retrieves the node type of the given tree node.
 *
 * @param T The tree node whose type is to be retrieved.
 */
int NodeKind(tree T) {
    return (T->NodeKind); // Return the node's type (e.g., EXPRNode, IDNode)
}

/**
 * Retrieves the integer value stored in a leaf node.
 *
 * @param T The tree node whose integer value is to be retrieved.
 */
int IntVal(tree T) {
    if (NodeKind(T) == EXPRNode) {
        printf("IntVal(): This node must be a leaf node!\n"); // Error if called on an expression node
        return -1;                                            // Return -1 to indicate an invalid operation
    }
    return (T->IntVal); // Return the integer value of the leaf node
}

/**
 * Checks if the given tree node is a dummy (null) node.
 *
 * @param T The tree node to check.
 */
int IsNull(tree T) {
    return (NodeKind(T) == DUMMYNode); // Returns 1 if the node is a dummy node, 0 otherwise
}

/**
 * Copies the content of the source node into the target node.
 *
 * @param Target The node that will be updated with the source's data.
 * @param Source The node whose data will be copied to the target.
 */
void SetNode(tree Target, tree Source) {
    if ((Target->NodeKind = Source->NodeKind) != EXPRNode) {
        // If the node is a leaf node, copy its value and reset its children
        Target->IntVal = Source->IntVal;
        Target->LeftC = NullExp();  // Set left child to dummy node
        Target->RightC = NullExp(); // Set right child to dummy node
    } else {
        // If the node is an expression node, copy its operator type and children
        Target->NodeOpType = Source->NodeOpType;
        Target->LeftC = Source->LeftC;
        Target->RightC = Source->RightC;
    }
}

/**
 * Sets the operation type of an expression node.
 *
 * @param T  The target tree node to update.
 * @param Op The operation type to assign (e.g., AddOp, SubOp).
 */
void SetNodeOp(tree T, int Op) {
    if (NodeKind(T) != EXPRNode) {
        printf("SetNodeOp(): This node must be an EXPRNode!\n"); // Error if T is not an expression node
    } else {
        T->NodeOpType = Op; // Set the operation type for the expression node
    }
}

/**
 * Sets the operation type (`NodeOpType`) for the root node and all nodes
 * along the leftmost path of the given tree.
 *
 * @param T  The root of the tree whose left path will be updated.
 * @param Op The operation type to assign to each node (e.g., AddOp, SubOp).
 */
void SetLeftTreeOp(tree T, int Op) {
    tree p;

    p = T;
    do {
        SetNodeOp(p, Op); // Set the operation type for the current node
        p = LeftChild(p); // Move to the left child
    } while (!IsNull(p)); // Continue until a dummy (null) node is reached
}

/**
 * Sets the operation type (`NodeOpType`) of the given node and all its right descendants.
 *
 * @param T  The root node of the subtree to update.
 * @param Op The operation type to assign to each node (e.g., AddOp, SubOp).
 */
void SetRightTreeOp(tree T, int Op) {
    tree p;

    p = T;
    do {
        SetNodeOp(p, Op);  // Set the operation type for the current node
        p = RightChild(p); // Move to the right child node
    } while (!IsNull(p)); // Continue until a dummy node is reached
}

/**
 * Sets the left child of the given expression node.
 *
 * @param T    The target node whose left child will be updated.
 * @param NewC The new left child node to attach.
 */
void SetLeftChild(tree T, tree NewC) {
    if (NodeKind(T) != EXPRNode)
        printf("SetLeftChild(): This node must be an EXPRNode!\n"); // Error if T is not an expression node
    else
        T->LeftC = NewC; // Attach the new left child
}

/**
 * Sets the right child of the given expression node.
 *
 * @param T    The target node whose right child will be updated.
 * @param NewC The new right child subtree to attach.
 */
void SetRightChild(tree T, tree NewC) {
    if (NodeKind(T) != EXPRNode)
        printf("SetRightChild(): This node must be an EXPRNode!\n"); // Error if not an expression node
    else
        T->RightC = NewC; // Attach the new right child to the node
}

/**
 * LeftDepth - Computes the depth of the leftmost branch of a tree.
 *
 * @param treenode The root of the tree.
 * @return The number of nodes along the leftmost path (depth).
 *
 * This function counts how many consecutive left children exist in the tree.
 * It is primarily used to determine the number of elements in a comma-separated
 * array initialization.
 */
int LeftDepth(tree treenode) {
    int ret = 0;
    while (!IsNull(treenode)) {
        ++ret;                          // Increment depth for each left child
        treenode = LeftChild(treenode); // Move to the next left child
    }
    return ret;
}

/**
 * External file pointer used for syntax tree printing. - Defined in codegen.c
 * This file pointer (`treelst`) is used to direct the output of the printed tree.
 * It should be initialized in the program before calling any tree printing functions.
 */
extern FILE *treelst;

/**
 * Array of operation names corresponding to different node operation types in the syntax tree.
 *
 * This array maps operation codes (e.g., AddOp, SubOp) to their human-readable string names.
 * It is primarily used for printing the syntax tree in a readable format.
 */
char *opnodenames[] = {
    "ProgramOp", "BodyOp", "DeclOp", "CommaOp", "ArrayTypeOp", "TypeIdOp",
    "BoundOp", "RecompOp",                                       // Array and record operations
    "ToOp", "DownToOp", "ConstantIdOp", "ProceOp", "FuncOp",     // Loop and function operations
    "HeadOp", "RArgTypeOp", "VargTypeOp", "StmtOp", "IfElseOp",  // Statement-related operations
    "LoopOp", "SpecOp", "RoutineCallOp", "AssignOp", "ReturnOp", // Control flow and assignment
    "AddOp", "SubOp", "MultOp", "DivOp",                         // Arithmetic operations
    "LTOp", "GTOp", "EQOp", "NEOp", "LEOp", "GEOp",              // Comparison operators
    "AndOp", "OrOp",                                             // Logical operators
    "UnaryNegOp", "NotOp",                                       // Unary operations
    "VarOp", "SelectOp", "IndexOp", "FieldOp",                   // Variable and field access
    "SubrangeOp", "ExitOp",                                      // Range and control operations
    "ClassOp", "MethodOp", "ClassDefOp"                          // Class and method operations
};

/**
 * Array used for managing indentation and line connections during tree printing.
 *
 * The `crosses` array tracks vertical connectors when printing the syntax tree.
 * It ensures proper formatting for hierarchical node relationships.
 */
static int crosses[162];

/**
 * Prints indentation and branch connectors for the syntax tree visualization.
 *
 * @param x The current depth level in the syntax tree.
 *          Determines how much to indent and where to place branch connectors.
 */
void indent(int x) {
    register int i;

    // Print vertical connectors or spaces for each depth level
    for (i = 0; i < x; i++) {
        fprintf(treelst, "%s", crosses[i] ? "| " : "  "); // Print "| " if branch continues, else space
    }

    // Print branch symbol: "+-" for child nodes, "R-" for the root node
    fprintf(treelst, "%s", x ? "+-" : "R-");

    // Toggle the crossing state to alternate the branch connection lines
    if (x)
        crosses[x] = (crosses[x] + 1) % 2;
}

/**
 * Resets the `crosses` array to zero.
 *
 * This function clears all branch connection indicators used in the syntax tree printing.
 * It ensures that no vertical connectors (`|`) are carried over when printing a new tree.
 */
void zerocrosses() {
    register int i;

    for (i = 0; i < 162; i++) {
        crosses[i] = 0; // Reset each entry to 0 (no branch connection)
    }
}

/**
 * External string table that stores all identifiers and string constants.
 *
 * This table is populated during lexical analysis and is used to retrieve
 * identifier names or string constants by their index.
 */
extern char strg_tbl[];

/**
 * Retrieves an identifier's name from the string table.
 *
 * @param i The index in the string table where the identifier's name is stored.
 */
char *getname(int i) {
    return (strg_tbl + i); // Return the identifier name at the given index
}

/**
 * Retrieves a string constant from the string table.
 *
 * @param i The index in the string table where the string constant is stored.
 */
char *getstring(int i) {
    return (strg_tbl + i); // Return the string constant at the given index
}

/**
 * Recursively prints the syntax tree in a structured and readable format.
 *
 * @param nd    The current tree node to print.
 * @param depth The depth level of the current node (used for indentation).
 */
void printtree(tree nd, int depth) {
    int id, indx;

    // Initialize the printout for the first call (root of the tree)
    if (!depth) {
        zerocrosses(); // Reset the vertical connection indicators
        fprintf(treelst, "************* SYNTAX TREE PRINTOUT ***********\n\n");
    }

    // Print a placeholder for null (dummy) nodes
    if (IsNull(nd)) {
        indent(depth); // Indent according to the depth level
        fprintf(treelst, "[DUMMYnode]\n");
        return;
    }

    // Recursively print the right subtree first for proper visual alignment
    if (NodeKind(nd) == EXPRNode)
        printtree(RightChild(nd), depth + 1);

    indent(depth); // Indent the current node based on depth

    // Print the current node based on its type
    switch (NodeKind(nd)) {
    case IDNode: // Identifier node
        indx = IntVal(nd);
        if (indx >= 0) {
            id = indx;
            fprintf(treelst, "[IDNode,%d,\"%s\"]\n", IntVal(nd), getname(id));
        } else {
            fprintf(treelst, "[IDNode,%d,\"%s\"]\n", indx, "err");
        }
        break;

    case STNode: // Symbol table node
        indx = IntVal(nd);
        if (indx > 0) {
            id = GetAttr(indx, NAME_ATTR);
            fprintf(treelst, "[STNode,%d,\"%s\"]\n", IntVal(nd), getname(id));
        } else {
            fprintf(treelst, "[IDNode,%d,\"%s\"]\n", indx, "err");
        }
        break;

    case INTEGERTNode: // Integer type node
        fprintf(treelst, "[INTEGERTNode]\n");
        break;

    case NUMNode: // Numeric constant node
        fprintf(treelst, "[NUMNode,%d]\n", IntVal(nd));
        break;

    case CHARNode: // Character constant node
        if (isprint(IntVal(nd))) {
            fprintf(treelst, "[CHARNode,%d,\'%c\']\n", IntVal(nd), IntVal(nd));
        } else {
            fprintf(treelst, "[CHARNode,%d,\'\\%o\']\n", IntVal(nd), IntVal(nd));
        }
        break;

    case STRINGNode: // String constant node
        fprintf(treelst, "[STRINGNode,%d,\"%s\"]\n", IntVal(nd), getstring(IntVal(nd)));
        break;

    case EXPRNode: // Expression node
        fprintf(treelst, "[%s]\n", opnodenames[NodeOp(nd) - ProgramOp]);
        break;

    default: // Handle any unknown or invalid node types
        fprintf(treelst, "INVALID!!!\n");
        break;
    }

    // Recursively print the left subtree
    if (NodeKind(nd) == EXPRNode)
        printtree(LeftChild(nd), depth + 1);
}