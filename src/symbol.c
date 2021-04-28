#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "node.h"
#include "type.h"
#include "symbol.h"

static struct string_table *head = NULL;
/************************
 * CREATE SYMBOL TABLES *
 ************************/

void symbol_initialize_table(struct symbol_table *table) {
  table->variables = NULL;
  table->labeltable = NULL;
  table->children = NULL;
  table->next = NULL;
  table->parent = NULL;
  table->haslabels = false;
  table->nvariables = 0;
}
/*
symbol_table_create- creates new symbol table 
side effect - creates new symbol table
returns new symbol table if success
        NULL if fails
*/
struct symbol_table *symbol_table_create(){
  struct symbol_table *table;

  table = malloc(sizeof(struct symbol_table));
  assert(table != NULL);
  symbol_initialize_table(table);
  return table;
}
/*
symbol_label_table- creates new symbol label table a
side effect - creates new label table
returns new label table if success
        NULL if fails
*/
struct symbol_label_table *symbol_label_table_create(){
  struct symbol_label_table *table;

  table = malloc(sizeof(struct symbol_label_table));
  assert(table != NULL);

  table->labels = NULL;
  return table;
}
/*
symbol_table_add- creates new symbol table and links to parent
parameters parent - parent table to which the added table will
                    be added as children
side effect - modifies parent table, creates new table
*/
struct symbol_table *symbol_table_add(struct symbol_table *parent){
   struct symbol_table *prev = NULL;
   struct symbol_table *next = NULL;
   struct symbol_table *curtable = NULL;
   int index = 0;
   assert(parent != NULL);

   curtable = symbol_table_create(); 
   if (parent->children == NULL){
    parent->children = curtable;
   }
   else{
     next = parent->children;
     while(next){
       prev = next;
       next = next->next;
       index++;
     }
     prev->next = curtable;
   }
   curtable->parent = parent;
   curtable->labeltable = parent->labeltable;
   sprintf(curtable->name, "%s.%d", parent->name, index);
   
   return curtable;
}

/**************************/
/* handle identifier */
static struct symbol *symbol_get(struct symbol_table *table, char name[]) {
  struct symbol_list *iter;
  for (iter = table->variables; NULL != iter; iter = iter->next) {
    if (!strcmp(name, iter->symbol.name)) {
      return &iter->symbol;
    }
  }
  return NULL;
}

static struct symbol *symbol_put(struct symbol_table *table, char name[]) {
  struct symbol_list *symbol_list;

  symbol_list = malloc(sizeof(struct symbol_list));
  assert(NULL != symbol_list);

  strncpy(symbol_list->symbol.name, name, IDENTIFIER_MAX);
  strncpy(symbol_list->symbol.table_name, table->name, IDENTIFIER_MAX);
  symbol_list->symbol.result.type = NULL;
  symbol_list->symbol.result.ir_operand = NULL;
  
  symbol_list->next = table->variables;
  table->variables = symbol_list;
  ++table->nvariables;
  return &symbol_list->symbol;
}

static int symbol_add_from_identifier(struct symbol_table *table, struct node *id, struct type *restype, bool define) {
  assert(NODE_IDENTIFIER == id->kind);

  id->data.identifier.symbol = symbol_get(table, id->data.identifier.name);
  if (NULL == id->data.identifier.symbol) {
    if (define) {
      if (restype != NULL && restype->kind == TYPE_VOID){
        compiler_print_error(id->location, "void type for %s is not allowed", id->data.identifier.name);
        return 1;
      }
      else if (restype != NULL &&  restype->kind == TYPE_POINTER && restype->data.ptrtype.target->kind == TYPE_FUNCTION){
        compiler_print_error(id->location, "function poiner for %s is not allowed", id->data.identifier.name);
        return 1;
      }
      id->data.identifier.symbol = symbol_put(table, id->data.identifier.name);
      id->data.identifier.symbol->result.type = restype;
      id->data.identifier.symbol->result.islvalue = 1;
      id->data.identifier.symbol->table = table;
    } else {
      compiler_print_error(id->location, "undefined identifier %s", id->data.identifier.name);
      return 1;
    }
  }
  else{      
     compiler_print_error(id->location, "multiple definitions found for %s", id->data.identifier.name);   
  }
  return 0;

}
/*symbol_find_identfier - finds identifier in symbol table that is passed
                          and in the parent tables
  parameters  table - table where the search identifier has to be added.
              id - identifier node
  side effect- no
  return symbol if found (success)
         NULL if fails
*/
struct symbol *symbol_find_identfier(struct symbol_table *table, struct node *id){
  struct symbol *symbol = NULL;
  if (table == NULL || id == NULL) return NULL;
  assert(id->kind == NODE_IDENTIFIER);
  
