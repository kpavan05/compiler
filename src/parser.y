%verbose
%debug
%defines
%locations
%pure-parser
%lex-param {yyscan_t scanner}
%parse-param {YYSTYPE *root}
%parse-param {int *error_count}
%parse-param {yyscan_t scanner}
%token-table

%{
  #include <stdio.h>

  #include "compiler.h"
  #include "parser.tab.h"
  #include "scanner.yy.h"
  #include "node.h"

  #define YYERROR_VERBOSE
  static void yyerror(YYLTYPE *loc, YYSTYPE *root,
                      int *error_count, yyscan_t scanner,
                      char const *s);
%}

%token IDENTIFIER NUMBER STRING

%token BREAK CHAR CONTINUE DO FOR GOTO WHILE
%token INT LONG RETURN SHORT SIGNED UNSIGNED VOID 
%token LEFT_PAREN LEFT_SQUARE RIGHT_SQUARE LEFT_CURLY RIGHT_CURLY PERIOD
%right IF RIGHT_PAREN
%right ELSE 
%token SEMICOLON SINGLE_QUOTE DOUBLE_QUOTE  BACK_SLASH
%left  COMMA
%right AMPERSAND_EQUAL GREATER_GREATER_EQUAL LESS_LESS_EQUAL VBAR_EQUAL CARET_EQUAL  
%right EQUAL MINUS_EQUAL PERCENT_EQUAL PLUS_EQUAL ASTERISK_EQUAL SLASH_EQUAL 
%right QUESTION COLON
%left  AMPERSAND_AMPERSAND VBAR_VBAR
%left  CARET VBAR
%left  AMPERSAND
%left  EQUAL_EQUAL EXCLAMATION_EQUAL
%left  LESS_EQUAL GREATER_EQUAL LESS GREATER
%left  LESS_LESS GREATER_GREATER
%left  MINUS PLUS
%left  ASTERISK SLASH PERCENT
%right UMINUS UPLUS
%right ADDRESS_OP
%right TILDE EXCLAMATION 
%left  PLUS_PLUS MINUS_MINUS


%start translation_unit

%%



abstract_declarator 
:  pointer {$$ = node_two_operands(NODE_POINTER_DECL, "pointer_decl", $1, NULL, yylloc);}
|  direct_abstract_declarator 
|  pointer direct_abstract_declarator {$$ = node_two_operands(NODE_POINTER_DECL, "pointer_decl", $1, $2, yylloc);}
;

additive_expr 
:  multiplicative_expr
|  additive_expr PLUS multiplicative_expr
   {$$ = node_two_operands(NODE_PLUS, "+", $1, $3, yylloc);} 
|  additive_expr MINUS multiplicative_expr
   {$$ = node_two_operands(NODE_MINUS, "-", $1, $3, yylloc);}
;

/*
add_op 
:  PLUS  
|  MINUS 
;
*/

address_expr :  AMPERSAND cast_expr %prec ADDRESS_OP {$$ = node_one_operand(NODE_ADDRESS_OP, "&", $2, yylloc); } 

array_declarator 
:  direct_declarator LEFT_SQUARE constant_expr RIGHT_SQUARE
   {$$ = node_two_operands(NODE_ARRAY_DECL, "arraydecl", $1, $3, yylloc);}
|  direct_declarator LEFT_SQUARE RIGHT_SQUARE
   {$$ = node_two_operands(NODE_ARRAY_DECL, "arraydecl", $1, NULL, yylloc);}

; 
 
assignment_expr 
:  conditional_expr
|  unary_expr EQUAL assignment_expr
   {$$ = node_two_operands(NODE_EQUAL, "=", $1, $3, yylloc);}
|  unary_expr PLUS_EQUAL assignment_expr
   {$$ = node_two_operands(NODE_PLUS_EQUAL, "+=", $1, $3, yylloc);}
|  unary_expr MINUS_EQUAL assignment_expr
   {$$ = node_two_operands(NODE_MINUS_EQUAL, "-=", $1, $3, yylloc);}
|  unary_expr ASTERISK_EQUAL assignment_expr
   {$$ = node_two_operands(NODE_ASTERISK_EQUAL, "*=", $1, $3, yylloc);}
|  unary_expr SLASH_EQUAL assignment_expr
   {$$ = node_two_operands(NODE_SLASH_EQUAL, "/=", $1, $3, yylloc);}
|  unary_expr PERCENT_EQUAL assignment_expr
   {$$ = node_two_operands(NODE_PERCENT_EQUAL, "%=", $1, $3, yylloc);}
