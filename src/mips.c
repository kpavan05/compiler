#include <stdlib.h>

#include <stdio.h>

#include <assert.h>

#include <string.h>



#include "node.h"

#include "type.h"

#include "symbol.h"

#include "ir.h"

#include "mips.h"



#define REG_EXHAUSTED   -1



#define FIRST_USABLE_REGISTER  8

#define LAST_USABLE_REGISTER  23

#define NUM_REGISTERS         32







static void mips_add_func_params(struct st_frame *frame, int *iblock, int *offset, struct symbol *symbol);

static void mips_add_save_register(struct st_frame *frame, int *iblock, int *offset);

static void mips_add_fp_ra(struct st_frame *frame, int *iblock, int *offset);

static void mips_add_local_stack(struct st_frame *frame, int *iblock, int *offset, struct symbol *symbol);

static struct st_frame *mips_construct_stack_frame(struct ir_instruction *instruction);

static void mips_release_stack_frame(struct ir_instruction *instruction, struct st_frame *frame);

static void mips_add_offset_blockvar(struct symbol_table *parent, int offset);

static struct st_frame *mips_current_frame(int setter, struct st_frame *frame);



static struct reg_map *top = NULL;

static struct param_holder *tail = NULL;

static struct reg_alloc *regtable = NULL;

static struct mips_instruction *first = NULL;

static struct mips_instruction *last = NULL;





/*

  mips instruction list -creation and adding to list

*/

struct mips_instruction *mips_instruction(struct ir_instruction *ircode) {

  struct mips_instruction *instruction = NULL;

  instruction = malloc(sizeof(struct mips_instruction));

  assert(instruction != NULL);

  instruction->is_proc_begin = 0;

  instruction->next = NULL;

  instruction->ircode = ircode;



  last->next = instruction;

  last = instruction;

  return instruction;

}

void mips_initialize() {

  first = malloc(sizeof(struct mips_instruction));

  assert(first != NULL);

  first->next = NULL;

  strcpy(first->string, "\n");



  last = first;

}

void mips_release() {

  struct mips_instruction *instruction = first;

  while (first) {

    instruction = first;

    first = instruction->next;

    free(instruction); instruction = NULL;

  }

  mips_clear_reg_map();

  first = last = NULL;

}





/*

  register table that maintains which register is available and

  which is allocated.

*/

void mips_init_reg_alloc(){

  int i;

  int scount = 0, tcount = 0;

  if (regtable == NULL){

    regtable = malloc(16 *sizeof(struct reg_alloc));

    assert(regtable != NULL);

  }

  for(i = 0; i < N_USEREGISTERS ; i++){

    if (i > 7){

      regtable[i].regnum = T0_REGISTER +tcount;

      regtable[i].isalloc = 0;

      tcount++;

    }

    else{

      regtable[i].regnum =  S0_REGISTER + scount;

      regtable[i].isalloc = 0;

      scount++;

    }

  }

}



int mips_get_available_register(){

  int i;

  for(i = 0; i < N_USEREGISTERS ; i++){

      if (regtable[i].isalloc == 0){

        regtable[i].isalloc = 1;

        return regtable[i].regnum;   

      }

  }

  return -1;

}



/*

  register map - maintains which temporary is mapped to which register

*/

void mips_clear_reg_map(){

   struct reg_map *item = top;

   while(top){ 

      top = item->next;

      free(item);item = NULL;

      item = top;

   }

   top = NULL;

   mips_init_reg_alloc();

}



struct reg_map *mips_find_in_reg_map(int temporary){

  struct reg_map *item = top;

  if (item == NULL) return NULL;

  

  while(item){

    if (item->ntemporary == temporary) {

      return item;

    }

    item = item->next;

  }

  return NULL;

}

int mips_s_register(int number){

  if (number >= S0_REGISTER && number <= S7_REGISTER)

    return 1;

  return 0;

}

int mips_is_reg_alloc(int number){

  if (number >= T0_REGISTER && number <= T7_REGISTER)

    return regtable[N_SPILLREGISTERS + number -T0_REGISTER].isalloc;

  else if (number >= S0_REGISTER && number <= S7_REGISTER)

    return regtable[number -S0_REGISTER].isalloc;

  else

    return 0;

}

void mips_handle_spill(){

  struct st_frame *frame = mips_current_frame(0, NULL);

  struct mips_instruction *m_instr = NULL;

  struct reg_map *item = top;

  int i =0;



  if (item == NULL) return;

 

  while (item && i < N_SPILLREGISTERS){

    

    if (mips_is_reg_alloc(item->nregister) &&

        item->isidentifier ==0 && 

        item->isonstack == 0 &&

        item->isduplicate == 0 

       ){

        item->offset = frame->size;

        frame->size += DEFAULT_REG_SIZE;

        item->isonstack = 1;

        m_instr = mips_instruction(NULL);

        if (item->nregister >= T0_REGISTER && item->nregister <= T7_REGISTER){

          regtable[N_SPILLREGISTERS + item->nregister- T0_REGISTER].isalloc = 0;

          sprintf(m_instr->string, "%10s %10s%d, %10d(%s)", "sw", "$t", item->nregister-T0_REGISTER, item->offset, "$fp");

        }

        else if(item->nregister >= S0_REGISTER && item->nregister <= S7_REGISTER){

          regtable[item->nregister- S0_REGISTER].isalloc = 0;

          sprintf(m_instr->string, "%10s %10s%d, %10d(%s)", "sw", "$s", item->nregister-S0_REGISTER, item->offset, "$fp");

        }

        i++;

    }

    item = item->next;

  }



  m_instr = first;

  while (m_instr != last){

    if (m_instr->is_proc_begin ==1) break;

    m_instr = m_instr->next;

  }

  if (m_instr){

    sprintf(m_instr->string, "%10s %10s, %10s, %10d", "addiu", "$sp", "$sp", -(frame->size));

  }

}



struct reg_map *mips_set_old_reg_map(int t1, int t2){

  struct reg_map *item = NULL;

  struct reg_map *prev = top;

  struct reg_map *next = top;

  struct reg_map *r1 =  mips_find_in_reg_map(t1);



  if (r1 == NULL) return NULL;

  item = malloc(sizeof(struct reg_map));

  assert(item != NULL);

  item->next = NULL;

  item->offset = 0;

  item->isonstack = 0;

  item->isidentifier = 0;

  item->isduplicate = 1;



  while(next){ prev = next; next = next->next;}

  prev->next = item;

  item->ntemporary = t2;

  item->nregister = r1->nregister;

  strcpy(item->name, r1->name);

  return item;

}



