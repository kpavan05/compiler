#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "node.h"
#include "compiler.h"
#include "symbol.h"
#include "type.h"
#include "mips.h"
#include "ir.h"

#define SMALL_ARR_SIZE 4
#define MED_ARR_SIZE   16
#define LARGE_ARR_SIZE 32
#define MAX_ITERATION  12
/***********************
 * ANALYSIS  *
 ***********************/


static struct ir_block *top_block =NULL;
static struct ir_block *end_block = NULL;

/*static void ir_do_analysis(struct ir_block *block);
static void ir_do_analysis_block(struct ir_block *block);*/
static void ir_add_operand(struct ir_instruction *instruction, struct ir_operand *operand);
static void ir_remove_operand(struct ir_instruction *instruction, struct ir_operand *operand);
static struct operand_list *ir_copy_livelist(struct ir_instruction *instruction, struct operand_list *original);
static void ir_generate_live_list(struct ir_instruction *instruction);
static int ir_temporary_count(int iset);

static void ir_update_tolink(struct ir_block *to, struct ir_block *from);
static struct ir_block *ir_basic_block(struct ir_instruction *instruction, struct ir_block *from);
static void ir_generate_blocks(struct ir_instruction *instruction, struct ir_block *curblock, enum ir_block_state state);
static int ir_search_fwd_label(struct ir_instruction *instruction, struct ir_operand *operand, struct ir_block *parent);
static int ir_search_back_label(struct ir_instruction *instruction, struct ir_operand *operand, struct ir_block *parent);
static int ir_check_label_link(struct ir_instruction *instruction);
static void ir_generate_interference_graph(struct ir_block *block);
static void ir_allocate_registers(struct ir_block *block);

static void ir_process_queue();
static void ir_process_block(struct ir_block *block);
static struct operand_list *ir_list(struct ir_block *block);
static struct operand_list *ir_append_list(struct operand_list *list1, struct operand_list *list2);

struct queue{
  struct ir_block *block;
  struct operand_list *list;
  int tot_items;
  int tot_size;
};

struct queue *m_queue = NULL;

void popQueue(){
  int i;
  if (!m_queue) return;
  for(i = 0; i < m_queue->tot_items -1; i++){
    m_queue[i].block = m_queue[i+1].block;
  }
  m_queue[m_queue->tot_items-1].block = NULL;
  m_queue->tot_items--;
}

void pushQueue(struct ir_block *block){
  int nsize;
  if (!m_queue) return;
  if (m_queue->tot_items ==m_queue->tot_size){
    nsize = m_queue ? 2*m_queue->tot_items : SMALL_ARR_SIZE;
    m_queue = realloc(m_queue, nsize*sizeof(struct queue));
    assert(m_queue != NULL);
    m_queue->tot_size = nsize; 
  }

  m_queue[m_queue->tot_items].block = block;
  m_queue->tot_items++;
}

void ir_generate_live_analysis(struct ir_section *ir){ 
  struct ir_instruction *iter = NULL;
  if (ir == NULL) return;
  
  ir_generate_blocks(ir->first, NULL, IR_BLOCK_NONE);
  
  m_queue = malloc(SMALL_ARR_SIZE*sizeof(struct queue));  
  m_queue->tot_size = SMALL_ARR_SIZE;
  m_queue->tot_items = 0;

  m_queue[0].block = end_block;
  m_queue->tot_items++;
  ir_process_queue(m_queue);
  
  iter = ir->first;
  while (NULL != iter) {
    if (iter->block != NULL){
      ir_generate_interference_graph(iter->block);
      ir_allocate_registers(iter->block);
    } 
    iter = iter->next;
  }
  
}


static void ir_process_queue(){
  struct ir_link *link = NULL;

  while(m_queue->tot_items > 0){ 
    ir_process_block(m_queue[0].block); 
    m_queue[0].block->niteration++;  
    link = m_queue[0].block->from_links;
    while(link){
      /* 
      This is bit shortcut because of time constraint
      if a loop is encountered then i cap the number of iterations
      instead of checking the change in list of live temporaries
      This mostly work since i set the number of iterations
      */
      if (link->block->niteration < 8)
      pushQueue(link->block);
      link = link->next;
    }
    popQueue();
  }
}