|  unary_expr LESS_LESS_EQUAL assignment_expr
   {$$ = node_two_operands(NODE_LESS_LESS_EQUAL, "<<=", $1, $3, yylloc);}
|  unary_expr GREATER_GREATER_EQUAL assignment_expr
   {$$ = node_two_operands(NODE_GREATER_GREATER_EQUAL, ">>=", $1, $3, yylloc);}
|  unary_expr AMPERSAND_EQUAL assignment_expr
   {$$ = node_two_operands(NODE_AMPERSAND_EQUAL, "&=", $1, $3, yylloc);}
|  unary_expr CARET_EQUAL assignment_expr  
   {$$ = node_two_operands(NODE_CARET_EQUAL, "^=", $1, $3, yylloc);}
|  unary_expr VBAR_EQUAL assignment_expr
   {$$ = node_two_operands(NODE_VBAR_EQUAL, "|=", $1, $3, yylloc);}
;

/*
assignment_op 
:  EQUAL  
|  PLUS_EQUAL  
|  MINUS_EQUAL  
|  ASTERISK_EQUAL  
|  SLASH_EQUAL  
|  PERCENT_EQUAL 
|  LESS_LESS_EQUAL  
|  GREATER_GREATER_EQUAL 
|  AMPERSAND_EQUAL  
|  CARET_EQUAL 
|  VBAR_EQUAL 
;
*/
 

 

bitwise_and_expr 
: equality_expr  
| bitwise_and_expr AMPERSAND equality_expr 
  {$$ =  node_two_operands(NODE_AMPERSAND, "&", $1, $3, yylloc);}
;  


bitwise_negation_expr 
:  TILDE cast_expr {$$ = node_one_operand(NODE_TILDE, "~", $2, yylloc);} 
;

bitwise_or_expr 
:  bitwise_xor_expr  
|  bitwise_or_expr VBAR bitwise_xor_expr
   {$$ = node_two_operands(NODE_VBAR, "|", $1, $3, yylloc);} 
;

bitwise_xor_expr 
:  bitwise_and_expr
|  bitwise_xor_expr CARET bitwise_and_expr
   {$$ = node_two_operands(NODE_CARET, "^", $1, $3, yylloc);}
;



break_statement 
:  BREAK SEMICOLON {$$ = node_one_operand(NODE_BREAK, "break", NULL, yylloc);}
;

 

cast_expr 
:  unary_expr  
|  LEFT_PAREN type_name RIGHT_PAREN cast_expr
   {$$ = node_two_operands(NODE_CAST, "cast", $2, $4, yylloc);}
;

/* 
character_constant 
: NUMBER
;
*/

character_type_specifier 
: CHAR {$$ = node_one_operand(NODE_SIGNED_CHAR, "signed char", NULL, yylloc);} 
| SIGNED CHAR {$$ = node_one_operand(NODE_SIGNED_CHAR, "signed char", NULL, yylloc);}
| UNSIGNED CHAR {$$ = node_one_operand(NODE_UNSIGNED_CHAR, "unsigned char", NULL, yylloc);}
;

comma_expr 
: assignment_expr  
| comma_expr COMMA assignment_expr {$$ = node_two_operands(NODE_COMMA_EXPR, ",", $1, $3, yylloc);}

 

compound_statement 
: LEFT_CURLY declaration_or_statement_list RIGHT_CURLY  {yylloc.first_column = @2.first_column;$$ = node_one_operand(NODE_COMP_STATEMENT, "comp_stmt", $2, yylloc);} 
| LEFT_CURLY RIGHT_CURLY {/*$$ = node_null_statement(yylloc);*/ $$ = node_one_operand(NODE_COMP_STATEMENT, "comp_stmt", NULL, yylloc);}
;

conditional_expr 
:  logical_or_expr  
|  logical_or_expr QUESTION expr COLON conditional_expr 
   { $$ = node_three_operands(NODE_TERNARY_OP, "?:", $1, $3, $5, yylloc);} 
;     
   

conditional_statement 
:  if_statement  
|  if_else_statement 
;
/*
constant 
: integer_constant  
| character_constant  
| string_constant
;
*/

constant 
: NUMBER  
| string_constant
;
constant_expr :  conditional_expr 

continue_statement 
:  CONTINUE SEMICOLON
   {$$ = node_one_operand(NODE_CONTINUE, "continue", NULL, yylloc);}
;
 
  

