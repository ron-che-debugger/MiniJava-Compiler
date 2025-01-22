/***************************************************************
 * This file implements a hash table for                       *
 * efficient storage and retrieval of identifiers and string   *
 * constants during lexical analysis in a compiler.            *
 *                                                             *
 * Key Components:                                             *
 *   1. **Hash Table (`hash_tbl`)**:                           *
 *      - Stores metadata about tokens using the PJW hash.     *
 *      - Handles collisions using separate chaining.          *
 *                                                             *
 *   2. **String Table (`strg_tbl`)**:                         *
 *      - Stores the actual text of identifiers and strings.   *
 *      - Uses a continuous character array with separators.   *
 *                                                             *
 *   3. **Hash Function (`hashpjw`)**:                         *
 *      - Computes hash values using the Peter J. Weinberger   *
 *        (PJW) hashing algorithm for even distribution.       *
 *                                                             *
 *   4. **Insertion (`install_id`)**:                          *
 *      - Inserts tokens into the hash table and string table. *
 *      - Prevents duplicates and manages escape sequences.    *
 *                                                             *
 * Example: Hashing and Handling Collision for Strings "cat"   *
 *          and "bat" (TBL_LEN = 101)                          *
 * ----------------------------------------------------------- *
 * Step-by-step demonstration of hash collision handling:      *
 *                                                             *
 *  Inserting "cat":                                           *
 *  ----------------                                           *
 *  - Compute hash: `"cat"` → hashpjw("cat") → index `5`       *
 *  - `hash_tbl[5]` is **empty**, so create a new entry:       *
 *      id = IDnum                                             *
 *      len = 3                                                *
 *      index = 0 (start in `strg_tbl`)                        *
 *  - Store `"cat"` in `strg_tbl`:                             *
 *      strg_tbl = ['c', 'a', 't', 0, ...]                     *
 *                                                             *
 *  Inserting "bat" (Collision Occurs):                        *
 *  -----------------------------------                        *
 *  - Compute hash: `"bat"` → hashpjw("bat") → index `5`       *
 *  - `hash_tbl[5]` already contains `"cat"` → **collision**   *
 *  - Create a new `hash_ele` entry for `"bat"`:               *
 *      id = IDnum                                             *
 *      len = 3                                                *
 *      index = 4 (next available in `strg_tbl`)               *
 *  - Store `"bat"` in `strg_tbl`:                             *
 *      strg_tbl = ['c', 'a', 't', 0, 'b', 'a', 't', 0, ...]   *
 *  - **Chaining**: Link `"bat"` to the existing `"cat"` node: *
 *                                                             *
 *      hash_tbl[5] → [cat | index: 0] → [bat | index: 4] → NULL *
 *                                                             *
 *  **Lookup Process:**                                        *
 *  -------------------                                        *
 *  - Searching for `"bat"`:                                   *
 *      1. Compute hash → index `5`                            *
 *      2. Check first node → `"cat"` ≠ `"bat"`                *
 *      3. Follow `next` pointer → `"bat"` matches → found!    *
 *                                                             *
 *  **Result:**                                                *
 *  - Both `"cat"` and `"bat"` coexist in the same bucket.     *
 *  - **Chaining** ensures both are accessible despite collision. *
 ***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TBL_LEN 101 /* Length of the hash table used for storing identifiers and strings. \
                     * A prime number is chosen to reduce hash collisions. */

#define STRTBL_LEN 3001 /* Length of the string table for storing actual text of identifiers \
                         * and string constants. A larger prime number reduces overflow risk. */

#define STR_SPRTR 0 /* String separator used in the string table to mark the end of each entry. \
                     * Helps distinguish between different strings stored consecutively. */

/**
 * It holds the number of characters in the currently matched token.
 */
extern int yyleng;
extern long yylval;

/**
 * Structure representing an entry in the hash table.
 *
 * Used to store identifiers or string constants along with their metadata.
 */
struct hash_ele {
    int id;                // Token ID (e.g., ICONSTnum for constants or IDnum for identifiers)
    int len;               // Length of the token (number of characters)
    int index;             // Starting index in the string table where the token is stored
    struct hash_ele *next; // Pointer to the next entry in case of a hash collision (chaining)
};

/**
 * Hash table for storing identifiers and string constants.
 *
 * The table uses separate chaining to handle collisions.
 */