struct reg_map *mips_add_to_reg_map(int temporary, int *is_spilled){

  struct reg_map *item = NULL;

  int regavailable = 0;



  *is_spilled = 0;

  regavailable = mips_get_available_register();



  if (regavailable == -1){

    *is_spilled = 1;

    return NULL;

  }



  item = malloc(sizeof(struct reg_map));

  assert(item != NULL);

  item->next = NULL;

  item->offset = 0;

  item->isonstack = 0;

  item->isidentifier = 0;  

  item->isduplicate = 0;



  if (top == NULL){     

    top = item;

  }

  else{

    struct reg_map *prev = top;

    struct reg_map *next = top;

    while(next){ prev = next; next = next->next;}

    prev->next = item;

  }

  item->ntemporary = temporary;

  

  if (regavailable >= T0_REGISTER && regavailable <= T7_REGISTER){

    item->nregister = regavailable;

    sprintf(item->name, "$t%d", regavailable - T0_REGISTER);

  } 

  else if(regavailable >= S0_REGISTER && regavailable <= S7_REGISTER){

    item->nregister = regavailable;

    sprintf(item->name, "$s%d", item->nregister -S0_REGISTER);

  }

  return item;

}



struct reg_map *mips_get_regmap_item(struct ir_operand *operand) {

  struct reg_map *item = NULL;

  int temporary = -1;

  int isspill =0;

  static int dummy;

  static int ist8alloc;

  static int ist9alloc;

  struct mips_instruction *m_instr = NULL;

  if (operand == NULL){

    /*

      need to maintain unique temporary for created mips instruction 

      which does not have corresponding ir which is why maintaining

      unique negative numbers.

    */  

    item = mips_add_to_reg_map(--dummy, &isspill);

    temporary = dummy;

  }

  else{

    assert(OPERAND_TEMPORARY == operand->kind);

    temporary = operand->data.temporary;

    if ((item = mips_find_in_reg_map(temporary)) == NULL){

      item = mips_add_to_reg_map(temporary, &isspill);

    }

    else if (item->isonstack){

      m_instr = mips_instruction(NULL);

      if (ist8alloc == 0 || (ist8alloc == 1 && ist9alloc == 1)){

        sprintf(m_instr->string, "%10s %10s, %10d(%s)", "lw", "$t8", item->offset, "$fp");

        strcpy(item->name, "$t8");

        ist8alloc = 1; ist9alloc = 0;

      }

      else{

        sprintf(m_instr->string, "%10s %10s, %10d(%s)", "lw", "$t9", item->offset, "$fp");

        strcpy(item->name, "$t9");

        ist9alloc = 1;

      }

    }

  }



  if (item == NULL && isspill){

    mips_handle_spill();

    item = mips_add_to_reg_map(temporary, &isspill);

  }

  return item;

}



int mips_find_offset(struct st_frame *frame, char *name){

  int i;

  assert(frame != NULL);

  if(name[0] == '\0') return -1;

  for(i=0; i< frame->nblocks; i++){

    if (strcmp(frame->blocks[i].variable.name,name) == 0)

      return frame->blocks[i].offset;   

  }

  assert(0);

  return -1;

}



/****************************

 * MIPS TEXT SECTION OUTPUT *

 ****************************/





void mips_print_identifier(char *minstruction, struct ir_operand *operand) {

  char str[16];

  if (operand->kind == OPERAND_LABEL){

    sprintf(str, "%10s", operand->data.label);

  }

  else if ( strcmp(operand->data.symbol->table_name, "global") == 0)

    sprintf(str, "%10s", operand->data.symbol->name); 

  else

    sprintf(str, "%10d(%s)", operand->data.symbol->st_offset, "$fp"); 

  strcat(minstruction, str);

}



void mips_print_number_operand(char *minstruction, struct ir_operand *operand) {

  char str[16];

  assert(OPERAND_NUMBER == operand->kind);

  sprintf(str, "%10lu", operand->data.number);

  strcat(minstruction, str);

}



/*

  void mips_print_temporary_operand(char **output, struct ir_operand *operand) {

  struct reg_map *item = NULL;

  if (operand == NULL){

    item = mips_add_register_map(operand->data.temporary);

  }

  else{

    assert(OPERAND_TEMPORARY == operand->kind);

    if ((item = mips_find_in_registermap(temporary)) == NULL){

      item = mips_add_register_map(operand->data.temporary);

    }

  }

  //sprintf(output, "%8s%02d", "$", operand->data.temporary + FIRST_USABLE_REGISTER);

  if (item->is_temp == 1)

    sprintf(output, "%8s%d", "$t", item->nregister -T0_REGISTER);

  else

    sprintf(output, "%8s%d", "$s", item->nregister - S0_REGISTER); 

}

*/

/*void mips_print_arithmetic(char **output, struct ir_instruction *instruction) {

  

  static char *opcodes[] = {

    NULL,

    "mulu",

    "divu",

    "addu",

    "subu",

    NULL

  };

  sprintf(output, "%10s ", opcodes[instruction->kind]);

  mips_print_temporary_operand(output, &instruction->operands[0]);

  fputs(", ", output);

  mips_print_temporary_operand(output, &instruction->operands[1]);

  fputs(", ", output);

  mips_print_temporary_operand(output, &instruction->operands[2]);

  fputs("\n", output);

}



void mips_print_copy(char **output, struct ir_instruction *instruction) {

  sprintf(output, "%10s ", "or");

  mips_print_temporary_operand(output, &instruction->operands[0]);

  fputs(", ", output);

  mips_print_temporary_operand(output, &instruction->operands[1]);

  sprintf(output, ", %10s\n", "$0");

}

*/

void mips_print_load_immediate(struct ir_instruction *instruction) {

  struct reg_map *item1 = mips_get_regmap_item(&instruction->operands[0]);

  struct mips_instruction *m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s %10s, ", "li", item1->name);

  mips_print_number_operand(m_instr->string, &instruction->operands[1]);

  /*strcat(m_instr->string, "\n");*/

}



void mips_print_print_number(struct ir_instruction *instruction) {

  /* Print the number. */

  struct reg_map *item1 = mips_get_regmap_item(&instruction->operands[0]);

  struct mips_instruction *m_instr = NULL;

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s %10s, %10s, %10d", "ori", "$v0", "$0", 1);

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s %10s, %10s, %10s", "or", "$a0", "$0", item1->name);

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s", "syscall");



  /* Print a newline. */

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s %10s, %10s, %10d", "ori", "$v0", "$0", 4);

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s %10s, %10s", "la", "$a0", "newline");

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s", "syscall");

}