decl 
:  declaration_specifiers initialized_declarator_list SEMICOLON
   {$$ = node_two_operands(NODE_DECL, "decl", $1, $2, yylloc);}
;

 
 

declaration_or_statement
: decl  
| statement 
;

declaration_or_statement_list 
:  declaration_or_statement {$$ = node_two_operands(NODE_STATEMENT_LIST, "stmtlst", NULL, $1, yylloc);}
|  declaration_or_statement_list declaration_or_statement { $$ = node_two_operands(NODE_STATEMENT_LIST, "stmtlst", $1, $2, yylloc);}    
;

declaration_specifiers 
:   type_specifier
; 
 
 
 

declarator 
:  pointer_declarator  
|  direct_declarator 
;

 

direct_abstract_declarator
: LEFT_PAREN abstract_declarator RIGHT_PAREN  {$$= $2;}
| LEFT_SQUARE RIGHT_SQUARE {$$ = node_two_operands(NODE_ABSTR_DECL, "abstr_decl", NULL, NULL, yylloc);}
| LEFT_SQUARE constant_expr RIGHT_SQUARE {$$ = node_two_operands(NODE_ABSTR_DECL, "abstr_decl", NULL, $2, yylloc);}
| direct_abstract_declarator LEFT_SQUARE RIGHT_SQUARE {$$ = node_two_operands(NODE_ABSTR_DECL, "abstr_decl", $1, NULL, yylloc);}
| direct_abstract_declarator LEFT_SQUARE constant_expr RIGHT_SQUARE {$$ = node_two_operands(NODE_ABSTR_DECL, "abstr_decl", $1, $3, yylloc);}
; 

direct_declarator 
:  simple_declarator  
|  LEFT_PAREN declarator RIGHT_PAREN  {$$ = $2;}
|  function_declarator 
|  array_declarator 
;

do_statement 
:  DO statement WHILE LEFT_PAREN expr RIGHT_PAREN SEMICOLON
   {$$ = node_two_operands(NODE_DOWHILE, "dowhile", $2, $5, yylloc);}

 

equality_expr 
:  relational_expr
|  equality_expr EQUAL_EQUAL relational_expr
   {$$ = node_two_operands(NODE_EQUAL_EQUAL, "==", $1, $3, yylloc);}
|  equality_expr EXCLAMATION_EQUAL relational_expr
   {$$ = node_two_operands(NODE_NOT_EQUAL, "!=", $1, $3, yylloc);}
; 

/*
equality_op 
:  EQUAL_EQUAL {$$ = $1;}
|  EXCLAMATION_EQUAL {$$ = $1;}
;
*/

expr :  comma_expr 

expression_list 
:  assignment_expr  { $$ = node_two_operands(NODE_EXPR_LIST, "expr_lst", NULL, $1, yylloc);} 
|  expression_list COMMA assignment_expr { $$ = node_two_operands(NODE_EXPR_LIST, "expr_lst", $1, $3, yylloc);} 
;

expression_statement 
:  expr SEMICOLON {$$ = $1;}
; 

 

for_expr 
: LEFT_PAREN initial_clause SEMICOLON expr SEMICOLON expr RIGHT_PAREN
  {$$ = node_three_operands(NODE_FOR_EXPR, "", $2, $4, $6, yylloc);}
| LEFT_PAREN SEMICOLON expr SEMICOLON expr RIGHT_PAREN
  {$$ = node_three_operands(NODE_FOR_EXPR, "", NULL, $3, $5, yylloc);}
| LEFT_PAREN SEMICOLON SEMICOLON expr RIGHT_PAREN
  {$$ = node_three_operands(NODE_FOR_EXPR, "", NULL, NULL, $4, yylloc);}
| LEFT_PAREN SEMICOLON SEMICOLON RIGHT_PAREN
  {$$ = node_three_operands(NODE_FOR_EXPR, "", NULL, NULL, NULL, yylloc);}
| LEFT_PAREN initial_clause SEMICOLON  SEMICOLON expr RIGHT_PAREN
  {$$ = node_three_operands(NODE_FOR_EXPR, "", $2, NULL, $5, yylloc);}
| LEFT_PAREN initial_clause SEMICOLON expr SEMICOLON RIGHT_PAREN
  {$$ = node_three_operands(NODE_FOR_EXPR, "", $2, $4, NULL, yylloc);}
| LEFT_PAREN initial_clause SEMICOLON SEMICOLON RIGHT_PAREN
  {$$ = node_three_operands(NODE_FOR_EXPR, "", $2, NULL, NULL, yylloc);}
