#ifndef _IR_H
#define _IR_H

#include <stdio.h>
#include <stdbool.h>


struct node;
struct symbol;
struct symbol_table;

enum ir_operand_kind {
  OPERAND_NUMBER,
  OPERAND_TEMPORARY,
  OPERAND_IDENTIFIER,
  OPERAND_LABEL
};
struct ir_operand {
  enum ir_operand_kind kind;
  int islvalue;
  union {
    unsigned long number;
    int temporary;
    struct symbol *symbol;
    char label[IDENTIFIER_MAX + 1];
  } data;
};

struct loop_id{
  int startid;
  int endid;
  struct loop_id *prev;
};

enum ir_loop_kind{
  IR_LOOP_NONE,
  IR_LOOP_INCR,
  IR_LOOP_END,
  IR_LOOP_BREAK,
  IR_LOOP_CONTINUE,
  IR_LOOP_DESTROY,
  IR_LOOP_PROCEND,
  IR_LOOP_RETURN,
  IR_LOOP_PROCDESTROY
};

enum ir_instruction_kind {
  IR_ADDR_OF,  
  IR_PROC_BEGIN,
  IR_PROC_END,
  IR_LOAD,
  IR_LOAD_HALF_WD,
  IR_LOAD_UNSIGNEDHALFWD,
  IR_LOAD_BYTE,
  IR_LOAD_UNSIGNEDBYTE,
  IR_STORE,
  IR_STORE_HALF_WD,
  IR_STORE_BYTE,
  IR_CONST,
  IR_ADD,
  IR_SIGNED_SUB,
  IR_UNSIGNED_SUB,
  IR_SIGNED_MULT,
  IR_UNSIGNED_MULT,
  IR_SIGNED_DIV,
  IR_UNSIGNED_DIV,
  IR_SIGNED_REM,
  IR_UNSIGNED_REM,
  IR_LSHIFT,
  IR_RSHIFT,
  IR_UNSIGNED_RSHIFT,  
  IR_LABEL,
  IR_GOTO,
  IR_GOTO_F,
  IR_GOTO_T,
  IR_PARAM,
  IR_CALL,
  IR_SYSCALL,
  IR_EQUAL, 
  IR_NOT_EQUAL,
  IR_LESS,
  IR_UNSIGNED_LESS,
  IR_LESS_EQUAL,
  IR_UNSIGNED_LESS_EQUAL,
  IR_GREATER,
  IR_UNSIGNED_GREATER,
  IR_GREATER_EQUAL,
  IR_UNSIGNED_GREATER_EQUAL,
  IR_BITOP_AND,
  IR_BITOP_OR,
  IR_BITOP_XOR,
  IR_BITOP_NEG,
  IR_NEGATION,

  IR_RESWD,
  IR_RETWD,
  IR_UMINUS,

  IR_CAST_VOID,
  IR_CASTARRAY_POINTER,
  
  IR_CASTUNSIGNEDWORD_BYTE,
  IR_CASTWORD_UNSIGNEDBYTE,
  IR_CASTUNSIGNEDWORD_UNSIGNEDBYTE,
  IR_CASTWORD_BYTE,

  IR_CASTUNSIGNEDBYTE_WORD,
  IR_CASTBYTE_UNSIGNEDWORD,
  IR_CASTUNSIGNEDBYTE_UNSIGNEDWORD,
  IR_CASTBYTE_WORD,

  IR_CASTUNSIGNEDHALFWD_WORD,
  IR_CASTHALFWD_UNSIGNEDWORD,
  IR_CASTUNSIGNEDHALFWD_UNSIGNEDWORD,
  IR_CASTHALFWD_WORD,

  IR_CASTUNSIGNEDBYTE_HALFWD,
  IR_CASTBYTE_UNSIGNEDHALFWD,
  IR_CASTUNSIGNEDBYTE_UNSIGNEDHALFWD,
  IR_CASTBYTE_HALFWD,

