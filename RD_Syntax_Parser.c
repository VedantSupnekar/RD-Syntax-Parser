/*

Vedant Supnekar (R11715927) | 11/11/2022

This is a recursive descent syntax parser in C Programming Language that can validate the syntax of a source code file provided by the user



The grammar of the language is as follows:

P ::= S
S ::= V=E | read(V) | print(V) | do { S } while C | S;S
C ::= E < E | E > E | E == E | E <> E | E <= E | E >= E
E ::= T | E + T | E - T
T ::= F | T * F | T / F | T % F
F ::= (E) | N | V
V ::= a | b | … | y | z | aV | bV | … | yV | zV
N ::= 0 | 1 | … | 8 | 9 | 0N | 1N | … | 8N | 9N



Program returns with an exit code of 0 if the file is free of syntax errors
Program returns with an exit code of 1 if the file contains syntax errors
Program returns with an exit code of 2 if the user has not provided any file
Program returns with an exit code of 3 if the file is given, but it does not exist



This program has been tested successfully on TTU's HPCC Cluster.

*/
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static void error();

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 25

/* Token codes */
#define ASSIGN_OP 2
#define LESSER_OP 3
#define GREATER_OP 4
#define EQUAL_OP 5
#define NEQUAL_OP 6
#define LEQUAL_OP 7
#define GEQUAL_OP 8
#define SEMICOLON 9

#define ADD_OP 10
#define SUB_OP 11
#define MULT_OP 12
#define DIV_OP 13
#define MOD_OP 14
#define IDENT 15
#define INT_LIT 16

#define LEFT_CBRACE 17
#define RIGHT_CBRACE 18
#define LEFT_PAREN 19
#define RIGHT_PAREN 20
#define KEY_READ 21
#define KEY_PRINT 22
#define KEY_WHILE 23
#define KEY_DO 24
#define NOTINLANGUAGE 26

/* Global Variable */
int nextToken;


/* Local Variables */
static int charClass;
static char lexeme[100];
static char nextChar;
static int lexLen;
static FILE *in_fp;
static int SEMICOLON_FLAG = 0;
static int RIGHTCBRACE_FLAG = 0;
static int COMP_FLAG = 0;
static int ERROR_FLAG = 0;
static int FLAG = 0;


/* Local Function declarations */
static void addChar();
static void getChar();
static void getNonBlank();

/**/
/* main driver */
int main(int argc, char *argv[]) {
  printf("DanC Analyzer :: Vedant Supnekar\n\n");
  //defining string array for token names. 
  char *a[26];
  a[0] = "LETTER";
  a[1] = "DIGIT";
  a[2] = "ASSIGN_OP";
  a[3] = "LESSER_OP";
  a[4] = "GREATER_OP";
  a[5] = "EQUAL_OP";
  a[6] = "NEQUAL_OP";
  a[7] = "LEQUAL_OP";
  a[8] = "GEQUAL_OP";
  a[9] = "SEMICOLON";
  a[10] = "ADD_OP";
  a[11] = "SUB_OP";
  a[12] = "MULT_OP";
  a[13] = "DIV_OP";
  a[14] = "MOD_OP";
  a[15] = "IDENT";
  a[16] = "INT_LIT";
  a[17] = "LEFT_CBRACE";
  a[18] = "RIGHT_CBRACE";
  a[19] = "LEFT_PAREN";
  a[20] = "RIGHT_PAREN";
  a[21] = "KEY_READ";
  a[22] = "KEY_PRINT";
  a[23] = "KEY_WHILE";
  a[24] = "KEY_DO";
  a[25] = "UNKNOWN";
  /* Open the input data file and process its contents */
  if(argc <= 1){
    //to check if file has been provided. If not, then give an error
    printf("ERROR - file has not been provided\n");
    return 2;
  }
  if ((in_fp = fopen(argv[1], "r")) == NULL) {
    //to check if file is empty, if it is, then give an error
    printf("ERROR - cannot open file \n");
    return 3;
  } else {
    getChar();

    do {
      lex(a);
      statement(a);
      if(ERROR_FLAG == 1){ 
        //ending program if error is found, returning 1
        return 1;
      }

    } while (nextToken != EOF);
  }
  printf("\n\nSyntax Validated\n\n");
  //ending program is error not found, returning 0
  return 0;
}