  symbol = symbol_get(table, id->data.identifier.name);
  if (symbol == NULL){
    return symbol_find_identfier(table->parent, id);
  }
  return symbol;
}


/**************************************/
/* symbol_add_from_decl- processes decl node*/
int symbol_add_from_decl(struct symbol_table *table, struct node *node){
  struct type *base = NULL;
  assert(node->kind == NODE_DECL);
  
  base = type_basic_create(node->data.binary.left_operand);
  assert(base != NULL);

  return symbol_add_from_decl_list(table, node->data.binary.right_operand, base);
}

/*symbol_add_from_func_def - process function_defintion node
  parameters table - symbol table to which function id has to be added
             node - function_definition node
  creates function type and addes typelist to params
*/
int symbol_add_from_func_def(struct symbol_table *table, struct node *node){
  struct symbol_table *newtable = NULL;
  struct node *curnode = NULL;
  if (table == NULL || node == NULL) return 0;
  assert(node->kind ==  NODE_FUNC_DEF);

  newtable = symbol_table_add(table);
  newtable->labeltable = symbol_label_table_create();
  newtable->haslabels = true;
  symbol_parse_ast(newtable,node->data.binary.left_operand);
  curnode = node->data.binary.right_operand;
  if ( curnode->kind == NODE_COMP_STATEMENT)
    symbol_parse_ast(newtable,curnode->data.unary.child_operand);
  else
    symbol_parse_ast(newtable,curnode);

  symbol_check_labels(newtable);
  return 0;
}
/*symbol_add_from_func_def_spec - processes function_def_spec node 
  parameters - table - symbol table to which entry has to be added
               node  - function_def_spec node in ast
  side effect creates function type and its associated members
  and attaches the type to identifier node.
  returns 0- if success 1- failure             
*/
int symbol_add_from_func_def_spec(struct symbol_table *table, struct node *node){
  struct type *base = NULL;
  struct type *func = NULL;
  struct node *curnode = NULL;
  struct type *pointert = NULL;
  struct symbol *symbol = NULL;
  
  assert(node->kind == NODE_FUNC_DEF_SPEC);  
  base = type_basic_create(node->data.binary.left_operand);
  assert(base != NULL);
  
  /*call helper function to create function type */
  curnode = node->data.binary.right_operand;
  /* handle pointer return type before creating function type */
  if (curnode->kind == NODE_POINTER_DECL){
    /*pointert = type_declarator(curnode, base, &idnode);*/
    pointert = type_pointer_decl_create(curnode->data.binary.left_operand);
    type_pointer_closure(pointert, base);
    curnode = curnode->data.binary.right_operand;
    func = symbol_func_create(curnode, pointert, true);
  }
  else if (curnode->kind == NODE_ARRAY_DECL){
    compiler_print_error(node->location, "function cannot have array return type");
    return 1;
  }
  else {
    /*create function type */
    func = symbol_func_create(curnode, base, true);
  }
  if (func == NULL) return 1;
  if (curnode->data.binary.left_operand->kind == NODE_POINTER_DECL){
     return symbol_add_from_pointer_decl(table, curnode->data.binary.left_operand, func);
  }
  /* update table name with function name */
  strcpy(table->name, curnode->data.binary.left_operand->data.identifier.name);

   /*call helper functions to add parameters to symbol table */
  if (symbol_add_from_func_paramlst(table, func) != 0) return 1;
  
  /* add function id to parent table if not found. If found check for type mismatch*/
  if ((symbol = symbol_find_identfier(table->parent, curnode->data.binary.left_operand)) != NULL){
     if (type_verify(symbol->result.type, func) != 0)
      compiler_print_error(curnode->data.binary.left_operand->location, "type mismatch for function %s\n", curnode->data.binary.left_operand->data.identifier.name);
    curnode->data.binary.left_operand->data.identifier.symbol = symbol;
    curnode->data.binary.left_operand->data.identifier.symbol->result.type = func;
  }
  else{
    return symbol_add_from_identifier(table->parent, curnode->data.binary.left_operand, func, true);
  }
  return 0;
}