| LEFT_PAREN SEMICOLON expr SEMICOLON RIGHT_PAREN
  {$$ = node_three_operands(NODE_FOR_EXPR, "", NULL, $3, NULL, yylloc);}
;

for_statement 
:  FOR for_expr statement {$$ = node_two_operands(NODE_FOR, "for", $2, $3, yylloc);}
;

function_call 
: postfix_expr LEFT_PAREN expression_list RIGHT_PAREN {$$ = node_two_operands(NODE_FUNC_CALL, "func_call", $1, $3, yylloc);}
| postfix_expr LEFT_PAREN  RIGHT_PAREN {$$ = node_two_operands(NODE_FUNC_CALL, "func_call", $1, NULL, yylloc);}
;

function_declarator 
:  direct_declarator LEFT_PAREN parameter_type_list RIGHT_PAREN 
   { $$ = node_two_operands(NODE_FUNC_DECL, "funcdecl", $1, $3, yylloc);}
; 

function_definition 
:  function_def_specifier compound_statement
   {$$ = node_two_operands(NODE_FUNC_DEF, "funcdef", $1, $2, yylloc); }

function_def_specifier 
: declaration_specifiers declarator
  { $$ = node_two_operands(NODE_FUNC_DEF_SPEC, "funcspec", $1, $2, yylloc); }
;
 
goto_statement 
: GOTO named_label SEMICOLON 
  {$$ = node_one_operand(NODE_GOTO, "goto", $2, yylloc);}
;

 
if_else_statement 
:  IF LEFT_PAREN expr RIGHT_PAREN statement ELSE statement
   {$$ = node_three_operands(NODE_IF_ELSE, "if else", $3, $5, $7, yylloc);}
;

if_statement 
: IF LEFT_PAREN expr RIGHT_PAREN statement
  {$$ = node_two_operands(NODE_IF, "if", $3, $5, yylloc);}
; 

 
 

indirection_expr 
: ASTERISK cast_expr {$$ = node_one_operand(NODE_INDIRECT, "*", $2, yylloc);}
;

initial_clause 
:  expr 
; 

initialized_declarator 
:  declarator 
; 

initialized_declarator_list 
:  initialized_declarator {$$ = node_two_operands(NODE_DECL_LIST, "decl_list", NULL, $1, yylloc);}
|  initialized_declarator_list COMMA initialized_declarator 
   {$$ = node_two_operands(NODE_DECL_LIST, "decl_list", $1, $3, yylloc);}
;
 
/*
integer_constant
: NUMBER
;
*/

integer_type_specifier 
:  signed_type_specifier  
|  unsigned_type_specifier  
|  character_type_specifier 
;                          

iterative_statement :  while_statement  |
                       do_statement  |
                       for_statement 

label :  named_label 
         
         

labeled_statement 
:  label COLON statement
   {$$ = node_two_operands(NODE_LABEL, ":", $1, $3, yylloc);}
; 

logical_and_expr 
:  bitwise_or_expr  
|  logical_and_expr AMPERSAND_AMPERSAND bitwise_or_expr
   {$$ = node_two_operands(NODE_AMPERSAND_AMPERSAND, "&&", $1, $3, yylloc);}
;

logical_negation_expr 
:  EXCLAMATION cast_expr {$$ = node_one_operand(NODE_EXCLAMATION, "!", $2, yylloc);} 
;

logical_or_expr 
:  logical_and_expr  
|  logical_or_expr VBAR_VBAR logical_and_expr
   {$$ = node_two_operands(NODE_VBAR_VBAR, "||", $1, $3, yylloc);}
;
 
 

multiplicative_expr 
:  cast_expr
|  multiplicative_expr ASTERISK cast_expr
   {$$ = node_two_operands(NODE_ASTERISK, "*", $1, $3, yylloc);}
|  multiplicative_expr SLASH cast_expr
   {$$ = node_two_operands(NODE_SLASH, "/", $1, $3, yylloc);}
|  multiplicative_expr PERCENT cast_expr
   {$$ = node_two_operands(NODE_PERCENT, "%", $1, $3, yylloc);}
;

/*
mult_op 
:  ASTERISK  
|  SLASH  
|  PERCENT
; 
*/

named_label 
:  IDENTIFIER
;


null_statement 
:  SEMICOLON
   { $$ = node_null_statement(yylloc);}   
;  
 

parameter_decl 
: declaration_specifiers declarator
  {$$ = node_two_operands(NODE_PARAM, "param", $1, $2, yylloc);}  
