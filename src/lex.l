%{
/* #include <stdlib.h> */
/* #include "token.h" */

/* External variables for tracking the current position in the source code */
extern int yycolumn, yylength, yyline;

/* Function to install string constants and identifiers into the string table */
extern void install_id(); /* install string constants & identifiers into string tbl */

/* Forward declarations for helper functions */
void tolowercase();
void ReportError(char* msg);
void match();
%}

/* -------------------- Regular Definitions -------------------- */

/* Delimiters: space, tab, and carriage return */
delim         [ \t\r]

/* Whitespace: one or more delimiters */
ws            {delim}+

/* Newline character */
newline       [\n]

/* Reserved words in MINI-JAVA, case-insensitive matching */

/* 'declarations' keyword */
declarations [dD][eE][cC][lL][aA][rR][aA][tT][iI][oO][nN][sS]

/* 'enddeclarations' keyword */
enddeclarations [eE][nN][dD][dD][eE][cC][lL][aA][rR][aA][tT][iI][oO][nN][sS]

/* 'iint' keyword */
iint         [iI][nN][tT]

/* 'method' keyword */
method      [mM][eE][tT][hH][oO][dD]

/* 'program' keyword */
program     [pP][rR][oO][gG][rR][aA][mM]

/* 'val' keyword */
val         [vV][aA][lL]

/* 'while' keyword */
while       [wW][hH][iI][lL][eE]

/* 'class' keyword */
class       [cC][lL][aA][sS][sS]

/* 'class' keyword */
else        [eE][lL][sS][eE]

/* 'if' keyword */
if          [iI][fF]

/* 'return' keyword */
return      [rR][eE][tT][uU][rR][nN]

/* 'void' keyword */
void        [vV][oO][iI][dD]

/* -------------------- Identifier and Constants -------------------- */

/* Identifiers: starts with a letter, followed by letters or digits */
identifier    [A-Za-z][A-Za-z0-9]*

/* Integer constants: a sequence of digits */
int           [0-9]+

/* Invalid identifier: digits followed by a valid identifier */
errid         [0-9]+{identifier}

/* String constants: characters within single quotes, supporting escape sequences */
strcnst       '([^'\n\\]*(\\.)?)*'

/* Error in string constants: unclosed string ending with a newline */
errstr        '([^'\n\\]*(\\.)?)*[\n]

/* Start of a multi-line comment */
slashstar     \/\*

/* Error in comments: closing comment without an opening comment*/
errcomm1      \*\/

%%
{ws}                {
                      /*
                       * Handles whitespace characters (spaces, tabs, carriage returns). 
                       * Updates the current column position (`yycolumn`). 
                       */
                      int i;
                      for ( i=0; i<yyleng; i++ ) // Iterate over each matched whitespace character in yytext
                      {
                        /*
                         * Handles a tab character (`\t`).
                         * In most systems, a tab is equivalent to moving to the next **tab stop**.
                         * Tab stops are usually set every 8 columns.
                         *
                         * The expression:
                         *     yycolumn = yycolumn + 8 - (yycolumn & 0x7);
                         * aligns the column to the next multiple of 8.
                         *
                         * Breakdown:
                         *   - (yycolumn & 0x7): Gets the remainder when `yycolumn` is divided by 8 (bitwise AND with 7).
                         *   - 8 - (yycolumn & 0x7): Calculates how many columns to move forward to reach the next tab stop.
                         *   - Adding this to `yycolumn` moves the cursor to the next tab stop.
                         *
                         * Example:
                         *   - If `yycolumn` is 3 → `yycolumn += 8 - 3` → `yycolumn = 8`
                         *   - If `yycolumn` is 9 → `yycolumn += 8 - 1` → `yycolumn = 16`
                         */
                        if ( yytext[i]=='\t' )
                          yycolumn = yycolumn + 8 - (yycolumn&0x7);
                        else
                          /*
                           * For spaces or carriage returns, simply move the column one step forward.
                           */
                          yycolumn++;
                      }
                    }

{newline}           {
                     /*
                      * Newline handler: move to the next line and reset column counter 
                      */
                     yyline++; yycolumn=0;}

"&&"                {yycolumn += yyleng; return(ANDnum);}


":="                {yycolumn += yyleng; return(ASSGNnum);}

{declarations}      {
                     yycolumn += yyleng; 
                     return(DECLARATIONSnum);
                    } 

"."                 {yycolumn += yyleng; return(DOTnum);}

{enddeclarations}   {
                     yycolumn+= yyleng;
                     return(ENDDECLARATIONSnum);
                    }

{method}            {yycolumn += yyleng; tolowercase(); return(METHODnum);}
{program}           {yycolumn += yyleng; tolowercase(); return(PROGRAMnum);}
{val}               {yycolumn += yyleng; tolowercase(); return(VALnum);}
{while}             {yycolumn += yyleng; tolowercase(); return(WHILEnum);}
{class}             {yycolumn += yyleng; tolowercase(); return(CLASSnum);}
{else}              {yycolumn += yyleng; tolowercase(); return(ELSEnum);}
{if}                {yycolumn += yyleng; tolowercase(); return(IFnum);}
{iint}              {yycolumn += yyleng; tolowercase(); return(INTnum);}
{return}            {yycolumn += yyleng; tolowercase(); return(RETURNnum);}
{void}              {yycolumn += yyleng; return(VOIDnum);}

