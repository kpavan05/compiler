#ifndef _MIPS_H
#define _MIPS_H

#include <stdio.h>
#define DEFAULT_STACK_SIZE   72
#define DEFAULT_STACK_BLOCKS 18
#define DEFAULT_REG_SIZE     4
#define N_USEREGISTERS       16
#define N_SPILLREGISTERS     8

#define ZERO_REGISTER   0
#define ASSEM_REGISTER  1
#define V0_REGISTER     2
#define V1_REGISTER     3

#define A0_REGISTER     4
#define A1_REGISTER     5
#define A2_REGISTER    	6
#define A3_REGISTER  	7

#define T0_REGISTER     8
#define T1_REGISTER     9
#define T2_REGISTER     10
#define T3_REGISTER     11
#define T4_REGISTER     12
#define T5_REGISTER     13
#define T6_REGISTER     14
#define T7_REGISTER     15

#define S0_REGISTER     16
#define S1_REGISTER     17
#define S2_REGISTER     18
#define S3_REGISTER     19
#define S4_REGISTER     20
#define S5_REGISTER     21
#define S6_REGISTER     22
#define S7_REGISTER     23

#define T8_REGISTER     24
#define T9_REGISTER     25

#define STACK_REGISTER  29
#define FRAME_REGISTER  30
#define RET_REGISTER    31

struct mips_instruction{
  char  string[64];

  int is_proc_begin;
  struct ir_instruction *ircode;
  struct mips_instruction *next;
};

struct reg_alloc{
	int regnum;
	int isalloc;
};

struct reg_map{
	int ntemporary;
	int nregister;	
	
	int offset;
	int isonstack;
	int isidentifier;
	int isduplicate;
	char name[IDENTIFIER_MAX];
	struct reg_map *next;
};

struct param_list {
	struct reg_map *item;
	struct param_list *next;
};

struct param_holder{
	struct param_list *list;
	struct param_holder *prev;
};

enum var_kind{
	VAR_NONE,
	VAR_AUTO,
	VAR_REGISTER
};

struct variable{
  enum var_kind kind;
  char name[IDENTIFIER_MAX+1];
  union{
  	int regnum;
  	struct symbol *symbol;
  }data;
};

struct st_block{
	struct variable variable;
	int actual_sz;
	int pad_sz;
	int offset;
};

struct st_frame{
  int size;
  int nblocks;
  int word_align;
  int dword_align;
  struct st_block *blocks;
};


void mips_print_program(FILE *output, struct ir_section *section, struct symbol_table *table);
void mips_clear_reg_map();

#endif