| declaration_specifiers
  {$$ = node_two_operands(NODE_PARAM, "param", $1, NULL, yylloc);}
| declaration_specifiers abstract_declarator
  {$$ = node_two_operands(NODE_PARAM, "param", $1, $2, yylloc);} 
;

parameter_list 
:  parameter_decl {$$ = node_two_operands(NODE_PARAM_LIST, "params", NULL, $1, yylloc);}
|  parameter_list COMMA parameter_decl 
   {$$ = node_two_operands(NODE_PARAM_LIST, "params", $1, $3, yylloc);}
;

parameter_type_list 
:  parameter_list 
;                       

parenthesized_expr 
:  LEFT_PAREN expr RIGHT_PAREN 
   {$$ = $2;}
;

pointer 
:  ASTERISK {$$ = node_one_operand(NODE_POINTER, "*", NULL, yylloc);} 
|  ASTERISK pointer  {$$ = node_one_operand(NODE_POINTER, "*", $2, yylloc);}
;

pointer_declarator 
:  pointer direct_declarator
   { $$ = node_two_operands(NODE_POINTER_DECL, "pointerdecl", $1, $2, yylloc);}
;    

postdecrement_expr 
:  postfix_expr MINUS_MINUS {$$ = node_one_operand(NODE_POST_DECR, "--", $2, yylloc);}
;

postfix_expr 
:  primary_expr  
|  subscript_expr  
|  function_call  
|  postincrement_expr  
|  postdecrement_expr 
;

postincrement_expr 
:  postfix_expr PLUS_PLUS {$$ = node_one_operand(NODE_POST_INCR, "++", $2, yylloc);}
;

predecrement_expr 
:  MINUS_MINUS unary_expr {$$ = node_one_operand(NODE_PRE_DECR, "--", $2, yylloc);}
;

preincrement_expr 
:  PLUS_PLUS unary_expr {$$ = node_one_operand(NODE_PRE_INCR, "++", $2, yylloc);}
;

 

primary_expr 
:  IDENTIFIER  
|  constant  
|  parenthesized_expr 
;
 

relational_expr 
:  shift_expr    
| relational_expr LESS shift_expr
  {$$ = node_two_operands(NODE_LESS, "<", $1, $3, yylloc);}
| relational_expr LESS_EQUAL shift_expr
  {$$ = node_two_operands(NODE_LESS_EQUAL, "<=", $1, $3, yylloc);}
| relational_expr GREATER shift_expr
  {$$ = node_two_operands(NODE_GREATER, ">", $1, $3, yylloc);}
| relational_expr GREATER_EQUAL shift_expr
  {$$ = node_two_operands(NODE_GREATER_EQUAL, ">=", $1, $3, yylloc);}
;

/*
relational_op 
:  LESS          {$$ = $1;}
|  LESS_EQUAL    {$$ = $1;}
|  GREATER       {$$ = $1;}
|  GREATER_EQUAL {$$ = $1;}
;
*/

return_statement 
: RETURN expr SEMICOLON {$$ = node_one_operand(NODE_RETURN, "return", $2, yylloc);}
| RETURN SEMICOLON {$$ = node_one_operand(NODE_RETURN, "return", NULL, yylloc);}
;


shift_expr 
:  additive_expr 
|  shift_expr LESS_LESS additive_expr
   {$$ = node_two_operands(NODE_SHIFT_LEFT, "<<", $1, $3, yylloc);}
|  shift_expr GREATER_GREATER additive_expr
   {$$ = node_two_operands(NODE_SHIFT_RIGHT, ">>", $1, $3, yylloc);}
;

/*
shift_op 
: LESS_LESS {$$ = $1;}
| GREATER_GREATER {$$ = $1;}
;
*/

signed_type_specifier 
:  SHORT {$$ = node_one_operand(NODE_SIGNED_SHORT, "signed short", NULL, yylloc);}
|  SHORT INT {$$ = node_one_operand(NODE_SIGNED_SHORT_INT, "signed short", NULL, yylloc);}
|  SIGNED SHORT  {$$ = node_one_operand(NODE_SIGNED_SHORT, "signed short int", NULL, yylloc);}
|  SIGNED SHORT INT {$$ = node_one_operand(NODE_SIGNED_SHORT_INT, "signed short int", NULL, yylloc);}
|  INT {$$ = node_one_operand(NODE_SIGNED_INT, "signed int", NULL, yylloc);}
|  SIGNED INT {$$ = node_one_operand(NODE_SIGNED_INT, "signed int", NULL, yylloc);}
|  SIGNED {$$ = node_one_operand(NODE_SIGNED, "signed", NULL, yylloc);}
|  LONG {$$ = node_one_operand(NODE_SIGNED_LONG, "signed long", NULL, yylloc);}
|  LONG INT {$$ = node_one_operand(NODE_SIGNED_LONG_INT, "signed int", NULL, yylloc);}
|  SIGNED LONG {$$ = node_one_operand(NODE_SIGNED_LONG, "signed long int", NULL, yylloc);}
|  SIGNED LONG INT  {$$ = node_one_operand(NODE_SIGNED_LONG_INT, "signed long", NULL, yylloc);}     
;
 