/*symbol_add_from_decl_list -  processes decl_list node
      parameters: table - symbol table to which entries should be added
                  node  - node of decl_list
*/
int symbol_add_from_decl_list(struct symbol_table *table, struct node *node, struct type *base){
  if (node == NULL) return 0;
  
  switch(node->data.binary.right_operand->kind){
    case NODE_FUNC_DECL:
      symbol_add_from_func_decl(table, node->data.binary.right_operand, base);
      break;
    case NODE_ARRAY_DECL:        
      symbol_add_from_array_decl(table, node->data.binary.right_operand, base);
      break;
    case NODE_POINTER_DECL:
      symbol_add_from_pointer_decl(table, node->data.binary.right_operand, base);
      break;
    case NODE_IDENTIFIER :
      symbol_add_from_identifier(table, node->data.binary.right_operand, base, true);
      break;
    default:
      break;
  }
  symbol_add_from_decl_list(table, node->data.binary.left_operand, base);

  return 0;
}

/*function_declarator*/
int symbol_add_from_func_decl(struct symbol_table *table, struct node *node, struct type *rettype){
  struct type *func = NULL;
  
  func = symbol_func_create(node, rettype, false);
  if (func == NULL) return 1;

  if (node->data.binary.left_operand->kind == NODE_POINTER_DECL){
    return symbol_add_from_pointer_decl(table, node->data.binary.left_operand, func);
  }
  return symbol_add_from_identifier(table, node->data.binary.left_operand, func, true); 
}

/*symbol_add_from_pointer_decl - processes pointer_decl node 
  parameters - table - symbol table to which entry has to be added
               node  - pointer decl node in ast
               base - type to attach as target
  side effect creates pointer type and attaches the target to base 
  processes the right nodes till it reaches identifier node.
  returns 0- if success 1- failure             
*/
int symbol_add_from_pointer_decl(struct symbol_table *table, struct node *node, struct type *base){
  struct node *curnode = NULL;
  struct type *pointert = NULL;

  if (base != NULL && base->kind == TYPE_VOID){
    compiler_print_error(node->location, "void type is not allowed");
    return 1;
  }

  pointert = type_pointer_decl_create(node->data.binary.left_operand);
  type_pointer_closure(pointert, base);
  
  curnode = node->data.binary.right_operand;
  if (curnode->kind == NODE_FUNC_DECL)
    return  symbol_add_from_func_decl(table, curnode, pointert);
  else if (curnode->kind == NODE_ARRAY_DECL)
    return symbol_add_from_array_decl(table, curnode, pointert); 
  else if (curnode->kind == NODE_IDENTIFIER)
    return symbol_add_from_identifier(table, curnode, pointert, true);

  return 0;
}

/*symbol_add_from_array_decl - processes array_decl node 
  parameters - table - symbol table to which entry has to be added
               node  - array decl node in ast
               base - type to attach as target
  side effect -creates array type and attaches the target to base 
  and modifies the array size of the type with the evaluate value in right node.
  processes the left nodes till it reaches identifier node.
  returns 0- if success 1- failure             
*/
int symbol_add_from_array_decl(struct symbol_table *table, struct node *node, struct type *base){
  struct node *curnode = NULL;
  struct type *arrayt = NULL;

  if (base != NULL && base->kind == TYPE_VOID){
    compiler_print_error(node->location, "void type is not allowed");
    return 1;
  }
  /*if (base->kind == TYPE_BASIC){
    newnode = symbol_convert_to_pointer_decl(node);
    if (node->parent == NULL) 
      return symbol_add_from_pointer_decl(table, newnode, base);
  }*/
  if (node->kind == NODE_ARRAY_DECL){
    arrayt = type_array_decl_create(node);
    arrayt->data.arraytype.target = base;
    curnode = node->data.binary.left_operand;
    if (type_verify_incomplete_array(arrayt, false)){
      compiler_print_error(node->location, "incomplete array type is not allowed.");
      return 1;
    }
  }
  
  if (curnode->kind == NODE_ARRAY_DECL)
    return symbol_add_from_array_decl(table, curnode, arrayt);
  else if (curnode->kind == NODE_POINTER_DECL)
    return symbol_add_from_pointer_decl(table, curnode, arrayt);
  else if (curnode->kind == NODE_IDENTIFIER)
    return symbol_add_from_identifier(table, curnode, arrayt, true); 
  else if (curnode->kind == NODE_FUNC_DECL){
    compiler_print_error(node->location, "function with array return type is not allowed.");
    return 1;
  }

  return 0;
}

