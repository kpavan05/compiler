#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "node.h"
#include "compiler.h"
#include "symbol.h"
#include "type.h"
#include "ir.h"

static int _hasmain = 0;
static struct loop_id *lastloop = NULL;
/************************
 * CREATE IR STRUCTURES *
 ************************/

/*
 * An IR section is just a list of IR instructions. Each node has an associated
 * IR section if any code is required to implement it.
 */
static struct ir_section *ir_section(struct ir_instruction *first, struct ir_instruction *last) {
  struct ir_section *code;
  code = malloc(sizeof(struct ir_section));
  assert(NULL != code);

  code->first = first;
  code->last = last;
  return code;
}

static struct ir_section *ir_copy(struct ir_section *orig) {
  return ir_section(orig->first, orig->last);
}

/*
 * This joins two IR sections together into a new IR section.
 */
static struct ir_section *ir_concatenate(struct ir_section *before, struct ir_section *after) {
  /* patch the two sections together */
  before->last->next = after->first;
  after->first->prev = before->last;

  return ir_section(before->first, after->last);
}

static struct ir_section *ir_add_section(struct ir_section *orig, struct ir_section *after) {
  if (orig != NULL &&  after != NULL){
    orig->last->next = after->first;
    after->first->prev = orig->last;
    orig->first = orig->first;
    orig->last = after->last;
    return orig;
  }
  else if (orig == NULL && after != NULL){
    return ir_copy(after);
  }
  else if (orig != NULL && after == NULL){
    return orig;
  }
  return NULL;
}

static struct ir_section *ir_append(struct ir_section *section,
                                                           struct ir_instruction *instruction) {
  if (NULL == section) {
    section = ir_section(instruction, instruction);

  } else if (NULL == section->first || NULL == section->last) {
    section = ir_section(instruction, instruction);
  } else {
    instruction->prev = section->last;
    section->last->next = instruction;
    
    section->first = section->first;
    section->last = instruction;
  }
  return section;
}

/*
 * An IR instruction represents a single 3-address statement.
 */
static struct ir_instruction *ir_instruction(enum ir_instruction_kind kind) {
  struct ir_instruction *instruction;

  instruction = malloc(sizeof(struct ir_instruction));
  assert(NULL != instruction);

  instruction->kind = kind;

  instruction->next = NULL;
  instruction->prev = NULL;
  instruction->livelist = NULL;
  instruction->block = NULL;
  instruction->isdead = 0;
  instruction->reset_reg = 0;
  return instruction;
}

static void ir_running_table_name(struct ir_operand *operand, char *table_name, bool bset){
  static char _curtable[IDENTIFIER_MAX + 1];
  if (bset){
    struct symbol_table *table = NULL;
    if (operand == NULL || operand->kind != OPERAND_IDENTIFIER) return;
    table = symbol_find_table(operand->data.symbol);
    strcpy(_curtable, table->name);
  }
  else{
    strcpy(table_name, _curtable);
  }  
}
static void ir_operand_number(struct ir_instruction *instruction, int position, struct node *number) {
  instruction->operands[position].kind = OPERAND_NUMBER;
  instruction->operands[position].data.number = number->data.number.value;
}

static void ir_operand_identifier(struct ir_instruction *instruction, int position, struct node *node) {
  instruction->operands[position].kind = OPERAND_IDENTIFIER;
  instruction->operands[position].data.symbol = node->data.identifier.symbol;
}

static void ir_operand_gen_user_label(struct ir_instruction *instruction, int position, struct node *node){
  assert(node->kind == NODE_IDENTIFIER);
  if (node->parent->kind == NODE_LABEL || node->parent->kind == NODE_GOTO){
    instruction->operands[position].kind = OPERAND_LABEL;
    sprintf(instruction->operands[position].data.label,"__%s_%s_%s","User_Lbl", node->data.identifier.symbol->table_name, node->data.identifier.name);
  }
  else assert(0);
}

int ir_loopid(int id, enum ir_loop_kind kind){
  struct loop_id *curloop = NULL;
  int retid = -1;

  if (kind == IR_LOOP_INCR){
    curloop = malloc(sizeof(struct loop_id));
    curloop->prev = NULL;
    curloop->startid = -1;
    curloop->endid = -1;
    if (lastloop == NULL) 
      lastloop = curloop;
    else{
      curloop->prev = lastloop;
      lastloop = curloop;
    }
  }

  switch(kind){
    case IR_LOOP_BREAK:
      retid = lastloop ? lastloop->endid : -1;
      break;
    case IR_LOOP_CONTINUE:
      retid = lastloop ? lastloop->startid : -1;
      break;
    case IR_LOOP_INCR:
      lastloop->startid = id;
      retid = lastloop->startid;
      break;
    case IR_LOOP_END:
      lastloop->endid = id;
      retid = lastloop->endid;
      break;
    case IR_LOOP_DESTROY:
      curloop =  lastloop;
      lastloop = curloop->prev;
      free(curloop); curloop = NULL;
      break;
    default:
      assert(0);
  }
  return retid;
}

static void ir_operand_label(struct ir_instruction *instruction, int position, enum ir_loop_kind kind) {
  static int next_labelid;
  /*static int loop_incrid;
  static int loop_endid;*/
  static int loop_procend;
  char table[IDENTIFIER_MAX];

  instruction->operands[position].kind = OPERAND_LABEL;
  ir_running_table_name(NULL, table, 0);

  /*assumes break and continue are used inside loop constructs*/
  switch(kind){
    case IR_LOOP_BREAK:
      {
        int id; 
        struct location loc;
        loc.first_line = 0; loc.first_column = 0; loc.last_line = 0; loc.last_column = 0;
        id = ir_loopid(0, IR_LOOP_BREAK);
        if (id == -1) compiler_print_error(loc, "break is found outside loop");
        sprintf(instruction->operands[position].data.label,"__%s_%s_%d","Generated_Lbl", table, id);
      } 
      break;
    case IR_LOOP_CONTINUE:
      {
        int id; 
        struct location loc;
        loc.first_line = 0; loc.first_column = 0; loc.last_line = 0; loc.last_column = 0;
        id = ir_loopid(0, IR_LOOP_CONTINUE);
        if(id == -1) compiler_print_error(loc, "continue is found outside loop");
        sprintf(instruction->operands[position].data.label,"__%s_%s_%d","Generated_Lbl", table, id);
      }
      break;
    case IR_LOOP_INCR:
      /*loop_incrid = next_labelid;*/
       ir_loopid(next_labelid, IR_LOOP_INCR);     
      sprintf(instruction->operands[position].data.label,"__%s_%s_%d","Generated_Lbl", table, next_labelid++);
     break;
    case IR_LOOP_END:
      /*loop_endid = next_labelid;*/
      ir_loopid(next_labelid, IR_LOOP_END); 
      sprintf(instruction->operands[position].data.label,"__%s_%s_%d","Generated_Lbl", table, next_labelid++);
     break;
    case IR_LOOP_NONE:
      sprintf(instruction->operands[position].data.label,"__%s_%s_%d","Generated_Lbl", table, next_labelid++);
      break;
    case IR_LOOP_DESTROY:
      ir_loopid(0, IR_LOOP_DESTROY);
      break;
    case IR_LOOP_PROCEND:
      loop_procend = next_labelid;
      sprintf(instruction->operands[position].data.label,"__%s_%s_%d","Generated_Lbl", table, next_labelid++);
      break;
    case IR_LOOP_RETURN:
      sprintf(instruction->operands[position].data.label,"__%s_%s_%d","Generated_Lbl", table, loop_procend);
      break;
    case IR_LOOP_PROCDESTROY:
      loop_procend = -1;
      break;
    default:
      break;
  }
}

static void ir_operand_temporary(struct ir_instruction *instruction, int position) {
  static int next_temporary;
  instruction->operands[position].kind = OPERAND_TEMPORARY;
  instruction->operands[position].data.temporary = next_temporary++;
}

static void ir_operand_copy(struct ir_instruction *instruction, int position, struct ir_operand *operand) {
  instruction->operands[position] = *operand;
}

/*******************************
 * GENERATE IR FOR EXPRESSIONS *
 *******************************/
static void ir_generate_for_number(struct node *number) {
  struct ir_instruction *instruction;
  assert(NODE_NUMBER == number->kind);

  instruction = ir_instruction(IR_CONST);
  ir_operand_temporary(instruction, 0);
  ir_operand_number(instruction, 1, number);

  number->ir = ir_section(instruction, instruction);

  number->data.number.result.ir_operand = &instruction->operands[0];
}
static void ir_generate_for_string(struct node *node){
  struct ir_instruction *instruction;
  assert(NODE_STRING == node->kind);

  instruction = ir_instruction(IR_ADDR_OF);

  ir_operand_temporary(instruction, 0);
  instruction->operands[1].kind = OPERAND_LABEL;
  assert(node->data.string_literal.entry != NULL);
  strcpy(instruction->operands[1].data.label, node->data.string_literal.entry->label);
  node->ir = ir_section(instruction, instruction);
  node->data.string_literal.entry->result.ir_operand = &instruction->operands[0];
}

static struct ir_instruction *ir_generate_for_gen_constant(long number, struct ir_operand *operand) {
  struct ir_instruction *instruction;
  struct ir_operand *constant = NULL;
  
  instruction = ir_instruction(IR_CONST);
  if (operand == NULL)
    ir_operand_temporary(instruction, 0);
  else
    ir_operand_copy(instruction, 0, operand);
  
  constant = malloc(sizeof(struct ir_operand));
  assert(constant != NULL);
  constant->kind = OPERAND_NUMBER;
  constant->data.number = number;
  ir_operand_copy(instruction,1, constant);
  return instruction;
}

static void ir_generate_for_identifier(struct node *identifier) {
  struct ir_instruction *instruction;
  assert(NODE_IDENTIFIER == identifier->kind);
  
  instruction = ir_instruction(IR_ADDR_OF);
  if (identifier->data.identifier.symbol->result.ir_operand == NULL){
    ir_operand_temporary(instruction, 0);
  }
  else ir_operand_copy(instruction, 0, identifier->data.identifier.symbol->result.ir_operand);
  ir_operand_identifier(instruction, 1, identifier); 
  identifier->ir = ir_section(instruction, instruction);
  identifier->data.identifier.symbol->result.ir_operand = &instruction->operands[0];   
  identifier->data.identifier.symbol->result.ir_operand->islvalue = identifier->data.identifier.symbol->result.islvalue;
}

static struct ir_instruction *ir_generate_for_load(struct node *node){
  struct ir_instruction *instruction = NULL;
  struct ir_operand *operand = NULL;
  struct result *res = NULL;
  res = node_get_result(node);
  operand = res->ir_operand;
  assert (operand != NULL);

  if (res->type->kind == TYPE_BASIC){
    switch(res->type->data.basic.datatype){
      case TYPE_BASIC_INT:
      case TYPE_BASIC_LONG:
       instruction = ir_instruction(IR_LOAD);
       break;
      case TYPE_BASIC_CHAR:
       if (res->type->data.basic.is_unsigned)
        instruction = ir_instruction(IR_LOAD_UNSIGNEDBYTE);
       else
        instruction = ir_instruction(IR_LOAD_BYTE);
       break;
      case TYPE_BASIC_SHORT:
       if (res->type->data.basic.is_unsigned)
        instruction = ir_instruction(IR_LOAD_UNSIGNEDHALFWD);
       else
        instruction = ir_instruction(IR_LOAD_HALF_WD);
       break;
      default : assert(0);return NULL;
    }
  }
  else
    instruction = ir_instruction(IR_LOAD);

  ir_operand_temporary(instruction, 0);
  ir_operand_copy(instruction, 1, operand);
  instruction->operands[0].islvalue = 0;
  return instruction;
}

static struct ir_instruction *ir_instruction_store(struct type *t){
  struct ir_instruction *instruction = NULL;
  if (t == NULL) return NULL;
  if (t->kind == TYPE_BASIC){
    switch(t->data.basic.datatype){
      case TYPE_BASIC_INT:
       instruction = ir_instruction(IR_STORE);
       break;
      case TYPE_BASIC_CHAR:
       instruction = ir_instruction(IR_STORE_BYTE);
       break;
      case TYPE_BASIC_SHORT:
       instruction = ir_instruction(IR_STORE_HALF_WD);
       break;
      default : assert(0);return NULL;
    }
  }
  else
    instruction = ir_instruction(IR_STORE);
  return instruction;
}

static void ir_generate_for_simple_assignment(struct node *node) {
  struct ir_instruction *instruction = NULL;
  struct ir_operand *operand = NULL;
  struct result *lres = NULL;
  struct result *rres = NULL;

  ir_generate_for_expression(node->data.binary.right_operand);
  ir_generate_for_expression(node->data.binary.left_operand);
  /*add ir of right operand*/
  node->ir = ir_concatenate(node->data.binary.left_operand->ir, node->data.binary.right_operand->ir);
  lres = node_get_result(node->data.binary.left_operand);
  rres = node_get_result(node->data.binary.right_operand);
  assert (lres != NULL && rres != NULL);  
  node->ir = rres->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.right_operand)) : node->ir;
  operand = rres->ir_operand->islvalue == 1 ? &node->ir->last->operands[0]: rres->ir_operand;

  instruction = ir_instruction_store(lres->type);
  ir_operand_copy(instruction, 0, lres->ir_operand);
  ir_operand_copy(instruction, 1, operand);  
  node->ir = ir_append(node->ir, instruction);
  node->data.binary.result.ir_operand = &instruction->operands[0];
  node->data.binary.result.ir_operand->islvalue = lres->ir_operand->islvalue;
  /*
  left = node->data.binary.left_operand;
  assert(NODE_IDENTIFIER == left->kind);  
  if (NULL == left->data.identifier.symbol->result.ir_operand) {
    ir_operand_temporary(instruction, 0);
    left->data.identifier.symbol->result.ir_operand = &instruction->operands[0];
  } else {
    ir_operand_copy(instruction, 0, left->data.identifier.symbol->result.ir_operand);
  }
  */
}