struct hash_ele *hash_tbl[TBL_LEN]; // Array of pointers to hash table entries

/**
 * String table for storing the actual text of identifiers and string constants.
 *
 * All strings are stored contiguously in this array, with a separator marking the end of each string.
 */
char strg_tbl[STRTBL_LEN]; // String table with fixed size

/**
 * Tracks the current end of the string table.
 *
 * Points to the next available index in `strg_tbl` where new strings can be inserted.
 */
int last = 0; // Initially, the string table is empty

/**
 * Initializes the hash table by setting all entries to NULL.
 *
 * This prepares the hash table for use by clearing any existing data.
 * Each entry in the table initially points to NULL, indicating that
 * no identifiers or strings have been stored yet.
 */
void init_hash_tbl() {
    int i;
    for (i = 0; i < TBL_LEN; i++) {
        hash_tbl[i] = NULL; // Clear each entry in the hash table
    }
}

/**
 * Initializes the string table by setting all characters to 0.
 *
 * This clears the string table to ensure no leftover data interferes
 * with new string entries. It effectively resets the table to an empty state.
 */
void init_string_tbl() {
    int i;
    for (i = 0; i < STRTBL_LEN; i++) {
        strg_tbl[i] = 0; // Clear each position in the string table
    }
}

/**
 * Prints the contents of the hash table.
 *
 * This function iterates through the hash table and displays each entry's
 * token ID, token length, and the index where the string is stored in the
 * string table. It also traverses linked lists to handle hash collisions.
 */
void prt_hash_tbl() {
    int i;
    struct hash_ele *p;

    printf("TokenID\tTokenLen\tIndex\tNext...\n"); // Table header

    for (i = 0; i < TBL_LEN; i++) {
        p = hash_tbl[i]; // Get the head of the linked list at index i

        // Traverse the linked list for collision handling
        while (p != NULL) {
            printf("%d\t%d\t%d\t\t", p->id, p->len, p->index); // Print entry details
            p = p->next;                                       // Move to the next entry in the list
        }
        printf("\n"); // Newline after each bucket
    }
}

/**
 * Prints the contents of the string table.
 *
 * This function iterates through the string table and prints all stored
 * characters. If it encounters a separator (-1), it prints a space to
 * visually separate strings.
 */
void prt_string_tbl() {
    int i;

    for (i = 0; i < last; i++) {
        if (strg_tbl[i] == -1)
            printf(" "); // Print a space for string separators
        else
            printf("%c", strg_tbl[i]); // Print the stored character
    }
    printf("\n");
}

/**
 * Computes a hash value for a given string using the PJW (Peter J. Weinberger) hash algorithm.
 * This function ensures even distribution of hash values and minimizes collisions.
 *
 * @param s Pointer to the input string to be hashed.
 * @param l The length of the input string.
 * @return The computed hash value, constrained to the size of the hash table.
 *
 * Example: Hashing the string "abc" with TBL_LEN = 101
 * -----------------------------------------------------
 * Step-by-step calculation:
 *
 * Initial state:
 *     h = 0
 *
 * Processing character 'a' (ASCII 97):
 *     h = (0 << 4) + 97  → 97
 *     No high bits set, skip overflow handling.
 *
 * Processing character 'b' (ASCII 98):
 *     h = (97 << 4) + 98 → (1552) + 98 → 1650
 *     No high bits set, skip overflow handling.
 *
 * Processing character 'c' (ASCII 99):
 *     h = (1650 << 4) + 99 → (26400) + 99 → 26499
 *     No high bits set, skip overflow handling.
 *
 * Final step:
 *     hash = 26499 % 101 → 72
 *
 * Result:
 *     The string "abc" maps to bucket 72 in the hash table.
 */
int hashpjw(char *s, int l) {
    int i;          // Loop counter
    char *p;        // Pointer to iterate through the string
    unsigned h = 0; // Hash value (accumulator)
    unsigned g;     // Temporary variable for high-order bits

    // Iterate over each character in the input string
    for (i = 0, p = s; i < l; p = p + 1, i++) {
        h = (h << 4) + (*p); // Shift hash left by 4 bits and add current character

        // Check if high-order bits (above 28 bits) are set
        if ((g = h & 0xf0000000)) {
            h = h ^ (g >> 24); // Mix high-order bits back into the hash
            h = h ^ g;         // Clear the high-order bits
        }
    }

    return h % TBL_LEN; // Map the hash value to the table size to avoid overflow
}