/*
convert first dimension of array decl to pointer decl
*/
struct node *symbol_convert_to_pointer_decl(struct node *top){
  struct node *newnode = NULL;
  struct node *pnode = NULL;
  enum node_type ntype;
  struct node *node = NULL;
  struct node *curnode = top;
  if (top == NULL) return NULL;
  /*assert(top->kind == NODE_ARRAY_DECL);*/
  if (curnode->kind == NODE_IDENTIFIER) return top;
  while(curnode && curnode->kind != NODE_IDENTIFIER){
    node = curnode;
    if (curnode->kind == NODE_POINTER_DECL)
      curnode = curnode->data.binary.right_operand;
    else if (curnode->kind == NODE_ARRAY_DECL )
      curnode = curnode->data.binary.left_operand;
    else if (curnode->kind == NODE_ABSTR_DECL )
      curnode = curnode->data.binary.left_operand;
    else
       break;
  }
  if (node->kind != NODE_ARRAY_DECL && node->kind != NODE_ABSTR_DECL) return top;

  pnode = node_one_operand(NODE_POINTER, "*", NULL, node->location);
  newnode = node_two_operands(NODE_POINTER_DECL, "pointer_decl", pnode, node->data.binary.left_operand, node->location);
  newnode->parent = node->parent;   
  pnode->parent = newnode;
  ntype = node->parent->ntype;
  switch (ntype){
    case NODE_UNARY:
      node->parent->data.unary.child_operand = newnode;
      break;
    case NODE_BINARY:
      if (node->parent->data.binary.left_operand == node)
        node->parent->data.binary.left_operand = newnode;
      else
        node->parent->data.binary.right_operand = newnode;
      break;
    case NODE_TERNARY:
      if (node->parent->data.ternary.left_operand == node)
        node->parent->data.ternary.left_operand = newnode;
      else if (node->parent->data.ternary.middle_operand == node)
        node->parent->data.ternary.left_operand = newnode;
      else
        node->parent->data.ternary.right_operand = newnode;
      break;
    default:
      return top;
  }
  node->data.binary.left_operand = NULL;
  node->data.binary.right_operand = NULL;
  node->parent = NULL;
  free(node); 
  return newnode;
}
/***********************************/
/* symbol_add_from_label - add new label to labels table
  parameters - table - symbol table that has link to labels table
               id - label or goto node
               bdefine - true if the node is label node
                         false if the node is goto node
  side effect - creates a new label node and adds it to linked list
  always returns 0
 */
int symbol_add_from_label(struct symbol_table *table, struct node *id, bool bdefine) {
  struct symbol_label_list *label_list = NULL;
  struct symbol_label_table *label_table = table->labeltable;
  assert(id->kind == NODE_IDENTIFIER);
  
  label_list = symbol_find_label(table, id);

  if (label_list != NULL) { 

    if (bdefine && label_list->kind == LABEL_DEFINE)
      compiler_print_error(id->location, "label: %s defined multiple times.", id->data.identifier.name);
    else 
      label_list->kind = bdefine ? LABEL_DEFINE: label_list->kind;
    
    return 0;
  }

  label_list = malloc(sizeof(struct symbol_label_list));
  assert(NULL != label_list);

  strncpy(label_list->name, id->data.identifier.name, IDENTIFIER_MAX);
  strncpy(label_list->symbol.name, id->data.identifier.name, IDENTIFIER_MAX);
  strncpy(label_list->symbol.table_name, table->name, IDENTIFIER_MAX);
  id->data.identifier.symbol = &label_list->symbol;

  label_list->location = id->location;
  label_list->kind = bdefine ? LABEL_DEFINE : LABEL_REFER;
  label_list->next = label_table->labels;
  label_table->labels = label_list;
  return 0;
}

/*symbol_find_label - finds the label node name in labels symbol table
    parameters table - symbol table that contains the label table linked to it.
               id - label node 
  returns the label entry in table if found
  else returns NULL 
*/
struct symbol_label_list *symbol_find_label(struct symbol_table *table, struct node *id){
  struct symbol_label_list *iter;
  struct symbol_label_table *lbltable = NULL; 
  if (table == NULL || id == NULL) return NULL;

  lbltable = table->labeltable;
  if (lbltable == NULL) return  NULL;

  assert(id->kind == NODE_IDENTIFIER);
  
  for (iter = lbltable->labels; NULL != iter; iter = iter->next) {
    if (strcmp(id->data.identifier.name, iter->name) == 0) {
      id->data.identifier.symbol = &iter->symbol;
      return iter;
    }
  }

  return NULL; 
}