{identifier}        {
                     yycolumn += yyleng;
                     tolowercase();
                     install_id(yytext, IDnum);
                     return(IDnum);}

{errid}             {ReportError("wrong identifier"); yycolumn += yyleng;}

"["                 {yycolumn += yyleng; return(LBRACnum);}
"("                 {yycolumn += yyleng; return(LPARENnum);}
"]"                 {yycolumn += yyleng; return(RBRACnum);}
")"                 {yycolumn += yyleng; return(RPARENnum);}

"{"                 {yycolumn += yyleng; return(LBRACEnum);}
"}"                 {yycolumn += yyleng; return(RBRACEnum);}

"!="                {yycolumn += yyleng; return(NEnum);}

"||"                {yycolumn += yyleng; return(ORnum);}

";"                 {yycolumn += yyleng; return(SEMInum);}

","                 {yycolumn += yyleng; return(COMMAnum);}

"/"                 {yycolumn += yyleng; return(DIVIDEnum);}

"=="                {yycolumn += yyleng; return(EQnum);}

">="                {yycolumn += yyleng; return(GEnum);}

{int}               {yycolumn += yyleng; yylval.intg = atoi(yytext); return(ICONSTnum);}

"="                 {yycolumn += yyleng; return(EQUALnum);}

">"                 {yycolumn += yyleng; return(GTnum);}

"<="                {yycolumn += yyleng; return(LEnum);}

"<"                 {yycolumn += yyleng; return(LTnum);}

"-"                 {yycolumn += yyleng; return(MINUSnum);}

"!"                 {yycolumn += yyleng; return(NOTnum);}

"+"                 {yycolumn += yyleng; return(PLUSnum);}

"*"                 {yycolumn += yyleng; return(TIMESnum);}

{strcnst}           {yycolumn += yyleng;
                     install_id( yytext, yyleng);
                     return(SCONSTnum);}

{errstr}            {ReportError("string doesn't end"); yyline++; yycolumn=0;}

{slashstar}         { yycolumn += yyleng; match(); }

{errcomm1}          {yycolumn++; ReportError("comment without beginning");}
.                   {ReportError("unknown string"); printf("--%s**\n", yytext); while (input() != '\n'); }
%%

/* -------------------- Helper Functions -------------------- */
/**
 * tolowercase()
 * -----------------
 * Converts all **uppercase letters** in the current matched text (`yytext`) 
 * to **lowercase**. This ensures case-insensitive handling of identifiers 
 * and keywords.
 *
 * Example:
 *   Input:  "HELLO"
 *   Output: "hello"
 *
 * Note:
 *   - `yytext` holds the currently matched token.
 *   - `yyleng` holds the length of `yytext`.
 */
void tolowercase()
{
/* transform an identifier to lower case representation */
  int i;
  for( i=0; i<yyleng; i++ )
  {
     if ( 'A'<=yytext[i] && yytext[i]<='Z' )
        yytext[i] = yytext[i] - 'A' + 'a';  // Convert to lowercase
  }
}

void ReportError(char* msg)
{
  printf("ERROR at line %d column %d: %s\n", yyline, yycolumn, msg);
}

/**
 * match()
 * -----------------
 * This function is used to handle multi-line comments starting with left enclosed comment symbol.
 * It scans through the input until it finds the comment's closing sequence right enclosed comment symbol. 
 * 
 * Behavior:
 *   - Correctly updates yyline and yycolumn for accurate error reporting.
 *   - Reports an error if the comment is not properly closed before EOF.
 * 
 * Edge Cases Handled:
 *   - Nested asterisks (*) before the closing /.
 *   - Newlines within comments (increments yyline and resets yycolumn).
 *   - Unexpected EOF without closing the comment (right enclosed comment symbol).
 *
 * Example:
 *   Input:  (left enclosed comment) this is a comment (right enclosed comment)
 *   Skips content until right enclosed symbol is found.
 */
void match()
{
   char nextchar;

   do{
     // 'input()' is a built-in function provided by Lex/Flex
     // - reads the next character from the input stream.
     // - returns the next character as an int.
     // - if the end of input (EOF) is reached, it returns 0.
     nextchar = input();      // Read the next character
     yycolumn++;              // Increment column counter

     switch ( nextchar ){ 
     case '*':  nextchar = input();
                if ( nextchar == '/')  // Look ahead to check if the next character is '/'
                {
                  yycolumn++;          // Matched '*/', exit the comment
                  return;
                }
                break;

     case '\n': yyline++;              // Handle new line inside comment
                yycolumn=0;            // Reset column count at the start of a new line
                break;

     case 0:    ReportError("Fatal error: unexpected EOF found in comment"); // End of file (EOF)
                exit(0);                                                     // Terminate program due to fatal error   

     default:   break;    // Any other character, continue scanning
     }
   } while(1);           // Infinite loop until '*/' is found or EOF occurs
}