void mips_print_uminus(struct ir_instruction *instruction) {

  struct reg_map *item1 = mips_get_regmap_item(&instruction->operands[0]);

  struct reg_map *item2 = mips_get_regmap_item(&instruction->operands[1]);

  struct mips_instruction *m_instr = NULL;

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s %10s, %10s", "neg", item1->name, item2->name);

}



void mips_print_load(struct ir_instruction *instruction){

  struct reg_map *item1 = mips_get_regmap_item(&instruction->operands[0]);

  struct reg_map *item2 = mips_get_regmap_item(&instruction->operands[1]);

  struct mips_instruction *m_instr = NULL;

  switch (instruction->kind){

    case IR_LOAD:

      m_instr = mips_instruction(instruction);

     sprintf(m_instr->string, "%10s %10s,%10s%s%s","lw", item1->name, "(", item2->name,")");

     break;

    case IR_LOAD_HALF_WD:

      m_instr = mips_instruction(instruction);

     sprintf(m_instr->string, "%10s %10s,%10s%s%s","lh", item1->name, "(",item2->name,")");

     break;

    case IR_LOAD_UNSIGNEDHALFWD:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s%s%s","lhu", item1->name, "(",item2->name,")");

      break;

    case IR_LOAD_UNSIGNEDBYTE:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s%s%s","lb", item1->name,"(", item2->name,")");

      break;

    case IR_LOAD_BYTE:

      m_instr = mips_instruction(instruction);

     sprintf(m_instr->string, "%10s %10s,%10s%s%s","lb", item1->name,"(", item2->name,")");

     break;

    default:

     assert(0);

  }

}



void mips_print_load_optz(struct ir_instruction *instruction){

  struct reg_map *item1 = mips_get_regmap_item(&instruction->operands[0]); 

  struct mips_instruction *m_instr = NULL;

  switch (instruction->kind){

    case IR_LOAD_OPTZ:

      m_instr = mips_instruction(instruction);

     sprintf(m_instr->string, "%10s %10s,","lw", item1->name);

     break;

    case IR_LOAD_HALF_WD_OPTZ:

      m_instr = mips_instruction(instruction);

     sprintf(m_instr->string, "%10s %10s,","lh", item1->name);

     break;

    case IR_LOAD_UNSIGNEDHALFWD_OPTZ:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,","lhu", item1->name);

      break;

    case IR_LOAD_UNSIGNEDBYTE_OPTZ:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,","lb", item1->name);

      break;

    case IR_LOAD_BYTE_OPTZ:

      m_instr = mips_instruction(instruction);

     sprintf(m_instr->string, "%10s %10s,","lb", item1->name);

     break;

    default:

     assert(0);

  }

  mips_print_identifier(m_instr->string, &instruction->operands[1]);

}



void mips_print_store_optz(struct ir_instruction *instruction){

  struct reg_map *item2 = mips_get_regmap_item(&instruction->operands[1]);

  struct mips_instruction *m_instr = NULL;

  switch (instruction->kind){

    case IR_STORE_OPTZ:

     m_instr = mips_instruction(instruction);

     sprintf(m_instr->string, "%10s %10s,","sw", item2->name);

     break;

    case IR_STORE_HALF_WD_OPTZ:

     m_instr = mips_instruction(instruction);

     sprintf(m_instr->string, "%10s %10s,","sh", item2->name);

     break;

    case IR_STORE_BYTE_OPTZ:

     m_instr = mips_instruction(instruction);

     sprintf(m_instr->string, "%10s %10s,","sb", item2->name);

     break;

    default:

     assert(0);

  }

  mips_print_identifier(m_instr->string, &instruction->operands[0]);

}



void mips_print_store(struct ir_instruction *instruction){

  struct reg_map *item1 = mips_get_regmap_item(&instruction->operands[0]);

  struct reg_map *item2 = mips_get_regmap_item(&instruction->operands[1]);

  struct mips_instruction *m_instr = NULL;

  switch (instruction->kind){

    case IR_STORE:

     m_instr = mips_instruction(instruction);

     sprintf(m_instr->string, "%10s %10s,%10s%s%s","sw", item2->name, "(",item1->name,")");

     break;

    case IR_STORE_HALF_WD:

     m_instr = mips_instruction(instruction);

     sprintf(m_instr->string, "%10s %10s,%10s%s%s","sh", item2->name, "(", item1->name,")");

     break;

    case IR_STORE_BYTE:

     m_instr = mips_instruction(instruction);

     sprintf(m_instr->string, "%10s %10s,%10s%s%s","sb", item2->name, "(", item1->name,")");

     break;

    default:

     assert(0);

  }

}



void mips_print_arithmetic_op(struct ir_instruction *instruction){

  struct reg_map *item1 = mips_get_regmap_item(&instruction->operands[0]);

  struct reg_map *item2 = mips_get_regmap_item(&instruction->operands[1]);

  struct reg_map *item3 = mips_get_regmap_item(&instruction->operands[2]);

  struct mips_instruction *m_instr = NULL;

  switch(instruction->kind){

    case IR_ADD:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","add", item1->name, item2->name, item3->name);

      break;

    case IR_SIGNED_SUB:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","sub", item1->name, item2->name, item3->name);

      break;

    case IR_UNSIGNED_SUB:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","subu", item1->name, item2->name, item3->name);

      break;

    case IR_SIGNED_MULT:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","mul", item1->name, item2->name, item3->name);

      break;

    case IR_UNSIGNED_MULT:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","mulou", item1->name, item2->name, item3->name);

      break;

    case IR_SIGNED_DIV:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","div", item1->name, item2->name, item3->name);

      break;

    case IR_UNSIGNED_DIV:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","divu", item1->name, item2->name, item3->name);

      break;

    case IR_SIGNED_REM:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","rem", item1->name, item2->name, item3->name);

      break;

    case IR_UNSIGNED_REM:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","remu", item1->name, item2->name, item3->name);

      break;

    default: assert (0);

  }

}



void mips_print_arithmetic_immediate_op(struct ir_instruction *instruction){

  struct reg_map *item1 = mips_get_regmap_item(&instruction->operands[0]);

  struct reg_map *item2 = mips_get_regmap_item(&instruction->operands[1]);

  struct mips_instruction *m_instr = NULL;

  switch(instruction->kind){

    case IR_ADD_IMM:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,","addi", item1->name, item2->name);

      break;

    case IR_SIGNED_SUB_IMM:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,","sub", item1->name, item2->name);

      break;

    case IR_UNSIGNED_SUB_IMM:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,","subu", item1->name, item2->name);

      break;

    case IR_SIGNED_MULT_IMM:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,","mul", item1->name, item2->name);

      break;

    case IR_UNSIGNED_MULT_IMM:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,","mulou", item1->name, item2->name);

      break;

    case IR_SIGNED_DIV_IMM:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,","div", item1->name, item2->name);

      break;

    case IR_UNSIGNED_DIV_IMM:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,","divu", item1->name, item2->name);

      break;

    case IR_SIGNED_REM_IMM:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,","rem", item1->name, item2->name);

      break;

    case IR_UNSIGNED_REM_IMM:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,","remu", item1->name, item2->name);

      break;

    default: assert (0);

  }

  mips_print_number_operand(m_instr->string, &instruction->operands[2]);

}