/**
 * Inserts an identifier or string constant into the hash table and updates the string table.
 * If the identifier or string already exists, it retrieves its index; otherwise, it stores it.
 *
 * @param text    The input text (identifier or string constant) to insert.
 * @param tokenid The token ID representing the type of the input (e.g., IDnum, SCONSTnum).
 */
void install_id(char *text, int tokenid) {
    int ind, i;
    struct hash_ele *p, *p0;

    yylval = last; // Set yylval to the current end of the string table

    /* Step 1: Compute the hash index for the input text */
    ind = hashpjw(text, yyleng); // Generate hash index using PJW hash function
    p = hash_tbl[ind];           // Get the head of the linked list at that index
    p0 = p;

    /* Step 2: Search for the string in the hash table to avoid duplicates */
    while (p != NULL) {
        // Compare existing strings in the table with the input text
        if (!strncmp((char *)&strg_tbl[p->index], text, yyleng)) {
            yylval = p->index; // If found, update yylval with the existing index
            return;            // Exit as the entry already exists
        }
        p0 = p;
        p = p->next; // Move to the next entry in case of a collision
    }

    /* Step 3: If the text is not found, insert it into the table */
    if (p == NULL) {
        // Check for string table overflow
        if (last + yyleng > STRTBL_LEN) {
            printf("There is not enough space in string table!!!\n");
            exit(0);
        }

        /* Allocate a new hash table entry */
        p = (struct hash_ele *)malloc(sizeof(struct hash_ele)); // Allocate memory for the new entry
        p->id = tokenid;                                        // Set the token ID (e.g., IDnum, SCONSTnum)
        p->len = yyleng;
        p->index = last; // Set the starting index in the string table
        p->next = NULL;  // Initialize next pointer to NULL

        // Insert the new entry into the hash table
        if (hash_tbl[ind] == NULL)
            hash_tbl[ind] = p; // Directly insert if no collision
        else
            p0->next = p; // Handle collision by chaining

        /* Step 4: Copy the input text into the string table */
        i = 0;
        while (i < yyleng) {
            // Handle escape sequences for string constants
            if (text[i] != '\\') {
                strg_tbl[last] = text[i]; // Directly copy the character
            } else {
                i++;
                switch (text[i]) {
                case 't':
                    strg_tbl[last] = '\t';
                    break; // Tab character
                case 'n':
                    strg_tbl[last] = '\n';
                    break; // Newline character
                case '\\':
                    strg_tbl[last] = '\\';
                    break; // Backslash
                case '\'':
                    strg_tbl[last] = '\'';
                    break; // Single quote
                default:
                    strg_tbl[last] = '\\';
                    i--; // Unrecognized escape, store backslash
                }
            }

            i++;
            last++; // Move to the next position in the string table
        }

        /* Step 5: Mark the end of the string in the string table */
        strg_tbl[last] = STR_SPRTR; // Add a separator to mark the end of the string
        last++;                     // Move the pointer forward for the next insertion
    }
}

/*
 * loc_str(): Searches for a string in the string table.
 * ----------------------------------------------------
 * This function checks if a given string is already stored in the global string table (`strg_tbl`).
 *
 * Parameters:
 *   - string: A pointer to the string to search for in the string table.
 *
 * Returns:
 *   - The **index** of the string in the string table if it is found.
 *   - **-1** if the string is not found in the string table.
 *
 * Note:
 *   - The string table (`strg_tbl`) stores strings consecutively, with each string
 *     **separated by `STR_SPRTR` (defined as `0`)**.
 *   - This separator allows for efficient storage and traversal of multiple strings.
 */
int loc_str(char *string) {
    int i = 0; /* Initialize index to start at the beginning of the string table */

    /* Step 1: Iterate through the string table */
    while (i < last) { // 'last' marks the end of the used portion of the string table

        /* Step 2: Compare the current string in the table with the target string */
        if (strcmp((char *)&strg_tbl[i], string) == 0) {
            return i; // If the string is found, return its starting index in the string table
        }

        /* Step 3: Move to the next stored string */
        i += strlen((char *)&strg_tbl[i]) + 1; // Skip to the next string (past the separator)
    }

    /* Step 4: String not found in the table */
    return -1; // Return -1 if the string is not stored in the string table
}