static struct operand_list *ir_copy_livelist(struct ir_instruction *instruction, struct operand_list *list){
  struct operand_list *original = list;
  struct operand_list *curitem = NULL;
  struct operand_list *previtem = NULL;
  
  if (original == NULL) return NULL; 
  instruction->livelist = malloc(sizeof(struct operand_list));
  assert(instruction->livelist != NULL);
  instruction->livelist->next = NULL;
  curitem = instruction->livelist;

  while(original){  
    curitem->operand = original->operand;
    original = original->next;
    previtem = curitem;
    if (original){
      curitem =  malloc(sizeof(struct operand_list));
      curitem->next = NULL;
      previtem->next = curitem;
    }
  }
  return instruction->livelist;
}

static struct operand_list *ir_append_list(struct operand_list *list1, struct operand_list *list2){
  struct operand_list *curlist = list1;
  struct operand_list *newlist = NULL;
  

  if (list1 == NULL && list2 == NULL) return NULL;
  if (list1 != NULL && list2 == NULL) return list1;
  if (list1 == NULL && list2 != NULL){
    newlist = malloc(sizeof(struct operand_list));
    assert(newlist != NULL);
    curlist = newlist;
    curlist->operand = list2->operand;
    curlist->next = list2->next;   
  }
  else{
    struct operand_list *prev = NULL;
    struct operand_list *next = NULL;
    prev = curlist;
    next = curlist;
    while(next){ prev = next; next = next->next;}
    if (prev) prev->next = list2;
  }
  return curlist;
}

static struct operand_list *ir_list(struct ir_block *block){
  struct ir_link *link = NULL;
  struct operand_list *list = NULL;

  link = block->to_links;
  while(link){
    list = ir_append_list(list, link->block->livelist);
    link = link->next;
  }
  return list;
}

static void ir_process_block(struct ir_block *block){
  struct ir_section *section = block->section;
  struct ir_instruction *iter = section->last;
  struct operand_list *list = ir_list(block);

  while (NULL != iter ) { 
    ir_copy_livelist(iter, list);  
    ir_generate_live_list(iter);
    list = iter->livelist;
    iter = iter->prev;
    block->livelist = list;
  }
}

static struct ir_block *ir_basic_block(struct ir_instruction *instruction, struct ir_block *from){
  struct ir_block *block = NULL;
  static int blocknum;

  if (instruction->block != NULL){
    struct ir_link *curlink = NULL;
    struct ir_link *prevlink = NULL;
    struct ir_link *newlink = NULL;
    bool bfound = false;
    curlink = instruction->block->from_links;
    /*find if the link exists*/
    while (curlink){
      if (curlink->block == from){
        bfound = true;
        break;
      } 
      curlink = curlink->next;
    }
    /* if not found add the from link if the block has multiple input paths*/
    if (!bfound && from != NULL){ 
      newlink = malloc(sizeof(struct ir_link));
      assert(newlink != NULL);
      newlink->block = from;
      newlink->next = NULL;
      curlink = instruction->block->from_links;

      while (curlink){   
        prevlink = curlink;
        curlink = curlink->next;
      }
      if (prevlink) prevlink->next = newlink;
      else instruction->block->from_links = newlink;
      /* add the to_link for from block*/
      ir_update_tolink(instruction->block, from);
    }
    return instruction->block;
  }
  else{
    block = malloc(sizeof(struct ir_block));
    assert(block != NULL);
    sprintf(block->name, "%s_%d","BasicBlock__",blocknum++);
    block->section = ir_block_section(instruction, instruction);
    block->from_links = NULL;
    block->to_links = NULL;
    block->vertices = NULL;
    block->livelist = NULL;
    block->rmap = NULL;
    block->niteration = 0;
    block->nvertices = 0;

    instruction->block = block;
    if (from != NULL){
      /* add from link to created block*/
      block->from_links = malloc(sizeof(struct ir_link));
      assert(block->from_links != NULL);
      block->from_links->block = from;
      block->from_links->next = NULL;
      /* add the to_link for from block*/
      ir_update_tolink(instruction->block, from);
    }
  
  }
  return block;
}

