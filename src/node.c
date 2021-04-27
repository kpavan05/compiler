#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>

#include "node.h"
#include "symbol.h"
#include "type.h"
#define INDENT_SPACE 3

static int nspace =0;

static const char STR_LEFT_PAREN[]     = "(";
static const char STR_RIGHT_PAREN[]    = ")";
static const char STR_LEFT_SQUARE[]    = "[";
static const char STR_RIGHT_SQUARE[]   = "]";
static const char STR_LEFT_CURLY[]     = "{";
static const char STR_RIGHT_CURLY[]    = "}";
static const char STR_QUESTION[]       = "?";
static const char STR_SEMICOLON[]      = ";";
static const char STR_COMMA[]          = ",";
static const char STR_COLON[]          = ":";
static const char STR_IF[]             = "if";
static const char STR_ELSE[]           = "else";
static const char STR_DO[]             = "do";
static const char STR_WHILE[]          = "while";


static char construct_escapechar(char c, int* iserror);
static int is_octal(char c, int *);

/***************************
 * CREATE PARSE TREE NODES *
 ***************************/

/* Allocate and initialize a generic node. */
static struct node *node_create(enum node_kind kind, YYLTYPE location) {
  struct node *n;

  n = malloc(sizeof(struct node));
  assert(NULL != n);

  n->kind = kind;
  n->location = location;

  n->ir = NULL;
  return n;
}

struct node *node_one_operand(enum node_kind kind, char *name, struct node *child_operand, YYLTYPE location)
{
  struct node *node = node_create(kind, location);
  strcpy(node->node_name, name);
  node->data.unary.child_operand = child_operand;
  return node;
}

struct node *node_two_operands(enum node_kind kind, char *name, struct node* left_operand, 
                               struct node* right_operand, YYLTYPE location)
{
  struct node *node = node_create(kind, location);
  strcpy(node->node_name, name);
  node->data.binary.left_operand = left_operand;
  node->data.binary.right_operand = right_operand;
  return node;
}
 
struct node *node_three_operands(enum node_kind kind, char *name, struct node* left_operand, 
                                 struct node* middle_operand, struct node* right_operand, YYLTYPE location)
{
  struct node *node = node_create(kind, location);  
  strcpy(node->node_name, name);
  node->data.ternary.left_operand = left_operand;
  node->data.ternary.middle_operand = middle_operand;
  node->data.ternary.right_operand = right_operand;
  return node;
}


void node_append_child_operand(struct node *parent, struct node *child)
{
	assert (parent != NULL && child != NULL);
	if (parent->data.unary.child_operand != NULL)
		node_append_child_operand(parent->data.unary.child_operand, child);
	else
		parent->data.unary.child_operand = child;
}
/*
 * node_identifier - allocate a node to represent an identifier
 *
 * Parameters:
 *   text - string - contains the name of the identifier
 *   length - integer - the length of text (not including terminating NUL)
 *
 * Side-effects:
 *   Memory may be allocated on the heap.
 *
 */
struct node *node_identifier(YYLTYPE location, char *text, int length)
{
  struct node *node = node_create(NODE_IDENTIFIER, location);
  memset(node->data.identifier.name, 0, IDENTIFIER_MAX + 1);
  strncpy(node->data.identifier.name, text, length);
  node->data.identifier.symbol = NULL;
  return node;
}

/*
* node_string - allocate a node to represent an string literal
*
* Parameters:
*   text - string - contains the name of the identifier
*   length - integer - the length of text (not including terminating NUL)
*
* Side-effects:
*   Memory may be allocated on the heap.
*
*/
struct node *node_string(YYLTYPE location, char *text, int length)
{
	int i, k=0;
	int* pos;
	int nescape = 0;
	int isescape = 0;
	int iserror = 0;
	int reslen = 0;
	char tmp[4];
	struct node *node = node_create(NODE_STRING, location);
	