static void ir_generate_for_addressop(struct node* node){
  struct ir_operand *operand = NULL;
  assert(NODE_ADDRESS_OP == node->kind);

  ir_generate_for_expression(node->data.unary.child_operand);
  operand = node_get_result(node->data.unary.child_operand)->ir_operand;
  assert(operand->islvalue == 1);
  node->ir = ir_copy(node->data.unary.child_operand->ir);
  node->data.unary.result.ir_operand = operand;
  node->data.unary.result.ir_operand->islvalue = operand->islvalue;
}

static void ir_generate_for_indirectop(struct node* node){
  struct result *res = NULL; 
  struct ir_operand *operand = NULL;
  assert(NODE_INDIRECT == node->kind);

  ir_generate_for_expression(node->data.unary.child_operand); 
  res = node_get_result(node->data.unary.child_operand); 
  assert (res != NULL);
  /* append ir code of child operand*/
  node->ir = ir_copy(node->data.unary.child_operand->ir);
  node->ir = res->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.unary.child_operand)) : node->ir;
  operand = res->ir_operand->islvalue == 1 ? &node->ir->last->operands[0] : res->ir_operand;
  node->data.unary.result.ir_operand = operand;
  node->data.unary.result.ir_operand->islvalue = 1;
}

static void ir_generate_for_logical_and(struct node* node){
  struct ir_operand *left = NULL; 
  struct ir_operand *right = NULL;
  struct result *lres = NULL;
  struct result *rres = NULL;
  struct ir_instruction *resinstr = NULL;
  struct ir_instruction *gotoFinstr = NULL; 
  struct ir_instruction *endinstr = NULL;
  struct ir_instruction *instruction = NULL;
  assert(NODE_AMPERSAND_AMPERSAND == node->kind);

  ir_generate_for_expression(node->data.binary.left_operand); 
  ir_generate_for_expression(node->data.binary.right_operand);
  lres = node_get_result(node->data.binary.left_operand);
  rres = node_get_result(node->data.binary.right_operand);
  
  gotoFinstr = ir_instruction(IR_LABEL);
  ir_operand_label(gotoFinstr, 0, IR_LOOP_NONE);
  endinstr = ir_instruction(IR_LABEL);
  ir_operand_label(endinstr, 0, IR_LOOP_NONE);

  /*append ir of left expression*/
  node->ir = ir_copy(node->data.binary.left_operand->ir);
  node->ir = lres->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.left_operand)):node->ir;
  left = lres->ir_operand->islvalue == 1 ? &node->ir->last->operands[0] : lres->ir_operand;
  /*gotoIfFalse for left expression*/
  instruction = ir_instruction(IR_GOTO_F);
  ir_operand_copy(instruction, 0, left);
  ir_operand_copy(instruction, 1, &gotoFinstr->operands[0]);
  node->ir = ir_append(node->ir, instruction);
  /*append ir of right expression*/
  node->ir = ir_add_section(node->ir, node->data.binary.right_operand->ir);
  node->ir = rres->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.right_operand)):node->ir;
  right = rres->ir_operand->islvalue == 1 ? &node->ir->last->operands[0] : rres->ir_operand;
  /*gotoIfFalse for right expression*/
  instruction = ir_instruction(IR_GOTO_F);
  ir_operand_copy(instruction, 0, right);
  ir_operand_copy(instruction, 1, &gotoFinstr->operands[0]);
  node->ir = ir_append(node->ir, instruction);  
  /* return 1 condition satisfies use the resinstr created above*/
  resinstr = ir_generate_for_gen_constant(1, NULL);
  node->ir = ir_append(node->ir, resinstr);
  /* goto end */
  instruction = ir_instruction(IR_GOTO);
  ir_operand_copy(instruction, 0, &endinstr->operands[0]);
  node->ir = ir_append(node->ir, instruction);  
  /*create false condition label*/  
  node->ir = ir_append(node->ir, gotoFinstr); 
  /* return 0 both condition fail and use the temporary created by resinstr*/
  instruction = ir_generate_for_gen_constant(0, &resinstr->operands[0]);
  node->ir = ir_append(node->ir, instruction);
  /*create end label*/
  node->ir = ir_append(node->ir, endinstr);
  node->data.binary.result.ir_operand = &resinstr->operands[0];
  node->data.binary.result.ir_operand->islvalue = 0;
}


static void ir_generate_for_logical_or(struct node* node){
  struct ir_operand *left = NULL; 
  struct ir_operand *right = NULL;
  struct ir_instruction *resinstr = NULL;
  struct ir_instruction *gotoTinstr = NULL; 
  struct ir_instruction *endinstr = NULL;
  struct ir_instruction *instruction = NULL;
  struct result *lres = NULL;
  struct result *rres = NULL;
  assert(NODE_VBAR_VBAR == node->kind);

  ir_generate_for_expression(node->data.binary.left_operand); 
  ir_generate_for_expression(node->data.binary.right_operand);
  lres = node_get_result(node->data.binary.left_operand); 
  rres = node_get_result(node->data.binary.right_operand);  
  resinstr =  ir_generate_for_gen_constant(1, NULL); 

  gotoTinstr = ir_instruction(IR_LABEL);
  ir_operand_label(gotoTinstr, 0, IR_LOOP_NONE);
  endinstr = ir_instruction(IR_LABEL);
  ir_operand_label(endinstr, 0, IR_LOOP_NONE);

  node->ir = ir_copy(node->data.binary.left_operand->ir);
  node->ir = lres->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.left_operand)):node->ir;
  left = lres->ir_operand->islvalue == 1 ? &node->ir->last->operands[0] : lres->ir_operand;
  /* goto if true instruction*/
  instruction = ir_instruction(IR_GOTO_T);
  ir_operand_copy(instruction, 0, left);
  ir_operand_copy(instruction, 1, &gotoTinstr->operands[0]);
  node->ir = ir_append(node->ir, instruction);
  
  node->ir = ir_add_section(node->ir, node->data.binary.right_operand->ir);
  node->ir = rres->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.right_operand)):node->ir;
  right = rres->ir_operand->islvalue == 1 ? &node->ir->last->operands[0] : rres->ir_operand;
  /* goto if true instruction*/
  instruction = ir_instruction(IR_GOTO_T);
  ir_operand_copy(instruction, 0, right);
  ir_operand_copy(instruction, 1, &gotoTinstr->operands[0]);
  node->ir =ir_append(node->ir, instruction);  
  /* return 0 condition fails and use the temporary created by resinstr*/
  instruction = ir_generate_for_gen_constant(0, &resinstr->operands[0]);
  node->ir =ir_append(node->ir, instruction);
  /*goto end*/
  instruction = ir_instruction(IR_GOTO);
  ir_operand_copy(instruction, 0, &endinstr->operands[0]);
  node->ir =ir_append(node->ir, instruction);
  /*create true condition label*/
  node->ir =ir_append(node->ir, gotoTinstr);
  /* return 1 condition satisfies use the resinstr created above*/  
  node->ir =ir_append(node->ir, resinstr);
  /*create end label*/
  node->ir = ir_append(node->ir, endinstr);
  
  node->data.binary.result.ir_operand = &resinstr->operands[0];
  node->data.binary.result.ir_operand->islvalue = 0;
}

static void ir_generate_for_pre_incr(struct node* node){  
  struct result *res = NULL;
  struct ir_instruction *instruction = NULL;
  struct ir_instruction *incrinstruction = NULL;
  struct ir_operand *operand = NULL; 
  int incr_size = 0;
  assert(NODE_PRE_INCR == node->kind || NODE_PRE_DECR == node->kind);

  ir_generate_for_expression(node->data.unary.child_operand); 
  res = node_get_result(node->data.unary.child_operand);
  incr_size = type_calc_size(res->type);
  incr_size = node->kind == NODE_PRE_INCR ? incr_size : -1*incr_size;

  /*add instructions of child operand first*/
  node->ir = ir_copy(node->data.unary.child_operand->ir);
  /*load instruction for lval to rval*/
  node->ir = res->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.unary.child_operand)) : node->ir;
  operand = res->ir_operand->islvalue == 1 ? &node->ir->last->operands[0] : res->ir_operand;
  /* constint instruction*/
  incrinstruction = ir_generate_for_gen_constant(incr_size, NULL);
  node->ir = ir_append(node->ir, incrinstruction);
  /* add instruction*/
  instruction = ir_instruction(IR_ADD);
  ir_operand_temporary(instruction, 0);
  ir_operand_copy(instruction, 1, operand);
  ir_operand_copy(instruction, 2, &incrinstruction->operands[0]);
  node->ir = ir_append(node->ir, instruction);
  node->data.unary.result.ir_operand = &instruction->operands[0];
  node->data.unary.result.ir_operand->islvalue = 0;
  /*store instruction*/
  instruction = ir_instruction_store(res->type);
  ir_operand_copy(instruction, 0, res->ir_operand);
  ir_operand_copy(instruction, 1, &node->ir->last->operands[0]);
  node->ir = ir_append(node->ir, instruction);   
}

static void ir_generate_for_post_incr(struct node* node){  
  struct result *res = NULL;
  struct ir_operand *operand = NULL;
   struct ir_instruction *increment= NULL;
  struct ir_instruction *instruction = NULL;
  int incr_size = 0;
  assert(NODE_POST_INCR == node->kind || node->kind == NODE_POST_DECR);

  ir_generate_for_expression(node->data.unary.child_operand); 
  res = node_get_result(node->data.unary.child_operand);

  incr_size = type_calc_size(res->type);
  incr_size = node->kind == NODE_POST_INCR ? incr_size : -1*incr_size;

  /*add instructions of child operand first*/
  node->ir = ir_copy(node->data.unary.child_operand->ir);
  /*load instruction for lval to rval*/
  node->ir = res->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.unary.child_operand)) : node->ir;
  operand = res->ir_operand->islvalue == 1 ? &node->ir->last->operands[0] : res->ir_operand;
  /* const int instruction*/
  increment = ir_generate_for_gen_constant(incr_size, NULL);
  node->ir = ir_append(node->ir, increment);
  /* add instruction*/
  instruction = ir_instruction(IR_ADD);
  ir_operand_temporary(instruction, 0);
  ir_operand_copy(instruction, 1, operand);
  ir_operand_copy(instruction, 2, &increment->operands[0]);
  node->ir = ir_append(node->ir, instruction);
  node->data.unary.result.ir_operand = operand;
  node->data.unary.result.ir_operand->islvalue = 0;
  /*store instruction*/
  instruction = ir_instruction_store(res->type);
  ir_operand_copy(instruction, 0, res->ir_operand);
  ir_operand_copy(instruction, 1, &node->ir->last->operands[0]);
  node->ir = ir_append(node->ir, instruction);  
}

static void ir_generate_for_unary_plus(struct node* node){
  struct ir_operand *child = NULL; 
  struct result *res = NULL;
  struct ir_operand *operand = NULL;
  struct ir_instruction *instruction = NULL;
  assert(node->kind == NODE_UNARY_PLUS|| node->kind == NODE_UNARY_MINUS);
  /*get child_operand*/
  ir_generate_for_expression(node->data.unary.child_operand); 
  res = node_get_result(node->data.unary.child_operand);
  child = res->ir_operand; 
  assert(child != NULL);
  /*get instructions of child operand*/
  node->ir = ir_copy(node->data.unary.child_operand->ir);
  node->ir = child->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.unary.child_operand)) : node->ir;
  operand = child->islvalue == 1 ? &node->ir->last->operands[0] : child;
  node->data.unary.result.ir_operand = operand;
  if (node->kind == NODE_UNARY_MINUS){
    instruction = ir_instruction(IR_UMINUS);
    ir_operand_temporary(instruction, 0);
    ir_operand_copy(instruction, 1, operand);
    node->ir = ir_append(node->ir, instruction);
    node->data.unary.result.ir_operand = &node->ir->last->operands[0];
  }
  node->data.unary.result.ir_operand->islvalue = 0;
}

static void ir_generate_for_shift_op(struct node* node){
  struct result *lres = NULL;
  struct result *rres = NULL;
  struct ir_instruction *shift = NULL;
  struct ir_operand *left = NULL;
  struct ir_operand *right = NULL;
  assert(node->kind == NODE_SHIFT_LEFT || node->kind == NODE_SHIFT_RIGHT);
  /*get left and right node operands*/
  ir_generate_for_expression(node->data.binary.left_operand);
  ir_generate_for_expression(node->data.binary.right_operand);  
  lres= node_get_result(node->data.binary.left_operand); 
  rres = node_get_result(node->data.binary.right_operand);
  assert(lres != NULL);  assert(rres != NULL);
  if (lres->type->kind == TYPE_BASIC && lres->type->data.basic.is_unsigned)
    shift = node->kind == NODE_SHIFT_LEFT ? ir_instruction(IR_LSHIFT) : ir_instruction(IR_UNSIGNED_RSHIFT);
  else
    shift = node->kind == NODE_SHIFT_LEFT ? ir_instruction(IR_LSHIFT) : ir_instruction(IR_RSHIFT);
  ir_operand_temporary(shift, 0);
  /*get instructions of right operand*/
  node->ir = ir_copy(node->data.binary.left_operand->ir);
  node->ir = lres->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.left_operand)) : node->ir;
  left = lres->ir_operand->islvalue == 1 ? &node->ir->last->operands[0]:lres->ir_operand;
  ir_operand_copy(shift, 1, left);

  node->ir = ir_concatenate(node->ir, node->data.binary.right_operand->ir);
  node->ir = rres->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.right_operand)) : node->ir;
  right = rres->ir_operand->islvalue == 1 ? &node->ir->last->operands[0] : rres->ir_operand;
  ir_operand_copy(shift, 2, right);
  ir_append(node->ir, shift);
  
  node->data.binary.result.ir_operand = &shift->operands[0];
  node->data.binary.result.ir_operand->islvalue = 0;
}