/*symbol_check_labels - 
    parameters table - symbol table to be processed
    processes all labels at the end of function definition
    in the symbol table that has label table linked to it.
    if any label is found in "refered" state instead of "defined" state
    method emits an error. 
*/
void symbol_check_labels(struct symbol_table *table){
  struct symbol_label_list *iter;
  struct symbol_label_table *lbltable =  NULL;
  if (table == NULL) return;

  lbltable = table->labeltable;
  if (lbltable == NULL ) return ;
  
  for (iter = lbltable->labels; NULL != iter; iter = iter->next) {
    if (iter->kind == LABEL_REFER) {
      compiler_print_error(iter->location, "label: %s not defined.", iter->name);
    }
  }
}

/****************************************/
static int has_params(struct node *node){
    struct node *curnode = NULL;
    struct type* vtype = NULL;
    if (node == NULL) return 1;
    
    curnode = node->data.binary.right_operand;
    if (curnode->kind == NODE_PARAM_LIST && curnode->data.binary.left_operand == NULL){
        curnode = curnode->data.binary.right_operand;
        if (curnode != NULL && curnode->kind == NODE_PARAM){
          vtype = type_basic_create(curnode->data.binary.left_operand);
          if (vtype->kind == TYPE_VOID &&  curnode->data.binary.right_operand == NULL)
           return 0;
        }
    }
    return 1;
}

int symbol_check_params(struct node *node, struct typelist* paramtypelist, int *nparams){
   struct typelist *lst = paramtypelist;
   int isvoidseen = 0;
   
   while(lst && lst->curtype){
      if (isvoidseen){
        compiler_print_error(node->location, "parameter cannot be a void type\n");
        return 1;
      }
      else if (lst->curtype->kind == TYPE_VOID)
        isvoidseen = 1;
      else if (lst->curtype->kind == TYPE_FUNCTION || 
               (lst->curtype->kind == TYPE_POINTER && lst->curtype->data.ptrtype.target->kind == TYPE_FUNCTION) ){
        compiler_print_error(node->location, "parameter cannot be a function type\n");
        return 1;
      }
      lst = lst->next;
      *nparams += 1;
   }
   return 0;
}

/*helper to create function type */
struct type *symbol_func_create(struct node *node, struct type *rettype, bool badd){
  struct typelist *paramtypelist = NULL;
  struct type *functype = NULL;
  int nparams = 0;

  if (rettype->kind == TYPE_FUNCTION || (rettype->kind == TYPE_POINTER && rettype->data.ptrtype.target->kind == TYPE_FUNCTION)){
    compiler_print_error(node->location, "function return type cannot be function or function pointer");
    return NULL;
  }
  if (node->data.binary.left_operand->kind == NODE_FUNC_DECL){
    compiler_print_error(node->data.binary.left_operand->location, "function return type cannot be function or function pointer");
    return NULL;
  }
  if (node->data.binary.left_operand->kind != NODE_IDENTIFIER){
    compiler_print_error(node->data.binary.left_operand->location, "function return type cannot be function or function pointer");
    return NULL;
  }
  if (node->kind == NODE_FUNC_DECL && has_params(node) == 0){
     paramtypelist = NULL;
  }
  else{
    paramtypelist = malloc(sizeof(struct typelist));
    assert (paramtypelist != NULL);
    paramtypelist->next = NULL;
    paramtypelist->curtype = NULL;
    type_list_create(node->data.binary.right_operand, paramtypelist, badd);
  }
  if (symbol_check_params(node,paramtypelist, &nparams) == 1) return NULL;

  functype = type_function();
  functype->data.functype.rvaltype = rettype;
  functype->data.functype.params = paramtypelist;
  functype->data.functype.nparams = nparams;
  return functype;
}



/*helper to add parameters to symbol table*/
int  symbol_add_from_func_paramlst(struct symbol_table *table, struct type *func){
   struct typelist *param = NULL;
   if (func == NULL )
    return 1;
   
   param = func->data.functype.params;
   while(param){
     if (param->id && param->curtype)
     symbol_add_from_identifier(table, param->id, param->curtype, true);

     param = param->next;
   }
   return 0;
}

void symbol_add_string(struct node *node){
  static int nlabel;
  struct string_table *next = NULL;
  struct string_table *prev = NULL;
  struct string_table *item = NULL;
  assert (node->kind == NODE_STRING);

  if (node->data.string_literal.string[0] == '\0') return;
  if ((item = symbol_find_string(node->data.string_literal.rstring)) != NULL){
    node->data.string_literal.entry = item;
    node->data.string_literal.entry->result.type = item->result.type;
  }
  else {    
    item = malloc (sizeof (struct string_table));
    assert (item != NULL);
    
    item->next = NULL; 
    item->string = strdup(node->data.string_literal.rstring);
    /*
    item->string = malloc(node->data.string_literal.strlen + 1);
    strcpy(item->string, node->data.string_literal.string);
    item->string[node->data.string_literal.strlen] = '\0';
    */
    sprintf(item->label, "StringLabel_%d",++nlabel);

    if (head == NULL) {
      head = item;
      head->next = NULL;
    }
    else{
      next = head;
      prev = head;
      while(next){
        prev = next;
        next = next->next;
      }
      prev->next = item;      
    }    
    item->result.type = type_array();
    item->result.type->data.arraytype.hassize = true;
    item->result.type->data.arraytype.arrsize = strlen(item->string); /*node->data.string_literal.strlen;*/
    item->result.type->data.arraytype.target = type_basic(false, TYPE_BASIC_CHAR);
    item->result.islvalue = 1;

    node->data.string_literal.entry = item;
  }
}