	pos = calloc(length, sizeof(int));
	for (i = 0; i < length; i++) {
		*(pos + i) = 0;
		if (text[i] == '\\' && isescape == 0) {
			isescape = 1;
			continue;
		}
		if (isescape == 1) {			
			*(pos + i) = (int)construct_escapechar(text[i], &iserror);
			nescape++;
			isescape = 0;
			continue;
		}
	}

	node->data.string_literal.string = (char*)malloc(length + nescape*(sizeof(int)) + 1);
	memset(node->data.string_literal.string, 0, length + nescape*(sizeof(int)) + 1);
	if (node->data.string_literal.string == NULL){
		return NULL;
	}
	for (i=0; i < length; i++){
		if (*(pos + i) != 0) {
			sprintf(tmp, "%o", *(pos+i));
			strcat(node->data.string_literal.string, tmp);
			k += strlen(tmp);
		}
		else {
			node->data.string_literal.string[k] = text[i];
			k++;
		}
	}
	reslen = strlen(node->data.string_literal.string);
	node->data.string_literal.string[reslen] = '\0';
	node->data.string_literal.strlen = reslen;
	free(pos);
	return node;
}

/*
 * node_number - allocate a node to represent a number
 *
 * Parameters:
 *   text - string - contains the numeric literal
 *   length - integer - the length of text (not including terminating NUL)
 *
 * Side-effects:
 *   Memory may be allocated on the heap.
 */
struct node *node_number(YYLTYPE location, char *text)
{
  struct node *node = node_create(NODE_NUMBER, location);
  errno = 0;
  if (text[0] == '0')
	  node->data.number.value = strtoul(text, NULL, 8);
  else
	node->data.number.value = strtoul(text, NULL, 10);

  if (node->data.number.value == ULONG_MAX && ERANGE == errno) {
	  /* Strtoul indicated overflow. */
	  node->data.number.overflow = true;
	  node->data.number.result.type = type_basic(false, TYPE_BASIC_LONG);
  }
  else if (node->data.number.value > 0xFFFFFFFFul) {
	  /* Value is too large for 32-bit unsigned long type. */
	  node->data.number.overflow = true;
	  node->data.number.result.type = type_basic(false, TYPE_BASIC_LONG);
  }
  else if (node->data.number.value < 0x80000000ul){
	  node->data.number.overflow = false;
	  node->data.number.result.type = type_basic(false, TYPE_BASIC_INT);
  }
 else {
	 node->data.number.overflow = false;
	 node->data.number.result.type = type_basic(true, TYPE_BASIC_LONG);
  }
  node->data.number.ischar = 0;
  node->data.number.result.ir_operand = NULL;
  return node;
}


/*
* node_cconst - allocate a node to represent a char constant 
*
* Parameters:
*   text - string - contains the numeric literal
*   length - integer - the length of text (not including terminating NUL)
*   create a newstring with no quotes and also
*   identify the escape sequence and construct is properly
*   also handle octal numbers and decimals as char constants
* Side-effects:
*   Memory may be allocated on the heap.
*/
struct node *node_cconst(YYLTYPE location, char *text, int length)
{
	struct node *node = NULL; 
	int i;
	int isnumber = 1;
	int curlen = 0;
	int iserror = 0;
	int isescape = 0;	
	int isoctal = 0;
	char* str = NULL;
	char c;
	long val;
	errno = 0;

	str = (char*)malloc(length + 1);
	for (i = 0; i < length; i++){
		if (i == 0 && text[i] == '\'')continue;
		if (i == (length - 1) && text[i] == '\'')continue;

		if (text[i] == '\\' && isescape == 0) {
			isescape = 1;
			continue;
		}
		if (isescape == 1){
			if (is_octal(text[i], &iserror)){
				if (curlen == 0)str[curlen++] = '0';
				str[curlen++] = text[i];
				isoctal = 1;
			}
			else{
				c = construct_escapechar(text[i], &iserror);
				if (iserror == 0) str[curlen++]	= c;
				isescape = 0;
			}
			continue;
		}
		str[curlen++] = text[i];
	}
	str[curlen] = '\0';