static void ir_generate_for_parent(struct node *node, struct node *left, struct node *right){
  struct ir_instruction *add = NULL; 
  struct node *numnode = NULL;
  struct result *lres = NULL;
  struct result *rres = NULL;
  struct ir_operand *operand1 = NULL;
  struct ir_operand *operand2 = NULL;
  struct ir_instruction *mult = NULL;
  struct ir_instruction *div = NULL;
  struct ir_instruction *constant = NULL;
  char num[16];
  
  lres = node_get_result(left); 
  rres = node_get_result(right);
  /*add right ir */
  node->ir = ir_add_section(node->ir, right->ir);
  node->ir = rres->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(right)): node->ir; 
  operand1 =  rres->ir_operand->islvalue == 1 ? &node->ir->last->operands[0] : rres->ir_operand;

  node->ir = ir_add_section(node->ir, left->ir);
  node->ir = lres->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(left)) : node->ir; 
  operand2 =  lres->ir_operand->islvalue == 1 ? &node->ir->last->operands[0] : lres->ir_operand;

  if (lres->type->kind == TYPE_POINTER && rres->type->kind == TYPE_BASIC){
    sprintf(num, "%d", type_calc_size(lres->type));
    /* create number node*/
    numnode = node_number(node->location, num);
    ir_generate_for_number(numnode);
    /*create mult instruction*/
    mult = ir_instruction(IR_SIGNED_MULT);
    ir_operand_temporary(mult, 0);
    ir_operand_copy(mult, 1, operand1);
    ir_operand_copy(mult, 2, &numnode->ir->last->operands[0]);
    /*add const instruction for pointer addition */
    node->ir = ir_add_section( node->ir, numnode->ir);
    /* add mult instruction*/
    node->ir = ir_append(node->ir, mult);
    operand1 = &mult->operands[0];
  }
  add = node->kind == NODE_PLUS ? ir_instruction(IR_ADD) : ir_instruction(IR_SIGNED_SUB);
  ir_operand_temporary(add, 0);
  ir_operand_copy(add, 1, operand2);  
  ir_operand_copy(add, 2, operand1);
  node->ir = ir_append(node->ir, add);

  node->data.binary.result.ir_operand = &add->operands[0];
  node->data.binary.result.ir_operand->islvalue = 0;
  if (lres->type->kind == TYPE_POINTER && rres->type->kind == TYPE_POINTER && node->kind == NODE_MINUS) {
    constant = ir_generate_for_gen_constant(type_calc_size(lres->type), NULL);
    node->ir = ir_append(node->ir, constant);
    div = ir_instruction(IR_SIGNED_DIV);
    ir_operand_temporary(div, 0);
    ir_operand_copy(div, 1, &add->operands[0]);
    ir_operand_copy(div, 2, &constant->operands[0]);
    node->ir = ir_append(node->ir, div);
    node->data.binary.result.ir_operand = &div->operands[0];
    node->data.binary.result.ir_operand->islvalue = 0;
  }  
}

static void ir_generate_for_add_op(struct node *node){
  
  struct result *lres = NULL;
  struct result *rres = NULL;
  struct type *left = NULL;
  struct type *right = NULL;
  
  /*get left and right node operands*/
  ir_generate_for_expression(node->data.binary.left_operand);
  ir_generate_for_expression(node->data.binary.right_operand);  
  lres = node_get_result(node->data.binary.left_operand); 
  rres = node_get_result(node->data.binary.right_operand);
  assert(lres != NULL);  assert(rres != NULL);
  
  left = lres->type;
  right = rres->type;
  if (left->kind == TYPE_POINTER && right->kind == TYPE_BASIC){
    ir_generate_for_parent(node, node->data.binary.left_operand, node->data.binary.right_operand);
  }
  else if (right->kind == TYPE_POINTER && left->kind == TYPE_BASIC){
    ir_generate_for_parent(node, node->data.binary.right_operand, node->data.binary.left_operand);
  }
  else {
    ir_generate_for_parent(node, node->data.binary.left_operand, node->data.binary.right_operand);
  } 
}

static void ir_generate_for_mult_op(struct node *node){
  struct ir_instruction *mult = NULL; 
  struct result *lres = NULL;
  struct result *rres = NULL;
  struct ir_operand *first = NULL;
  struct ir_operand *second = NULL;
  assert(node->kind == NODE_ASTERISK || node->kind == NODE_SLASH || node->kind == NODE_PERCENT);
  /*get left and right node operands*/
  ir_generate_for_expression(node->data.binary.left_operand);
  ir_generate_for_expression(node->data.binary.right_operand);  
  lres = node_get_result(node->data.binary.left_operand); 
  rres = node_get_result(node->data.binary.right_operand);
  assert(lres != NULL);  assert(rres != NULL);
  
  /*get instructions of both operands and concatenate*/
  node->ir = ir_copy(node->data.binary.left_operand->ir);
  if (!lres->type->data.basic.is_unsigned || !rres->type->data.basic.is_unsigned)
    mult = node->kind == NODE_ASTERISK ? ir_instruction(IR_SIGNED_MULT) : node->kind == NODE_SLASH ? ir_instruction(IR_SIGNED_DIV):ir_instruction(IR_SIGNED_REM);
  else
    mult = node->kind == NODE_ASTERISK ? ir_instruction(IR_UNSIGNED_MULT): node->kind == NODE_SLASH ? ir_instruction(IR_UNSIGNED_DIV) :ir_instruction(IR_UNSIGNED_REM);
  
  ir_operand_temporary(mult, 0);
  node->ir = lres->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.left_operand)) : node->ir;         
  first = lres->ir_operand->islvalue == 1 ? &node->ir->last->operands[0] : lres->ir_operand;
  ir_operand_copy(mult, 1, first);

  node->ir = ir_add_section(node->ir, node->data.binary.right_operand->ir);
  node->ir = rres->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.right_operand)): node->ir; 
  second = rres->ir_operand->islvalue == 1 ? &node->ir->last->operands[0] : rres->ir_operand;
  ir_operand_copy(mult, 2, second);

  ir_append(node->ir, mult);
  node->data.binary.result.ir_operand = &mult->operands[0];
  node->data.binary.result.ir_operand->islvalue = 0;
}

static void ir_generate_for_neg_op(struct node *node){
  struct ir_instruction *instruction = NULL;
  struct result *res = NULL;
  struct ir_operand *operand = NULL;
  ir_generate_for_expression(node->data.unary.child_operand);
  res = node_get_result(node->data.unary.child_operand);

  node->ir = ir_copy(node->data.unary.child_operand->ir);
  node->ir = res->ir_operand->islvalue == 1? ir_append(node->ir, ir_generate_for_load(node->data.unary.child_operand)):node->ir;
  operand = res->ir_operand->islvalue ==1 ? &node->ir->last->operands[0] : res->ir_operand;
  instruction = node->kind == NODE_TILDE ? ir_instruction(IR_BITOP_NEG) : ir_instruction(IR_NEGATION);
  ir_operand_temporary(instruction, 0);
  ir_operand_copy(instruction, 1, operand);
  node->ir = ir_append(node->ir, instruction);
  node->data.unary.result.ir_operand = &instruction->operands[0];
  node->data.unary.result.ir_operand->islvalue = 0;
}
static void ir_generate_for_bitwise_op(struct node *node){
  struct ir_instruction *instruction = NULL;
  struct result *lres = NULL;
  struct result *rres = NULL;
  struct ir_operand *first = NULL;
  struct ir_operand *second = NULL;
  switch(node->kind){
    case NODE_VBAR:
    case NODE_VBAR_EQUAL:
      instruction = ir_instruction(IR_BITOP_OR);
      break;
    case NODE_CARET:
    case NODE_CARET_EQUAL:
      instruction = ir_instruction(IR_BITOP_XOR);
      break;
    case NODE_AMPERSAND:
    case NODE_AMPERSAND_EQUAL:
      instruction = ir_instruction(IR_BITOP_AND);
      break;
    default:
      assert(0);
      break;
  }
  ir_operand_temporary(instruction, 0);
  /*get left and right node operands*/
  ir_generate_for_expression(node->data.binary.left_operand);
  ir_generate_for_expression(node->data.binary.right_operand);  
  lres = node_get_result(node->data.binary.left_operand); 
  rres = node_get_result(node->data.binary.right_operand);
  assert(lres != NULL);  assert(rres != NULL); 
   /*get instructions of both operands and concatenate*/
  node->ir = ir_copy(node->data.binary.left_operand->ir);
  node->ir = lres->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.left_operand)) : node->ir;
  first = lres->ir_operand->islvalue == 1 ?  &node->ir->last->operands[0] : lres->ir_operand;
  ir_operand_copy(instruction, 1, first);

  node->ir = ir_add_section(node->ir, node->data.binary.right_operand->ir);
  node->ir = rres->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.right_operand)) : node->ir;
  second = rres->ir_operand->islvalue == 1 ?  &node->ir->last->operands[0] : rres->ir_operand;
  ir_operand_copy(instruction, 2, second);
  ir_append(node->ir, instruction);
  node->data.binary.result.ir_operand = &instruction->operands[0];
  node->data.binary.result.ir_operand->islvalue = 0;
}

static void ir_generate_for_compound_assignment(struct node *node){
  struct node *tnode = NULL;
  struct result *res = NULL;
  struct node *lnode = NULL;
  struct ir_operand *left = NULL, *right = NULL;
  struct ir_instruction *instruction = NULL;

  switch(node->kind){
    case NODE_VBAR_EQUAL:
      tnode =node_two_operands(NODE_VBAR, "|",node->data.binary.left_operand, node->data.binary.right_operand, node->location);
      ir_generate_for_bitwise_op(tnode);
      break;
    case NODE_CARET_EQUAL:
      tnode =node_two_operands(NODE_CARET, "^",node->data.binary.left_operand, node->data.binary.right_operand, node->location);
      ir_generate_for_bitwise_op(tnode);
      break;
    case NODE_AMPERSAND_EQUAL:
      tnode =node_two_operands(NODE_AMPERSAND, "&",node->data.binary.left_operand, node->data.binary.right_operand, node->location);
      ir_generate_for_bitwise_op(tnode);
      break;
    case NODE_PLUS_EQUAL:
      tnode =node_two_operands(NODE_PLUS, "+",node->data.binary.left_operand, node->data.binary.right_operand, node->location);
      ir_generate_for_add_op(tnode);
      break;
    case NODE_MINUS_EQUAL:
      tnode =node_two_operands(NODE_MINUS, "-",node->data.binary.left_operand, node->data.binary.right_operand, node->location);
      ir_generate_for_add_op(tnode);
      break;
    case NODE_SLASH_EQUAL:
      tnode =node_two_operands(NODE_SLASH, "/",node->data.binary.left_operand, node->data.binary.right_operand, node->location);
      ir_generate_for_mult_op(tnode);
      break;
    case NODE_ASTERISK_EQUAL:
      tnode =node_two_operands(NODE_ASTERISK, "*",node->data.binary.left_operand, node->data.binary.right_operand, node->location);
      ir_generate_for_mult_op(tnode);
      break;
    case NODE_PERCENT_EQUAL:
      tnode =node_two_operands(NODE_PERCENT, "%",node->data.binary.left_operand, node->data.binary.right_operand, node->location);
      ir_generate_for_mult_op(tnode);
      break;
     case NODE_GREATER_GREATER_EQUAL:
      tnode =node_two_operands(NODE_PERCENT, ">>",node->data.binary.left_operand, node->data.binary.right_operand, node->location);
      ir_generate_for_shift_op(tnode);
      break;
     case NODE_LESS_LESS_EQUAL:
      tnode =node_two_operands(NODE_PERCENT, "<<",node->data.binary.left_operand, node->data.binary.right_operand, node->location);
      ir_generate_for_shift_op(tnode);
      break;
    default:
      assert(0);
      break;
  }
  node->ir = ir_copy(tnode->ir);
  /*ir_generate_for_expression( node->data.binary.left_operand);
  node->ir = ir_add_section(node->ir, node->data.binary.left_operand->ir);*/
  lnode = node->data.binary.left_operand;
  while (lnode && lnode->kind == NODE_TYPE_CAST){
    lnode = lnode->data.unary.child_operand;
  }
  assert (lnode != NULL);
  res = node_get_result(lnode);

  left = res->ir_operand;
  right = node_get_result(tnode)->ir_operand;
  instruction = ir_instruction_store(res->type);
  ir_operand_copy(instruction, 0, left);
  ir_operand_copy(instruction, 1, right);
  node->ir = ir_append(node->ir, instruction);
  node->data.binary.result.ir_operand = left;
  node->data.binary.result.ir_operand->islvalue = left->islvalue;

  free(tnode);
}

static void ir_generate_for_if_statement(struct node *node){
  struct result *lres = NULL;
  struct ir_operand *operand = NULL;
  struct ir_instruction *instruction = NULL;
  struct ir_instruction *lblinstr = NULL;

  ir_generate_for_expression(node->data.binary.left_operand);
  ir_generate_for_expression(node->data.binary.right_operand);
  
  lblinstr = ir_instruction(IR_LABEL);
  ir_operand_label(lblinstr, 0, IR_LOOP_NONE);
  lres = node_get_result(node->data.binary.left_operand);
  node->ir = ir_copy(node->data.binary.left_operand->ir);
  node->ir = lres->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.left_operand)) : node->ir;
  operand = lres->ir_operand->islvalue == 1 ?  &node->ir->last->operands[0] : lres->ir_operand;
  
  instruction = ir_instruction(IR_GOTO_F);
  instruction->reset_reg = 1;
  ir_operand_copy(instruction, 0, operand);
  ir_operand_copy(instruction, 1, &lblinstr->operands[0]);
  node->ir = ir_append(node->ir, instruction);
  node->ir = ir_add_section(node->ir, node->data.binary.right_operand->ir);
  /*instruction = ir_instruction(IR_GOTO);
  ir_operand_copy(instruction, 0, &lblinstr->operands[0]);
  node->ir = ir_append(node->ir, instruction);*/
  node->ir = ir_append(node->ir, lblinstr);
  node->data.binary.result.ir_operand = operand;
}