void mips_print_binary_op(struct ir_instruction *instruction){

  struct reg_map *item1 = mips_get_regmap_item(&instruction->operands[0]);

  struct reg_map *item2 = mips_get_regmap_item(&instruction->operands[1]);

  struct reg_map *item3 = NULL;

  struct mips_instruction *m_instr = NULL;

  item3 = instruction->kind != IR_BITOP_NEG ? mips_get_regmap_item(&instruction->operands[2]):NULL;



  switch(instruction->kind){

    case IR_BITOP_AND:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","and", item1->name, item2->name, item3->name);

      break;

    case IR_BITOP_OR:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","or", item1->name, item2->name, item3->name);

      break;

    case IR_BITOP_XOR:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","xor", item1->name, item2->name, item3->name);

      break;

    case IR_BITOP_NEG:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s","not", item1->name, item2->name);

      break;

    default:

      assert(0);

  }

}



void mips_print_shift(struct ir_instruction *instruction){

  struct reg_map *item1 = mips_get_regmap_item(&instruction->operands[0]);

  struct reg_map *item2 = mips_get_regmap_item(&instruction->operands[1]);

  struct reg_map *item3 = mips_get_regmap_item(&instruction->operands[2]);

  struct mips_instruction *m_instr = NULL;

  switch(instruction->kind){

    case IR_LSHIFT:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","sll", item1->name, item2->name, item3->name);

      break;

    case IR_RSHIFT:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","sra", item1->name, item2->name, item3->name);

      break;

    case IR_UNSIGNED_RSHIFT:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","srl", item1->name, item2->name, item3->name);

      break;

    default: assert (0);

  }

}



void mips_print_branch(struct ir_instruction *instruction){

  struct reg_map *item1 = mips_get_regmap_item(&instruction->operands[0]);

  struct mips_instruction *m_instr = NULL;

  switch(instruction->kind){

    case IR_GOTO_F:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s","blez", item1->name, instruction->operands[1].data.label);

      break;

    case IR_GOTO_T:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s,%10s","bgtz", item1->name, instruction->operands[1].data.label);

      break;

    default : assert (0);

  }

}



void mips_print_compare(struct ir_instruction *instruction){

  struct reg_map *item1 = mips_get_regmap_item(&instruction->operands[0]);

  struct reg_map *item2 = mips_get_regmap_item(&instruction->operands[1]);

  struct reg_map *item3 = mips_get_regmap_item(&instruction->operands[2]);

  struct mips_instruction *m_instr = NULL;

  m_instr = mips_instruction(instruction);

  switch(instruction->kind){

    case IR_EQUAL: 

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","seq", item1->name, item2->name, item3->name);

      break; 

    case IR_NOT_EQUAL:

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","sne", item1->name, item2->name, item3->name);

      break;

    case IR_LESS:

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","slt", item1->name, item2->name, item3->name);

      break;

    case IR_UNSIGNED_LESS:

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","sltu", item1->name, item2->name, item3->name);

      break;

    case IR_LESS_EQUAL:

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","sle", item1->name, item2->name, item3->name);

      break;

    case IR_UNSIGNED_LESS_EQUAL:

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","sleu", item1->name, item2->name, item3->name);

      break;

    case IR_GREATER:

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","sgt", item1->name, item2->name, item3->name);

      break;

    case IR_UNSIGNED_GREATER:

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","sgtu", item1->name, item2->name, item3->name);

      break;

    case IR_GREATER_EQUAL:

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","sge", item1->name, item2->name, item3->name);

      break;

    case IR_UNSIGNED_GREATER_EQUAL:

      sprintf(m_instr->string, "%10s %10s,%10s,%10s","sgeu", item1->name, item2->name, item3->name);

      break;

    default : assert(0);

  }

}



struct reg_map *mips_add_load_for_cast(struct ir_instruction *instruction){

  struct reg_map *item = NULL;

  struct reg_map *opitem = NULL;

  struct mips_instruction *m_instr = NULL;

  struct ir_operand *operand = &instruction->operands[1];

  opitem = mips_get_regmap_item(operand); 

  if (operand->islvalue != 1) return opitem;

  

  item = mips_get_regmap_item(NULL);



  switch(instruction->kind) {

    case IR_CASTUNSIGNEDWORD_BYTE:   

    case IR_CASTWORD_BYTE:

    case IR_CASTUNSIGNEDHALFWD_BYTE:  

    case IR_CASTHALFWD_BYTE:    

    case IR_CASTUNSIGNEDBYTE_BYTE:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s, %10s%s%s", "lb", item->name, "(", opitem->name, ")");

      break;

    case IR_CASTWORD_UNSIGNEDBYTE:

    case IR_CASTUNSIGNEDWORD_UNSIGNEDBYTE:

    case IR_CASTBYTE_UNSIGNEDBYTE:

    case IR_CASTHALFWD_UNSIGNEDBYTE:

    case IR_CASTUNSIGNEDHALFWD_UNSIGNEDBYTE:  

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s, %10s%s%s", "lbu", item->name, "(", opitem->name, ")");

      break;       

    case IR_CASTUNSIGNEDBYTE_HALFWD:

    case IR_CASTBYTE_HALFWD:

    case IR_CASTUNSIGNEDWORD_HALFWD:

    case IR_CASTUNSIGNEDHALFWD_HALFWD:

    case IR_CASTWORD_HALFWD:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s, %10s%s%s", "lh", item->name, "(", opitem->name, ")");

      break;

    case IR_CASTWORD_UNSIGNEDHALFWD:

    case IR_CASTUNSIGNEDWORD_UNSIGNEDHALFWD:

    case IR_CASTHALFWD_UNSIGNEDHALFWD:

    case IR_CASTBYTE_UNSIGNEDHALFWD:

    case IR_CASTUNSIGNEDBYTE_UNSIGNEDHALFWD:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s, %10s%s%s", "lhu", item->name, "(", opitem->name, ")");

      break;   

    default :

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s, %10s%s%s", "lw", item->name, "(", opitem->name, ")");

      break;

  }