	for (i = 0; str[i] != '\0'; i++){
		isnumber = (isdigit(str[i]) && isnumber) ? 1 : 0;	
	}

	node = node_create(NODE_NUMBER, location);

	if (isnumber == 1){		
		if (isoctal)
			val = strtol(str, NULL, 8);
		else
			val = strtol(str, NULL, 10);
	}
	else {
		if (strlen(str) > 1) iserror = 1;
		val = (int)str[0];
	}

	if (val > 0xff || ERANGE == errno){
		if (val & 0x00000080)
			node->data.number.value = (val | 0xffffff00);
		else
			node->data.number.value = val ;
		node->data.number.overflow = true;
		node->data.number.result.type = type_basic(false, TYPE_BASIC_INT);
	}
	else{
		if (val & 0x00000080)
		   node->data.number.value = (val | 0xffffff00);
		else 
			node->data.number.value = val;		
		node->data.number.overflow = false;
		node->data.number.result.type = type_basic(false, TYPE_BASIC_INT);
	}

	node->data.number.ischar = 1;
	node->data.number.result.ir_operand = NULL;
	node->iserror = iserror;
	free(str);
		
	return node;
}

/*
construct_escapechar - constructs the actual escape sequence 
*
* Parameters:
*   c - char - character for each escape sequence has to be constructed
*   iserror -int - if the charcter does not match any escape sequence set it to 1.
*
* Side-effects:
*   modify the value of iserror
*/
static char construct_escapechar(char c, int* iserror)
{
	char ret;
	*iserror = 0;
	if (c == 'n')       ret = '\n';
	else if (c == 't')  ret = '\t';
	else if (c == 'b')  ret = '\b';
	else if (c == '?')  ret = '\?';
	else if (c == 'a')  ret = '\a';
	else if (c == '"')  ret = '\"';
	else if (c == 'r')  ret = '\r';
	else if (c == 'f')  ret = '\f';
	else if (c == 'v')  ret = '\v';
	else if (c == '\\') ret = '\\';
	else if (c == '\'') ret = '\'';
	else if (c == '0')  ret = '\0';
	else {*iserror = 1;   }

	return ret;
}



static int is_octal(char c, int* iserror)
{
	*iserror = 0;
	if (isdigit(c) && ((c - 48 >= 0) && (c - 48 < 8))) {
		return 1;
	}
	else
		*iserror = 1;
	return 0;
}

struct node *node_binary_operation(YYLTYPE location,
                                   enum node_binary_operation operation,
                                   struct node *left_operand,
                                   struct node *right_operand)
{
  struct node *node = node_create(NODE_BINARY_OPERATION, location);
  node->data.binary_operation.operation = operation;
  node->data.binary_operation.left_operand = left_operand;
  node->data.binary_operation.right_operand = right_operand;
  node->data.binary_operation.result.type = NULL;
  node->data.binary_operation.result.ir_operand = NULL;
  return node;
}



struct node *node_expression_statement(YYLTYPE location, struct node *expression)
{
  struct node *node = node_create(NODE_EXPRESSION_STATEMENT, location);
  node->data.expression_statement.expression = expression;
  return node;
}

struct node *node_statement_list(YYLTYPE location,
                                 struct node *init,
                                 struct node *statement)
{
  struct node *node = node_create(NODE_STATEMENT_LIST, location);
  node->data.statement_list.init = init;
  node->data.statement_list.statement = statement;
  return node;
}

struct node *node_null_statement(YYLTYPE location)
{
  return node_create(NODE_NULL_STATEMENT, location);
}


struct result *node_get_result(struct node *expression) {
  switch (expression->kind) {
    case NODE_NUMBER:
      return &expression->data.number.result;
    case NODE_IDENTIFIER:
      return &expression->data.identifier.symbol->result;
    case NODE_BINARY_OPERATION:
      return &expression->data.binary_operation.result;
    default:
      assert(0);
      return NULL;
  }
}