static void ir_generate_for_if_else_statement(struct node *node){
  struct result *lres = NULL;
  struct ir_operand *operand = NULL;
  struct ir_instruction *instruction = NULL;
  struct ir_instruction *lblinstr = NULL;
  struct ir_instruction *endlbl = NULL;

  ir_generate_for_expression(node->data.ternary.left_operand);
  ir_generate_for_expression(node->data.ternary.middle_operand);
  ir_generate_for_expression(node->data.ternary.right_operand);
  lblinstr = ir_instruction(IR_LABEL);
  ir_operand_label(lblinstr, 0, IR_LOOP_NONE);

  endlbl = ir_instruction(IR_LABEL);
  ir_operand_label(endlbl, 0, IR_LOOP_NONE);

  lres = node_get_result(node->data.binary.left_operand);
  node->ir = ir_copy(node->data.binary.left_operand->ir);
  node->ir = lres->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.left_operand)) : node->ir;
  operand = lres->ir_operand->islvalue == 1 ?  &node->ir->last->operands[0] : lres->ir_operand;
  instruction = ir_instruction(IR_GOTO_F);
  instruction->reset_reg = 1;
  ir_operand_copy(instruction, 0, operand);
  ir_operand_copy(instruction, 1, &lblinstr->operands[0]);
  node->ir = ir_append(node->ir, instruction);
  node->ir = ir_add_section(node->ir, node->data.ternary.middle_operand->ir);
  instruction = ir_instruction(IR_GOTO);
  instruction->reset_reg = 1;
  ir_operand_copy(instruction, 0, &endlbl->operands[0]);
  node->ir = ir_append(node->ir, instruction);

  node->ir = ir_append(node->ir, lblinstr);
  node->ir = ir_add_section(node->ir, node->data.ternary.right_operand->ir);  
  node->ir = ir_append(node->ir, endlbl);
  node->data.binary.result.ir_operand = operand;
}

static void ir_generate_for_while_statement(struct node *node){
  struct result *res = NULL;
  struct ir_operand *operand = NULL;
  struct ir_instruction *instruction = NULL;
  struct ir_instruction *begin, *end;
  
  /*add loop begin instruction*/
  begin = ir_instruction(IR_LABEL);
  ir_operand_label(begin, 0, IR_LOOP_INCR);
  node->ir = ir_section(begin, begin);
  /*construction end label instruction but do not add to section yet*/
  end = ir_instruction(IR_LABEL);
  ir_operand_label(end, 0, IR_LOOP_END);

  ir_generate_for_expression(node->data.binary.left_operand);
  ir_generate_for_expression(node->data.binary.right_operand); 
  /*append ir of conditional expression*/
  res = node_get_result(node->data.binary.left_operand);
  node->ir = ir_add_section(node->ir, node->data.binary.left_operand->ir);
  node->ir = res->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.left_operand)) : node->ir;
  operand = res->ir_operand->islvalue == 1 ?  &node->ir->last->operands[0] : res->ir_operand;
  /* goto false instruction*/
  instruction = ir_instruction(IR_GOTO_F);
  instruction->reset_reg = 1;
  ir_operand_copy(instruction, 0, operand);
  ir_operand_copy(instruction, 1, &end->operands[0]);
  node->ir = ir_append(node->ir, instruction);
  /*append ir code for statements inside while loop*/
  node->ir = ir_add_section(node->ir, node->data.binary.right_operand->ir);
  instruction = ir_instruction(IR_GOTO);
  instruction->reset_reg = 1;
  ir_operand_copy(instruction, 0, &begin->operands[0]);
  node->ir = ir_append(node->ir, instruction);
  node->ir = ir_append(node->ir, end);
  node->data.binary.result.ir_operand = operand;
}

static void ir_generate_for_for_expr(struct node *node){
  struct result *mres = NULL;
  struct ir_instruction *instruction = NULL;
  struct ir_instruction *lbl1, *lbl2, *lbl3, *start;

  ir_generate_for_expression(node->data.ternary.left_operand);
  if (node->data.ternary.left_operand->ir)
    node->data.ternary.left_operand->ir->last->reset_reg =1;
  ir_generate_for_expression(node->data.ternary.middle_operand);
  ir_generate_for_expression(node->data.ternary.right_operand);
  node->ir = ir_copy(node->data.ternary.left_operand->ir);
  /*create label for condition expr (middle operand)*/
  lbl1 = ir_instruction(IR_LABEL);
  ir_operand_label(lbl1, 0, IR_LOOP_NONE);
  node->ir = ir_append(node->ir, lbl1);
  node->ir = ir_add_section(node->ir, node->data.ternary.middle_operand->ir);
  mres = node_get_result(node->data.ternary.middle_operand);
  /*create label for increment expr (right operand)*/
  lbl2 = ir_instruction(IR_LABEL);
  ir_operand_label(lbl2, 0, IR_LOOP_INCR);
  /*create label for end of loop */
  lbl3 = ir_instruction(IR_LABEL);
  ir_operand_label(lbl3, 0, IR_LOOP_END);
  /*create start label for body of loop */
  start = ir_instruction(IR_LABEL);
  ir_operand_label(start, 0, IR_LOOP_NONE);

  instruction = ir_instruction(IR_GOTO_F);
  instruction->reset_reg = 1;
  ir_operand_copy(instruction, 0, mres->ir_operand);
  ir_operand_copy(instruction, 1, &lbl3->operands[0]);
  node->ir = ir_append(node->ir, instruction);

  instruction = ir_instruction(IR_GOTO);
  instruction->reset_reg = 1;
  ir_operand_copy(instruction, 0, &start->operands[0]);
  node->ir = ir_append(node->ir, instruction);

  /*append label for increment expr*/
  node->ir = ir_append(node->ir, lbl2);
  node->ir = ir_add_section(node->ir, node->data.ternary.right_operand->ir);
  /*add goto condition expr */
  instruction = ir_instruction(IR_GOTO);
  instruction->reset_reg = 1;
  ir_operand_copy(instruction, 0, &lbl1->operands[0]);
  node->ir = ir_append(node->ir, instruction);
  /*add start of loop label */
  node->ir = ir_append(node->ir, start);
  /* append the end label instruction*/
  node->ir = ir_append(node->ir, lbl3);
}

static void ir_generate_for_for_statement(struct node *node){
  struct ir_section *section = NULL;
  struct ir_instruction *instruction = NULL;
  struct ir_instruction *dummy = NULL;

  ir_generate_for_expression(node->data.binary.left_operand);
  ir_generate_for_expression(node->data.binary.right_operand);
  node->ir = ir_copy(node->data.binary.left_operand->ir);
  section = node->data.binary.right_operand->ir ? ir_copy(node->data.binary.right_operand->ir) :NULL;
  instruction = ir_instruction(IR_GOTO);
  instruction->reset_reg = 1;
  ir_operand_label(instruction, 0, IR_LOOP_CONTINUE);
  section = section == NULL ? ir_section(instruction, instruction) :ir_append(section, instruction);
  
  node->ir->last->prev->next = section->first;
  section->first->prev = node->ir->last->prev;
  section->last->next = node->ir->last;
  node->ir->last->prev = section->last;

  dummy = ir_instruction(IR_NO_OPERATION);
  ir_operand_label(dummy, 0, IR_LOOP_DESTROY);
  free(dummy);
}

static void ir_generate_for_dowhile_statement(struct node *node){
  struct result *res= NULL;
  struct ir_operand *operand = NULL;
  struct ir_instruction *instruction = NULL;
  struct ir_instruction *begin, *end;
  
  /*add loop begin instruction*/
  begin = ir_instruction(IR_LABEL);
  ir_operand_label(begin, 0, IR_LOOP_INCR);
  node->ir = ir_section(begin, begin);
  /*construction end label instruction but do not add to section yet*/
  end = ir_instruction(IR_LABEL);
  ir_operand_label(end, 0, IR_LOOP_END);

  ir_generate_for_expression(node->data.binary.left_operand);
  ir_generate_for_expression(node->data.binary.right_operand);
  node->ir = ir_add_section(node->ir, node->data.binary.left_operand->ir);
  node->ir = ir_add_section(node->ir, node->data.binary.right_operand->ir);
  /*append ir of conditional expression*/
  res = node_get_result(node->data.binary.right_operand);  
  node->ir = res->ir_operand->islvalue == 1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.right_operand)) : node->ir;
  operand = res->ir_operand->islvalue == 1 ?  &node->ir->last->operands[0] : res->ir_operand;
  /* goto true instruction*/
  instruction = ir_instruction(IR_GOTO_T);
  instruction->reset_reg = 1;
  ir_operand_copy(instruction, 0, operand);
  ir_operand_copy(instruction, 1, &begin->operands[0]);
  node->ir = ir_append(node->ir, instruction);
  /*append ir code for statements inside while loop*/

  node->ir = ir_append(node->ir, end);
  node->data.binary.result.ir_operand = operand;
}


static void ir_generate_for_break(struct node *node){
  struct ir_instruction *instruction = NULL;
  instruction = ir_instruction(IR_GOTO);
  instruction->reset_reg = 1;
  ir_operand_label(instruction, 0, IR_LOOP_BREAK);
  node->ir = ir_section(instruction, instruction);
}
static void ir_generate_for_continue(struct node *node){
  struct ir_instruction *instruction = NULL;
  instruction = ir_instruction(IR_GOTO);
  instruction->reset_reg = 1;
  ir_operand_label(instruction, 0, IR_LOOP_CONTINUE);
  node->ir = ir_section(instruction, instruction); 
}

static void ir_generate_for_relational_op(struct node *node){
  struct ir_instruction *instruction = NULL;
  struct result *lres = NULL, *rres = NULL;
  struct ir_operand *left = NULL, *right = NULL;
  bool isunsigned = false;
  ir_generate_for_expression(node->data.binary.left_operand);
  ir_generate_for_expression(node->data.binary.right_operand);
  lres = node_get_result(node->data.binary.left_operand);
  rres = node_get_result(node->data.binary.right_operand);
  left = lres->ir_operand;
  right = rres->ir_operand;
  node->ir = ir_concatenate(node->data.binary.left_operand->ir, node->data.binary.right_operand->ir);
  node->ir = left->islvalue==1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.left_operand)):node->ir;
  left = left->islvalue == 1 ? &node->ir->last->operands[0] : left;
  node->ir = right->islvalue == 1? ir_append(node->ir, ir_generate_for_load(node->data.binary.right_operand)) :node->ir;
  right = right->islvalue == 1? &node->ir->last->operands[0] : right;
  
  isunsigned = type_is_unsigned(lres->type) || type_is_unsigned(rres->type);
  switch(node->kind){  
    case NODE_LESS:      
      instruction = isunsigned ?ir_instruction(IR_UNSIGNED_LESS) : ir_instruction(IR_LESS);
      break;
    case NODE_GREATER:
      instruction = isunsigned ? ir_instruction(IR_UNSIGNED_GREATER) : ir_instruction(IR_GREATER);   
      break;
    case NODE_LESS_EQUAL:
      instruction = isunsigned ? ir_instruction(IR_UNSIGNED_LESS_EQUAL) : ir_instruction(IR_LESS_EQUAL);   
      break;
    case NODE_GREATER_EQUAL:
      instruction = isunsigned ? ir_instruction(IR_UNSIGNED_GREATER_EQUAL) : ir_instruction(IR_GREATER_EQUAL);    
      break;
    default :
      assert(0);
      break;
  }
  ir_operand_temporary(instruction, 0);
  ir_operand_copy(instruction, 1, left);
  ir_operand_copy(instruction, 2, right);
  node->ir=  ir_append(node->ir, instruction);
  node->data.binary.result.ir_operand = &instruction->operands[0];
  node->data.binary.result.ir_operand->islvalue = 0;
}