struct string_table *symbol_find_string(char *s){
  struct string_table *curitem = head;
  if (head == NULL) return NULL;
  if (s[0] == '\0') return NULL;

  while (curitem){
    if (strcmp(curitem->string, s) == 0) return curitem;
    curitem = curitem->next;
  }
  return NULL;
}
/****************************************/
/*symbol_parse_ast - parses the AST 
  parameters: table - current symbol table in which entry has to be added
              node - current node in the ast
  side effect - creates new symbol table at apporpriate nodes and passes it
                next nodes. 
*/
void symbol_parse_ast(struct symbol_table *table, struct node *node) {
  struct symbol *symbol = NULL;
  if (node != NULL){
    switch (node->kind) {
      case NODE_IDENTIFIER:
        if ((symbol = symbol_find_identfier(table, node)) == NULL)
          compiler_print_error(node->location, "undefined identifier %s", node->data.identifier.name);
        else
          node->data.identifier.symbol = symbol;
        break;       
      case NODE_DECL:
        symbol_add_from_decl(table, node);
        break;
      case NODE_FUNC_DEF:
        symbol_add_from_func_def(table, node);
        break;
      case NODE_FUNC_DEF_SPEC:
        symbol_add_from_func_def_spec(table, node);
        break;
      case NODE_FUNC_CALL:
        symbol_parse_ast(table, node->data.binary.left_operand);
        symbol_parse_ast(table, node->data.binary.right_operand);
        /*symbol_find_function(table, node);*/
        break;
      case NODE_COMP_STATEMENT:
        symbol_parse_ast(symbol_table_add(table),node->data.unary.child_operand);
        break;     
      case NODE_LABEL:
        symbol_add_from_label(table,node->data.binary.left_operand, true);
        symbol_parse_ast(table,node->data.binary.right_operand);  
        break;
      case NODE_GOTO:
        symbol_add_from_label(table, node->data.unary.child_operand, false);
        break;
      case NODE_IF_ELSE:   
      case NODE_FOR_EXPR:
      case NODE_TERNARY_OP:
        symbol_parse_ast(table, node->data.ternary.left_operand);
        symbol_parse_ast(table, node->data.ternary.middle_operand);
        symbol_parse_ast(table, node->data.ternary.right_operand);
        break;
      case NODE_TR_UNIT:
      case NODE_STATEMENT_LIST:
      case NODE_EXPR_LIST:
      case NODE_DOWHILE:
      case NODE_IF:
      case NODE_WHILE:
      case NODE_FOR:
      case NODE_COMMA_EXPR:
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
        symbol_parse_ast(table, node->data.binary.left_operand);
        symbol_parse_ast(table, node->data.binary.right_operand);
        break;
      case NODE_CAST :
        symbol_parse_ast(table, node->data.binary.left_operand);
        symbol_parse_ast(table, node->data.binary.right_operand);
        break;
      case NODE_POST_INCR:
      case NODE_POST_DECR:
      case NODE_TILDE:
      case NODE_POINTER:      
      case NODE_PRE_INCR:
      case NODE_PRE_DECR:
      case NODE_INDIRECT:
      case NODE_ADDRESS_OP:
      case NODE_UNARY_PLUS:
      case NODE_UNARY_MINUS:
      case NODE_EXCLAMATION:
      case NODE_BREAK:
      case NODE_RETURN:     
      case NODE_CONTINUE:
        symbol_parse_ast(table, node->data.unary.child_operand);
        break;
      
      case NODE_STRING:
        symbol_add_string(node);
        break;
      case NODE_NUMBER:
      case NODE_EXPRESSION_STATEMENT:
      case NODE_BINARY_OPERATION: 
      case NODE_NULL_STATEMENT:
        break;        
      default:
        break;
    }

  }
}

