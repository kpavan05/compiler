#ifndef _NODE_H
#define _NODE_H

#include <stdio.h>
#include <stdbool.h>
#include "compiler.h"
#include "parser.tab.h"
struct type;

enum node_kind {
  NODE_NUMBER,
  NODE_STRING,
  NODE_IDENTIFIER,
  NODE_STATEMENT_LIST,
  NODE_BINARY_OPERATION,
  NODE_EXPRESSION_STATEMENT,
  
  
  /* type specifiers*/
  NODE_VOID,
  NODE_CHAR,
  NODE_SIGNED,
  NODE_UNSIGNED,
  NODE_SIGNED_INT, 
  NODE_SIGNED_LONG,
  NODE_SIGNED_CHAR,
  NODE_SIGNED_SHORT,
  NODE_UNSIGNED_INT,
  NODE_UNSIGNED_CHAR,
  NODE_UNSIGNED_LONG,
  NODE_UNSIGNED_SHORT,
  NODE_SIGNED_LONG_INT,
  NODE_SIGNED_SHORT_INT,
  NODE_UNSIGNED_LONG_INT,
  NODE_UNSIGNED_SHORT_INT,
  
  /* binary operations*/
  
  
  NODE_LESS,    
  NODE_VBAR,
  NODE_PLUS,
  NODE_MINUS,
  NODE_CARET, 
  NODE_SLASH,
  NODE_PERCENT,
  NODE_GREATER,
  NODE_ASTERISK,
  NODE_AMPERSAND,
  NODE_NOT_EQUAL,
  NODE_VBAR_VBAR,
  NODE_LESS_EQUAL,
  NODE_SHIFT_LEFT,
  NODE_SHIFT_RIGHT,
  NODE_EQUAL_EQUAL,
  NODE_GREATER_EQUAL,
  NODE_AMPERSAND_AMPERSAND,
  
  /* ternary operation*/ 
  NODE_FOR_EXPR,
  NODE_TERNARY_OP,

  /*assginment equal operations */
  NODE_EQUAL,
  NODE_VBAR_EQUAL,
  NODE_PLUS_EQUAL,
  NODE_CARET_EQUAL,
  NODE_SLASH_EQUAL,
  NODE_MINUS_EQUAL,
  NODE_PERCENT_EQUAL,
  NODE_ASTERISK_EQUAL,
  NODE_LESS_LESS_EQUAL,
  NODE_AMPERSAND_EQUAL,
  NODE_GREATER_GREATER_EQUAL,
  
  /*unary operations */
  NODE_TILDE,
  NODE_LABEL,
  NODE_INDIRECT, 
  NODE_PRE_INCR,
  NODE_PRE_DECR,
  NODE_POST_INCR,
  NODE_POST_DECR,
  NODE_ADDRESS_OP,
  NODE_UNARY_PLUS,
  NODE_EXCLAMATION,
  NODE_UNARY_MINUS,
   
  /*reserved words */
  NODE_IF,
  NODE_FOR,  
  NODE_GOTO,
  NODE_BREAK,
  NODE_WHILE,
  NODE_RETURN,  
  NODE_IF_ELSE,
  NODE_DOWHILE,
  NODE_CONTINUE,


  NODE_CAST,  
  NODE_DECL,
  NODE_PARAM,
  NODE_TR_UNIT,
  NODE_POINTER,  
  NODE_FUNC_DEF,
  NODE_FUNC_CALL, 
  NODE_FUNC_DECL,
  NODE_STATEMENT,  
  NODE_DECL_LIST,
  NODE_EXPR_LIST,
  NODE_COMMA_EXPR,
  NODE_ARRAY_DECL,
  NODE_PARAM_LIST,
  NODE_ABSTR_DECL,
  NODE_POINTER_DECL,
  NODE_FUNC_DEF_SPEC,
  NODE_COMP_STATEMENT,
  NODE_NULL_STATEMENT,
  NODE_TYPE_CAST,
  NODE_DUMMY
};

enum node_type {
  NODE_NONE =0,
  NODE_UNARY,
  NODE_BINARY,
  NODE_TERNARY
};

struct node {
  enum node_kind kind;
  enum node_type ntype;
  struct location location;
  struct ir_section *ir;

  struct node* parent;
  char node_name[32];

  union {
    struct {
      unsigned long value;
      bool overflow;
      struct result result;
	    int ischar;
    } number;

    struct {
      char name[IDENTIFIER_MAX + 1];
      struct symbol *symbol;
    } identifier;
	
    struct {
      int operation;
      struct node *left_operand;
      struct node *right_operand;
      struct result result;
    } binary_operation;

    struct {
      struct node *expression;
    } expression_statement;

    struct {
      struct node *init;
      struct node *statement;
    } statement_list;

    struct {
	    char* string;
      char* rstring;
      int rlen;
	    int strlen;
      struct string_table *entry;
    } string_literal;
   
   struct {
      int isspecial;
      struct node* child_operand;
      struct result result;
    }unary;

   struct {      
      struct node  *left_operand;
      struct node *right_operand;
      struct result result;
    }binary;
   
   struct {     
     struct node *left_operand;
     struct node *middle_operand;
     struct node *right_operand;
     struct result result;
    }ternary;

  } data;

  
};


enum node_binary_operation {
  BINOP_MULTIPLICATION,
  BINOP_DIVISION,
  BINOP_ADDITION,
  BINOP_SUBTRACTION,
  BINOP_ASSIGN
};

/* Constructors */
struct node *node_number(YYLTYPE location, char *text);
struct node *node_identifier(YYLTYPE location, char *text, int length);
struct node *node_string(YYLTYPE location, char *text, int length);
struct node *node_cconst(YYLTYPE location, char *text, int length);


struct node *node_one_operand(enum node_kind kind, char* name, struct node *child_operand, YYLTYPE location);
struct node *node_two_operands(enum node_kind kind, char* name, struct node* left_operand, 
                               struct node* right_operand, YYLTYPE location);
struct node *node_three_operands(enum node_kind kind, char* name, struct node* left_operand, 
                                 struct node* middle_operand, struct node* right_operand, YYLTYPE location);
void node_append_child_operand(struct node *parent, struct node *child);
int eval_expr(struct node* node, int *val);


struct node *node_binary_operation(YYLTYPE location, enum node_binary_operation operation,
                                   struct node *left_operand, struct node *right_operand);
struct node *node_expression_statement(YYLTYPE location, struct node *expression);
struct node *node_statement_list(YYLTYPE location, struct node *init, struct node *statement);
struct node *node_null_statement(YYLTYPE location);

struct result *node_get_result(struct node *expression);
void node_print_statement_list(FILE *output, struct node *statement_list);
void node_print_parsetree_ouput(FILE *output, struct node *statement);
#endif