  return item;

}



void mips_print_cast_expr(struct ir_instruction *instruction){

  struct reg_map *item1 = NULL;

  struct reg_map *item = NULL;

  struct mips_instruction *m_instr = NULL;

  

  if (instruction->kind == IR_CASTARRAY_POINTER) {

    item = mips_set_old_reg_map(instruction->operands[1].data.temporary, instruction->operands[0].data.temporary);

    return;

  }



  item1 = mips_get_regmap_item(&instruction->operands[0]);

  item =  mips_add_load_for_cast(instruction);



  switch(instruction->kind){

    case IR_CASTUNSIGNEDBYTE_WORD:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s, %10s, %#10x", "andi", item1->name, item->name, 255);

      break;

    case IR_CASTUNSIGNEDBYTE_HALFWD:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s, %10s, %#10x", "andi", item1->name, item->name, 255);

      break;

    case IR_CASTUNSIGNEDHALFWD_WORD:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s, %10s, %#10x", "andi", item1->name, item->name, 65535);

      break;

    case IR_CASTUNSIGNEDWORD_BYTE:

      {

        struct reg_map *shift = mips_get_regmap_item(NULL);

        m_instr = mips_instruction(instruction);

        sprintf(m_instr->string, "%10s %10s,%10s,%10d","sll", shift->name, item->name, 24);

        m_instr = mips_instruction(instruction);

        sprintf(m_instr->string, "%10s %10s,%10s,%10d","sra", item1->name, shift->name, 24);

      }

      break;

    case IR_CASTUNSIGNEDHALFWD_BYTE:

      {

        struct reg_map *shift = mips_get_regmap_item(NULL);

        m_instr = mips_instruction(instruction);

        sprintf(m_instr->string, "%10s %10s,%10s,%10d","sll", shift->name, item->name, 8);

        m_instr = mips_instruction(instruction);

        sprintf(m_instr->string, "%10s %10s,%10s,%10d","sra", item1->name, shift->name, 8);

      }

      break;

    case IR_CASTUNSIGNEDWORD_HALFWD:

      {

        struct reg_map *shift = mips_get_regmap_item(NULL);

        m_instr = mips_instruction(instruction);

        sprintf(m_instr->string, "%10s %10s,%10s,%10d","sll", shift->name, item->name, 16);

        m_instr = mips_instruction(instruction);

        sprintf(m_instr->string, "%10s %10s,%10s,%10d","sra", item1->name, shift->name, 16);

      }

      break;

    default:

      m_instr = mips_instruction(instruction);

      sprintf(m_instr->string, "%10s %10s, %10s, %#10x", "ori", item1->name, item->name, 0);

      break;

  }

}



void mips_print_load_address(struct ir_instruction *instruction){

  struct reg_map *item1 = mips_get_regmap_item(&instruction->operands[0]);

  struct mips_instruction *m_instr = NULL;

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s %10s,","la", item1->name);

  item1->isidentifier = 1;

  mips_print_identifier(m_instr->string, &instruction->operands[1]);



}



void mips_print_proc_begin(struct ir_instruction *instruction, struct st_frame *frame){

  struct type *functype = NULL;

  struct ir_operand *operand = &instruction->operands[0];

  struct mips_instruction *m_instr = NULL;

  int offset = 0;

  int i;



  assert(operand->kind == OPERAND_IDENTIFIER);

  m_instr = mips_instruction(instruction);

  m_instr->is_proc_begin = 1;

  sprintf(m_instr->string, "%10s %10s, %10s, %10d", "addiu", "$sp", "$sp", -(frame->size));

  offset = mips_find_offset(frame, "$fp");

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s %10s, %10d(%s)", "sw", "$fp", offset, "$sp");

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s %10s, %10s, %10s", "or", "$fp", "$sp", "$0");

  offset = mips_find_offset(frame, "$ra");

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s %10s, %10d(%s)", "sw", "$ra", offset, "$fp");



  functype = operand->data.symbol->result.type;

  for (i = 0;  i < functype->data.functype.nparams ; i++ ){

    if (i== 0) offset = mips_find_offset(frame, "$a0");

    m_instr = mips_instruction(instruction);

    sprintf(m_instr->string, "%10s %10s%d, %10d(%s)", "sw", "$a", i, offset, "$fp");

    offset += DEFAULT_REG_SIZE;

  }



  offset = mips_find_offset(frame, "$s0");

  for (i = S0_REGISTER;  i <= S7_REGISTER ; i++ ){

    m_instr = mips_instruction(instruction);

    sprintf(m_instr->string, "%10s %10s%d, %10d(%s)", "sw", "$s", i-S0_REGISTER, offset, "$fp");

    offset += DEFAULT_REG_SIZE;

  }

  for (i = T0_REGISTER;  i <= T7_REGISTER ; i++ ){

    m_instr = mips_instruction(instruction);

    sprintf(m_instr->string, "%10s %10s%d, %10d(%s)", "sw", "$t", i -T0_REGISTER, offset, "$fp");

    offset += DEFAULT_REG_SIZE;

  }



}



void mips_print_proc_end(struct ir_instruction *instruction, struct st_frame *frame){

  int offset, i;

  struct ir_operand *operand = &instruction->operands[0];

  struct mips_instruction *m_instr = NULL;

  assert(operand->kind == OPERAND_IDENTIFIER);



  offset = mips_find_offset(frame, "$s0");

  for (i = S0_REGISTER;  i <= S7_REGISTER ; i++ ){

    m_instr = mips_instruction(instruction);

    sprintf(m_instr->string, "%10s %10s%d, %10d(%s)", "lw", "$s", i-S0_REGISTER, offset, "$fp");

    offset += DEFAULT_REG_SIZE;

  }

  for (i = T0_REGISTER;  i <= T7_REGISTER ; i++ ){

    m_instr = mips_instruction(instruction);

    sprintf(m_instr->string, "%10s %10s%d, %10d(%s)", "lw", "$t", i -T0_REGISTER, offset, "$fp");

    offset += DEFAULT_REG_SIZE;

  }

  offset = mips_find_offset(frame, "$ra");

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s %10s, %10d(%s)", "lw", "$ra", offset, "$fp");

  offset = mips_find_offset(frame, "$fp");

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s %10s, %10d(%s)", "lw", "$fp", offset, "$fp");



  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s %10s, %10s, %10d", "addiu", "$sp", "$sp", frame->size);

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s %10s", "jr", "$ra");

  mips_clear_reg_map();

}