static void ir_generate_for_ternary_op(struct node *node){
  struct ir_instruction *label1 = NULL;
  struct ir_instruction *label2 = NULL;
  struct ir_instruction *instruction = NULL;
  struct ir_instruction *constinstr = NULL;
  struct result *lres = NULL, *mres = NULL, *rres= NULL;
  struct ir_operand *operand = NULL;

  ir_generate_for_expression(node->data.ternary.left_operand);
  ir_generate_for_expression(node->data.ternary.middle_operand);
  ir_generate_for_expression(node->data.ternary.right_operand);
  node->ir = ir_copy(node->data.ternary.left_operand->ir);
  lres = node_get_result(node->data.ternary.left_operand);
  node->ir = lres->ir_operand->islvalue ==1 ? ir_append(node->ir, ir_generate_for_load(node->data.ternary.left_operand)):node->ir;
  operand = lres->ir_operand->islvalue ==1 ? &node->ir->last->operands[0] : lres->ir_operand;

  label1 = ir_instruction(IR_LABEL);
  ir_operand_label(label1, 0, IR_LOOP_NONE);
  label2 = ir_instruction(IR_LABEL);
  ir_operand_label(label2, 0, IR_LOOP_NONE);

  instruction = ir_instruction(IR_GOTO_F);
  ir_operand_copy(instruction, 0, operand);
  ir_operand_copy(instruction, 1, &label1->operands[0]);
  node->ir = ir_append(node->ir, instruction);
  node->ir = ir_add_section(node->ir, node->data.ternary.middle_operand->ir);
  mres = node_get_result(node->data.ternary.middle_operand);
  node->ir = mres->ir_operand->islvalue ==1 ? ir_append(node->ir, ir_generate_for_load(node->data.ternary.middle_operand)):node->ir;
  operand = mres->ir_operand->islvalue ==1 ? &node->ir->last->operands[0] : mres->ir_operand;

  instruction = ir_instruction(IR_GOTO);
  ir_operand_copy(instruction, 0, &label2->operands[0]);
  node->ir = ir_append(node->ir, instruction);
  
  /* add label 2 and add right node ir */
  node->ir = ir_append(node->ir, label1);
  node->ir = ir_add_section(node->ir, node->data.ternary.right_operand->ir);
  rres = node_get_result(node->data.ternary.right_operand);
  if (rres->ir_operand->islvalue == 1){
    instruction = ir_instruction(IR_LOAD);
    ir_operand_copy(instruction, 0, operand);
    ir_operand_copy(instruction, 1, rres->ir_operand);
    node->ir = ir_append(node->ir, instruction);
  }
  else{
    constinstr = ir_generate_for_gen_constant(0, NULL);
    node->ir = ir_append(node->ir, constinstr);
    instruction = ir_instruction(IR_ADD);
    ir_operand_copy(instruction, 0, operand);
    ir_operand_copy(instruction, 1, rres->ir_operand);
    ir_operand_copy(instruction, 2, &constinstr->operands[0]);
    node->ir = ir_append(node->ir, instruction);
  }

  node->ir = ir_append(node->ir, label2);
  node->data.ternary.result.ir_operand = operand;
  node->data.ternary.result.ir_operand->islvalue= 0;
}

void ir_cast_instruction(struct node *node, struct ir_operand *operand, struct type *t1, struct type *t2){
   struct ir_instruction *instruction = NULL;
   /*
   t1 - source type ; t2 -  target type
   */
   assert (t1 != NULL); assert(t2 != NULL);

   if (type_is_arithmetic(t1) && type_is_arithmetic(t2)){
    if (t1->data.basic.datatype == TYPE_BASIC_CHAR && t2->data.basic.datatype == TYPE_BASIC_INT){
      if (t2->data.basic.is_unsigned  && !t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTUNSIGNEDWORD_BYTE);
      else if (!t2->data.basic.is_unsigned  && t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTWORD_UNSIGNEDBYTE);
      else if (t2->data.basic.is_unsigned  && t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTUNSIGNEDWORD_UNSIGNEDBYTE);
      else
        instruction = ir_instruction(IR_CASTWORD_BYTE);
    }
    else if (t1->data.basic.datatype == TYPE_BASIC_INT && t2->data.basic.datatype == TYPE_BASIC_CHAR){
      if (t2->data.basic.is_unsigned  && !t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTUNSIGNEDBYTE_WORD);
      else if (!t2->data.basic.is_unsigned  && t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTBYTE_UNSIGNEDWORD);
      else if (t2->data.basic.is_unsigned  && t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTUNSIGNEDBYTE_UNSIGNEDWORD);
      else
        instruction = ir_instruction(IR_CASTBYTE_WORD);
    }
    else if (t1->data.basic.datatype == TYPE_BASIC_INT && t2->data.basic.datatype == TYPE_BASIC_SHORT){
      if (t2->data.basic.is_unsigned  && !t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTUNSIGNEDHALFWD_WORD);
      else if (!t2->data.basic.is_unsigned  && t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTHALFWD_UNSIGNEDWORD);
      else if (t2->data.basic.is_unsigned  && t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTUNSIGNEDHALFWD_UNSIGNEDWORD);
      else
        instruction = ir_instruction(IR_CASTHALFWD_WORD);
    }
    else if (t1->data.basic.datatype == TYPE_BASIC_SHORT && t2->data.basic.datatype == TYPE_BASIC_CHAR){
      if (t2->data.basic.is_unsigned  && !t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTUNSIGNEDBYTE_HALFWD);
      else if (!t2->data.basic.is_unsigned  && t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTBYTE_UNSIGNEDHALFWD);
      else if (t2->data.basic.is_unsigned  && t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTUNSIGNEDBYTE_UNSIGNEDHALFWD);
      else
        instruction = ir_instruction(IR_CASTBYTE_HALFWD);
    }
    else if (t1->data.basic.datatype == TYPE_BASIC_CHAR && t2->data.basic.datatype == TYPE_BASIC_SHORT){
      if (t2->data.basic.is_unsigned  && !t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTUNSIGNEDHALFWD_BYTE);
      else if (!t2->data.basic.is_unsigned  && t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTHALFWD_UNSIGNEDBYTE);
      else if (t2->data.basic.is_unsigned  && t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTUNSIGNEDHALFWD_UNSIGNEDBYTE);
      else
        instruction = ir_instruction(IR_CASTHALFWD_BYTE);
    }

    else if (t1->data.basic.datatype == TYPE_BASIC_SHORT && t2->data.basic.datatype == TYPE_BASIC_INT){
      if (t2->data.basic.is_unsigned  && !t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTUNSIGNEDWORD_HALFWD);
      else if (!t2->data.basic.is_unsigned  && t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTWORD_UNSIGNEDHALFWD);
      else if (t2->data.basic.is_unsigned  && t1->data.basic.is_unsigned)
        instruction = ir_instruction(IR_CASTUNSIGNEDWORD_UNSIGNEDHALFWD);
      else
        instruction = ir_instruction(IR_CASTWORD_HALFWD);
    }
    else if (t1->data.basic.datatype == t2->data.basic.datatype){
      switch(t1->data.basic.datatype){
        case TYPE_BASIC_INT:
          if (t1->data.basic.is_unsigned && !t2->data.basic.is_unsigned)
            instruction = ir_instruction(IR_CASTWORD_UNSIGNEDWORD);
          else if (!t1->data.basic.is_unsigned && t2->data.basic.is_unsigned)
            instruction = ir_instruction(IR_CASTUNSIGNEDWORD_WORD);
          else return;
          break;
        case TYPE_BASIC_SHORT:
          if (t1->data.basic.is_unsigned && !t2->data.basic.is_unsigned)
            instruction = ir_instruction(IR_CASTHALFWD_UNSIGNEDHALFWD);
          else if (!t1->data.basic.is_unsigned && t2->data.basic.is_unsigned)
            instruction = ir_instruction(IR_CASTUNSIGNEDHALFWD_HALFWD);
          else return;
          break;
        case TYPE_BASIC_CHAR:
          if (t1->data.basic.is_unsigned && !t2->data.basic.is_unsigned)
            instruction = ir_instruction(IR_CASTBYTE_UNSIGNEDBYTE);
          else if (!t1->data.basic.is_unsigned && t2->data.basic.is_unsigned)
            instruction = ir_instruction(IR_CASTUNSIGNEDBYTE_BYTE);
          else return;
          break;
        default: return;
      }
    }

    ir_operand_temporary(instruction, 0);
    ir_operand_copy(instruction, 1, operand);
    node->ir = ir_append(node->ir, instruction);
    node->data.binary.result.ir_operand = &instruction->operands[0];
    node->data.binary.result.ir_operand->islvalue = 0;
  }
  if (t1->kind == TYPE_ARRAY && t2->kind == TYPE_POINTER){
    instruction = ir_instruction(IR_CASTARRAY_POINTER);
    ir_operand_temporary(instruction, 0);
    ir_operand_copy(instruction, 1, operand);
    node->ir = ir_append(node->ir, instruction);
    node->data.binary.result.ir_operand = &instruction->operands[0];
    node->data.binary.result.ir_operand->islvalue = 0;
  }
  if ( t2->kind == TYPE_VOID){
    instruction = ir_instruction(IR_CAST_VOID);
    ir_operand_temporary(instruction, 0);
    ir_operand_copy(instruction, 1, operand);
    node->ir = ir_append(node->ir, instruction);
    node->data.binary.result.ir_operand = &instruction->operands[0];
    node->data.binary.result.ir_operand->islvalue = 0;
  }

}

void ir_generate_for_cast_expr(struct node *node){
  struct type *t1 = NULL;
  struct type *t2 = NULL;
  struct result *lres = NULL;
  struct result *rres = NULL;
  struct ir_operand *operand = NULL;
  lres = node_get_result(node->data.binary.left_operand);
  rres = node_get_result(node->data.binary.right_operand);
  t1 = lres->type;
  t2 = rres->type;

  ir_generate_for_expression(node->data.binary.right_operand);
  node->ir = ir_copy(node->data.binary.right_operand->ir);
  if (t2->kind == TYPE_ARRAY && t1->kind == TYPE_POINTER){
    operand = rres->ir_operand;
    ir_cast_instruction(node, operand, t2, t1); 
  }
  else{
    node->ir = rres->ir_operand->islvalue == 1? ir_append(node->ir, ir_generate_for_load(node->data.binary.right_operand)):node->ir;
    operand = rres->ir_operand->islvalue == 1 ? &node->ir->last->operands[0]: rres->ir_operand;
    ir_cast_instruction(node, operand, t2, t1); 
  }  
}

void ir_generate_for_func_parameters(struct node *node, int *counter){
  struct ir_instruction* instruction = NULL;
  struct ir_operand *operand = NULL;
  struct ir_operand *roperand = NULL;
  if (node == NULL) return;
  assert(node->kind == NODE_EXPR_LIST);
  
  ir_generate_for_func_parameters(node->data.binary.left_operand, counter);
  ir_generate_for_expression(node->data.binary.right_operand);      
  if (node->data.binary.right_operand != NULL && node->data.binary.left_operand != NULL)
    node->ir = ir_add_section(node->data.binary.left_operand->ir, node->data.binary.right_operand->ir);
  else if (node->data.binary.right_operand != NULL)
    node->ir = ir_add_section(node->ir, node->data.binary.right_operand->ir); 
  
  roperand = node_get_result(node->data.binary.right_operand)->ir_operand;
  if (roperand->islvalue == 1){
    node->ir = ir_append(node->ir, ir_generate_for_load(node->data.binary.right_operand));
    roperand = &node->ir->last->operands[0];
  }
  /* add parameter instruction*/
  instruction = ir_instruction(IR_PARAM);    
  operand = malloc(sizeof(struct ir_operand));
  assert (operand != NULL);
  operand->kind = OPERAND_NUMBER;
  operand->data.number = *counter; /*nparam++;*/
  ir_operand_copy(instruction, 0, operand);
  ir_operand_copy(instruction, 1, roperand);
  node->ir = ir_append(node->ir, instruction);
  *counter = *counter + 1;
}