static void ir_update_tolink(struct ir_block *to, struct ir_block *from){
  struct ir_link *curlink = NULL;
  struct ir_link *prevlink = NULL;
  struct ir_link *newlink = NULL;
  bool bfound= false;
  if (from == NULL || to == NULL) return;

  curlink = from->to_links;
    /*find if the link exists*/
  while (curlink){
    if (curlink->block == to){
      bfound = true;
      break;
    } 
    curlink = curlink->next;
  }
  /* if not found add the to link if the block has multiple input paths*/
  if (!bfound){ 
    newlink = malloc(sizeof(struct ir_link));
    assert(newlink != NULL);
    newlink->block = to;
    newlink->next = NULL;

    curlink = from->to_links;
    while (curlink){   
      prevlink = curlink;
      curlink = curlink->next;
    }
    if (prevlink) prevlink->next = newlink;
    else from->to_links = newlink;
  }

}

static int ir_search_fwd_label(struct ir_instruction *instruction, struct ir_operand *operand, struct ir_block *parent){
  struct ir_instruction *iter = instruction;
  if (operand == NULL || instruction == NULL) return 0;
  while (NULL != iter) {   
    if (iter->kind == IR_LABEL && strcmp(iter->operands[0].data.label, operand->data.label)== 0){
        ir_basic_block(iter, parent);
        return 1;
    }
    iter = iter->next;
  }
  return 0;
}

static int ir_search_back_label(struct ir_instruction *instruction, struct ir_operand *operand, struct ir_block *parent){
  struct ir_instruction *iter = instruction;
  if (operand == NULL || instruction == NULL) return 0;
  while (NULL != iter) {   
    if (iter->kind == IR_LABEL && strcmp(iter->operands[0].data.label, operand->data.label)== 0){
        ir_basic_block(iter, parent);
        return 1;
    }
    iter = iter->prev;
  }
  return 0;
}

static int ir_check_label_link(struct ir_instruction *instruction){
  struct ir_instruction *gotoInstr = NULL;
  if (instruction->block == NULL || instruction->kind != IR_LABEL) 
    return 0;

  gotoInstr = instruction->prev; 
  if(gotoInstr->kind == IR_GOTO &&
    strcmp(instruction->operands[0].data.label, gotoInstr->operands[0].data.label) != 0){
    return 1;
  }
  return 0;
}

static void ir_generate_blocks(struct ir_instruction *instruction, struct ir_block *curblock, enum ir_block_state state){
  struct ir_block *newblock =curblock;
  struct ir_instruction *curinstruction = instruction;
  if (instruction == NULL) return;

  switch(instruction->kind){
    case IR_PROC_BEGIN:
      state = IR_BLOCK_BEGIN;
      newblock = ir_basic_block(instruction, curblock);   
      top_block = newblock;   
      break;
    case IR_GOTO_F:
    case IR_GOTO_T:
      state = IR_BLOCK_END;
      curblock->section = ir_block_append(curblock->section, instruction);
      if (ir_search_fwd_label(instruction, &instruction->operands[1], curblock) == 0){
        ir_search_back_label(instruction, &instruction->operands[1], curblock);
      }  
      break;
    case IR_GOTO:
      if (state == IR_BLOCK_END){
        state = IR_BLOCK_BEGIN;
        newblock = ir_basic_block(instruction, curblock);
        state = IR_BLOCK_END;
        newblock->section = ir_block_append(curblock->section, instruction);
        if (ir_search_fwd_label(instruction, &instruction->operands[0], newblock) == 0){
          ir_search_back_label(instruction, &instruction->operands[0], newblock);
        } 
      }
      else{
        state = IR_BLOCK_END;
        curblock->section = ir_block_append(curblock->section, instruction);
        if (ir_search_fwd_label(instruction, &instruction->operands[0], curblock) == 0){
          ir_search_back_label(instruction, &instruction->operands[0], curblock);
        } 
      } 
      break;
    case IR_LABEL:
      state = IR_BLOCK_BEGIN;
      if (ir_check_label_link(instruction) == 1){
        newblock = instruction->block;
      }
      else if(instruction->prev->kind == IR_GOTO){
        newblock = ir_basic_block(instruction, NULL);
      }
      else{
        newblock = ir_basic_block(instruction, curblock);  
      }   
      break;
    case IR_PROC_END:
      state = IR_BLOCK_END;
      curblock->section = ir_block_append(curblock->section, instruction);
      end_block = curblock;
      break; 
    default:
      if (state == IR_BLOCK_END){
        state = IR_BLOCK_BEGIN;
        newblock = ir_basic_block(instruction, curblock);
      }
      else{
       state = IR_BLOCK_INBLOCK;
       curblock->section = ir_block_append(curblock->section, instruction);
      }
      break;
  }
  ir_generate_blocks(curinstruction->next, newblock, state);
}