void mips_hold_parameters(struct ir_instruction *instruction){

  struct ir_operand *operand = &instruction->operands[0];

  struct reg_map *item = mips_get_regmap_item(&instruction->operands[1]);

  struct param_holder *holder = NULL;

  struct param_list *list = NULL;

  assert(operand->kind == OPERAND_NUMBER);    

  assert (operand->data.number < 4);



  list = malloc(sizeof(struct param_list));

  assert(list != NULL);

  list->item = item;

  list->next = NULL;

  if (operand->data.number == 0){

    holder = malloc(sizeof(struct param_holder));

    assert(holder != NULL);

    holder->list = list;

    holder->prev = NULL;

    if (tail == NULL) tail = holder;

    else {

      holder->prev = tail;

      tail = holder;

    }

  }

  else{

    struct param_list *next = tail->list;

    struct param_list *prev = tail->list;

    while(next){

      prev = next;

      next = next->next;

    }

    prev->next = list;

  }

}

void mips_print_parameters(){

  struct param_list *list = NULL;

  struct mips_instruction *m_instr = NULL;

  

  int nparam = 0;



  if (tail && tail->list){   

    list= tail->list;   

    while (list){

      m_instr = mips_instruction(NULL);

      sprintf(m_instr->string, "%10s %10s%d, %10s, %10s", "or", "$a", nparam++, list->item->name, "$0");

      list = list->next;

    }

  }

}



void mips_release_parameters(){

  struct param_holder *holder = tail; 

  /*release the holder for function parameters */

  if (holder){   

    tail = holder->prev;

    free(holder); holder = NULL;

  }

}



void mips_print_syscall(struct ir_instruction *instruction){

  struct ir_operand *operand = &instruction->operands[0];

  struct mips_instruction *m_instr = NULL;

  assert(operand->kind == OPERAND_IDENTIFIER);



  mips_print_parameters();

  m_instr = mips_instruction(instruction);

  if (strcmp(operand->data.symbol->name, "syscall_print_string") == 0)

    sprintf(m_instr->string, "%10s %10s, %10d", "li", "$v0", 4);

  else if (strcmp(operand->data.symbol->name, "syscall_print_int") == 0)

    sprintf(m_instr->string, "%10s %10s, %10d", "li", "$v0", 1);

  else if (strcmp(operand->data.symbol->name, "syscall_read_int") == 0)

    sprintf(m_instr->string, "%10s %10s, %10d", "li", "$v0", 5);

  

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s","syscall");

  mips_release_parameters();

}



void mips_print_funccall(struct ir_instruction *instruction){

  struct ir_operand *operand = &instruction->operands[0];

  struct mips_instruction *m_instr = NULL;

  

  /*generate instructions for function parameters*/

  mips_print_parameters();

  m_instr = mips_instruction(instruction);

  sprintf(m_instr->string, "%10s %10s", "jal", operand->data.symbol->name);

  mips_release_parameters();

}

void mips_print_return(struct ir_instruction *instruction){

  struct reg_map *item = mips_get_regmap_item(&instruction->operands[0]);

  struct mips_instruction *m_instr = NULL;

  m_instr = mips_instruction(instruction);

  assert(item != NULL);

  sprintf(m_instr->string, "%10s %10s, %10s, %10s", "or", "$v0", item->name, "$0");

}

void mips_print_result(struct ir_instruction *instruction){

  struct reg_map *item = mips_get_regmap_item(&instruction->operands[0]);

  struct mips_instruction *m_instr = NULL;

  m_instr = mips_instruction(instruction);

  assert(item != NULL);

  sprintf(m_instr->string, "%10s %10s, %10s, %10s", "or", item->name, "$v0","$0");

}

void mips_print_instruction(struct ir_instruction *instruction, struct st_frame *frame) { 

  if (instruction == NULL) return;  

  if (instruction->isdead == 1) return;

  switch (instruction->kind) {

  case IR_ADDR_OF: 

    mips_print_load_address(instruction);

    break;

  case IR_PROC_BEGIN: 

    mips_print_proc_begin(instruction, frame);

    break;

  case IR_PROC_END:

    mips_print_proc_end(instruction, frame);

    break;

  case IR_LOAD:

  case IR_LOAD_BYTE:

  case IR_LOAD_HALF_WD:

  case IR_LOAD_UNSIGNEDBYTE:

  case IR_LOAD_UNSIGNEDHALFWD:

    mips_print_load(instruction);

    break;

  case IR_LOAD_OPTZ:

  case IR_LOAD_BYTE_OPTZ:

  case IR_LOAD_HALF_WD_OPTZ:

  case IR_LOAD_UNSIGNEDBYTE_OPTZ:

  case IR_LOAD_UNSIGNEDHALFWD_OPTZ:

    mips_print_load_optz(instruction);

    break;

  case IR_STORE:

  case IR_STORE_BYTE:

  case IR_STORE_HALF_WD:

    mips_print_store(instruction);

    break;

  case IR_STORE_OPTZ:

  case IR_STORE_BYTE_OPTZ:

  case IR_STORE_HALF_WD_OPTZ:

    mips_print_store_optz(instruction);

    break;

  case IR_CONST:

    mips_print_load_immediate(instruction);

    break;

  case IR_ADD:

  case IR_SIGNED_SUB:

  case IR_UNSIGNED_SUB:

  case IR_SIGNED_MULT:

  case IR_UNSIGNED_MULT:

  case IR_SIGNED_DIV:

  case IR_UNSIGNED_DIV:

  case IR_SIGNED_REM:

  case IR_UNSIGNED_REM:

    mips_print_arithmetic_op(instruction);

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

    mips_print_arithmetic_immediate_op(instruction);

    break;

  case IR_LSHIFT:

  case IR_RSHIFT:

  case IR_UNSIGNED_RSHIFT:

    mips_print_shift(instruction);

    break;

  case IR_LABEL:

  {

    struct mips_instruction *m_instr = NULL;

    m_instr = mips_instruction(instruction);

    sprintf(m_instr->string, "%s:", instruction->operands[0].data.label);

  }

    break;

  case IR_GOTO:

  {

    struct mips_instruction *m_instr = NULL;

    m_instr = mips_instruction(instruction);

    sprintf(m_instr->string, "%10s %10s", "b", instruction->operands[0].data.label);

  } 

    break;

  case IR_GOTO_F:

  case IR_GOTO_T:

    mips_print_branch(instruction);

    break;

  case IR_PARAM:

    mips_hold_parameters(instruction);

    break;

  case IR_CALL:

    mips_print_funccall(instruction);

    break;

  case IR_SYSCALL:

    mips_print_syscall(instruction);

    break;

  case IR_EQUAL: 

  case IR_NOT_EQUAL:

  case IR_LESS:

  case IR_UNSIGNED_LESS:

  case IR_LESS_EQUAL:

  case IR_UNSIGNED_LESS_EQUAL:

  case IR_GREATER:

  case IR_UNSIGNED_GREATER:

  case IR_GREATER_EQUAL:

  case IR_UNSIGNED_GREATER_EQUAL:

    mips_print_compare(instruction);

    break;

  case IR_BITOP_AND:

  case IR_BITOP_OR:

  case IR_BITOP_XOR:

  case IR_BITOP_NEG:

  case IR_NEGATION:

    mips_print_binary_op(instruction);

    break;

  case IR_RESWD:

    mips_print_result(instruction);

    break;

  case IR_RETWD:

    mips_print_return( instruction);

    break;

  case IR_UMINUS:

    mips_print_uminus( instruction);

    break;

  case IR_CAST_VOID:

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

    mips_print_cast_expr( instruction);

    break;

    /*case IR_COPY:

      mips_print_copy(output, instruction);

      break;

    case IR_LOAD_IMMEDIATE:

      mips_print_load_immediate(output, instruction);

      break;

    */

  case IR_PRINT_NUMBER:

     mips_print_print_number( instruction);

     break;

  case IR_NO_OPERATION:

     break;

  default:

     assert(0);

     break;

  }

  

  if (instruction->reset_reg == 1){

    /*

    char resetstr[24];  

    sprintf(resetstr, "\n#%s", "registers are reset.");

    strcat(last->string, resetstr);

    */

    mips_clear_reg_map(); 

  }

}