void ir_generate_for_expression(struct node* node){
  if (node == NULL) return;
  switch (node->kind) {
    case NODE_BINARY_OPERATION:
      break;
    case NODE_IDENTIFIER:
      ir_generate_for_identifier(node);
      break;      
    case NODE_NUMBER:
      ir_generate_for_number(node);
      break;
    case NODE_STRING:
      ir_generate_for_string(node);
      break;
    case NODE_EXPRESSION_STATEMENT:
      break; 
    case NODE_DECL:
    case NODE_TR_UNIT:
      ir_generate_for_expression(node->data.binary.left_operand);
      ir_generate_for_expression(node->data.binary.right_operand);
      node->ir = ir_add_section(node->ir, node->data.binary.left_operand->ir);
      node->ir = ir_add_section(node->ir, node->data.binary.right_operand->ir);
      break;
    case NODE_ARRAY_DECL:
    case NODE_POINTER_DECL: 
      ir_generate_for_expression(node->data.binary.right_operand);
      if (node->data.binary.right_operand != NULL) {
        node->ir = ir_copy(node->data.binary.right_operand->ir);
        node->data.binary.result.ir_operand = node_get_result(node->data.binary.right_operand)->ir_operand;
        node->data.binary.result.ir_operand->islvalue = node->data.binary.right_operand->ir->last->operands[0].islvalue;
      }      
      break;                       
    case NODE_DECL_LIST:
    case NODE_COMMA_EXPR:
      ir_generate_for_expression(node->data.binary.left_operand);        
      ir_generate_for_expression(node->data.binary.right_operand);      
      break;
    case NODE_FOR:        
      ir_generate_for_for_statement(node);
      break; 
    case NODE_WHILE:
      {
        struct ir_instruction *dummy = ir_instruction(IR_NO_OPERATION);
        ir_generate_for_while_statement(node);
        ir_operand_label( dummy ,0, IR_LOOP_DESTROY);
        free(dummy);
        break;
      }  
    case NODE_DOWHILE:
      {
        struct ir_instruction *dummy = ir_instruction(IR_NO_OPERATION);
        ir_generate_for_dowhile_statement(node);
        ir_operand_label( dummy ,0, IR_LOOP_DESTROY);
        free(dummy);
        break;
      }  
    case NODE_IF_ELSE:
      ir_generate_for_if_else_statement(node);
      break;
    case NODE_IF:
      ir_generate_for_if_statement(node);
      break;
    case NODE_FUNC_DEF:
      {
        struct ir_instruction *instruction = NULL;
        struct ir_instruction *endlabel = NULL;
        struct ir_instruction *dummy = NULL;
        dummy = ir_instruction(IR_NO_OPERATION);
        ir_operand_label(dummy, 0, IR_LOOP_PROCDESTROY);
        ir_generate_for_expression(node->data.binary.left_operand);
        /*set the table name as preceeding token for all generated labels in the function*/
        ir_running_table_name(&node->data.binary.left_operand->ir->last->operands[1], NULL, 1);        
        /*add proce begin instruction*/
        instruction = ir_instruction(IR_PROC_BEGIN);
        ir_operand_copy(instruction, 0, &node->data.binary.left_operand->ir->last->operands[1]);
        node->ir =ir_append(node->ir, instruction);
        endlabel = ir_instruction(IR_LABEL);
        ir_operand_label(endlabel, 0, IR_LOOP_PROCEND);
        /*add all the instructions of the function body*/
        ir_generate_for_expression(node->data.binary.right_operand);
        node->ir =ir_add_section(node->ir, node->data.binary.right_operand->ir);
        /* add proc end instruction*/
        node->ir = ir_append(node->ir, endlabel);
        instruction = ir_instruction(IR_PROC_END);
        ir_operand_copy(instruction, 0, &node->data.binary.left_operand->ir->last->operands[1]);
        node->ir = ir_append(node->ir, instruction);
        free(dummy);
      }
      break;    
    case NODE_FUNC_DEF_SPEC:
      ir_generate_for_expression(node->data.binary.left_operand);
      ir_generate_for_expression(node->data.binary.right_operand);
      node->ir = ir_copy(node->data.binary.right_operand->ir);
      break;
    case NODE_FUNC_DECL:
      {
        struct result *res = NULL;
        struct symbol *symbol = NULL;
        ir_generate_for_expression(node->data.binary.left_operand);
        node->ir = ir_copy(node->data.binary.left_operand->ir);
        res = node_get_result(node->data.binary.left_operand);
        node->data.binary.result.ir_operand = res->ir_operand;
        symbol = node->data.binary.left_operand->kind == NODE_IDENTIFIER ?node->data.binary.left_operand->data.identifier.symbol:NULL;
        if (symbol && strcmp(symbol->name, "main") == 0){
          _hasmain = 1;
        }        
      }
      break;
    case NODE_COMP_STATEMENT:
      ir_generate_for_expression(node->data.unary.child_operand);
      if (node->data.unary.child_operand != NULL)
      node->ir = ir_add_section(node->ir, node->data.unary.child_operand->ir);
      break;   
    case NODE_STATEMENT_LIST:
      ir_generate_for_expression(node->data.binary.left_operand);
      ir_generate_for_expression(node->data.binary.right_operand);
      
      /*if (node->data.binary.right_operand != NULL)node->data.binary.right_operand->ir->first->is_sequencept = 1;*/
      if (node->data.binary.right_operand != NULL && node->data.binary.left_operand != NULL)
        node->ir = ir_add_section(node->data.binary.left_operand->ir, node->data.binary.right_operand->ir);
      else if (node->data.binary.right_operand != NULL)
        node->ir = ir_add_section(node->ir, node->data.binary.right_operand->ir);    
      if (node->ir != NULL) node->ir->last->reset_reg = 1;  
      break;

    case NODE_FUNC_CALL:
      {       
        struct ir_instruction* instruction = NULL;
        struct ir_operand *operand = NULL;
        struct type * t = NULL;
        char *ptr = NULL;
        /*nparam = 0;*/       
        ir_generate_for_expression(node->data.binary.left_operand);
        ir_generate_for_expression(node->data.binary.right_operand);

        if (node->data.binary.right_operand != NULL){
          node->ir = ir_copy(node->data.binary.right_operand->ir);
        }
        operand = &node->data.binary.left_operand->ir->last->operands[1];
        ptr = strstr(operand->data.symbol->name, "syscall");
        if(ptr != NULL){
          instruction = ir_instruction(IR_SYSCALL);
          ir_operand_copy(instruction, 0, operand); 
        }
        else{
          instruction = ir_instruction(IR_CALL);
          ir_operand_copy(instruction, 0, operand); 
        }        
        node->ir = ir_append(node->ir, instruction);
        t =operand->data.symbol->result.type; 
        if (t->kind == TYPE_FUNCTION && t->data.functype.rvaltype->kind != TYPE_VOID){
          instruction = ir_instruction(IR_RESWD);
          ir_operand_temporary(instruction, 0); 
          node->ir = ir_append(node->ir, instruction);
          node->data.binary.result.ir_operand = &instruction->operands[0];
          node->data.binary.result.ir_operand->islvalue = 0;
        }
        
      }
      break;
    case NODE_EXPR_LIST:
      {
        int counter = 0;
        ir_generate_for_func_parameters(node, &counter);
        /*
        struct ir_instruction* instruction = NULL;
        struct ir_operand *operand = NULL;
        struct ir_operand *roperand = NULL;
        ir_generate_for_expression(node->data.binary.left_operand);
        ir_generate_for_expression(node->data.binary.right_operand);      
        if (node->data.binary.right_operand != NULL && node->data.binary.left_operand != NULL)
          node->ir = ir_add_section(node->data.binary.left_operand->ir, node->data.binary.right_operand->ir);
        else if (node->data.binary.right_operand != NULL)
          node->ir = ir_add_section(node->ir, node->data.binary.right_operand->ir); 
        
        roperand = node_get_result(node->data.binary.right_operand)->ir_operand;
        if (roperand->islvalue == 1){
          node->ir = ir_append(node->ir, ir_generate_for_load(node->data.binary.right_operand));
          roperand = &node->ir->last->operands[0];
        }
        */
        /* add parameter instruction*/
        /*
        instruction = ir_instruction(IR_PARAM);    
        operand = malloc(sizeof(struct ir_operand));
        assert (operand != NULL);
        operand->kind = OPERAND_NUMBER;
        operand->data.number = nparam++;
        ir_operand_copy(instruction, 0, operand);
        ir_operand_copy(instruction, 1, roperand);
        node->ir = ir_append(node->ir, instruction);
        */
      }
      break;  
    case NODE_FOR_EXPR:
      ir_generate_for_for_expr(node);
      break;
    case NODE_ABSTR_DECL:
      break;    
     case NODE_RETURN:
      {
        struct ir_instruction *instruction = NULL;
        struct ir_operand *operand = NULL;
        ir_generate_for_expression(node->data.unary.child_operand);
        
        if (node->data.unary.child_operand != NULL){
          operand = node_get_result(node->data.unary.child_operand)->ir_operand;
          node->ir = ir_copy(node->data.unary.child_operand->ir);
          node->ir = operand->islvalue == 1? ir_append(node->ir, ir_generate_for_load(node->data.unary.child_operand)):node->ir;
          operand = operand->islvalue == 1? &node->ir->last->operands[0] : operand;
          instruction = ir_instruction(IR_RETWD);
          ir_operand_copy(instruction, 0, operand);
          node->ir = ir_append(node->ir, instruction);
          node->data.unary.result.ir_operand = &instruction->operands[0];
          node->data.unary.result.ir_operand->islvalue = 0;
        }
        instruction = ir_instruction(IR_GOTO);
        instruction->reset_reg = 1;
        ir_operand_label(instruction, 0, IR_LOOP_RETURN);
        node->ir = ir_append(node->ir, instruction);
      }
      break;
    case NODE_GOTO:
      {
        struct ir_instruction *instruction = NULL;
        ir_generate_for_expression(node->data.unary.child_operand);
        instruction = ir_instruction(IR_GOTO);
        instruction->reset_reg = 1;
        ir_operand_gen_user_label(instruction, 0, node->data.unary.child_operand);
        /*ir_operand_identifier(instruction, 0, node->data.unary.child_operand);*/
        node->ir = ir_section(instruction, instruction);
      }
      break;
    case NODE_BREAK:
      ir_generate_for_break(node);
      break;
    case NODE_CONTINUE:
      ir_generate_for_continue(node);
      break;
    case NODE_LABEL:
      {
        struct ir_instruction *instruction = NULL;
        ir_generate_for_expression(node->data.binary.left_operand);
        ir_generate_for_expression(node->data.binary.right_operand);        
        instruction = ir_instruction(IR_LABEL);
        ir_operand_gen_user_label(instruction, 0, node->data.binary.left_operand);
        /*ir_operand_identifier(instruction, 0, node->data.binary.left_operand);*/
        node->ir = ir_section(instruction, instruction);
        node->ir = ir_add_section(node->ir, node->data.binary.right_operand->ir);
      }
      break;         
    /* */    
  /*binary expression*/
    case NODE_VBAR:
    case NODE_CARET:
    case NODE_AMPERSAND:
      ir_generate_for_bitwise_op(node);
      break;
    case NODE_PLUS:
    case NODE_MINUS:
      ir_generate_for_add_op(node);
      break;            
    case NODE_SLASH:
    case NODE_PERCENT:
    case NODE_ASTERISK:
      ir_generate_for_mult_op(node);
      break;
    case NODE_LESS:
    case NODE_GREATER:
    case NODE_LESS_EQUAL:
    case NODE_GREATER_EQUAL:
      ir_generate_for_relational_op(node);
      break;
    case NODE_VBAR_VBAR:
      ir_generate_for_logical_or(node);
      break;
    case NODE_AMPERSAND_AMPERSAND:
      ir_generate_for_logical_and(node);
      break;
    case NODE_SHIFT_LEFT:
    case NODE_SHIFT_RIGHT:
      ir_generate_for_shift_op(node);
      break;      
    case NODE_NOT_EQUAL:
    case NODE_EQUAL_EQUAL:
      {
        struct ir_instruction *instruction = NULL;
        struct ir_operand *left = NULL, *right = NULL;
        ir_generate_for_expression(node->data.binary.left_operand);
        ir_generate_for_expression(node->data.binary.right_operand);
        left = node_get_result(node->data.binary.left_operand)->ir_operand;
        right = node_get_result(node->data.binary.right_operand)->ir_operand;
        node->ir = ir_concatenate(node->data.binary.left_operand->ir, node->data.binary.right_operand->ir);
        node->ir = left->islvalue==1 ? ir_append(node->ir, ir_generate_for_load(node->data.binary.left_operand)):node->ir;
        left = left->islvalue == 1 ? &node->ir->last->operands[0] : left;
        node->ir = right->islvalue == 1? ir_append(node->ir, ir_generate_for_load(node->data.binary.right_operand)) :node->ir;
        right = right->islvalue == 1? &node->ir->last->operands[0] : right;
        instruction = node->kind == NODE_EQUAL_EQUAL ? ir_instruction(IR_EQUAL) : ir_instruction(IR_NOT_EQUAL);
        ir_operand_temporary(instruction, 0);
        ir_operand_copy(instruction, 1, left);
        ir_operand_copy(instruction, 2, right);
        node->ir=  ir_append(node->ir, instruction);
        node->data.binary.result.ir_operand = &instruction->operands[0];
        node->data.binary.result.ir_operand->islvalue = 0;
      }
      break;
    case NODE_EQUAL:
      ir_generate_for_simple_assignment(node);
      break;
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
      ir_generate_for_compound_assignment(node);
      break;
    case NODE_CAST:
      ir_generate_for_cast_expr(node);
      break;
    case NODE_TYPE_CAST:
      {
        struct node *child = node->data.unary.child_operand;
        struct result *rres = NULL;
        struct result *lres = NULL;
        struct ir_operand *operand = NULL;
        ir_generate_for_expression(child);
        node->ir = ir_copy(child->ir);
        rres = node_get_result(child);
        lres = node_get_result(node);
        if (rres->type->kind == TYPE_ARRAY && lres->type->kind == TYPE_POINTER){
           operand = rres->ir_operand;
           ir_cast_instruction(node, operand, rres->type, lres->type); 
        }
        else{
          node->ir = rres->ir_operand->islvalue==1 ? ir_append(node->ir, ir_generate_for_load(child)) : node->ir;
          operand = rres->ir_operand->islvalue==1 ? &node->ir->last->operands[0]: rres->ir_operand;
          ir_cast_instruction(node, operand, rres->type, lres->type);
        }
        
        /*node->data.unary.result.ir_operand = res->ir_operand;
        node->data.unary.result.ir_operand->islvalue = res->ir_operand->islvalue;   */   
        /*node->ir = res->islvalue == 1 && res->type->kind != TYPE_ARRAY ? ir_append(node->ir,ir_generate_for_load(child)): node->ir;
        node->data.unary.result.ir_operand = res->islvalue == 1 && res->type->kind != TYPE_ARRAY ? &node->ir->last->operands[0] : res->ir_operand;*/
      }      
      break;
    /* */    
    /*unary expression*/     
    case NODE_TILDE:
      ir_generate_for_neg_op(node);
      break;
    case NODE_UNARY_PLUS:        
    case NODE_UNARY_MINUS:
      ir_generate_for_unary_plus(node);
      break;
    case NODE_POST_INCR:
    case NODE_POST_DECR:
      ir_generate_for_post_incr(node);
      break;
    case NODE_PRE_INCR:
    case NODE_PRE_DECR:
      ir_generate_for_pre_incr(node);
      break;
    case NODE_INDIRECT:
      ir_generate_for_indirectop(node);    
      break;
    case NODE_ADDRESS_OP:
      ir_generate_for_addressop(node);
      break;
    case NODE_EXCLAMATION:
      ir_generate_for_neg_op(node);
      break;
    /* */    
  /*ternary expression*/
    case NODE_TERNARY_OP:
      ir_generate_for_ternary_op(node);
      break;     
    default:
      break;
  }  
}