  IR_CASTUNSIGNEDWORD_HALFWD,
  IR_CASTWORD_UNSIGNEDHALFWD,
  IR_CASTUNSIGNEDWORD_UNSIGNEDHALFWD,
  IR_CASTWORD_HALFWD,

  IR_CASTUNSIGNEDHALFWD_BYTE,
  IR_CASTHALFWD_UNSIGNEDBYTE,
  IR_CASTUNSIGNEDHALFWD_UNSIGNEDBYTE,
  IR_CASTHALFWD_BYTE,

  IR_CASTWORD_UNSIGNEDWORD,
  IR_CASTUNSIGNEDWORD_WORD,
  IR_CASTHALFWD_UNSIGNEDHALFWD,
  IR_CASTUNSIGNEDHALFWD_HALFWD,
  IR_CASTBYTE_UNSIGNEDBYTE,
  IR_CASTUNSIGNEDBYTE_BYTE,


  IR_LOAD_OPTZ,
  IR_LOAD_HALF_WD_OPTZ,
  IR_LOAD_UNSIGNEDHALFWD_OPTZ,
  IR_LOAD_BYTE_OPTZ,
  IR_LOAD_UNSIGNEDBYTE_OPTZ,

  IR_STORE_OPTZ,
  IR_STORE_HALF_WD_OPTZ,
  IR_STORE_BYTE_OPTZ,

  IR_ADD_IMM,
  IR_SIGNED_SUB_IMM,
  IR_UNSIGNED_SUB_IMM,
  IR_SIGNED_MULT_IMM,
  IR_UNSIGNED_MULT_IMM,
  IR_SIGNED_DIV_IMM,
  IR_UNSIGNED_DIV_IMM,
  IR_SIGNED_REM_IMM,
  IR_UNSIGNED_REM_IMM,

  IR_NO_OPERATION,  
  IR_COPY,
  IR_DIVIDE,
  IR_SUBTRACT,
  IR_MULTIPLY,
  IR_PRINT_NUMBER,
  IR_LOAD_IMMEDIATE
};

struct operand_list{
  struct ir_operand *operand;
  struct operand_list *next;
};

struct reg_usage{
  int nregister; 
  int arr_items;
  int arr_size;
  int *arr_temp;
};

struct edge{
  struct ir_operand *operand;
};
struct vertex{
  struct ir_operand *vertex;
  struct edge *edges;
  int    nedges;
  int    tot_size;
};

struct ir_instruction {
  enum   ir_instruction_kind kind;
  struct ir_instruction *prev, *next;
  struct ir_operand operands[3];
  int    reset_reg;
  int    isdead;
  struct ir_block *block;
  struct operand_list *livelist;
};

struct ir_section {
  struct ir_instruction *first, *last;
};

enum ir_block_state{
  IR_BLOCK_NONE,
  IR_BLOCK_BEGIN,
  IR_BLOCK_INBLOCK,
  IR_BLOCK_END
};

struct ir_link{
  struct ir_block *block;
  struct ir_link *next;
};

struct ir_block{
  char   name[IDENTIFIER_MAX];
  struct ir_section *section;

  struct ir_link *to_links;
  struct ir_link *from_links;
  struct operand_list *livelist;
  struct vertex *vertices;
  struct reg_usage *rmap;
  int    nvertices;
  int    niteration;
};


struct ir_section *ir_block_section(struct ir_instruction *first, struct ir_instruction *last);
struct ir_section *ir_block_append(struct ir_section *section, struct ir_instruction *ir_print_instruction);

int ir_generate_for_statement_list(struct node *statement_list);
void ir_generate_for_expression(struct node *node);
void ir_print_section(FILE *output, struct ir_section *section);
void ir_generate_for_ast(struct node *node);
void ir_optimize(struct ir_section *section);
void ir_generate_live_analysis(struct ir_section *ir);

#endif