static void ir_add_operand(struct ir_instruction *instruction, struct ir_operand *operand){
  struct operand_list *list = instruction->livelist;
  struct operand_list *item = NULL;
  int bfound = 0;
  if (operand->kind != OPERAND_TEMPORARY) return;
  while (list){
    if (list->operand->data.temporary == operand->data.temporary) {
      bfound = 1;
      break;
    }
    list = list->next;
  }
  if (!bfound){
    item =  malloc(sizeof(struct operand_list));
    item->operand = operand;
    item->next = instruction->livelist;
    instruction->livelist = item;
    ir_temporary_count(!bfound);
  }
}

static void ir_remove_operand(struct ir_instruction *instruction, struct ir_operand *operand){
  struct operand_list *item = instruction->livelist;
  struct operand_list *prev = NULL;
  if (operand->kind != OPERAND_TEMPORARY) return;
  while (item){
    if (item->operand->data.temporary == operand->data.temporary) {
      if (prev == NULL)
        instruction->livelist = item->next;
      else
        prev->next = item->next;

      free(item); item = NULL;
      break;
    }
    prev = item;
    item = item->next;
  }
}
static int ir_temporary_count(int iset){
  static int tcount;
  if (iset == 1) ++tcount;
  return tcount;
}

static void ir_generate_live_list(struct ir_instruction *instruction){
  if(instruction == NULL) return;

  switch (instruction->kind) {
    
    case IR_LESS:
    case IR_EQUAL:
    case IR_GREATER:
    case IR_NOT_EQUAL:
    case IR_LESS_EQUAL:
    case IR_UNSIGNED_LESS:
    case IR_GREATER_EQUAL:
    case IR_UNSIGNED_GREATER:
    case IR_UNSIGNED_LESS_EQUAL:
    case IR_UNSIGNED_GREATER_EQUAL:
    case IR_ADD:
    case IR_LSHIFT:
    case IR_RSHIFT:
    case IR_BITOP_OR:
    case IR_BITOP_AND:
    case IR_BITOP_XOR:
    case IR_SIGNED_SUB:
    case IR_SIGNED_DIV:
    case IR_SIGNED_REM:
    case IR_SIGNED_MULT:
    case IR_UNSIGNED_REM:
    case IR_UNSIGNED_DIV:
    case IR_UNSIGNED_MULT:
    case IR_UNSIGNED_RSHIFT:    
      ir_add_operand(instruction, &instruction->operands[1]);
      ir_add_operand(instruction, &instruction->operands[2]);
      ir_remove_operand(instruction, &instruction->operands[0]); 
      break;
   
    case IR_LOAD:
    case IR_LOAD_BYTE:
    case IR_LOAD_HALF_WD: 
    case IR_LOAD_UNSIGNEDBYTE:
    case IR_LOAD_UNSIGNEDHALFWD:
      ir_add_operand(instruction, &instruction->operands[1]);
      ir_remove_operand(instruction, &instruction->operands[0]);
      break;
    case IR_PARAM:
      break;
    case IR_CONST:
      ir_remove_operand(instruction, &instruction->operands[0]);
      break;   
    case IR_STORE:
    case IR_STORE_BYTE:
    case IR_STORE_HALF_WD:
      ir_add_operand(instruction, &instruction->operands[1]);
      ir_add_operand(instruction, &instruction->operands[0]);
      break;
    case IR_GOTO_T:
    case IR_GOTO_F: 
      break;  
    case IR_ADDR_OF:
      ir_remove_operand(instruction, &instruction->operands[0]);
      break;
    case IR_UMINUS:
    case IR_BITOP_NEG:
    case IR_NEGATION:
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
      ir_add_operand(instruction, &instruction->operands[1]);
      ir_remove_operand(instruction, &instruction->operands[0]);  
      break;
    case IR_CALL:
    case IR_GOTO:
    case IR_LABEL:
      break;
    case IR_RESWD:
    case IR_RETWD:
      ir_add_operand(instruction, &instruction->operands[0]);
      break;
    case IR_SYSCALL:
    case IR_PROC_END:
    case IR_PROC_BEGIN:      
      break;


    case IR_LOAD_IMMEDIATE:
    case IR_COPY:
      break;
    case IR_PRINT_NUMBER:
      break;
    case IR_NO_OPERATION:
      break;
    default:
      assert(0);
      break;
  }

}