/*
static int symbol_add_from_expression(struct symbol_table *table, struct node *expression);

static int symbol_add_from_binary_operation(struct symbol_table *table, struct node *binary_operation) {
  assert(NODE_BINARY_OPERATION == binary_operation->kind);

  switch (binary_operation->data.binary_operation.operation) {
    case BINOP_MULTIPLICATION:
    case BINOP_DIVISION:
    case BINOP_ADDITION:
    case BINOP_SUBTRACTION:
      return symbol_add_from_expression(table, binary_operation->data.binary_operation.left_operand)
           + symbol_add_from_expression(table, binary_operation->data.binary_operation.right_operand);
    case BINOP_ASSIGN:
      if (NODE_IDENTIFIER == binary_operation->data.binary_operation.left_operand->kind) {
        return symbol_add_from_identifier(table, binary_operation->data.binary_operation.left_operand, true)
             + symbol_add_from_expression(table, binary_operation->data.binary_operation.right_operand);
      } else {
        compiler_print_error(binary_operation->data.binary_operation.left_operand->location,
                             "left operand of assignment must be an identifier");
        return 1
             + symbol_add_from_expression(table, binary_operation->data.binary_operation.left_operand)
             + symbol_add_from_expression(table, binary_operation->data.binary_operation.right_operand);
      }
    default:
      assert(0);
      return 1;
  }
}

static int symbol_add_from_expression(struct symbol_table *table, struct node *expression) {
  switch (expression->kind) {
    case NODE_BINARY_OPERATION:
      return symbol_add_from_binary_operation(table, expression);
    case NODE_IDENTIFIER:
      return symbol_add_from_identifier(table, expression, false);
    case NODE_NUMBER:
      return 0;
    default:
      assert(0);
      return 1;
  }
}

static int symbol_add_from_expression_statement(struct symbol_table *table, struct node *expression_statement) {
  assert(NODE_EXPRESSION_STATEMENT == expression_statement->kind);

  return symbol_add_from_expression(table, expression_statement->data.expression_statement.expression);
}

int symbol_add_from_statement_list(struct symbol_table *table, struct node *statement_list)
{
  int error_count = 0;
  assert(NODE_STATEMENT_LIST == statement_list->kind);

  if (NULL != statement_list->data.statement_list.init) {
    error_count += symbol_add_from_statement_list(table, statement_list->data.statement_list.init);
  }
  return error_count
       + symbol_add_from_expression_statement(table, statement_list->data.statement_list.statement);
}
*/

/***********************
 * PRINT SYMBOL TABLES *
 ***********************/

void symbol_print_table(FILE *output, struct symbol_table *table) {
  struct symbol_list *sym_iter = NULL;
  struct symbol_label_list *lbl_iter = NULL;
  struct type *t = NULL;
  struct symbol_table *child= NULL;

  /*print variables in symbol table*/
  if (table->variables){
    fprintf(output, "symbol table: %s \n", table->name);
    fprintf(output, "   variables:\n");
    for (sym_iter = table->variables; NULL != sym_iter; sym_iter = sym_iter->next) {
      fprintf(output, "    variable: %s /* %p  */\n", sym_iter->symbol.name, (void *)&sym_iter->symbol);

      t = sym_iter->symbol.result.type;
      fputs("     ", output);
      symbol_print_type(output, t);
      fputs("\n", output);
    }
    fputs("\n", output);
  }

  /*print labels in symbol table */
  if (table->haslabels && table->labeltable && table->labeltable->labels) {
    fprintf(output, "   statement_labels:\n");
    for (lbl_iter = table->labeltable->labels; NULL != lbl_iter; lbl_iter = lbl_iter->next) {
      fprintf(output, "    label: %s \n", lbl_iter->name);
      fputs("\n", output);
    }
    fputs("\n", output);
  }

  /* print children symbol table */
  child = table->children;
  while(child){

    symbol_print_table(output, child);
    child = child->next;
  }
  fputs("\n", output);
}

void symbol_print_type(FILE *output, struct type *t){
  if (t == NULL) return;
  
  if (t->kind == TYPE_POINTER){
    fprintf(output, "[%s", t->name);
    t= t->data.ptrtype.target;
    symbol_print_type(output, t);
    fputs("]", output);
  }
  else if (t->kind == TYPE_ARRAY){
    if (t->data.arraytype.hassize)
     fprintf(output, "[%s[%d,", t->name, t->data.arraytype.arrsize);
    else
      fprintf(output, "[%s[,", t->name);
    t= t->data.arraytype.target;
    symbol_print_type(output, t);
    fputs("]", output);
  }
  else if (t->kind == TYPE_FUNCTION){         
    symbol_print_type(output, t->data.functype.rvaltype);
    fprintf(output, "[%s", t->name);
    symbol_print_paramlist(output, t);
    fputs("]", output);
  }
  else if (t->kind == TYPE_VOID)
    fprintf(output, "[%s]", t->name);
  else if (t->kind == TYPE_BASIC)
    fprintf(output, "[%s]", t->name);
    
}