/*

add $a parameter registers to stack frame

*/

static void mips_add_func_params(struct st_frame *frame, int *iblock, int *offset, struct symbol *symbol){

   int index = 0;

   int pos = *iblock;

   struct type *t = NULL;

   char name[IDENTIFIER_MAX];

   struct typelist *params = NULL;  

   if (symbol == NULL) return;

   

   t = symbol->result.type;

   assert(t->kind == TYPE_FUNCTION);

   assert(t->data.functype.nparams < 5);

   params = t->data.functype.params;

   if (params == NULL) return;

   

   while(params && index < 4){        

      frame->blocks[pos].offset =  *offset; 

      frame->blocks[pos].pad_sz = 0;

      frame->blocks[pos].actual_sz = DEFAULT_REG_SIZE;   

      frame->blocks[pos].variable.kind = VAR_REGISTER;

      frame->blocks[pos].variable.data.regnum = A0_REGISTER + index;

      

      sprintf(name, "$a%d", index);

      strcpy(frame->blocks[pos].variable.name, name);



      params->id->data.identifier.symbol->st_offset = *offset;  

      params = params->next;

      *offset += DEFAULT_REG_SIZE; 

      pos++;index++;     

   }

   *iblock = pos;

   return;

}



/*

add $s (save registers) and $t (temporary registers) to stack frame

*/

static void mips_add_save_register(struct st_frame *frame, int *iblock, int *offset){

   int nreg = 0;

   int pos = *iblock;

   char name[IDENTIFIER_MAX];

   while(nreg < 8){     

      frame->blocks[pos].offset = *offset;

      frame->blocks[pos].actual_sz = DEFAULT_REG_SIZE;

      frame->blocks[pos].pad_sz = 0;

      frame->blocks[pos].variable.kind = VAR_REGISTER;

      sprintf(name, "$s%d", nreg);

      strcpy(frame->blocks[pos].variable.name, name);

      frame->blocks[pos].variable.data.regnum = S0_REGISTER + nreg;

      *offset += DEFAULT_REG_SIZE;

      pos++;nreg++;     

   }

   nreg =0;

   while(nreg < 8){     

      frame->blocks[pos].offset = *offset;

      frame->blocks[pos].actual_sz = DEFAULT_REG_SIZE;

      frame->blocks[pos].pad_sz = 0;

      frame->blocks[pos].variable.kind = VAR_REGISTER;

      sprintf(name, "$t%d", nreg);

      strcpy(frame->blocks[pos].variable.name, name);

      frame->blocks[pos].variable.data.regnum = T0_REGISTER + nreg;

      *offset += DEFAULT_REG_SIZE;

      pos++;nreg++;     

   }

   *iblock = pos;

   return;

}



/*

add old frame pointer ($fp) and return address ($ra) registers to stack frame

*/

static void mips_add_fp_ra(struct st_frame *frame, int *iblock, int *offset){

  int pos = *iblock;

   

  frame->blocks[pos].offset = *offset;

  frame->blocks[pos].actual_sz = DEFAULT_REG_SIZE;

  frame->blocks[pos].pad_sz = 0;

  frame->blocks[pos].variable.kind = VAR_REGISTER;

  strcpy(frame->blocks[pos].variable.name, "$fp");

  frame->blocks[pos].variable.data.regnum = FRAME_REGISTER;

  pos++; *offset += DEFAULT_REG_SIZE; 



  frame->blocks[pos].offset = *offset;

  frame->blocks[pos].actual_sz = DEFAULT_REG_SIZE;

  frame->blocks[pos].pad_sz = 0;

  frame->blocks[pos].variable.kind = VAR_REGISTER;

  strcpy(frame->blocks[pos].variable.name, "$ra");

  frame->blocks[pos].variable.data.regnum = RET_REGISTER ;

  pos++; *offset += DEFAULT_REG_SIZE;



  *iblock = pos;

  return;

}





/*

add auto variables to stack frame

*/

static void mips_add_local_stack(struct st_frame *frame, int *iblock, int *offset, struct symbol *symbol){

  struct symbol_table *table = NULL;

  struct symbol_list *sym_iter = NULL;  

  struct type *t = NULL;

  struct type *functype = NULL;

  int index = 0;

  int cursz = 0;

  int padsz = 0;

  int pos = *iblock;

  

  table = symbol_find_table(symbol);

  if (table == NULL) return;

  functype = symbol->result.type;



  for (sym_iter = table->variables; NULL != sym_iter; sym_iter = sym_iter->next) {

    t = sym_iter->symbol.result.type;

    /*skip the function parameters as they are already stored in $a registers*/

    if (symbol_is_func_param(functype->data.functype.params, sym_iter->symbol.name) ) continue;

    /* get the size of current symbol and pad size based on offset*/

    cursz = type_get_alignment_size(t, *offset, &padsz, DEF_MULTIPLIER);

    /*add the pad size to previous offset to get the actual offset of current symbol*/

    *offset += padsz;

    frame->blocks[pos].offset = *offset;

    *offset += cursz;

    /*set the frame block members*/

    frame->blocks[pos-1].pad_sz = padsz;    

    frame->blocks[pos].actual_sz = cursz;  

    frame->blocks[pos].variable.kind = VAR_AUTO;

    frame->blocks[pos].variable.data.symbol = &sym_iter->symbol;

    strcpy(frame->blocks[pos].variable.name, sym_iter->symbol.name);

    /* set the offset, size on the symbol*/

    sym_iter->symbol.st_offset = frame->blocks[pos].offset;

    sym_iter->symbol.actual_sz = frame->blocks[pos].actual_sz;

    pos++; index++;

  }

  mips_add_offset_blockvar(table, *offset);

  frame->blocks[pos-1].pad_sz = frame->word_align; 

  *iblock = pos;

}



