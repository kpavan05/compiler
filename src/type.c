#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "compiler.h"
#include "node.h"
#include "symbol.h"
#include "type.h"

static struct type *voidtype = NULL;
static struct type *signedint = NULL;
static struct type *signedchar = NULL;
static struct type *signedlong = NULL;
static struct type *signedshort = NULL;

static struct type *unsignedint = NULL;
static struct type *unsignedchar = NULL;
static struct type *unsignedlong = NULL;
static struct type *unsignedshort = NULL;

/***************************
 * CREATE TYPE EXPRESSIONS *
 ***************************/

struct type *type_basic(bool is_unsigned, enum type_basic_kind datatype) {
  struct type *basic;

  basic = malloc(sizeof(struct type));
  assert(NULL != basic);

  basic->kind = TYPE_BASIC;
  basic->data.basic.is_unsigned = is_unsigned;
  basic->data.basic.datatype = datatype;
  if (datatype == TYPE_BASIC_INT)
    is_unsigned ? strcpy(basic->name, "unsigned int") :strcpy(basic->name, "signed int");
  else if (datatype == TYPE_BASIC_LONG)
    is_unsigned ? strcpy(basic->name, "unsigned long") :strcpy(basic->name, "signed long");
  else if(datatype == TYPE_BASIC_SHORT)
    is_unsigned ? strcpy(basic->name, "unsigned short") :strcpy(basic->name, "signed short");
  else if( datatype == TYPE_BASIC_CHAR)
    is_unsigned ? strcpy(basic->name, "unsigned char") :strcpy(basic->name, "signed char");
  else
    strcpy(basic->name, "");
  return basic;
}

struct type *type_function() {
  struct type *funct;
  funct = malloc(sizeof(struct type));
  assert(NULL != funct);

  funct->kind = TYPE_FUNCTION;
  funct->data.functype.params = NULL;
  funct->data.functype.rvaltype = NULL;
  strcpy(funct->name, "function");
  return funct;
}

struct type *type_pointer(){
  struct type *pointer;
  pointer = malloc(sizeof(struct type));
  assert(NULL != pointer);

  pointer->kind = TYPE_POINTER;
  pointer->data.ptrtype.target = NULL;
  strcpy(pointer->name, "pointer");
  return pointer;
}

struct type *type_void(){
  struct type *voidt;
  voidt = malloc(sizeof(struct type));
  assert(NULL != voidt);

  voidt->kind = TYPE_VOID;
  strcpy(voidt->name, "void");
  return voidt;
}

struct type *type_array(){
  struct type *arrayt;
  arrayt = malloc(sizeof(struct type));
  assert(NULL != arrayt);

  arrayt->kind = TYPE_ARRAY;
  arrayt->data.arraytype.hassize = false;
  strcpy(arrayt->name, "array");
  return arrayt;
}

struct type *type_basic_create(struct node *node){

  if (node == NULL) return NULL;
  switch (node->kind){
    case NODE_SIGNED_INT: 
       if (signedint == NULL) signedint = type_basic(false, TYPE_BASIC_INT);
       return signedint;
    case NODE_SIGNED_CHAR : 
       if (signedchar == NULL) signedchar = type_basic(false, TYPE_BASIC_CHAR);
       return signedchar;
    case NODE_SIGNED_LONG : 
       if (signedlong == NULL) signedlong = type_basic(false, TYPE_BASIC_LONG);
       return signedlong;
    case NODE_SIGNED_SHORT:
       if (signedshort == NULL) signedshort = type_basic(false, TYPE_BASIC_SHORT); 
       return signedshort;
    case NODE_UNSIGNED_INT : 
       if (unsignedint == NULL) unsignedint = type_basic(true, TYPE_BASIC_INT);
       return unsignedint;
    case NODE_UNSIGNED_CHAR : 
       if (unsignedchar == NULL) unsignedchar = type_basic(true, TYPE_BASIC_CHAR);
       return unsignedchar;
    case NODE_UNSIGNED_LONG :
       if (unsignedlong == NULL) unsignedlong = type_basic(true, TYPE_BASIC_LONG); 
       return unsignedlong;
    case NODE_UNSIGNED_SHORT :
       if (unsignedshort == NULL) unsignedshort =type_basic(true, TYPE_BASIC_SHORT); 
       return unsignedshort;
    case NODE_VOID :
       if (voidtype == NULL) voidtype =type_void(); 
       return voidtype;
    default: return NULL;
  }

}


struct typelist *type_list_create(struct node *node, struct typelist *head, bool bdefine){
  struct type *decltype = NULL;
  struct typelist *sublist = NULL;
  struct typelist *curlist = NULL;
  struct type *base = NULL;
  struct node *inode = NULL;
  if (node == NULL) return NULL;

  sublist = type_list_create(node->data.binary.left_operand, head, bdefine);  
  assert(node->data.binary.right_operand->kind == NODE_PARAM);
  
  base = type_basic_create(node->data.binary.right_operand->data.binary.left_operand);
  assert (base != NULL);
  