static int ir_is_vertex_created(struct ir_block *block, struct ir_operand *operand){
  int i;
  int nvertices = block->nvertices;
  for(i = 0; i < nvertices; i++){
    if (block->vertices[i].vertex->data.temporary == operand->data.temporary)
      return i;
  }  
  return -1;
}

static int compare_vertices(const void *p1, const void *p2)
{
  struct vertex *v1 = (struct vertex *)p1;
  struct vertex *v2 = (struct vertex *)p2;
  if (v1->vertex->data.temporary < v2->vertex->data.temporary)
    return -1;
  else
    return 1;
}

static int compare_edges(const void *p1, const void *p2)
{
  struct vertex *v1 = (struct vertex *)p1;
  struct vertex *v2 = (struct vertex *)p2;
  if (v1->nedges < v2->nedges)
    return -1;
  else
    return 1;
}

static int ir_vertex_has_edge(struct vertex v1, struct vertex v2){
  int k;
  for (k = 0 ;  k < v1.nedges; k++){
    if (v1.edges[k].operand->data.temporary == v2.vertex->data.temporary)
      return 1;
  }
  return 0;
}


static int ir_register_taken(struct ir_operand *operand, struct reg_usage *map_item){
  int i;
  for (i = 0; i < map_item->arr_items; i++){
    if (operand->data.temporary == map_item->arr_temp[i]) return 1;
  }
  return 0;
}

static void ir_find_register(struct vertex v, struct reg_usage *map){
  int ireg, j;
  int nfound = 0;
  /*loop through all the registers that are assigned temporaries*/
  for(ireg = 0; ireg < N_USEREGISTERS; ireg++){
    if (map[ireg].nregister == -1){
      nfound = 0; break;
    }
    /*loop through vertex edges and see if any edge has already taken the register*/
    for (j = 0; j < v.nedges ; j++){
      if (ir_register_taken(v.edges[j].operand, &map[ireg]) == 1){
        nfound = 0; break;    
      }
      nfound = 1;
    }
    /*if register is not taken then use this register*/
    if (nfound == 1) break;
  }

  /* if not found create a new array and add the temporary*/
  if (nfound == 0 && ireg < N_USEREGISTERS){
    map[ireg].arr_temp = malloc(SMALL_ARR_SIZE * sizeof(int));
    assert(map[ireg].arr_temp != NULL);
    map[ireg].nregister =  ireg;
    map[ireg].arr_size = SMALL_ARR_SIZE;
    map[ireg].arr_temp[map[ireg].arr_items] = v.vertex->data.temporary;
    map[ireg].arr_items++;
  }

  /* if found add the temporary to existing register */
  if (nfound == 1){
    if (map[ireg].arr_items == map[ireg].arr_size){
      map[ireg].arr_temp = realloc(map[ireg].arr_temp, 2*map[ireg].arr_items*SMALL_ARR_SIZE);
      map[ireg].arr_size = 2*map[ireg].arr_items;
    }
    map[ireg].arr_temp[map[ireg].arr_items] = v.vertex->data.temporary;
    map[ireg].arr_items++;
  }
}

static void ir_allocate_registers(struct ir_block *block){
  int i;
  struct reg_usage *rmap = NULL;
  int nvertex = block->nvertices;
  
  rmap = malloc(N_USEREGISTERS * sizeof(struct reg_usage));
  /*initialize register temporary map */
  for (i = 0; i < N_USEREGISTERS ; i++){
    rmap[i].nregister = -1;
    rmap[i].arr_items = 0;
    rmap[i].arr_size = 0;
    rmap[i].arr_temp = NULL;
  }

  /*allocate register to temporaries */
  for(i = nvertex - 1; i >= 0; i--){
    /*find the relevant register that can be allocated to vertex */
    ir_find_register(block->vertices[i], rmap);
  }
  block->rmap = rmap;
}