/***/
/* lookup - a function to lookup operators and parentheses and return the token */
static int lookup(char ch) {
  switch (ch) {

  case '=':
    addChar();
    nextToken = ASSIGN_OP;
    break;
  case '<':
    addChar();
    nextToken = LESSER_OP;

    break;
  case '>':
    addChar();
    nextToken = GREATER_OP;
    break;

  case ';':
    addChar();
    nextToken = SEMICOLON;
    FLAG = 1;
    break;

  case '(':
    addChar();
    nextToken = LEFT_PAREN;
    FLAG = 1;
    break;

  case '{':
    addChar();
    nextToken = LEFT_CBRACE;
    FLAG = 1;
    break;

  case '}':
    addChar();
    nextToken = RIGHT_CBRACE;
    FLAG = 1;
    break;

  case '+':
    addChar();
    nextToken = ADD_OP;

    FLAG = 1;
    break;
  case '-':
    addChar();
    nextToken = SUB_OP;
    FLAG = 1;
    break;
  case '*':
    addChar();
    nextToken = MULT_OP;
    FLAG = 1;
    break;
  case '/':
    addChar();
    nextToken = DIV_OP;
    FLAG = 1;
    break;
  case '%':
    addChar();
    nextToken = MOD_OP;
    FLAG = 1;
    break;
  case ')':
    addChar();
    nextToken = RIGHT_PAREN;
    FLAG = 1;
    break;
  default:
    addChar();
    nextToken = UNKNOWN;
    FLAG = 2;
    break;
  }
  return nextToken;
}

/***/
/* addChar - a function to add nextChar to lexeme */
static void addChar() {
  if (lexLen <= 98) {
    lexeme[lexLen++] = nextChar;
    FLAG = 0;
    lexeme[lexLen] = 0;
  } 
  else {
    //printf("Error encounter: The next lexeme was %s and the next token was %s", lexeme, a[nextToken]);
    //ERROR_FLAG = 1;  
  }
}

/***/
/* getChar - a function to get the next character of input and determine its character class */
static void getChar() {
  if ((nextChar = getc(in_fp)) != EOF) {

    if (isalpha(nextChar))
      charClass = LETTER;
    else if (isdigit(nextChar))
      charClass = DIGIT;

    else {
      charClass = UNKNOWN;
    }
  } 
  else {
    charClass = EOF;
  }
}

/***/
/* getNonBlank - a function to call getChar until it returns a non-whitespace character */
static void getNonBlank() {
  while (isspace(nextChar))
    getChar();
}

//strip - function to remove unncessary blank spaces and new lines attached to the lexeme
void strip(char *s) {
  char *p2 = s;
  while (*s != '\0') {
    if (*s != '\t' && *s != '\n' && *s != ' ') {
      *p2++ = *s++;
    } else {
      ++s;
    }
  }
  *p2 = '\0';
}