static void mips_add_offset_blockvar(struct symbol_table *parent, int offset){

  struct symbol_table *child= parent->children;

  struct symbol_list *sym_iter = NULL;

  struct type *t = NULL;

  int table_offset = 0; 

  int size = 0,padsz =0;

  

  while(child){

    table_offset = offset;

    for (sym_iter = child->variables; NULL != sym_iter; sym_iter = sym_iter->next) {

      t = sym_iter->symbol.result.type;

      size = type_get_alignment_size(t, table_offset, &padsz, DEF_MULTIPLIER);

      table_offset += padsz;

      sym_iter->symbol.st_offset = table_offset;

      table_offset += size;

    }

    mips_add_offset_blockvar(child, table_offset);

    child = child->next;

  }

  return ;

}



static struct st_frame *mips_construct_stack_frame(struct ir_instruction *instruction){

  int pos = 0;

  int offset = 0;

  int padsz = 0;

  int locals_sz = 0;



  int size = DEFAULT_STACK_SIZE;

  int nblocks = DEFAULT_STACK_BLOCKS;

  

  struct st_frame *frame = NULL;

  struct type *functype = NULL;

  struct symbol_table *table = NULL;

  struct ir_operand *operand = &instruction->operands[0];

  assert(operand->kind == OPERAND_IDENTIFIER);

  functype = operand->data.symbol->result.type;

  table = symbol_find_table(operand->data.symbol);

  if (table == NULL) return NULL;

  /*

    determine number of blocks needed in stack

    based on number of symbols in symbol table

  */

  nblocks += table->nvariables;

  if (table->children != NULL) nblocks++;

  /* calculate the stack frame size based on symbols in symbol table */

  locals_sz = symbol_calculate_stack_frame_size(operand->data.symbol);

  /* pad the last symbol to nearest word */

  padsz = ((locals_sz + 3)/4) *4 - locals_sz;

  size += (locals_sz + padsz);

  size += (functype->data.functype.nparams * DEFAULT_REG_SIZE);

  /*create stack frame and its entries*/

  frame = malloc(sizeof(struct st_frame));

  assert(frame != NULL);

  frame->word_align = padsz;

  /*frame needs to double word alignment*/

  frame->size = ((size + 7)/8*8);

  frame->dword_align = frame->size - size;  

  if (frame->dword_align > 0) nblocks++;

  /*allocate memory for blocks in stack */

  frame->nblocks = nblocks;

  frame->blocks = malloc(nblocks *sizeof(struct st_block));

  if (frame->dword_align > 0){

    frame->blocks[pos].offset = offset;

    frame->blocks[pos].actual_sz = frame->dword_align;

    frame->blocks[pos].variable.kind = VAR_NONE;

    strcpy(frame->blocks[pos].variable.name, "");

    offset = frame->dword_align;

    pos++;

  }

  mips_add_func_params(frame, &pos, &offset, operand->data.symbol);

  mips_add_save_register(frame, &pos, &offset);

  mips_add_fp_ra(frame, &pos, &offset);

  mips_add_local_stack(frame, &pos, &offset, operand->data.symbol);



  mips_current_frame(1, frame);

  return frame;

}



static struct st_frame *mips_current_frame(int setter, struct st_frame *frame){

  static struct st_frame *stack;



  if (setter){

    stack = frame;

  }

  return stack;

}



void mips_release_stack_frame(struct ir_instruction *instruction, struct st_frame *frame){

  struct ir_operand *operand = &instruction->operands[0];

  assert(operand->kind == OPERAND_IDENTIFIER);

  

  memset(frame->blocks, 0, frame->nblocks *sizeof(struct st_block));

  free(frame->blocks);

  free(frame);

}



void mips_print_stack_frame(FILE *output, struct st_frame *frame){

  int i;

  fprintf(output, "#%10s %10s  %10s %10s\n","Name" , "Offset", "Actual Size", "Pad Size");

  for(i=0; i< frame->nblocks; i++){

    fprintf(output, "#%10s %10d  %10d %10d\n",frame->blocks[i].variable.name , frame->blocks[i].offset, frame->blocks[i].actual_sz, frame->blocks[i].pad_sz);

  }

}



void mips_print(FILE *output, int *ninstructions) {

  struct mips_instruction *m_instr = first;

  while (m_instr) {

    fprintf(output, "%s", m_instr->string);

    fprintf(output, "\n");

    m_instr = m_instr->next;

    *ninstructions = *ninstructions +1;

  }

}



void mips_print_text_section(FILE *output, struct ir_section *section) {

  struct ir_instruction *instruction;

  struct st_frame *cur_frame = NULL;

  struct ir_operand *operand = NULL;

  int ninstructions = 0;

  fputs("\n.text\n.globl  main \n", output);



  mips_init_reg_alloc();

  /*fprintf(output, "\n%10s %10s,%10s,%10s\n", "and", "$ra", "$ra", "$0");*/

  for (instruction = section->first; instruction != section->last->next; instruction = instruction->next) {

    if (instruction->kind == IR_PROC_BEGIN){

      operand = &instruction->operands[0];

      fprintf(output, "%s:\n", operand->data.symbol->name);

      cur_frame = mips_construct_stack_frame(instruction);

      /*mips_print_stack_frame(output, cur_frame);*/

      mips_initialize();

    }

    mips_print_instruction(instruction, cur_frame);

    if (instruction->kind == IR_PROC_END){

      mips_print(output, &ninstructions);

      mips_release();

      mips_release_stack_frame(instruction, cur_frame);

    }

  }

  fprintf(output, "#Number of instructions:%d\n", ninstructions);

  /* Return from main. */

  

}



void mips_print_data_section(FILE *output, struct symbol_table *global){

  fputs("\n.data\n", output);

  symbol_print_strings(output);

  symbol_print_globals(output, global);

}



void mips_print_program(FILE *output, struct ir_section *section, struct symbol_table *global) {

  mips_print_data_section(output, global);

  mips_print_text_section(output, section);

}