void symbol_print_paramlist(FILE *output, struct type *f){
  struct typelist *lst = f->data.functype.params;
  fputs("(", output);
  while(lst){
      symbol_print_type(output, lst->curtype);
      lst = lst->next;
      if (lst)fputs(",", output);
  }
  fputs(")", output);
}

struct symbol_table *symbol_find_table(struct symbol *symbol){
  struct symbol_table *parent = NULL;
  struct symbol_table *curtable = NULL;
  struct type *t = NULL;
  
  t = symbol->result.type;
  assert(t->kind == TYPE_FUNCTION);
 
  parent = symbol->table;
  curtable = parent->children;

  while(curtable){
    if (strcmp(symbol->name, curtable->name) == 0)return curtable;
    curtable = curtable->next;
  }
  return NULL;
}

int symbol_is_func_param(struct typelist *params, char *name){
  struct typelist *lst = params;
   if (name[0] == '\0') return 0;
   if (lst == NULL) return 0;

   while(lst && lst->curtype){
      if (strcmp(lst->id->data.identifier.name, name) == 0) return 1;
      lst = lst->next;
   }
   return 0;
}

int symbol_calculate_stack_frame_size(struct symbol *symbol){
  struct symbol_table *curtable = symbol_find_table(symbol);
  if (curtable == NULL) return 0;
  return symbol_calculate_curtable_size(curtable, symbol);
}

int symbol_calculate_curtable_size(struct symbol_table *table, struct symbol *symbol){
  struct symbol_list *sym_iter = NULL;  
  struct type *functype = NULL;
  struct type *t = NULL;
  int size = 0;
  int pack = 0;
  int cursz = 0;
  if (table == NULL) return 0;
  
  functype = symbol->result.type;

  for (sym_iter = table->variables; NULL != sym_iter; sym_iter = sym_iter->next) { 
     t = sym_iter->symbol.result.type;
     if (symbol_is_func_param(functype->data.functype.params, sym_iter->symbol.name)) continue;

     cursz = type_get_alignment_size(t, size, &pack, DEF_MULTIPLIER); /*default multiplier 1*/
     size += (cursz + pack);
  }

  size += symbol_calculate_block_size(table, symbol);
  return size;
}

int symbol_calculate_block_size(struct symbol_table *parent, struct symbol *symbol){
  struct symbol_table *child= parent->children;
  int block_sz = 0;
  int ret_sz = 0;

  while(child){
    block_sz = symbol_calculate_curtable_size(child, symbol);
    if (block_sz >= ret_sz) ret_sz = block_sz;
    child = child->next;
  }
  return ret_sz;
}

void symbol_print_strings(FILE *output){
  struct string_table *curitem = head;
  if (head == NULL) return;
 
  while (curitem){
    fprintf(output, "%s: %10s %s\n",curitem->label, ".asciiz",curitem->string);
    curitem = curitem->next;
  }
}

void symbol_print_globals(FILE *output, struct symbol_table *global){
  struct symbol_list *sym_iter = NULL;  
  struct type *t = NULL;
  /*int size = 0, padding = 0;*/
  if (global == NULL) return;
 
  for (sym_iter = global->variables; NULL != sym_iter; sym_iter = sym_iter->next) { 
     t = sym_iter->symbol.result.type;
    if (t->kind == TYPE_FUNCTION) continue;
     /*size = type_get_alignment_size(t, 0, &padding, 1);*/
    if (t->kind == TYPE_POINTER)
      fprintf(output, "%s:\n.word %d\n",sym_iter->symbol.name, 0); 

    if (t->kind == TYPE_BASIC && t->data.basic.datatype == TYPE_BASIC_INT)
      fprintf(output, "%s:\n.word %d\n",sym_iter->symbol.name, 0); 
    if (t->kind == TYPE_BASIC && t->data.basic.datatype == TYPE_BASIC_SHORT)
      fprintf(output, "%s:\n.half %d\n",sym_iter->symbol.name, 0); 
    if (t->kind == TYPE_BASIC && t->data.basic.datatype == TYPE_BASIC_CHAR)
      fprintf(output, "%s:\n.byte %d\n",sym_iter->symbol.name, 0); 
   }
}