  decltype = type_declarator(node->data.binary.right_operand->data.binary.right_operand, base, &inode);
  if (decltype == NULL) return sublist;
  
  if (type_verify_incomplete_array(decltype, true)){
    compiler_print_error(node->location, "incomplete array type is used");
    return sublist;
  }

  if (sublist == NULL) {
    head->curtype = decltype;
    head->id = (bdefine && inode) ? inode : NULL;
    curlist = head;
  }
  else {
    curlist = malloc(sizeof(struct typelist));
    assert(curlist != NULL);
    curlist->next = NULL;
    curlist->curtype = decltype;      
    curlist->id = (bdefine && inode) ? inode : NULL;
    sublist->next = curlist;
  }
  return curlist;

}
struct type *type_declarator(struct node *node, struct type *base, struct node **idnode){
  struct type *curtype = NULL;
  struct type *rettype = NULL;
  if (node == NULL) return base;
  
  switch(node->kind){
   case NODE_ARRAY_DECL:
   case NODE_ABSTR_DECL:
     if (base->kind == TYPE_VOID){
      compiler_print_error(node->location, "void type is not allowed");
      return NULL;
     }
     curtype = type_array_decl_create(node);
     curtype->data.arraytype.target = base;
     rettype = type_declarator(node->data.binary.left_operand, curtype, idnode);
     break;
   case NODE_POINTER_DECL :
     curtype = type_pointer_decl_create(node->data.binary.left_operand);
     type_pointer_closure(curtype, base);
     rettype = type_declarator(node->data.binary.right_operand, curtype, idnode);
     break;
   case NODE_FUNC_DECL:
     rettype = base;
     break;
   case NODE_IDENTIFIER:
     if (base->kind == TYPE_VOID){
      compiler_print_error(node->location, "void type for  %s is not allowed", node->data.identifier.name);
      return NULL;
     }
     rettype = base;
     *idnode = node;
     break;
   default :
     assert(0);
     break;
  }

  return rettype;
}


void type_pointer_closure(struct type *first, struct type *last){
   struct type *prev = NULL;
   struct type *next = first;
   while(next){
     prev = next;
     next = next->data.ptrtype.target;
   }
   prev->data.ptrtype.target = last;
}

struct type *type_array_decl_create(struct node *node){
  struct type *arrtype = NULL;
  int rval = -1;
  assert(node->kind == NODE_ARRAY_DECL || node->kind == NODE_ABSTR_DECL);
  arrtype = type_array();

  if (node->data.binary.right_operand != NULL && 
      eval_expr(node->data.binary.right_operand, &rval) == 0){
      arrtype->data.arraytype.hassize = true;
      arrtype->data.arraytype.arrsize = rval;
  }
  else{
     arrtype->data.arraytype.hassize = false;
  }
  return arrtype;    
}

struct type *type_pointer_decl_create(struct node *node){
  struct type *pointer = NULL;
  struct type *rettype = NULL;
  if (node == NULL) return NULL;
  
  pointer = type_pointer();
  rettype = type_pointer_decl_create(node->data.binary.left_operand);
  
  if (rettype)pointer->data.ptrtype.target = rettype;
  return pointer;
}

int type_verify_incomplete_array(struct type *t, bool isfunc){
  struct type *target = NULL;
  if (t == NULL || t->kind != TYPE_ARRAY) return 0;
  if (isfunc)
    target = t->data.arraytype.target;
  else
    target = t;

  while (target){
     if (target->kind == TYPE_ARRAY){
        if (!target->data.arraytype.hassize)
          return 1;
        target = target->data.arraytype.target;
     }
     else
      return 0;
  }
  return 0;
}

int type_verify(struct type *t1, struct type *t2){
  struct typelist *list1 = NULL;
  struct typelist *list2 = NULL;
  
  if (t1 == NULL && t2 == NULL) return 1;
  if (t1 == NULL && t2 != NULL) return 1;
  if (t1 != NULL && t2 == NULL) return 1;
  if (t1->kind != t2->kind)     return 1;

  switch(t1->kind){
    case TYPE_POINTER:
       return type_verify(t1->data.ptrtype.target, t2->data.ptrtype.target);
    case TYPE_ARRAY :
       return type_verify(t1->data.arraytype.target, t2->data.arraytype.target);
    case TYPE_BASIC:
      return !(t1->data.basic.datatype == t2->data.basic.datatype);
    case TYPE_VOID:
      return 0;
    case TYPE_FUNCTION:
       if (type_verify(t1->data.functype.rvaltype, t2->data.functype.rvaltype) == 0){
         list1 = t1->data.functype.params;
         list2 = t2->data.functype.params;
         while(list1 && list2){
             if (type_verify(list1->curtype, list2->curtype) == 0){
                list1 = list1->next;
                list2 = list2->next;
             }
             else return 1;
             if (list1 == NULL && list2 != NULL) return 1;
             if (list1 != NULL && list1 == NULL) return 1;
         }
         return 0;
       }
       else return 1;
       
    default:
      return 1;
  }
  return 0;

}