/*static void ir_generate_for_expr(struct node *expression);

static void ir_generate_for_arithmetic_binary_operation(enum ir_instruction_kind kind, struct node *binary_operation) {
  struct ir_instruction *instruction;
  assert(NODE_BINARY_OPERATION == binary_operation->kind);

  ir_generate_for_expression(binary_operation->data.binary_operation.left_operand);
  ir_generate_for_expression(binary_operation->data.binary_operation.right_operand);

  instruction = ir_instruction(kind);
  ir_operand_temporary(instruction, 0);
  ir_operand_copy(instruction, 1, node_get_result(binary_operation->data.binary_operation.left_operand)->ir_operand);
  ir_operand_copy(instruction, 2, node_get_result(binary_operation->data.binary_operation.right_operand)->ir_operand);

  binary_operation->ir = ir_concatenate(binary_operation->data.binary_operation.left_operand->ir,
                                        binary_operation->data.binary_operation.right_operand->ir);
  ir_append(binary_operation->ir, instruction);
  binary_operation->data.binary_operation.result.ir_operand = &instruction->operands[0];
}

static void ir_generate_for_binary_operation(struct node *binary_operation) {
  assert(NODE_BINARY_OPERATION == binary_operation->kind);

  switch (binary_operation->data.binary_operation.operation) {
    case BINOP_MULTIPLICATION:
      ir_generate_for_arithmetic_binary_operation(IR_MULTIPLY, binary_operation);
      break;

    case BINOP_DIVISION:
      ir_generate_for_arithmetic_binary_operation(IR_DIVIDE, binary_operation);
      break;

    case BINOP_ADDITION:
      ir_generate_for_arithmetic_binary_operation(IR_ADD, binary_operation);
      break;

    case BINOP_SUBTRACTION:
      ir_generate_for_arithmetic_binary_operation(IR_SUBTRACT, binary_operation);
      break;

    case BINOP_ASSIGN:
      ir_generate_for_simple_assignment(binary_operation);
      break;

    default:
      assert(0);
      break;
  }
}


static void ir_generate_for_expression_statement(struct node *expression_statement) {
  struct ir_instruction *instruction;
  struct node *expression = expression_statement->data.expression_statement.expression;
  assert(NODE_EXPRESSION_STATEMENT == expression_statement->kind);
  ir_generate_for_expression(expression);

  instruction = ir_instruction(IR_PRINT_NUMBER);
  ir_operand_copy(instruction, 0, node_get_result(expression)->ir_operand);

  expression_statement->ir = ir_copy(expression_statement->data.expression_statement.expression->ir);
  ir_append(expression_statement->ir, instruction);
}

int ir_generate_for_statement_list(struct node *statement_list) {
  struct node *init = statement_list->data.statement_list.init;
  struct node *statement = statement_list->data.statement_list.statement;

  assert(NODE_STATEMENT_LIST == statement_list->kind);

  if (NULL != init) {
    ir_generate_for_statement_list(init);
    ir_generate_for_expression_statement(statement);
    statement_list->ir = ir_concatenate(init->ir, statement->ir);
  } else {
    ir_generate_for_expression_statement(statement);
    statement_list->ir = statement->ir;
  }
  return 0;
}
*/
/***********************
 * PRINT IR STRUCTURES *
 ***********************/

static void ir_print_opcode(FILE *output, enum ir_instruction_kind kind) {
  static char const * const instruction_names[] = {  
    "addressOf",
    "procBegin",
    "procEnd",
    "loadWord",
    "loadHalfWord",
    "loadUnsignedHalfWord",
    "loadByte",
    "loadUnsignedByte",
    "storeWord",
    "storeHalfWord",
    "storeByte",
    "constInt",
    "addWord",
    "subSignedWord",
    "subUnsignedWord",
    "multSignedWord",
    "multUnsignedWord",
    "divSignedWord",
    "divUnsignedWord",
    "remSignedWord",
    "remUnsignedWord",
    "leftShiftWord",
    "rightShiftSignedWord",
    "rightShiftUnsignedWord",
    "label",
    "goto",
    "gotoIfFalse",
    "gotoIfTrue",
    "parameter",
    "call",
    "syscall",
    "eqWord",
    "neWord",
    "ltSignedWord",
    "ltUnsignedWord",
    "leSignedWord",
    "leUnsignedWord",
    "gtSignedWord",
    "gtUnsignedWord",
    "geSignedWord",
    "geUnsignedWord",
    "bitwiseAndWord",
    "bitwiseOrWord",
    "bitwiseXorWord",
    "unaryBitwiseNegation",
    "unaryLogicalNegation",
    "resultWord",
    "returnWord",
    "unaryMinus",

    "castVoid",
    "castArrayToPointer",
    "castUnsignedWordToByte",
    "castWordToUnsignedByte",
    "castUnsignedWordToUnsignedByte",
    "castWordToByte",
    "castUnsignedByteToWord",
    "castByteToUnsignedWord",
    "castUnsignedByteToUnsignedWord",
    "castByteToWord",
    "castUnsignedHalfWordToWord",
    "castHalfWordToUnsignedWord",
    "castUnsignedHalfWordToUnsignedWord",
    "castHalfWordToWord",
    "castUnsignedByteToHalfWord",
    "castByteToUnsignedHaldWord",
    "castUnsignedByteToUnsignedHalfWord",
    "castByteToHalfWord",
    "castUnsignedWordToHalfWord",
    "castWordToUnsignedHalfWord",
    "castUnsignedWordToUnsignedHalfWord",
    "castWordToHalfWord",
    "castUnsignedHalfWordToByte",
    "castHalfWordToUnsignedByte",
    "castUnsignedHalfWordToUnsignedByte",
    "castHalfWordToByte",
    "castWordToUnsignedWord",
    "castUnsignedWordToWord",
    "castHalfWordToUnsignedHalfWord",
    "castUnsignedHalfWordToHalfWord",
    "castByteToUnsignedByte",
    "castUnsignedByteToByte",
    
    "loadWordoptz",
    "loadHalfWordoptz",
    "loadUnsignedHalfWordoptz",
    "loadByteoptz",
    "loadUnsignedByteoptz",

    "storeWordoptz",
    "storeHalfWordoptz",
    "storeByteoptz",

    "addWordImm",
    "subSignedWordImm",
    "subUnsignedWordImm",
    "multSignedWordImm",
    "multUnsignedWordImm",
    "divSignedWordImm",
    "divUnsignedWordImm",
    "remSignedWordImm",
    "remUnsignedWordImm",

    "NOP",
    "MULT",
    "DIV",
    "ADD",
    "SUB",
    "LI",
    "COPY",
    "PNUM",
    NULL
  };

  fprintf(output, "%-35s", instruction_names[kind]);
}

static void ir_print_operand(FILE *output, struct ir_operand *operand) {
  switch (operand->kind) {
    case OPERAND_NUMBER:
      fprintf(output, "%10hu", (unsigned short)operand->data.number);
      break;
    case OPERAND_IDENTIFIER:
      fprintf(output, "%10s", operand->data.symbol->name);
      break;
    case OPERAND_LABEL:
      fprintf(output, "%10s", operand->data.label);
      break;
    case OPERAND_TEMPORARY:
      fprintf(output, "     t%04d", operand->data.temporary);
      break;
  }
}
static void ir_print_instruction(FILE *output, struct ir_instruction *instruction, int *noperands) {
  ir_print_opcode(output, instruction->kind);
  fprintf(output, ", ");
  switch (instruction->kind) {
    case IR_ADD:
    case IR_LESS:
    case IR_EQUAL:
    case IR_LSHIFT:
    case IR_RSHIFT:
    case IR_GREATER:
    case IR_BITOP_OR:
    case IR_BITOP_AND:
    case IR_BITOP_XOR:
    
    case IR_NOT_EQUAL:
    case IR_LESS_EQUAL:
    case IR_SIGNED_SUB:
    case IR_SIGNED_DIV:
    case IR_SIGNED_REM:
    case IR_SIGNED_MULT:
    case IR_UNSIGNED_REM:
    case IR_UNSIGNED_DIV:
    case IR_UNSIGNED_MULT:
    case IR_UNSIGNED_LESS:
    case IR_GREATER_EQUAL:
    case IR_UNSIGNED_RSHIFT:    
    case IR_UNSIGNED_GREATER:
    case IR_UNSIGNED_LESS_EQUAL:
    case IR_UNSIGNED_GREATER_EQUAL:
      ir_print_operand(output, &instruction->operands[0]);
      fprintf(output, ", ");
      ir_print_operand(output, &instruction->operands[1]);
      fprintf(output, ", ");
      ir_print_operand(output, &instruction->operands[2]);
      *noperands = 3;
      break;
    case IR_ADD_IMM:
    case IR_SIGNED_SUB_IMM:
    case IR_UNSIGNED_SUB_IMM:
    case IR_SIGNED_MULT_IMM:
    case IR_UNSIGNED_MULT_IMM:
    case IR_SIGNED_DIV_IMM:
    case IR_UNSIGNED_DIV_IMM:
    case IR_SIGNED_REM_IMM:
    case IR_UNSIGNED_REM_IMM:
      ir_print_operand(output, &instruction->operands[0]);
      fprintf(output, ", ");
      ir_print_operand(output, &instruction->operands[1]);
      fprintf(output, ", ");
      ir_print_operand(output, &instruction->operands[2]);
      *noperands = 3;
      break;
    case IR_LOAD_IMMEDIATE:
    case IR_COPY:
      ir_print_operand(output, &instruction->operands[0]);
      fprintf(output, ", ");
      ir_print_operand(output, &instruction->operands[1]);
      *noperands = 2;
      break;
    case IR_PRINT_NUMBER:
      ir_print_operand(output, &instruction->operands[0]);
      *noperands = 1;
      break;
    case IR_NO_OPERATION:
      break;
    case IR_GOTO_T:
    case IR_GOTO_F:
      ir_print_operand(output, &instruction->operands[0]);
      fprintf(output, ", ");
      ir_print_operand(output, &instruction->operands[1]);
      *noperands = 3;
      break;
      break;
    case IR_LOAD:
    case IR_PARAM:
    case IR_CONST:    
    case IR_STORE:     
    case IR_UMINUS:   
    case IR_ADDR_OF:
    case IR_LOAD_BYTE:
    case IR_STORE_BYTE:
    case IR_BITOP_NEG:
    case IR_NEGATION:
    case IR_CAST_VOID:
    case IR_LOAD_HALF_WD:
    case IR_STORE_HALF_WD:
    case IR_LOAD_UNSIGNEDHALFWD:
    case IR_LOAD_UNSIGNEDBYTE:
    
    case IR_LOAD_OPTZ:
    case IR_STORE_OPTZ:
    case IR_LOAD_BYTE_OPTZ:
    case IR_STORE_BYTE_OPTZ:   
    case IR_LOAD_HALF_WD_OPTZ:
    case IR_STORE_HALF_WD_OPTZ:
    case IR_LOAD_UNSIGNEDHALFWD_OPTZ:
    case IR_LOAD_UNSIGNEDBYTE_OPTZ:

    case IR_CASTARRAY_POINTER:

    case IR_CASTUNSIGNEDWORD_BYTE:
    case IR_CASTWORD_UNSIGNEDBYTE:
    case IR_CASTUNSIGNEDWORD_UNSIGNEDBYTE:
    case IR_CASTWORD_BYTE:

    case IR_CASTUNSIGNEDBYTE_WORD:
    case IR_CASTBYTE_UNSIGNEDWORD:
    case IR_CASTUNSIGNEDBYTE_UNSIGNEDWORD:
    case IR_CASTBYTE_WORD:

    case IR_CASTUNSIGNEDHALFWD_WORD:
    case IR_CASTHALFWD_UNSIGNEDWORD:
    case IR_CASTUNSIGNEDHALFWD_UNSIGNEDWORD:
    case IR_CASTHALFWD_WORD:

    case IR_CASTUNSIGNEDBYTE_HALFWD:
    case IR_CASTBYTE_UNSIGNEDHALFWD:
    case IR_CASTUNSIGNEDBYTE_UNSIGNEDHALFWD:
    case IR_CASTBYTE_HALFWD:

    case IR_CASTUNSIGNEDWORD_HALFWD:
    case IR_CASTWORD_UNSIGNEDHALFWD:
    case IR_CASTUNSIGNEDWORD_UNSIGNEDHALFWD:
    case IR_CASTWORD_HALFWD:
    case IR_CASTUNSIGNEDHALFWD_BYTE:
    case IR_CASTHALFWD_UNSIGNEDBYTE:
    case IR_CASTUNSIGNEDHALFWD_UNSIGNEDBYTE:
    case IR_CASTHALFWD_BYTE:
    case IR_CASTWORD_UNSIGNEDWORD:
    case IR_CASTUNSIGNEDWORD_WORD:
    case IR_CASTHALFWD_UNSIGNEDHALFWD:
    case IR_CASTUNSIGNEDHALFWD_HALFWD:
    case IR_CASTBYTE_UNSIGNEDBYTE:
    case IR_CASTUNSIGNEDBYTE_BYTE:
      ir_print_operand(output, &instruction->operands[0]);
      fprintf(output, ", ");
      ir_print_operand(output, &instruction->operands[1]);
      *noperands = 2;
      break;
    
    case IR_GOTO:
    case IR_LABEL:
      ir_print_operand(output, &instruction->operands[0]);
      *noperands = 2;
      break;
    case IR_CALL:
    case IR_RESWD:
    case IR_RETWD:
    case IR_SYSCALL:
    case IR_PROC_END:
    case IR_PROC_BEGIN:
      ir_print_operand(output, &instruction->operands[0]);
      *noperands = 1;
      break;

    default:
      assert(0);
      break;
  }
}


void ir_print_livelist(FILE *output, struct ir_instruction *instruction){
  struct operand_list *list = instruction->livelist;
  fprintf(output,"%s", "{");
  while(list){
    ir_print_operand(output, list->operand);
    fprintf(output, ", ");
    list = list->next;
  }
  fprintf(output,"}");
}

void ir_print_register_allocation(FILE *output, struct ir_block *block){
  int i, j;
  fprintf(output, "REGISTER ALLOCATION FOR BLOCK: %s\n", block->name);
  for(i = 0; i< 16; i++){
    if (block->rmap[i].nregister == -1) break;
    fprintf(output,"%d -->",  block->rmap[i].nregister);
    for (j = 0 ; j < block->rmap[i].arr_items; j++){
      fprintf(output,"t%04d,", block->rmap[i].arr_temp[j]);
    }
    fprintf(output, "\n");
  }
}