static void ir_create_graph_edges(struct ir_block *block, struct operand_list *list, struct ir_operand *operand){
  int vindex = -1;
  int eindex = 0;
  static int vmultiplier = 1;
  if (list == NULL || operand == NULL) return;


  vindex = ir_is_vertex_created(block, operand);
  if (vindex == -1){
    vindex = block->nvertices;
    if (block->nvertices == vmultiplier*LARGE_ARR_SIZE){
      vmultiplier = 2*vmultiplier;
      block->vertices = realloc(block->vertices, vmultiplier*LARGE_ARR_SIZE*sizeof(struct vertex));
    }
    block->vertices[vindex].vertex = operand;
    block->vertices[vindex].edges  = NULL;
    block->vertices[vindex].nedges = 0;
    ++block->nvertices;
  }

  while (list){
    if (list->operand->data.temporary != operand->data.temporary) {
      if (block->vertices[vindex].edges == NULL){
        block->vertices[vindex].edges = malloc(MED_ARR_SIZE * sizeof(struct edge));
        assert(block->vertices[vindex].edges != NULL);
        block->vertices[vindex].nedges = 0;
        block->vertices[vindex].tot_size = MED_ARR_SIZE;
        block->vertices[vindex].edges[0].operand = list->operand;
        ++block->vertices[vindex].nedges;
      }
      else{
        if (block->vertices[vindex].nedges == block->vertices[vindex].tot_size){
          block->vertices[vindex].tot_size = 2*block->vertices[vindex].tot_size;
          block->vertices[vindex].edges = realloc(block->vertices[vindex].edges, block->vertices[vindex].tot_size*sizeof(struct edge));
        }
        eindex =block->vertices[vindex].nedges;
        block->vertices[vindex].edges[eindex].operand = list->operand;
        ++block->vertices[vindex].nedges;
      }
    }
    list = list->next;
  }
 
}

/* 
 if one vertex has an edge then other vertex should also list the vertex in its edge list.
 listed in on with two way edges. complete the bidirectionality of graph. 
*/
static void ir_create_connection(struct ir_block *block){
  int nvertices = block->nvertices;
  int i, j, k;
  for(i = 0; i < nvertices; i++){
    for( j = 0 ; j < nvertices ; j++){   
      if (j == i) continue; 
      for (k = 0 ;  k < block->vertices[j].nedges; k++){        
        if (block->vertices[i].vertex->data.temporary == block->vertices[j].edges[k].operand->data.temporary &&
          ir_vertex_has_edge( block->vertices[i], block->vertices[j]) == 0){

          if (block->vertices[i].edges == NULL){
            block->vertices[i].edges = malloc(MED_ARR_SIZE * sizeof(struct edge));
            assert(block->vertices[i].edges != NULL);
            block->vertices[i].nedges = 0;
            block->vertices[i].tot_size = MED_ARR_SIZE;
          }
          else if (block->vertices[i].nedges == block->vertices[i].tot_size){
            block->vertices[i].tot_size = 2*block->vertices[i].tot_size;
            block->vertices[i].edges = realloc(block->vertices[i].edges, block->vertices[i].tot_size*sizeof(struct edge));
          }
          block->vertices[i].edges[block->vertices[i].nedges].operand = block->vertices[j].vertex;
          ++block->vertices[i].nedges;
        }
      }
      
    }
    qsort(block->vertices[i].edges, block->vertices[i].nedges, sizeof(struct edge), compare_vertices);
  }  
}

static void ir_generate_interference_graph(struct ir_block *block){
  struct ir_section *section = block->section;
  struct ir_instruction *iter = section->first;
  struct operand_list *list = NULL;

  block->nvertices = 0;
  block->vertices = malloc(LARGE_ARR_SIZE*sizeof(struct vertex));

  while(iter){ 
    if (iter->operands[0].kind == OPERAND_TEMPORARY){
      list = iter->next->livelist;
      ir_create_graph_edges(block, list, &iter->operands[0]);
    }
    iter = iter->next;
  }
  ir_create_connection(block);
  qsort(block->vertices, block->nvertices, sizeof(struct vertex), compare_edges);
}