/****************************************
 * TYPE EXPRESSION INFO AND COMPARISONS *
 ****************************************/

static bool type_is_equal(struct type *left, struct type *right) {
  if (left->kind == right->kind) {
    switch (left->kind) {
      case TYPE_BASIC:
        return left->data.basic.is_unsigned == right->data.basic.is_unsigned
            && left->data.basic.datatype == right->data.basic.datatype;
      default:
        assert(0);
        break;
    }
  } else {
    return false;
  }
}

bool type_is_arithmetic(struct type *t) {
  return TYPE_BASIC == t->kind;
}

bool type_is_unsigned(struct type *t) {
  return type_is_arithmetic(t) && t->data.basic.is_unsigned;
}

int type_size(struct type *t) {
  switch (t->kind) {
    case TYPE_BASIC:
      switch (t->data.basic.datatype) {
        case TYPE_BASIC_CHAR:
          return 1;
        case TYPE_BASIC_SHORT:
          return 2;
        case TYPE_BASIC_INT:
          return 4;
        case TYPE_BASIC_LONG:
          return 4;
        default:
          assert(0);
          break;
      }
    case TYPE_POINTER:
      return 4;
    default:
      return 0;
  }
}

/*****************
 * TYPE CHECKING *
 *****************/

static void type_assign_in_expression(struct node *expression);

static void type_convert_usual_binary(struct node *binary_operation) {
  assert(NODE_BINARY_OPERATION == binary_operation->kind);
  assert(type_is_equal(node_get_result(binary_operation->data.binary_operation.left_operand)->type,
                       node_get_result(binary_operation->data.binary_operation.right_operand)->type));
  binary_operation->data.binary_operation.result.type =
    node_get_result(binary_operation->data.binary_operation.left_operand)->type;
}

static void type_convert_assignment(struct node *binary_operation) {
  assert(NODE_BINARY_OPERATION == binary_operation->kind);
  assert(type_is_equal(node_get_result(binary_operation->data.binary_operation.left_operand)->type,
                       node_get_result(binary_operation->data.binary_operation.right_operand)->type));
  binary_operation->data.binary_operation.result.type =
    node_get_result(binary_operation->data.binary_operation.left_operand)->type;
}

static void type_assign_in_binary_operation(struct node *binary_operation) {
  assert(NODE_BINARY_OPERATION == binary_operation->kind);
  type_assign_in_expression(binary_operation->data.binary_operation.left_operand);
  type_assign_in_expression(binary_operation->data.binary_operation.right_operand);

  switch (binary_operation->data.binary_operation.operation) {
    case BINOP_MULTIPLICATION:
    case BINOP_DIVISION:
    case BINOP_ADDITION:
    case BINOP_SUBTRACTION:
      type_convert_usual_binary(binary_operation);
      break;

    case BINOP_ASSIGN:
      type_convert_assignment(binary_operation);
      break;

    default:
      assert(0);
      break;
  }
}


static void type_assign_in_expression(struct node *expression) {
  switch (expression->kind) {
    case NODE_IDENTIFIER:
      if (NULL == expression->data.identifier.symbol->result.type) {
        expression->data.identifier.symbol->result.type = type_basic(false, TYPE_BASIC_INT);
      }
      break;

    case NODE_NUMBER:
      expression->data.number.result.type = type_basic(false, TYPE_BASIC_INT);
      break;

    case NODE_BINARY_OPERATION:
      type_assign_in_binary_operation(expression);
      break;
    default:
      assert(0);
      break;
  }
}

static void type_assign_in_expression_statement(struct node *expression_statement) {
  assert(NODE_EXPRESSION_STATEMENT == expression_statement->kind);
  type_assign_in_expression(expression_statement->data.expression_statement.expression);
}

int type_assign_in_statement_list(struct node *statement_list) {
  assert(NODE_STATEMENT_LIST == statement_list->kind);
  if (NULL != statement_list->data.statement_list.init) {
    type_assign_in_statement_list(statement_list->data.statement_list.init);
  }
  type_assign_in_expression_statement(statement_list->data.statement_list.statement);
  return 0;
}


/**************************
 * PRINT TYPE EXPRESSIONS *
 **************************/

static void type_print_basic(FILE *output, struct type *basic) {
  assert(TYPE_BASIC == basic->kind);

  if (basic->data.basic.is_unsigned) {
    fputs("unsigned", output);
  } else {
    fputs("  signed", output);
  }

  switch (basic->data.basic.datatype) {
    case TYPE_BASIC_INT:
      fputs("  int", output);
      break;
    case TYPE_BASIC_LONG:
      fputs(" long", output);
      break;
    default:
      assert(0);
      break;
  }
}

void type_print(FILE *output, struct type *kind) {
  assert(NULL != kind);

  switch (kind->kind) {
    case TYPE_BASIC:
      type_print_basic(output, kind);
      break;
    default:
      assert(0);
      break;
  }
}