/***/
/* lex - a simple lexical analyzer for arithmetic expressions */
int lex(char **a) {
  lexLen = 0;
  getNonBlank();

  switch (charClass) {
  /* Parse identifiers */
  case LETTER:
    addChar();
    getChar();
    while (charClass == LETTER || charClass == DIGIT) {
      addChar();
      getChar();
    }
    if (strcmp(lexeme, "print") == 0) {
      nextToken = KEY_PRINT;
    }
    else if (strcmp(lexeme, "do") == 0) {
      nextToken = KEY_DO;
    }
    else if (strcmp(lexeme, "while") == 0) {
      nextToken = KEY_WHILE;
    }
    else if (strcmp(lexeme, "read") == 0) {
      nextToken = KEY_READ;
    }
    else {
      nextToken = IDENT;
    }
    break;

  /* Parse integer literals */
  case DIGIT:
    addChar();
    getChar();
    while (charClass == DIGIT) {
      addChar();
      getChar();
    }
    nextToken = INT_LIT;
    break;

  /* Parentheses and operators */
  case UNKNOWN:
    lookup(nextChar);
    getChar();

    if (charClass == UNKNOWN && FLAG == 0) {
      if ((lexeme[0] == '=') && (nextChar == '=')) {
        addChar();
        getChar();
      } else if ((lexeme[0] == '<') && (nextChar == '=' || nextChar == '>')) {
        addChar();
        getChar();
      } else if ((lexeme[0] == '>') && (nextChar == '=')) {
        addChar();
        getChar();
      }
    }

    if (strcmp(lexeme, "==") == 0) {
      nextToken = EQUAL_OP;
    } else if (strcmp(lexeme, "<=") == 0) {
      nextToken = LEQUAL_OP;
    } else if (strcmp(lexeme, ">=") == 0) {
      nextToken = GEQUAL_OP;
    } else if (strcmp(lexeme, "<>") == 0) {
      nextToken = NEQUAL_OP;
    } else if (FLAG == 2) {
      nextToken = UNKNOWN;
      printf("Error encounter: The next lexeme was %s and the next token was %s", lexeme, a[nextToken]);
      ERROR_FLAG = 1;  
      return 1;
    } 
    break;

  case NOTINLANGUAGE:
    lookup(nextChar);
    break;
  /* EOF */
  case EOF:
    nextToken = EOF;
    lexeme[0] = 'E';
    lexeme[1] = 'O';
    lexeme[2] = 'F';
    lexeme[3] = 0;
    break;
  } /* End of switch */
  strip(lexeme);
  return nextToken;
} /* End of function lex */


//function to recognize statements and parse through them
void statement(char **a) {

  SEMICOLON_FLAG = 0;
  //checking for read in statement
  if (nextToken == KEY_READ) {
    lex(a);
    if (nextToken == LEFT_PAREN) {
      lex(a);
      if (nextToken == IDENT) {
        lex(a);
        if (nextToken == RIGHT_PAREN) {
          lex(a);
        }
        else {
          printf("Error encounter: The next lexeme was %s and the next token was %s",lexeme, a[nextToken]);
          ERROR_FLAG = 1;
          return 1;
        }
      } 
      else {
        printf("Error encounter: The next lexeme was %s and the next token was %s", lexeme, a[nextToken]);
        ERROR_FLAG = 1;
        return 1;
      }
    } 
    else {
      printf("Error encounter: The next lexeme was %s and the next token was %s",lexeme, a[nextToken]);
      ERROR_FLAG = 1;
      return 1;
    }
  }

  //checking for print statement
  else if (nextToken == KEY_PRINT) {
    lex(a);
    if (nextToken == LEFT_PAREN) {
      lex(a);
      if (nextToken == IDENT) {
        lex(a);
        if (nextToken == RIGHT_PAREN) {
          lex(a);
        } 
        else {
          printf("Error encounter: The next lexeme was %s and the next token was %s",lexeme, a[nextToken]);
          ERROR_FLAG = 1;
          return 1;
        }
      } 
      else {
        printf("Error encounter: The next lexeme was %s and the next token was %s",lexeme, a[nextToken]);
        ERROR_FLAG = 1;
        return 1;
      }
    } 
    else {
      printf("Error encounter: The next lexeme was %s and the next token was %s", lexeme, a[nextToken]);
      ERROR_FLAG = 1;
      return 1;
    }
  }

  //checking for do while statement
  else if (nextToken == KEY_DO) {
    lex(a);
    if (nextToken == LEFT_CBRACE) {
      lex(a);
      statement(a);
      while (nextToken != RIGHT_CBRACE) {
        statement(a);
      }
      lex(a);
      if (nextToken == KEY_WHILE) {
        lex(a);
        comp(a);
      } 
      else {
        printf("Error encounter: The next lexeme was %s and the next token was %s",lexeme, a[nextToken]);
        ERROR_FLAG = 1;
        return 1;
      }
    }
    else {
      printf("Error encounter: The next lexeme was %s and the next token was %s",lexeme, a[nextToken]);
      ERROR_FLAG = 1;
      return 1;
    }
    SEMICOLON_FLAG = 0;
    RIGHTCBRACE_FLAG = 0;
  }

  //checking for V = E
  else if (nextToken == IDENT) {
    lex(a);
    if (nextToken == ASSIGN_OP){ 
      lex(a);
      expr(a);
    }
  } 
  
  //if the statement ends with semicolon, consider next statement
  else if (nextToken == SEMICOLON) {
    lex(a);
    statement(a);
  }

  //if the statement starts with none of the above elements, you have an error
  else {
    printf("Error encounter: The next lexeme was %s and the next token was %s",lexeme, a[nextToken]);
    ERROR_FLAG = 1;
    return 1;
  }
}