void ir_print_vertex_edges(FILE *output, struct ir_block *block){
  int i, j;
  fprintf(output, "INTERFERENCE GRAPH FOR BLOCK: %s\n", block->name);
  for(i = 0; i< block->nvertices; i++){
    fprintf(output, "%10s","/*");
    ir_print_operand(output, block->vertices[i].vertex);
    fprintf(output,"%s", "-->{");
    for( j = 0 ; j < block->vertices[i].nedges ; j++){
      fprintf(output, "t%04d", block->vertices[i].edges[j].operand->data.temporary);
      /*ir_print_operand(output, block->vertices[i].edges[j].operand);*/
      fprintf(output, ", ");
    }
    fprintf(output,"}");
    fprintf(output, "*/");
    fprintf(output, "\n");
  }
  ir_print_register_allocation(output, block);
}

void ir_print_block(FILE *output, struct ir_block *block){
  struct ir_link *link = NULL;
  
  if (block == NULL) return;
  fprintf(output, "%s", block->name);
  fprintf(output, "*/");

  link = block->from_links;
  if (link == NULL) return;
      
  fprintf(output, "/*(From:");
  while(link){
    fprintf(output, "%s,",link->block->name);
    link = link->next;
  }
  fprintf(output, ")*/"); 
}

void ir_print_section(FILE *output, struct ir_section *section) {
  struct ir_instruction *iter = NULL;
  struct ir_instruction *prev = NULL;
  int i = 0 ,noperands = 0;
  int ncolwidth = 0;
  if (section == NULL) return;
  iter = section->first;
  
  while (NULL != iter && section->last != prev) {
    fprintf(output, "%5d     ", i++);
    ir_print_instruction(output, iter, &noperands);
    
    ncolwidth = (4-noperands)*10 - noperands + 1;
    if (iter->block){
      fprintf(output, "%*s", ncolwidth,"/*");
      ir_print_block(output, iter->block);  
    }

    if (iter->livelist != NULL) {
      fprintf(output, "%*s", ncolwidth,"/*");
      ir_print_livelist(output, iter);
      fprintf(output, "*/");
    }
    fprintf(output, "\n");
    iter = iter->next;
  }
  
  iter = section->first;
  while (NULL != iter ) { 
    if (iter->block)ir_print_vertex_edges(output, iter->block);
    iter = iter->next;
  }
  
}

void ir_generate_for_ast(struct node *node){
  ir_generate_for_expression(node);
  /*
  if (_hasmain == 0)
    compiler_print_error(node->location, "no main declared");
  */
}


void ir_generate_load_optz(struct ir_instruction *instruction, struct ir_operand *operand){
  struct ir_instruction *prev = NULL;
  struct ir_instruction *next = NULL;
  struct ir_instruction *newinstruction = NULL;

  prev = instruction->prev;
  next = instruction->next;

  switch(instruction->kind){
    case IR_LOAD:
      newinstruction = ir_instruction(IR_LOAD_OPTZ);
      break;
    case IR_LOAD_HALF_WD:
      newinstruction = ir_instruction(IR_LOAD_HALF_WD_OPTZ);
      break;
    case IR_LOAD_BYTE:
      newinstruction = ir_instruction(IR_LOAD_BYTE_OPTZ);
      break;
    case IR_LOAD_UNSIGNEDHALFWD:
      newinstruction = ir_instruction(IR_LOAD_UNSIGNEDHALFWD_OPTZ);
      break;
    case IR_LOAD_UNSIGNEDBYTE:
      newinstruction = ir_instruction(IR_LOAD_UNSIGNEDBYTE_OPTZ);
      break;
    default:
      assert(0);       
  }
  ir_operand_copy(newinstruction, 0, &instruction->operands[0]);
  ir_operand_copy(newinstruction,1, operand);
  prev->next = newinstruction;
  next->prev = newinstruction;
  newinstruction->prev = prev;
  newinstruction->next = next;
}

void ir_generate_store_optz(struct ir_instruction *instruction, struct ir_operand *operand){
  struct ir_instruction *prev = NULL;
  struct ir_instruction *next = NULL;
  struct ir_instruction *newinstruction = NULL;

  prev = instruction->prev;
  next = instruction->next;

  switch(instruction->kind){
    case IR_STORE:
      newinstruction = ir_instruction(IR_STORE_OPTZ);
      break;
    case IR_STORE_HALF_WD:
      newinstruction = ir_instruction(IR_STORE_HALF_WD_OPTZ);
      break;
    case IR_STORE_BYTE:
      newinstruction = ir_instruction(IR_STORE_BYTE_OPTZ);
      break;
    default:
      assert(0);       
  }
  ir_operand_copy(newinstruction, 0, operand);
  ir_operand_copy(newinstruction,1, &instruction->operands[1]);
  prev->next = newinstruction;
  next->prev = newinstruction;
  newinstruction->prev = prev;
  newinstruction->next = next;
  newinstruction->reset_reg = instruction->reset_reg;
}

void ir_generate_immediate_optz(struct ir_instruction *instruction, struct ir_operand *operand, int operand_pos){
  struct ir_instruction *prev = NULL;
  struct ir_instruction *next = NULL;
  struct ir_instruction *newinstruction = NULL;

  prev = instruction->prev;
  next = instruction->next;

  switch(instruction->kind){
    case IR_ADD:
      newinstruction = ir_instruction(IR_ADD_IMM);
      break;
    case IR_SIGNED_SUB:
      newinstruction = ir_instruction(IR_SIGNED_SUB_IMM);
      break;
    case IR_UNSIGNED_SUB:
      newinstruction = ir_instruction(IR_UNSIGNED_SUB_IMM);
      break;
    case IR_SIGNED_MULT:
      newinstruction = ir_instruction(IR_SIGNED_MULT_IMM);
      break;
    case IR_UNSIGNED_MULT:
      newinstruction = ir_instruction(IR_UNSIGNED_MULT_IMM);
      break;
    case IR_SIGNED_DIV:
      newinstruction = ir_instruction(IR_SIGNED_DIV_IMM);
      break;
    case IR_UNSIGNED_DIV:
      newinstruction = ir_instruction(IR_UNSIGNED_DIV_IMM);
      break;
    case IR_SIGNED_REM:
      newinstruction = ir_instruction(IR_SIGNED_REM_IMM);
      break;
    case IR_UNSIGNED_REM:
      newinstruction = ir_instruction(IR_UNSIGNED_REM_IMM);
      break;
    default:
      assert(0);       
  }
  ir_operand_copy(newinstruction,0, &instruction->operands[0]);
  if (operand_pos == 1)
    ir_operand_copy(newinstruction, 1, &instruction->operands[2]);
  else
    ir_operand_copy(newinstruction, 1, &instruction->operands[1]);
  ir_operand_copy(newinstruction, 2, operand);
  prev->next = newinstruction;
  next->prev = newinstruction;
  newinstruction->prev = prev;
  newinstruction->next = next;
  newinstruction->reset_reg = instruction->reset_reg;
}

void ir_generate_constant_optz(struct ir_instruction *instruction, struct ir_operand *operand){
  struct ir_instruction *prev = NULL;
  struct ir_instruction *next = NULL;
  struct ir_instruction *newinstruction = NULL;

  prev = instruction->prev;
  next = instruction->next;

  newinstruction = ir_instruction(IR_CONST);
  ir_operand_copy(newinstruction,0, &instruction->operands[0]);
  newinstruction->operands[1].kind = OPERAND_NUMBER;
  
  switch(instruction->kind){
    case IR_ADD_IMM:
      newinstruction->operands[1].data.number = operand->data.number + instruction->operands[2].data.number;
      break;
    case IR_SIGNED_SUB_IMM:
      newinstruction->operands[1].data.number = operand->data.number - instruction->operands[2].data.number;
      break;
    case IR_UNSIGNED_SUB_IMM:
      newinstruction->operands[1].data.number = (unsigned int)(operand->data.number - instruction->operands[2].data.number);
      break;
    case IR_SIGNED_MULT_IMM:
      newinstruction->operands[1].data.number = operand->data.number * instruction->operands[2].data.number;
      break;
    case IR_UNSIGNED_MULT_IMM:
      newinstruction->operands[1].data.number = (unsigned int)(operand->data.number * instruction->operands[2].data.number);
      break;
    case IR_SIGNED_DIV_IMM:
      newinstruction->operands[1].data.number = operand->data.number / instruction->operands[2].data.number;
      break;
    case IR_UNSIGNED_DIV_IMM:
      newinstruction->operands[1].data.number = (unsigned int)(operand->data.number / instruction->operands[2].data.number);
      break;
    case IR_SIGNED_REM_IMM:
      newinstruction->operands[1].data.number = operand->data.number % instruction->operands[2].data.number;
      break;
    case IR_UNSIGNED_REM_IMM:
      newinstruction->operands[1].data.number = (unsigned int)(operand->data.number % instruction->operands[2].data.number);
      break;
    default:
      assert(0);       
  }
  prev->next = newinstruction;
  next->prev = newinstruction;
  newinstruction->prev = prev;
  newinstruction->next = next;
  newinstruction->reset_reg = instruction->reset_reg;
}

int ir_replace_load_store(struct ir_instruction *instruction){
  struct ir_instruction *iter = instruction->next;
  struct ir_instruction *next = NULL;
  struct ir_operand *operand = &instruction->operands[0];
  int breplaced = 0;

  while(iter){
    if (iter->kind == IR_LOAD || iter->kind == IR_LOAD_HALF_WD || iter->kind == IR_LOAD_BYTE ||
        iter->kind == IR_LOAD_UNSIGNEDBYTE || iter->kind == IR_LOAD_UNSIGNEDHALFWD ){
        if (iter->operands[1].data.temporary == operand->data.temporary){
          ir_generate_load_optz(iter, &instruction->operands[1]);
          next = iter->next; 
          iter->prev = NULL; iter->next = NULL;
          free(iter); iter = NULL;
          breplaced = 1;
          iter = next; continue;
        }
    }
    else if (iter->kind == IR_STORE || iter->kind == IR_STORE_HALF_WD || iter->kind == IR_STORE_BYTE){     
        if (iter->operands[0].data.temporary == operand->data.temporary){
          ir_generate_store_optz(iter, &instruction->operands[1]); 
          next = iter->next;
          iter->prev = NULL; iter->next = NULL;
          free(iter); iter = NULL;
          breplaced = 1;
          iter = next; continue;
        }
    }
    else if (iter->kind == IR_ADDR_OF){   
        if (iter->operands[0].data.temporary == operand->data.temporary) iter->isdead = 1;
    }
    else if (iter->kind == IR_LABEL || iter->kind == IR_GOTO_T || iter->kind == IR_GOTO_F || iter->kind == IR_GOTO){
      break;
    }
    else if (iter->kind == IR_PROC_END || iter->kind == IR_PROC_BEGIN){
      break;
    }
    else {
      if ((iter->operands[1].kind == OPERAND_TEMPORARY && iter->operands[1].data.temporary == operand->data.temporary) ||
          (iter->operands[2].kind == OPERAND_TEMPORARY && iter->operands[2].data.temporary == operand->data.temporary)){
        break;
      }
    }
    iter = iter->next;
  }
  return breplaced;
}

int ir_replace_constants(struct ir_instruction *instruction){
  struct ir_instruction *iter = instruction->next;
  struct ir_instruction *next = NULL;
  struct ir_operand *operand = &instruction->operands[0];
  int breplaced = 0;

  while(iter){
    if (iter->kind == IR_ADD || 
        iter->kind == IR_SIGNED_SUB || iter->kind == IR_SIGNED_MULT ||  iter->kind == IR_SIGNED_DIV || iter->kind == IR_SIGNED_REM ||
        iter->kind == IR_UNSIGNED_SUB || iter->kind == IR_UNSIGNED_MULT || iter->kind == IR_UNSIGNED_DIV || iter->kind == IR_UNSIGNED_REM
       ){

        if (iter->operands[1].data.temporary == operand->data.temporary)
          ir_generate_immediate_optz(iter, &instruction->operands[1], 1);
        else if (iter->operands[2].data.temporary == operand->data.temporary)
          ir_generate_immediate_optz(iter, &instruction->operands[1], 2);
        else{
          iter = iter->next; continue;
        }
        next = iter->next; 
        iter->prev = NULL; iter->next = NULL;
        free(iter); iter = NULL;
        breplaced = 1;
        iter = next; continue;       
    }
    else if (iter->kind == IR_LABEL || iter->kind == IR_GOTO_T || iter->kind == IR_GOTO_F || iter->kind == IR_GOTO){
      break;
    }
    else if (iter->kind == IR_PROC_END || iter->kind == IR_PROC_BEGIN){
      break;
    }
    iter = iter->next;
  }
  return breplaced;
}



void ir_optimize_load_store(struct ir_section *section){
  struct ir_instruction *iter = NULL;
  if (section == NULL) return;

  iter = section->first;
  while (NULL != iter) {
    if (iter->kind == IR_ADDR_OF && ir_replace_load_store(iter)){
      iter->isdead = 1;
    }  
    iter = iter->next;
  }
}

void ir_optimize_const_folding(struct ir_section *section){
  struct ir_instruction *iter = NULL;
  if (section == NULL) return;

  iter = section->first;
  while (NULL != iter) {
    if (iter->kind == IR_CONST && ir_replace_constants(iter)){
      iter->isdead = 1;
    }  
    iter = iter->next;
  }
}

void ir_optimize(struct ir_section *section){
  ir_optimize_load_store(section);
  ir_optimize_const_folding(section);
}


struct ir_section *ir_block_section(struct ir_instruction *first, struct ir_instruction *last) { return ir_section(first, last); }
struct ir_section *ir_block_append(struct ir_section *section, struct ir_instruction *instruction){ return ir_append(section, instruction);}