/**************************
 * PRINT PARSE TREE NODES *
 **************************/

static void node_print_expression(FILE *output, struct node *expression, const char *sep);

static void node_print_binary_operation(FILE *output, struct node *binary_operation) {
  static const char *binary_operators[] = {
    "*",    /*  0 = BINOP_MULTIPLICATION */
    "/",    /*  1 = BINOP_DIVISION */
    "+",    /*  2 = BINOP_ADDITION */
    "-",    /*  3 = BINOP_SUBTRACTION */
    "=",    /*  4 = BINOP_ASSIGN */
    NULL
  };

  assert(NODE_BINARY_OPERATION == binary_operation->kind);

  fputs("(", output);
  node_print_expression(output, binary_operation->data.binary_operation.left_operand, "");
  fputs(" ", output);
  fputs(binary_operators[binary_operation->data.binary_operation.operation], output);
  fputs(" ", output);
  node_print_expression(output, binary_operation->data.binary_operation.right_operand, "");
  fputs(")", output);
}

static void node_print_number(FILE *output, struct node *number) {
  assert(NODE_NUMBER == number->kind);

  fprintf(output, "%lu", number->data.number.value);
}

static void node_print_string(FILE *output, struct node *expression) {
  assert(NODE_STRING == expression->kind);

  fprintf(output, "%s", expression->data.string_literal.string);
}

/*
 * After the symbol table pass, we can print out the symbol address
 * for each identifier, so that we can compare instances of the same
 * variable and ensure that they have the same symbol.
 */
static void node_print_identifier(FILE *output, struct node *identifier) {
  assert(NODE_IDENTIFIER == identifier->kind);
  
  
  /*fprintf(output, "%s / * %p * /", identifier->data.identifier.name, 
                                 (void *)identifier->data.identifier.symbol);*/
  if (identifier->data.identifier.symbol != NULL)
  	fprintf(output, "%s /* %s */", identifier->data.identifier.name,identifier->data.identifier.symbol->table_name);
  else
  	fprintf(output, "%s ", identifier->data.identifier.name);
}

static void node_print_array_decl(FILE *output, struct node *arr_decl) {
  
  if (arr_decl != NULL){
        fprintf(output,"%s", STR_LEFT_SQUARE);
        node_print_expression(output, arr_decl, STR_RIGHT_SQUARE);     
  }
  else{
  	 fprintf(output, "%s %s", STR_LEFT_SQUARE, STR_RIGHT_SQUARE); 
  }
  
}

static void node_print_pointer_decl(FILE *output, struct node *node){
	struct node* curnode = node;
    if (curnode == NULL) return;

    while (curnode){
        assert( curnode->kind == NODE_POINTER);
        fprintf(output, "%s %s", STR_LEFT_PAREN, curnode->node_name);
        curnode = curnode->data.unary.child_operand;
    }
}

static void node_print_pointer_decl_closure(FILE *output, struct node *node){
	struct node* curnode = node;
    if (curnode == NULL) return;

    while (curnode){
        assert( curnode->kind == NODE_POINTER);
        fprintf(output, "%s", STR_RIGHT_PAREN);
        curnode = curnode->data.unary.child_operand;
    }
}

static void node_print_each_statement(FILE *output, struct node *expression){
	int curspace = 0;
    if (expression == NULL || expression->data.binary.right_operand == NULL) return;
    assert(expression->kind == NODE_STATEMENT_LIST);
    fputs("\n", output);

    /*while (curspace < expression->data.binary.right_operand->location.first_column)*/
    while (curspace < nspace){
    	fputc(' ', output);
    	curspace++;
    }

    if (expression->data.binary.right_operand && 
	    (expression->data.binary.right_operand->kind != NODE_DECL &&
	    expression->data.binary.right_operand->kind != NODE_FOR &&
	   	expression->data.binary.right_operand->kind != NODE_IF &&
	   	expression->data.binary.right_operand->kind != NODE_IF_ELSE &&
	    expression->data.binary.right_operand->kind != NODE_COMP_STATEMENT &&
	    expression->data.binary.right_operand->kind !=  NODE_LABEL &&
	    expression->data.binary.right_operand->kind != NODE_WHILE))
	  	node_print_expression(output, expression->data.binary.right_operand, STR_SEMICOLON);
	else
        node_print_expression(output, expression->data.binary.right_operand, "");
   
    
}