simple_declarator 
:  IDENTIFIER
;

 

statement :  expression_statement  |
             labeled_statement  |
             compound_statement  |
             conditional_statement  |
             iterative_statement  |             
             break_statement  |
             continue_statement  |
             return_statement  |
             goto_statement  |
             null_statement |
             error SEMICOLON { $$ =0;yyerrok;}
;

 
string_constant
: STRING {$$ = $1;}
;

subscript_expr 
:  postfix_expr LEFT_SQUARE expr RIGHT_SQUARE  
{ 
   $$ = node_one_operand(NODE_INDIRECT, "*", node_two_operands(NODE_PLUS, "+", $1, $3, yylloc), yylloc);
}
;

top_level_decl 
: decl
| function_definition
;

translation_unit 
:  top_level_decl { *root = $1;}
|  translation_unit top_level_decl
   {
    $$ = node_two_operands(NODE_TR_UNIT, "program", $1, $2, yylloc);
    *root = $$;
   }
; 

type_name 
:  declaration_specifiers abstract_declarator
|  declaration_specifiers 
;


type_specifier 
: integer_type_specifier  
|  void_type_specifier 
;

unary_expr :  postfix_expr  |
              unary_minus_expr  |
              unary_plus_expr  |
              logical_negation_expr  |
              bitwise_negation_expr  |
              address_expr  |
              indirection_expr  |
              preincrement_expr  |
              predecrement_expr 
;

unary_minus_expr 
:  MINUS cast_expr %prec UMINUS {$$ = node_one_operand(NODE_UNARY_MINUS, "-", $2, yylloc);}
;

unary_plus_expr 
: PLUS cast_expr %prec UPLUS {$$ = node_one_operand(NODE_UNARY_PLUS, "+", $2, yylloc);}
;
 

unsigned_type_specifier 
 : UNSIGNED SHORT {$$ = node_one_operand(NODE_UNSIGNED_SHORT, "unsigned short", NULL, yylloc);}
 | UNSIGNED SHORT INT {$$ = node_one_operand(NODE_UNSIGNED_SHORT_INT, "unsigned short", NULL, yylloc);}
 | UNSIGNED {$$ = node_one_operand(NODE_UNSIGNED_INT, "unsigned", NULL, yylloc);}
 | UNSIGNED INT {$$ = node_one_operand(NODE_UNSIGNED_INT, "unsigned int", NULL, yylloc);}
 | UNSIGNED LONG INT {$$ = node_one_operand(NODE_UNSIGNED_LONG_INT, "unsigned long int", NULL, yylloc);}
 | UNSIGNED LONG {$$ = node_one_operand(NODE_UNSIGNED_LONG_INT, "unsigned long", NULL, yylloc);}
 ; 
 

void_type_specifier 
: VOID {$$ = node_one_operand(NODE_VOID, "void", NULL, yylloc);}
;

while_statement 
 :  WHILE LEFT_PAREN expr RIGHT_PAREN statement 
    {$$ = node_two_operands(NODE_WHILE, "while", $3, $5, yylloc);}
 ;  
 

%%

static void yyerror(YYLTYPE *loc,
                    YYSTYPE *root __attribute__((unused)),
                    int *error_count,
                    yyscan_t scanner __attribute__((unused)),
                    char const *s)
{
  compiler_print_error(*loc, s);
  (*error_count)++;
}

struct node *parser_create_tree(int *error_count, yyscan_t scanner) {
  struct node *parse_tree;
  int result = yyparse(&parse_tree, error_count, scanner);
  if (result == 1 || *error_count > 0) {
    return NULL;
  } else if (result == 2) {
    fprintf(stdout, "Parser ran out of memory.\n");
    return NULL;
  } else {
    return parse_tree;
  }
}

char const *parser_token_name(int token) {
  return yytname[token - 255];
}