/* comp
 * Parses strings in the language generated by the rule:
 * <comp> -> <expr> {(< | > | == | <> | <= | >=) <expr>}
 */
void comp(char **a) {
  expr(a);
  COMP_FLAG = 1;

  while (nextToken == LESSER_OP || nextToken == GREATER_OP ||
         nextToken == EQUAL_OP || nextToken == NEQUAL_OP ||
         nextToken == LEQUAL_OP || nextToken == GEQUAL_OP) {
    lex(a);
    expr(a);
  }
  COMP_FLAG = 0;
} /* End of function comp */

/* expr
 * Parses strings in the language generated by the rule:
 * <expr> -> <term> {(+ | -) <term>}
 */
void expr(char **a) {
  /* Parse the first term */
  term(a);

  /* As long as the next token is + or -, get the next token and parse the next term */
  while (nextToken == ADD_OP || nextToken == SUB_OP) {
    lex(a);
    term(a);
  }
} /* End of function expr */

/* term
 * Parses strings in the language generated by the rule:
 * <term> -> <factor> {(* | /) <factor>)
 */
void term(char **a) {
  /* Parse the first factor */
  factor(a);
  /* As long as the next token is * or /, get the next token and parse the next factor */
  while (nextToken == MULT_OP || nextToken == DIV_OP || nextToken == MOD_OP) {
    lex(a);
    factor(a);
  }
} /* End of function term */

/* factor
 * Parses strings in the language generated by the rule:
 * <factor> -> id | int_constant | ( <expr )
 * */
void factor(char **a) {
  /* Determine which RHS */
  if (nextToken == IDENT || nextToken == INT_LIT) {
    lex(a); /* Get the next token */
    if (nextToken == SEMICOLON) {
      SEMICOLON_FLAG = 1;
    }
    else if (nextToken == RIGHT_CBRACE) {
      RIGHTCBRACE_FLAG = 1;
    } 
    else if (COMP_FLAG == 1) {
      if (nextToken == LESSER_OP || nextToken == GREATER_OP ||
          nextToken == EQUAL_OP || nextToken == NEQUAL_OP ||
          nextToken == LEQUAL_OP || nextToken == GEQUAL_OP) {
        printf("Error encounter: The next lexeme was %s and the next token was %s", lexeme, a[nextToken]);
        ERROR_FLAG = 1;
        return 1;
      }
    }
  } 
  
  else {
    /* If the RHS is (<expr>), call lex to pass over the left parenthesis, call expr, and check for the right parenthesis */
    if (nextToken == LEFT_PAREN) {
      lex(a);
      expr(a);

      if (nextToken == RIGHT_PAREN) {
        lex(a);
      } 
      else {
        error(a);
      }
    } /* End of if (nextToken == ... */
    /* It was not an id, an integer literal, or a left parenthesis */
    else {
      error(a);
    }
  } /* End of else */
  ;
} /* End of function factor */

static void error(char **a) {
  printf("Error encounter: The next lexeme was %s and the next token was %s",lexeme, a[nextToken]);
  ERROR_FLAG = 1;
  return 1;
}