static void node_print_compound_statement(FILE *output, struct node *expression, int curspace){
	int i = curspace;
	if (expression == NULL ) return;
    assert(expression->kind == NODE_COMP_STATEMENT);

	fprintf(output, "%s", STR_LEFT_CURLY);
    nspace = expression->location.first_column - 1;
    node_print_expression(output, expression->data.unary.child_operand, "");
    
    fprintf(output, "\n");
    while(i >= 0) {
    	fputc(' ', output); i--;
    }
    fprintf(output, "%s\n", STR_RIGHT_CURLY);
    nspace = curspace; 
}

static void node_print_expression(FILE *output, struct node *expression,const char* separator) {

  if (expression != NULL){
  	  /*fprintf(output, "parse tree: %s\n", expression->node_name);*/
	  switch (expression->kind) {

	    case NODE_BINARY_OPERATION:
	      node_print_binary_operation(output, expression);
	      break;
	    case NODE_IDENTIFIER:
	      node_print_identifier(output, expression);
	      break;
	    
	    case NODE_NUMBER:
	      node_print_number(output, expression);
	      break;
	    case NODE_STRING:
	      node_print_string(output, expression);
	      break;

	    case NODE_EXPRESSION_STATEMENT:
	      node_print_expression(output, expression->data.expression_statement.expression, "");
	      break;
 
        case NODE_TR_UNIT:
	      node_print_expression(output, expression->data.binary.left_operand, "");
	      node_print_expression(output, expression->data.binary.right_operand, "");
	      break;

	    case NODE_ARRAY_DECL:
	      node_print_expression(output, expression->data.binary.left_operand, "");
	      node_print_array_decl(output, expression->data.binary.right_operand);
	      break;
	    
	    case NODE_POINTER_DECL:
	      node_print_pointer_decl(output, expression->data.binary.left_operand);   
	      node_print_expression(output, expression->data.binary.right_operand, "");
	      node_print_pointer_decl_closure(output, expression->data.binary.left_operand);
	      break;
        
        case NODE_FUNC_CALL:
	    case NODE_FUNC_DECL:
	      node_print_expression(output, expression->data.binary.left_operand, STR_LEFT_PAREN);
	      node_print_expression(output, expression->data.binary.right_operand, STR_RIGHT_PAREN);
	      break;        
	    
	    case NODE_DECL_LIST:
	    case NODE_EXPR_LIST:
	    case NODE_COMMA_EXPR:
	    case NODE_PARAM_LIST:
	      node_print_expression(output, expression->data.binary.left_operand, "");
	      if (expression->data.binary.left_operand != NULL) fprintf(output, STR_COMMA);
	      node_print_expression(output, expression->data.binary.right_operand, "");      
	      break;

	    case NODE_FOR:
	      fprintf(output,"%s", expression->node_name);
	      node_print_expression(output, expression->data.binary.left_operand, "");
	      node_print_expression(output, expression->data.binary.right_operand, "");
	      break;

        case NODE_IF:
	    case NODE_WHILE:
	      fprintf(output,"%s %s", expression->node_name, STR_LEFT_PAREN);
	      node_print_expression(output, expression->data.binary.left_operand, STR_RIGHT_PAREN);
	      node_print_expression(output, expression->data.binary.right_operand, "");
	      break;

        case NODE_DOWHILE:
	      fprintf(output,"%s", STR_DO);
	      node_print_expression(output, expression->data.binary.left_operand, "");
	      fprintf(output, "%s %s", STR_WHILE, STR_LEFT_PAREN);
	      node_print_expression(output, expression->data.binary.right_operand, STR_RIGHT_PAREN);
	      break;

	    case NODE_FUNC_DEF:
	      node_print_expression(output, expression->data.binary.left_operand, "");
	      node_print_expression(output, expression->data.binary.right_operand, "");
	      break;

	    case NODE_STATEMENT_LIST:
	       node_print_expression(output, expression->data.binary.left_operand, "");
	       node_print_each_statement(output, expression);	       
	      break;
	 
	    case NODE_IF_ELSE:
	      fprintf(output,"%s %s", STR_IF, STR_LEFT_PAREN);
	      node_print_expression(output, expression->data.ternary.left_operand, STR_RIGHT_PAREN);
	      node_print_expression(output, expression->data.ternary.middle_operand, "");
	      fprintf(output,"%s ", STR_ELSE);
	      node_print_expression(output, expression->data.ternary.right_operand, "");
	      break;
	    
	    case NODE_FOR_EXPR:
	      fprintf(output,"%s", STR_LEFT_PAREN);
	      node_print_expression(output, expression->data.ternary.left_operand, STR_SEMICOLON);
	      node_print_expression(output, expression->data.ternary.middle_operand, STR_SEMICOLON);
	      node_print_expression(output, expression->data.ternary.right_operand, STR_RIGHT_PAREN);
	      break;

	    case NODE_TERNARY_OP:
	      fprintf(output,"%s", STR_LEFT_PAREN);
	      node_print_expression(output, expression->data.ternary.left_operand, "");
	      fprintf(output, "%s %s %s", STR_RIGHT_PAREN, STR_QUESTION, STR_LEFT_PAREN );
	      node_print_expression(output, expression->data.ternary.middle_operand, "");
	      fprintf(output, "%s %s %s", STR_RIGHT_PAREN, STR_COLON, STR_LEFT_PAREN );
	      node_print_expression(output, expression->data.ternary.right_operand, STR_RIGHT_PAREN);
	      break;

	    case NODE_POST_INCR:
	    case NODE_POST_DECR:
	      fprintf(output,"%s", STR_LEFT_PAREN);
	      node_print_expression(output, expression->data.unary.child_operand, expression->node_name);
	      fprintf(output, "%s", STR_RIGHT_PAREN);
	      break;

	    case NODE_DECL:
	      node_print_expression(output, expression->data.binary.left_operand, "");
	      node_print_expression(output, expression->data.binary.right_operand, STR_SEMICOLON);
	      fputs("\n", output);
	      break;

        case NODE_ABSTR_DECL:
	      node_print_expression(output, expression->data.binary.left_operand, STR_LEFT_SQUARE);
	      node_print_expression(output, expression->data.binary.right_operand, STR_RIGHT_SQUARE);
	      break;

	    case NODE_PARAM:	    
	    case NODE_FUNC_DEF_SPEC:
	      node_print_expression(output, expression->data.binary.left_operand, "");
	      node_print_expression(output, expression->data.binary.right_operand, "");
	      break;
	    
	    case NODE_VBAR:
	    case NODE_PLUS:
	    case NODE_LESS:
	    case NODE_MINUS:
	    case NODE_SLASH:
	    case NODE_CARET:
	    case NODE_GREATER:
	    case NODE_PERCENT:
	    case NODE_ASTERISK:
	    case NODE_AMPERSAND:
	    case NODE_NOT_EQUAL:
	    case NODE_VBAR_VBAR:
        case NODE_LESS_EQUAL:
	    case NODE_SHIFT_LEFT:
	    case NODE_SHIFT_RIGHT:
	    case NODE_EQUAL_EQUAL:
	    case NODE_GREATER_EQUAL:
	    case NODE_AMPERSAND_AMPERSAND:
	      fprintf(output,"%s", STR_LEFT_PAREN);
	      node_print_expression(output, expression->data.binary.left_operand, expression->node_name);
	      node_print_expression(output, expression->data.binary.right_operand, STR_RIGHT_PAREN);
	      break;

	    
	    case NODE_VOID:
	    case NODE_CHAR:
	    case NODE_SIGNED :
	    case NODE_UNSIGNED :
	    case NODE_SIGNED_INT:
	    case NODE_SIGNED_LONG :
	    case NODE_SIGNED_CHAR :
	    case NODE_SIGNED_SHORT :
	    case NODE_UNSIGNED_INT :
	    case NODE_UNSIGNED_CHAR :	    
	    case NODE_UNSIGNED_SHORT :
	    case NODE_SIGNED_LONG_INT :
	    case NODE_SIGNED_SHORT_INT :
	    case NODE_UNSIGNED_SHORT_INT :
	    case NODE_UNSIGNED_LONG :
	    case NODE_UNSIGNED_LONG_INT :
	      fprintf(output, "%s", expression->node_name);
	      fprintf(output, "%s", " ");
	      break;

	    case NODE_EQUAL:
	    case NODE_PLUS_EQUAL:
	    case NODE_VBAR_EQUAL:
	    case NODE_MINUS_EQUAL:	    
	    case NODE_SLASH_EQUAL:
	    case NODE_CARET_EQUAL:
	    case NODE_PERCENT_EQUAL:
	    case NODE_ASTERISK_EQUAL:
	    case NODE_LESS_LESS_EQUAL:	    
	    case NODE_AMPERSAND_EQUAL:
	    case NODE_GREATER_GREATER_EQUAL:
	      fprintf(output,"%s", STR_LEFT_PAREN);
	      node_print_expression(output, expression->data.binary.left_operand, expression->node_name);
	      node_print_expression(output, expression->data.binary.right_operand, ")");
	      break;
	    
	    	    
	    case NODE_TILDE:
	    case NODE_POINTER:	    
	    case NODE_PRE_INCR:
	    case NODE_PRE_DECR:
	    case NODE_INDIRECT:
	    case NODE_ADDRESS_OP:
	    case NODE_UNARY_PLUS:
	    case NODE_UNARY_MINUS:
	    case NODE_EXCLAMATION:
	      fprintf(output, "%s %s", STR_LEFT_PAREN, expression->node_name);
	      node_print_expression(output, expression->data.unary.child_operand, STR_RIGHT_PAREN);
	      break;

	    case NODE_GOTO:
	    case NODE_BREAK:
	    case NODE_RETURN:	    
	    case NODE_CONTINUE:
	      fprintf(output, "%s ", expression->node_name);
	      node_print_expression(output, expression->data.unary.child_operand, STR_SEMICOLON);
	      break;
        case NODE_LABEL:
          node_print_expression(output, expression->data.binary.left_operand, expression->node_name);
          fputs("\n", output);
	      node_print_expression(output, expression->data.binary.right_operand, "");
	      break;

        case NODE_COMP_STATEMENT:
          node_print_compound_statement(output, expression, nspace);
	      break;
        
        case NODE_CAST:
          fprintf(output, "%s", STR_LEFT_PAREN);
          node_print_expression(output, expression->data.binary.left_operand, STR_RIGHT_PAREN);
	      node_print_expression(output, expression->data.binary.right_operand, "");
	      break;

        case NODE_NULL_STATEMENT:
          break;  	    

	    default:
	      assert(0);
	      break;

	  }

  }
  if (separator[0] != '\0')   fprintf(output,"%s", separator);
}


