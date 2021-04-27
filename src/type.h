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

struct type *type_basic(bool is_unsigned, enum type_basic_kind datatype);


int type_assign_in_statement_list(struct node *statement_list);

void type_print(FILE *output, struct type *type);
void type_pointer_closure(struct type *first, struct type *last);

struct type *type_function() ;
struct type *type_pointer();
struct type *type_void();
struct type *type_basic_create(struct node *node);
struct typelist *type_list_create(struct node *node, struct typelist *head, bool bdefine);
struct type *type_array_decl_create(struct node *node);
struct type *type_pointer_decl_create(struct node *node);
struct type *type_declarator(struct node *node, struct type *base, struct node **inode);
int type_verify(struct type *t1, struct type *t2);
int type_verify_incomplete_array(struct type *t, bool isfunc);
#endif /* _TYPE_H */
