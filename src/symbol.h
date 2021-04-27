#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <stdio.h>

#include "compiler.h"
struct node;
struct type;

enum label_kind{
  LABEL_REFER,
  LABEL_DEFINE,
  LABEL_NONE
};

struct symbol {
  char name[IDENTIFIER_MAX + 1];
  char table_name[IDENTIFIER_MAX + 1];
  struct result result;
};

struct symbol_list {
  struct symbol symbol;
  struct symbol_list *next;
};

struct symbol_label_list {
  char name[IDENTIFIER_MAX + 1];
  enum label_kind kind;
  struct location location;
  struct symbol_label_list *next;
};

struct symbol_label_table {
	struct symbol_label_list *labels;
};

struct symbol_table {
  char name[IDENTIFIER_MAX + 1];
  struct symbol_list *variables;
  struct symbol_table *children;
  struct symbol_table *next;
  struct symbol_table *parent;

  bool haslabels;
  struct symbol_label_table * labeltable;
};


/*
struct symbol_table_list{
   struct symbol_table *curtable;
   struct symbol_table_list *next;
}
*/

void symbol_initialize_table(struct symbol_table *table);
struct symbol_table *symbol_table_create();
struct symbol_label_table *symbol_label_table_create();
struct symbol_table *symbol_table_add(struct symbol_table *t);

int symbol_add_from_statement_list(struct symbol_table *table, struct node *statement_list);
void symbol_print_table(FILE *output, struct symbol_table *table);
void symbol_print_type(FILE *output, struct type *t);
void symbol_print_paramlist(FILE *output, struct type *f);

void symbol_parse_ast(struct symbol_table *table, struct node *expression);
int symbol_add_from_decl(struct symbol_table *table, struct node *node);
int symbol_add_from_func_def(struct symbol_table *table, struct node *node);
int symbol_add_from_func_def_spec(struct symbol_table *table, struct node *node);
int symbol_add_from_decl_list(struct symbol_table *table, struct node *node, struct type *base);
int symbol_add_from_func_decl(struct symbol_table *table, struct node *node, struct type *rettype);
int symbol_add_from_decl_list(struct symbol_table *table, struct node *node, struct type *base);
int symbol_add_from_array_decl(struct symbol_table *table, struct node *node, struct type *base);
int symbol_add_from_pointer_decl(struct symbol_table *table, struct node *node, struct type *base);
int symbol_add_from_func_paramlst(struct symbol_table *table, struct type *func);
int symbol_add_from_label(struct symbol_table *table, struct node *id, bool bdefine);

struct symbol_label_list *symbol_find_label(struct symbol_table *table, struct node *id);

struct type *symbol_func_create(struct node *node, struct type *rettype, bool badd);

void symbol_check_labels(struct symbol_table *table);
struct symbol *symbol_find_identfier(struct symbol_table *table, struct node *id);


#endif /* _SYMBOL_H */