int eval_expr(struct node* node, int *val){
   int left = 0;
   int right = 0;
   if (node== NULL) return -1;

   switch (node->kind){
		case NODE_VBAR:
		    if ((eval_expr(node->data.binary.left_operand, &left) == 0) && (eval_expr(node->data.binary.right_operand, &right) == 0)){
		       *val = left | right; 
		       return 0;
		    }
		    return -1;
		case NODE_PLUS:
		    if ((eval_expr(node->data.binary.left_operand, &left) == 0) && (eval_expr(node->data.binary.right_operand, &right) == 0)){
		       *val = left + right; 
		       return 0;
		    }
		    return -1;
	    case NODE_MINUS:
	        if ((eval_expr(node->data.binary.left_operand, &left) == 0) && (eval_expr(node->data.binary.right_operand, &right) == 0)){
		       *val = left - right; 
		       return 0;
		    }
		    return -1;
	    case NODE_SLASH:
	        if ((eval_expr(node->data.binary.left_operand, &left) == 0) && (eval_expr(node->data.binary.right_operand, &right) == 0)){
		       *val = left / right; 
		       return 0;
		    }
		    return -1;
	    case NODE_CARET:
	        if ((eval_expr(node->data.binary.left_operand, &left) == 0) && (eval_expr(node->data.binary.right_operand, &right) == 0)){
		       *val = left ^ right; 
		       return 0;
		    }
		    return -1;
	    case NODE_PERCENT:
	        if ((eval_expr(node->data.binary.left_operand, &left) == 0) && (eval_expr(node->data.binary.right_operand, &right) == 0)){
		       *val = left % right; 
		       return 0;
		    }
		    return -1;
	    case NODE_ASTERISK:
	        if ((eval_expr(node->data.binary.left_operand, &left) == 0) && (eval_expr(node->data.binary.right_operand, &right) == 0)){
		       *val = left * right; 
		       return 0;
		    }
		    return -1;
	    case NODE_AMPERSAND:
	        if ((eval_expr(node->data.binary.left_operand, &left) == 0) && (eval_expr(node->data.binary.right_operand, &right) == 0)){
		       *val = left & right; 
		       return 0;
		    }
		    return -1;
	    case NODE_SHIFT_LEFT:
	        if ((eval_expr(node->data.binary.left_operand, &left) == 0) && (eval_expr(node->data.binary.right_operand, &right) == 0)){
		       *val = left << right; 
		       return 0;
		    }
		    return -1;
	    case NODE_SHIFT_RIGHT:
	        if ((eval_expr(node->data.binary.left_operand, &left) == 0) && (eval_expr(node->data.binary.right_operand, &right) == 0)){
		       *val = left >> right; 
		       return 0;
		    }
		    return -1;
		case NODE_POST_INCR:
        case NODE_POST_DECR:
           if (eval_expr(node->data.unary.child_operand, &left) == 0){
           	  *val = left;
           	  return 0;
           }
           return 1; 
        case NODE_PRE_INCR:
           if (eval_expr(node->data.unary.child_operand, &left) == 0){
           	  *val = left + 1;
           	  return 0;
           }
           return 1;
        case NODE_PRE_DECR:
           if (eval_expr(node->data.unary.child_operand, &left) == 0){
           	*val = left - 1;
           	return 0;
           }
           return 1;   
	    case NODE_NUMBER:
	        *val = (signed int)(node->data.number.value); 
	        return 0; 
        default:
            return -1;
     
	}
}

static void node_print_expression_statement(FILE *output, struct node *expression_statement) {
  assert(NODE_EXPRESSION_STATEMENT == expression_statement->kind);

  node_print_expression(output, expression_statement->data.expression_statement.expression, "");

}

void node_print_statement_list(FILE *output, struct node *statement_list) {
  assert(NODE_STATEMENT_LIST == statement_list->kind);

  if (NULL != statement_list->data.statement_list.init) {
    node_print_statement_list(output, statement_list->data.statement_list.init);
  }
  node_print_expression_statement(output, statement_list->data.statement_list.statement);
  fputs(";\n", output);
}


void node_print_parsetree_ouput(FILE *output, struct node *statement) {
  
  node_print_expression(output, statement, "");
  fputs("\n", output);
}
