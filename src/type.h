#ifndef _TYPE_H
#define _TYPE_H

#include <stdio.h>
#include <stdbool.h>

struct node;

enum type_kind {
  TYPE_BASIC,
  TYPE_VOID,
  TYPE_POINTER,
  TYPE_ARRAY,
  TYPE_FUNCTION,
  TYPE_LABEL
};

enum type_basic_kind {
  TYPE_BASIC_CHAR,
  TYPE_BASIC_SHORT,
  TYPE_BASIC_INT,
  TYPE_BASIC_LONG
};
struct type {
  enum type_kind kind;
  char name[16];
  union {
    struct {
      bool is_unsigned;
      enum type_basic_kind datatype;
    } basic;

    struct {
      struct typelist *params;
      struct type *rvaltype;
      int nparams;
    }functype;

    struct {
      int arrsize;
      bool hassize;
      struct type *target;
    }arraytype;
    
    struct {
      struct type *target;
    }ptrtype;

  } data;
};

struct typelist{
  struct type *curtype;
  struct node *id;
  struct typelist *next;
};

struct list{
  char item[16];
  struct list *next;
};

struct type *type_basic(bool is_unsigned, enum type_basic_kind datatype);
bool type_is_arithmetic(struct type *t);
bool type_is_unsigned(struct type *t);

int type_assign_in_statement_list(struct node *statement_list);

void type_print(FILE *output, struct type *type);
void type_pointer_closure(struct type *first, struct type *last);

struct type *type_function() ;
struct type *type_pointer();
struct type *type_void();
struct type *type_array();
struct type *type_basic_create(struct node *node);
struct typelist *type_list_create(struct node *node, struct typelist *head, bool bdefine);
struct type *type_array_decl_create(struct node *node);
struct type *type_pointer_decl_create(struct node *node);
struct type *type_declarator(struct node *node, struct type *base, struct node **inode);
int type_verify(struct type *t1, struct type *t2);
int type_verify_incomplete_array(struct type *t, bool isfunc);
void type_eval_expression(struct node* node);
int type_calc_size(struct type *t);
/*type checking/conversion*/
void type_create_cast_node(struct node* curnode, struct type* convtype);
struct type* type_after_conversion(struct node* node);
struct type* type_usual_unary_conversion(struct node* node);
void type_usual_assign_conversion(struct node* lnode, struct node* rnode);
struct type* type_usual_binary_conversion(struct node* lnode, struct node* rnode);
void type_address_op(struct node *node);
void type_indirection_op(struct node *node);
void type_logical_negation_op(struct node* node);
void type_cast_op(struct node* node);
void type_bitwise_negation_op(struct node* node);
void type_unary_arithmetic_op(struct node* node);
void type_mult_op(struct node* node);
void type_add_op(struct node* node);
void type_shift_op(struct node* node);
void type_relational_op(struct node* node);
void type_equal_op(struct node* node);
void type_bitwise_op(struct node* node);
void type_logical_andor_op(struct node* node);
void type_simple_assign_op(struct node* node);
void type_compound_assign_op(struct node* node);
void type_pre_increment_op(struct node* node);
int type_check_num_params(struct node* node, struct node *parent, struct typelist* params);
int type_check_param(struct node* node, struct node *parent, struct typelist* params);
void type_func_call(struct node* node);
void type_check_return(struct node* node);
void type_check_main(struct node *node);
void type_eval_expression(struct node* node);
struct node *type_get_id_node(struct node *node);
int type_get_alignment_size(struct type *t, int size, int *pack, int multiplier);
#endif /* _TYPE_H */
