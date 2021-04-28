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
		case NODE_SIGNED:
		case NODE_SIGNED_INT: 		
			 if (signedint == NULL) signedint = type_basic(false, TYPE_BASIC_INT);
			 return signedint;
		case NODE_SIGNED_CHAR : 
			 if (signedchar == NULL) signedchar = type_basic(false, TYPE_BASIC_CHAR);
			 return signedchar;
		case NODE_SIGNED_LONG : 
		case NODE_SIGNED_LONG_INT:
			 if (signedlong == NULL) signedlong = type_basic(false, TYPE_BASIC_LONG);
			 return signedlong;
		case NODE_SIGNED_SHORT:
		case NODE_SIGNED_SHORT_INT:
			 if (signedshort == NULL) signedshort = type_basic(false, TYPE_BASIC_SHORT); 
			 return signedshort;
		case NODE_UNSIGNED:
		case NODE_UNSIGNED_INT : 
			 if (unsignedint == NULL) unsignedint = type_basic(true, TYPE_BASIC_INT);
			 return unsignedint;
		case NODE_UNSIGNED_CHAR : 
			 if (unsignedchar == NULL) unsignedchar = type_basic(true, TYPE_BASIC_CHAR);
			 return unsignedchar;
		case NODE_UNSIGNED_LONG :
		case NODE_UNSIGNED_LONG_INT:
			 if (unsignedlong == NULL) unsignedlong = type_basic(true, TYPE_BASIC_LONG); 
			 return unsignedlong;
		case NODE_UNSIGNED_SHORT :
		case NODE_UNSIGNED_SHORT_INT:
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
  	struct node *curnode = NULL;
  	struct node *newnode = NULL;

 struct node *idnode = NULL;
	if (node == NULL) return NULL;

	sublist = type_list_create(node->data.binary.left_operand, head, bdefine);  
	assert(node->data.binary.right_operand->kind == NODE_PARAM);
	
	base = type_basic_create(node->data.binary.right_operand->data.binary.left_operand);
	assert (base != NULL);
	
  	curnode = node->data.binary.right_operand->data.binary.right_operand;
  	if (curnode && curnode->kind == NODE_FUNC_DECL){
  		compiler_print_error(curnode->location, "function parameter cannot be function type");
  		return sublist;
  	}	
  	if (base->kind == TYPE_BASIC && curnode){
  		newnode = symbol_convert_to_pointer_decl(curnode);
  		decltype = curnode->parent == NULL ? type_declarator(newnode, base, &inode) : type_declarator(curnode, base, &inode);
    if (bdefine) idnode = curnode->parent == NULL ? type_get_id_node(newnode ) : type_get_id_node(curnode);
  	}
  	else{
		  decltype = type_declarator(curnode, base, &inode);
    if (bdefine) idnode = type_get_id_node(curnode);
   }

	if (decltype == NULL) return sublist;
	
	if (type_verify_incomplete_array(decltype, true)){
		compiler_print_error(node->location, "incomplete array type is used");
		return sublist;
	}
	if (bdefine && inode == NULL){
		compiler_print_error(node->location, "abstract declarator is not allowed in function definition");
		return sublist;
	}

	if (sublist == NULL) {
		head->curtype = decltype;

		head->id = (bdefine && idnode) ? idnode : NULL;
		curlist = head;
	}
	else {
		curlist = malloc(sizeof(struct typelist));
		assert(curlist != NULL);
		curlist->next = NULL;
		curlist->curtype = decltype;      
		curlist->id = (bdefine && idnode) ? inode : NULL;
		sublist->next = curlist;
	}
	return curlist;

}

static int isvoid(struct type *t){
	 if (t == NULL) return 1;

	 switch (t->kind){
		 case TYPE_POINTER:
			 t = t->data.ptrtype.target;
			 return isvoid(t);
		 case TYPE_ARRAY:
			 t = t->data.arraytype.target;
			 return isvoid(t);
		 case TYPE_BASIC:
			 return 1;
		 case TYPE_VOID:
			 return 0;
		 default:
			 return 1;
	 }   
}

struct node *type_get_id_node(struct node *node){
  struct node *idnode = NULL;
  switch(node->kind){
   case NODE_ARRAY_DECL:
   case NODE_ABSTR_DECL:
    idnode = type_get_id_node(node->data.binary.left_operand);
   break;
   case NODE_POINTER_DECL :
    idnode = type_get_id_node(node->data.binary.right_operand);
   break;
   case NODE_IDENTIFIER:
    idnode = node;
   break;
  default :
   assert(0);
   break;
 }
 return idnode;
} 
struct type *type_declarator(struct node *node, struct type *base, struct node **idnode){
	struct type *curtype = NULL;
	struct type *rettype = NULL;
	if (node == NULL) return base;
	
	switch(node->kind){
	 case NODE_ARRAY_DECL:
	 case NODE_ABSTR_DECL:
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
		 *idnode = node;
		 if (isvoid(base) == 0){
			 compiler_print_error(node->location, "void type for  %s is not allowed", node->data.identifier.name);
			 return NULL;
		 }
		 rettype = base;     
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
	rettype = type_pointer_decl_create(node->data.unary.child_operand);
	
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

/*
 return 1 - if not matching
        0 - if matching
*/
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
						 if (list1 != NULL && list2 == NULL) return 1;
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
			case TYPE_ARRAY:
				if (left->data.arraytype.arrsize != right->data.arraytype.arrsize) return false;
				return type_is_equal(left->data.arraytype.target, right->data.arraytype.target);
			case TYPE_POINTER:
				return type_is_equal(left->data.ptrtype.target, right->data.ptrtype.target);
			case TYPE_FUNCTION:
				if (!type_is_equal(left->data.functype.rvaltype, right->data.functype.rvaltype)) return false;
				else
				{
					struct typelist* param1 = left->data.functype.params;
					struct typelist* param2 = right->data.functype.params;
					while (true){
						if (param1 == NULL && param2 == NULL) return true;
						if (param1 == NULL && param2 != NULL) return false;
						if (param1 != NULL && param2 == NULL) return false;          
						if (!type_is_equal(param1->curtype , param2->curtype)) return false;
						param1 = param1->next;
						param2 = param2->next;         
					}
				}
			case TYPE_VOID:
				return true;
			default:
				assert(0);
				break;
		}
	} else {
		return false;
	}
}

static bool type_is_compatible(struct type *left, struct type *right) {
	bool iscompatible = false;
	if (left != NULL && right == NULL) return false;
	if (left == NULL && right != NULL) return false;
	if (left == NULL &&  right == NULL) return true;
 	if (type_is_equal(left, right)) return true;

 	if (left->kind == TYPE_POINTER && right->kind == TYPE_POINTER)
	 iscompatible = (left->data.ptrtype.target->kind == TYPE_VOID || right->data.ptrtype.target->kind == TYPE_VOID) ? true : false;    
 
 return iscompatible; 
}

bool type_is_arithmetic(struct type *t) {
	if (t == NULL) return false;
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
		case TYPE_ARRAY:
		  return t->data.arraytype.arrsize;
		default:
			return 0;
	}
}

int type_calc_size(struct type *t){
  struct type *target = NULL;
  if (t == NULL) return 0;
  switch (t->kind) {
	case TYPE_POINTER:
   		target = t->data.ptrtype.target;
   		if (target->kind == TYPE_POINTER || target->kind == TYPE_BASIC)
			return type_size(target);
   		else if (target->kind == TYPE_ARRAY)
     		return type_calc_size(target);
	case TYPE_ARRAY:
   		target = t->data.arraytype.target;
   		if (target->kind == TYPE_ARRAY)
     		return t->data.arraytype.arrsize * type_calc_size(target);
   		else
			return t->data.arraytype.arrsize * type_size(target);
	default:
			return 1;
	}
}

int type_get_alignment_size(struct type *t, int size, int *padding, int multiplier){
	int varsize =0;
	switch (t->kind) {
      case TYPE_BASIC:
        switch (t->data.basic.datatype) {
          case TYPE_BASIC_CHAR:
            *padding = 0;
            varsize = 1*multiplier;
            break;
          case TYPE_BASIC_SHORT:
            *padding = ((size + 1)/2)*2 - size;
            varsize  =  multiplier*2;
            break;
          case TYPE_BASIC_INT:
            *padding = ((size + 3)/4)*4 -size;
            varsize =  multiplier*4;
            break;
          case TYPE_BASIC_LONG:
          	*padding = ((size + 3)/4)*4 - size;
            varsize =  multiplier*4;
            break;
          default:
            assert(0);
            break;
        }
        break;
      case TYPE_POINTER:      	
	      *padding = ((size + 3)/4)*4 -size;
	       varsize =  multiplier*4;
        break;
      case TYPE_ARRAY:
        { 
        	int factor;
        	struct type *target = NULL;
        	factor = t->data.arraytype.arrsize;
        	target = t->data.arraytype.target;
        	while(target->kind == TYPE_ARRAY){
        		factor *= target->data.arraytype.arrsize;
        		target = target->data.arraytype.target;
        	}
        	varsize = type_get_alignment_size(target, size, padding, factor);
        } 
        break;     	
      default:
        return varsize;
    }
    return varsize;
}

int type_rank(struct type *t) {
	if (!type_is_arithmetic(t)) return 0;

	switch (t->data.basic.datatype) {
		case TYPE_BASIC_CHAR:
			return 20;
		case TYPE_BASIC_SHORT:
			return 30;
		case TYPE_BASIC_INT:
			return 40;
		case TYPE_BASIC_LONG:
			return 50;
		default:
			assert(0);
			break;
	}
	 
}

void type_create_cast_node(struct node* curnode, struct type* convtype){
	struct node* cnode = NULL;
  struct node* child = curnode;
	struct node* parent = curnode->parent;
  struct result* res =  node_get_result(child);

  while(parent && parent->kind == NODE_TYPE_CAST){
   child = parent;
   parent = parent->parent;
  }
	
	cnode = node_one_operand(NODE_TYPE_CAST, "type_cast", child, curnode->location);
	cnode->data.unary.result.type = convtype;
	cnode->data.unary.result.islvalue = 0;
	cnode->parent = parent;
  if (res && res->type->kind == TYPE_ARRAY)
     cnode->data.unary.isspecial = 1;

  if (!parent) return;

	switch (parent->ntype){
		case NODE_UNARY:
			parent->data.unary.child_operand = cnode;
			break;
		case NODE_BINARY:
			parent->data.binary.left_operand = parent->data.binary.left_operand == child ? cnode : parent->data.binary.left_operand;
			parent->data.binary.right_operand = parent->data.binary.right_operand == child ? cnode : parent->data.binary.right_operand;
			break;
		case NODE_TERNARY:
		  if (parent->data.ternary.left_operand == curnode)
				parent->data.ternary.left_operand =  cnode;
		  else if (parent->data.ternary.middle_operand == curnode)
				parent->data.ternary.middle_operand = cnode;
			else
				parent->data.ternary.right_operand = cnode;
			break;
		default:
			break;
	}
}

struct type* type_after_conversion(struct node* node){
	struct node* cnode = NULL;
 struct result* res =  node_get_result(node);
	struct type* t = NULL;
 if (!res){
  compiler_print_error(node->location, "type is not evaluated for the node");
  return NULL;
 }
 t = res->type;

	cnode = node->parent;
	while(cnode && cnode->kind == NODE_TYPE_CAST){
		t = cnode->data.unary.result.type;
		cnode = cnode->parent;
	}
	return t;    
}
/*
create cast nodes as link of unary nodes for the passed in node.
for e.g. if passed in node is identifier, then a new node is created
and linked to cnode member of struct node object if it is null.
if cnode is not null for the passed in node then the newly created node
will be linked to unary child_operand member and so on..
*/
/*struct node* type_create_cast_node_0(struct node* curnode, struct type* convtype){
		struct node* cnode = NULL;    
		cnode = node_one_operand(NODE_TYPE_CAST, "type_cast", NULL, curnode->location);   
		cnode->data.unary.result.type = convtype;
		cnode->data.unary.result.islvalue = false;

		if (curnode->cnode == NULL){
			curnode->cnode = cnode;
		}
		else{
			struct node* next = curnode->cnode;
			struct node* prev = curnode->cnode;
			while(next){
				prev = next;
				next = next->data.unary.child_operand;
			}
			prev->data.unary.child_operand = cnode;
		}
		return cnode
}*/

/*
get the final cast node types that are created in unary and binary conversions
*/
/*struct type* type_after_conversion_0(struct node* node){
	struct node* cnode = NULL;
	struct type* t = node_get_result(node)->type;

	cnode = node->cnode;
	while(cnode){
		t = cnode->data.unary.result.type;
		cnode = node->data.unary.child_operand;
	}
	return t;    
}*/

/*
handle usual unary conversions Table 6-5
*/
struct type* type_usual_unary_conversion(struct node* node){
	 struct type* t = NULL; 
  struct result* res = node_get_result(node);
	 struct type* newt = NULL;
  if (!res){
   compiler_print_error(node->location, "type is not evaluated for this node ");
   return NULL;
  }
  t = res->type;
	 switch (t->kind){
		case TYPE_BASIC:
	   if(t->data.basic.datatype == TYPE_BASIC_SHORT || t->data.basic.datatype == TYPE_BASIC_CHAR)    
					newt = type_basic(false, TYPE_BASIC_INT);
	   else return t;      
		 break;    
		case TYPE_ARRAY:     
			 newt = type_pointer();
			 newt->data.ptrtype.target = t->data.arraytype.target;
			 break;     
		case TYPE_FUNCTION:
			 newt = type_pointer();
			 newt->data.ptrtype.target = t;
			 break;
		default:
			return t;
		}
		type_create_cast_node(node, newt);
		return newt;
}

/*
handle usual assignment conversion Table 6-3
*/
void type_usual_assign_conversion(struct node* lnode, struct node* rnode){
	struct type* left = NULL;
	struct type* right = NULL;
	struct type* newt = NULL;	
  left = type_after_conversion(lnode);
  right = type_after_conversion(rnode);
  
  if (!left || !right){
   compiler_print_error(rnode->location, "type is not evaluated for the node or child nodes");
   return;
  }

	if (type_is_arithmetic(left) && type_is_arithmetic(right) && !type_is_compatible(left, right)){
		 newt = type_basic(left->data.basic.is_unsigned, left->data.basic.datatype);
		 type_create_cast_node(rnode, newt);
		 return;
	}
	if (left->kind == TYPE_POINTER){
		 if (right->kind == TYPE_POINTER && left->data.ptrtype.target->kind == TYPE_VOID)
			return;
		 else if(right->kind == TYPE_POINTER && type_is_equal(left, right))
     		return;
		 else if(rnode->kind == NODE_NUMBER && rnode->data.number.value == 0){
 			newt = type_pointer();
 			newt->data.ptrtype.target = left->data.ptrtype.target;
 			type_create_cast_node(rnode, newt);
 			return;
		 }
		 else
			compiler_print_error( rnode->location,"assignment conversion is not possible") ;  
	}
}

/*
handle usual binary conversions Table 6-6
*/
struct type* type_usual_binary_conversion(struct node* lnode, struct node* rnode){
	struct type* newt = NULL;
	struct type* left = NULL;
	struct type* right = NULL;
	int lrank = 0, lsize = 0; 
	int rrank = 0, rsize = 0;
	bool is_leftunsigned = false;
	bool is_rightunsigned = false;
	int branch = 0;
	left = type_usual_unary_conversion(lnode);
	right = type_usual_unary_conversion(rnode);
 if (!left || !right){
   compiler_print_error(lnode->location, "type is not evaluated for this node or child nodes ");
   return NULL;
 }
	if (left->kind != TYPE_BASIC || right->kind != TYPE_BASIC){
		/*compiler_print_error(lnode->parent->location,"no conversion is possible");*/
		return NULL;
	}
	
	lrank = type_rank(left);
	rrank = type_rank(right);
	is_leftunsigned = left->data.basic.is_unsigned;
	is_rightunsigned = right->data.basic.is_unsigned;

	if (is_leftunsigned && is_rightunsigned){ /*row 4 of table 6-6*/
		 if   (lrank < rrank) type_create_cast_node(lnode, right); 
		 else if(rrank < lrank) type_create_cast_node(rnode, left);
		 return (lrank < rrank ? right : left);
	}
	else if (!is_leftunsigned && !is_rightunsigned){ /*row 5 of table 6-6*/
		if   (lrank < rrank ) type_create_cast_node(lnode, right) ;
		else if(rrank < lrank) type_create_cast_node(rnode, left);
		return (lrank < rrank ? right : left);
	}
	else if (is_leftunsigned && !is_rightunsigned){ /*rows 6,7,8 of table 6-6*/
		branch = rrank <= lrank ? 6 : rsize > lsize ? 7 : 8 ;
		if 			(branch == 6) type_create_cast_node(rnode, left);
		else if (branch == 7) type_create_cast_node(lnode, right);
		else if (branch == 8) { /*row 8*/
			newt = type_basic(true, right->data.basic.datatype);
			type_create_cast_node(lnode, newt);
			type_create_cast_node(rnode, newt);
		}
		return (rrank <= lrank ? left : rsize > lsize ? right : newt) ;
	}
	else if (!is_leftunsigned && is_rightunsigned){ /*rows 6,7,8 of table 6-6*/
		branch = lrank <= rrank ? 6  : lsize > rsize ? 7  : 8 ;
		if 			(branch == 6) type_create_cast_node(lnode, right);
		else if (branch == 7) type_create_cast_node(rnode, left);
		else if (branch == 8) { /*row*/
			newt = type_basic(true, left->data.basic.datatype);
			type_create_cast_node(lnode, newt);
			type_create_cast_node(rnode, newt);
		}
		return (lrank <= rrank ? right : rsize > lsize ? left : newt) ;
	}
	else
		return NULL;   
}

/*
handle address op (&)
*/
void type_address_op(struct node *node){
	struct node* child = node->data.unary.child_operand;
	struct type* curtype = NULL;
	struct result* res = NULL;
	if (child == NULL) return;
	
	res = node_get_result(child);
	curtype = res->type;
	if (curtype->kind != TYPE_FUNCTION && res->islvalue == 0){
		compiler_print_error(node->location, "operand is not compatible for address op");
		return;
	}

	if (curtype->kind == TYPE_FUNCTION){
		node->data.unary.result.type = type_usual_unary_conversion(child);
	}
	else{
		node->data.unary.result.type = type_pointer();
		node->data.unary.result.type->data.ptrtype.target = curtype;
	}
	node->data.unary.result.islvalue = 0;
}

/*
handle pointer dereference (*) op
*/
void type_indirection_op(struct node *node){
	struct node* child = node->data.unary.child_operand;
	struct type* curtype = NULL;
	struct type* convtype = NULL;
	if (child == NULL) return;

	curtype = node_get_result(child)->type;
	if (!curtype) {
  	compiler_print_error(node->location, "types of child are not evaluated");
  	return;
  }
	if (curtype->kind != TYPE_POINTER) {
		compiler_print_error(node->location, "operand has to be pointer for indirection op");
		return;
	}
	convtype = type_usual_unary_conversion(child);
	node->data.unary.result.type = convtype->data.ptrtype.target;
	node->data.unary.result.islvalue = 1;  
	
}

void type_logical_negation_op(struct node* node){
	struct type* t = NULL;
	struct node* child = node->data.unary.child_operand;
	if (child == NULL) return;
	
	t = node_get_result(child)->type;
	if (!t) {
  	compiler_print_error(node->location, "types of child are not evaluated");
  	return;
  }
	if (type_is_arithmetic(t) || t->kind == TYPE_POINTER){
		type_usual_unary_conversion(child);
		node->data.unary.result.type = type_basic(true, TYPE_BASIC_INT);
		node->data.unary.result.islvalue = 0;
	}
	else
		compiler_print_error(node->location, "operand is not compatible with logical negation op");	
}

void type_cast_op(struct node* node){
  struct type* t1 = NULL;
  struct type* t2 = NULL;
  struct node* lnode = node->data.binary.left_operand;
  struct node* rnode = node->data.binary.right_operand;

  t1 = node_get_result(lnode)->type;
  t2 = node_get_result(rnode)->type;
  if (!t1 || !t2) {
  	compiler_print_error(node->location, "types of child are not evaluated");
  	return;
  }

  if ((type_is_arithmetic(t1) && type_is_arithmetic(t2)) || t1->kind == TYPE_VOID){
  	node->data.binary.result.type = t1;
  	node->data.binary.result.islvalue = false;
  	return;
  }
  else if ((t2->kind == TYPE_POINTER && t1->kind == TYPE_BASIC) || 
           (t1->kind == TYPE_POINTER && t2->kind == TYPE_BASIC)){
    node->data.binary.result.type = t1->kind == TYPE_BASIC ? t1 : t2;
  	node->data.binary.result.islvalue = false;
  	return;
  }
  else if (type_is_compatible(t1, t2)){
  	node->data.binary.result.type = t1;
  	node->data.binary.result.islvalue = false;
  	return;
  }
  else
  	compiler_print_error(node->location, "incompatible operands for cast op");
  
}

/*
handle tilde (~) op
*/
void type_bitwise_negation_op(struct node* node){
	struct node* child = node->data.unary.child_operand;
	struct type* convtype = NULL;
	struct type* curtype = NULL;
	if (child == NULL) return;

	curtype = node_get_result(child)->type;
	if (!type_is_arithmetic(curtype)){
		compiler_print_error(node->location,"non arithmetic operand is not allowed for bitwise negation op");
		return;
	}
	convtype = type_usual_unary_conversion(child);  
	node->data.unary.result.type = convtype;
	node->data.unary.result.islvalue = 0;
}

/*
handle unary plus and minus op
*/
void type_unary_arithmetic_op(struct node* node){
	struct node* child = node->data.unary.child_operand;
	struct type* convtype = NULL;
	struct type* curtype = NULL;
	if (child == NULL) return;

	curtype = node_get_result(child)->type;
	if (!type_is_arithmetic(curtype)){
		compiler_print_error(node->location,"non arithmetic operand is not allowed for unary op");
		return;
	}
	convtype = type_usual_unary_conversion(child);  
	node->data.unary.result.type = convtype;
	node->data.unary.result.islvalue = 0;
}

/*
handle multiplication, division and remainder op
*/
void type_mult_op(struct node* node){
	struct type* t1 = NULL;
	struct type* t2 = NULL;
	struct type* convtype = NULL;
	struct node* lnode = node->data.binary.left_operand;
	struct node* rnode = node->data.binary.right_operand;

	t1 = node_get_result(lnode)->type;
	t2 = node_get_result(rnode)->type;
	if (!type_is_arithmetic(t1) || !type_is_arithmetic(t2)){
		compiler_print_error(node->location,"non arithmetic operands are not allowed for multiplication op");
		return;
	}
	
	if ((convtype = type_usual_binary_conversion(lnode, rnode)) == NULL){
		compiler_print_error(node->location,"incompatible operands for multiplication op");
		return;
	}
	node->data.binary.result.type = convtype;
	node->data.binary.result.islvalue = 0;
}

/*
handle addition and subtraction op
*/
void type_add_op(struct node* node){
	struct type* t1 = NULL;
	struct type* t2 = NULL;
	struct type* convtype = NULL;
	bool is_arithmetic1 = false;
	bool is_arithmetic2 = false;

	struct node* lnode = node->data.binary.left_operand;
	struct node* rnode = node->data.binary.right_operand;
	convtype = type_usual_binary_conversion(lnode, rnode);

	t1 = type_after_conversion(lnode);
	t2 = type_after_conversion(rnode);
	is_arithmetic1 = type_is_arithmetic(t1);
	is_arithmetic2 = type_is_arithmetic(t2);

	if (is_arithmetic1 && is_arithmetic2){
		node->data.binary.result.type = convtype;
		node->data.binary.result.islvalue = 0;
		return;
	}
	if (is_arithmetic1 && t2->kind == TYPE_POINTER){
		node->data.binary.result.type = t2;
		node->data.binary.result.islvalue = 0;
		return;
	}
	else if (is_arithmetic2 && t1->kind == TYPE_POINTER){
		node->data.binary.result.type = t1;
		node->data.binary.result.islvalue = 0;
		return;
	}
	else if ((node->kind == NODE_MINUS || node->kind == NODE_MINUS_EQUAL) && 
					 t1->kind == TYPE_POINTER && t2->kind == TYPE_POINTER){
		node->data.binary.result.type = type_basic(false, TYPE_BASIC_INT);
		node->data.binary.result.islvalue = 0;
		return;
	}
	else{
		compiler_print_error(node->location,"incompatible operands for additive op");
		return;
	}
 
}
/*
handle left shift and right shift op
*/
void type_shift_op(struct node* node){
	struct type* t1 = NULL;
	struct type* t2 = NULL;
	struct node* lnode = node->data.binary.left_operand;
	struct node* rnode = node->data.binary.right_operand;

	t1 = type_usual_unary_conversion(lnode);
	t2 = type_usual_unary_conversion(rnode);
	if (type_is_arithmetic(t1) && type_is_arithmetic(t2)){
		node->data.binary.result.type = t1;
		node->data.binary.result.islvalue = 0;
	}
	else
		compiler_print_error(node->location,"operands incompatible for shift op");	
}

/*
handle relational operators <, <=,>,>=
*/
void type_relational_op(struct node* node){
	struct type* t1 = NULL;
	struct type* t2 = NULL;
	struct node* lnode = node->data.binary.left_operand;
	struct node* rnode = node->data.binary.right_operand;
	type_usual_binary_conversion(lnode, rnode);

	t1 = type_after_conversion(lnode);
	t2 = type_after_conversion(rnode);

	if(type_is_compatible(t1, t2)){
		node->data.binary.result.type = type_basic(false, TYPE_BASIC_INT);
		node->data.binary.result.islvalue = 0;
	} 
	else{
		 compiler_print_error(node->location,"operands incompatible for relational op");
	} 
}

void type_equal_op(struct node* node){
	struct type* t1 = NULL;
	struct type* t2 = NULL;
	struct node* lnode = node->data.binary.left_operand;
	struct node* rnode = node->data.binary.right_operand;
	
	type_usual_binary_conversion(lnode, rnode);
	t1 = type_after_conversion(lnode);
	t2 = type_after_conversion(rnode);
	if (type_is_arithmetic(t1) && type_is_arithmetic(t2)){  
		node->data.binary.result.type = type_basic(false, TYPE_BASIC_INT);
		node->data.binary.result.islvalue = 0;
	}
	else if(t1->kind == TYPE_POINTER && t2->kind == TYPE_POINTER && type_is_compatible(t1, t2)){
		node->data.binary.result.type = type_basic(false, TYPE_BASIC_INT);
		node->data.binary.result.islvalue = 0;
	} 
	else if (lnode->kind == NODE_NUMBER && t2->kind == TYPE_POINTER){
		if (lnode->data.number.value != 0){
			compiler_print_error(node->location,"cannot be other than 0 for equal op");
			return;
		}
		node->data.binary.result.type = type_basic(false, TYPE_BASIC_INT);
		node->data.binary.result.islvalue = 0;
	}
	else if (rnode->kind == NODE_NUMBER && t1->kind == TYPE_POINTER){
		if (rnode->data.number.value != 0){
			compiler_print_error(node->location,"cannot be other than 0 for equal op");
			return;
		}
		node->data.binary.result.type = type_basic(false, TYPE_BASIC_INT);
		node->data.binary.result.islvalue = 0;
	}
	else{
		compiler_print_error(node->location,"incompatible operands for equal op");
	} 
}

void type_bitwise_op(struct node* node){
	struct type* t1 = NULL;
	struct type* t2 = NULL;
 struct type* convtype = NULL;
	struct node* lnode = node->data.binary.left_operand;
	struct node* rnode = node->data.binary.right_operand;
	
 t1 = node_get_result(lnode)->type;
 t2 = node_get_result(rnode)->type;	
	if (type_is_arithmetic(t1) && type_is_arithmetic(t2)){
   convtype = type_usual_binary_conversion(lnode, rnode);
			node->data.binary.result.type = convtype;
			node->data.binary.result.islvalue = 0;		
	}
	else{
		 compiler_print_error(node->location,"incompatible operands for bitwise op");
	} 
}

void type_logical_andor_op(struct node* node){
	struct type* t1 = NULL;
	struct type* t2 = NULL;
	struct node* lnode = node->data.binary.left_operand;
	struct node* rnode = node->data.binary.right_operand;
	
	t1 = type_usual_unary_conversion(lnode);
	t2 = type_usual_unary_conversion(rnode);
	if ((type_is_arithmetic(t1) || t1->kind == TYPE_POINTER) && 
			(type_is_arithmetic(t2) || t2->kind == TYPE_POINTER)){
		node->data.binary.result.type = type_basic(false, TYPE_BASIC_INT);
		node->data.binary.result.islvalue = 0;
	}
	else{
		compiler_print_error(node->location,"incompatible operands for logical op");
	}   
}

void type_conditional_op(struct node* node){
	struct type* t1 = NULL;
	struct type* t2 = NULL;
	struct type* t3 = NULL;
	struct type* convtype = NULL;
	struct node* lnode = node->data.ternary.left_operand;
	struct node* mnode = node->data.ternary.middle_operand;
	struct node* rnode = node->data.ternary.right_operand;
	
	t1 = node_get_result(lnode)->type;
	t2 = type_usual_unary_conversion(mnode);
	t3 = type_usual_unary_conversion(rnode);
	node->data.ternary.result.islvalue = 0; 
	if (type_is_arithmetic(t1) || t1->kind == TYPE_POINTER){
		if (type_is_arithmetic(t2) && type_is_arithmetic(t3)){
			convtype = type_usual_binary_conversion(mnode, rnode);
			node->data.ternary.result.type = convtype;
		}
		else if (t2->kind == TYPE_VOID && t3->kind == TYPE_VOID)
			node->data.ternary.result.type = type_void();    
		else if (t2->kind == TYPE_POINTER && t3->kind == TYPE_POINTER && type_is_compatible(t2, t3))     
			node->data.ternary.result.type = t2;         
		else if (t2->kind == TYPE_POINTER && rnode->kind == NODE_NUMBER && rnode->data.number.value == 0)
			node->data.ternary.result.type = t2;
		else if (t3->kind == TYPE_POINTER && mnode->kind == NODE_NUMBER && mnode->data.number.value == 0)  
			node->data.ternary.result.type = t3;
		else{
			compiler_print_error(node->location,"incompatible operands for conditional op");
		}
	}
	else{
		compiler_print_error(node->location,"incompatible operands for conditional op");
	}   
}

void type_simple_assign_op(struct node* node){
	struct type* t1 = NULL;
	struct type* t2 = NULL;
	struct result* res = NULL;
	struct node* lnode = node->data.binary.left_operand;
	struct node* rnode = node->data.binary.right_operand;
        
  t1 = node_get_result(lnode)->type;
  t2 = node_get_result(rnode)->type; 
  if (t1 == NULL || t2 == NULL){
  	compiler_print_error(node->location,"one of the operands type is unknown");
		return;
  }	
	if (t1->kind == TYPE_FUNCTION || t2->kind == TYPE_FUNCTION){
		compiler_print_error(node->location,"not a valid assignment");
		return;
	}

	if(t1->kind == TYPE_ARRAY){
		compiler_print_error(node->location,"array name cannot be specified as lvalue");
		return;
	}
	res = node_get_result(lnode);
	if (res->islvalue == 0){
		compiler_print_error(node->location,"left side is not an lvalue");
		return;
	}
	else if(t2->kind == TYPE_ARRAY){
		type_usual_unary_conversion(rnode);
	}
	type_usual_assign_conversion(lnode, rnode);
	node->data.binary.result.type = t1;
	node->data.binary.result.islvalue = 0;
}

void type_compound_assign_op(struct node* node){
	struct type* t1 = NULL;
	struct type* t2 = NULL;
	struct result* res = NULL;
 struct node* orig = NULL;
	struct node* lnode = node->data.binary.left_operand;
	struct node* rnode = node->data.binary.right_operand;
	bool bvalid = false;
	
	res = node_get_result(lnode);
	if (res->islvalue == 0){
		compiler_print_error(node->location,"left side is not an lvalue");
		return;
	}

	t1 = res->type;
	t2 = node_get_result(rnode)->type;
	switch(node->kind){
		case NODE_VBAR_EQUAL:
		case NODE_SLASH_EQUAL:
		case NODE_CARET_EQUAL:
		case NODE_PERCENT_EQUAL:
		case NODE_ASTERISK_EQUAL:
		case NODE_AMPERSAND_EQUAL:  
		case NODE_LESS_LESS_EQUAL:
		case NODE_GREATER_GREATER_EQUAL:
				bvalid = type_is_arithmetic(t1) && type_is_arithmetic(t2)? true : false;
			break;    
		case NODE_PLUS_EQUAL:
		case NODE_MINUS_EQUAL:
			bvalid = type_is_arithmetic(t1) && type_is_arithmetic(t2)? true : false;
			bvalid = bvalid ?  true : t1->kind == TYPE_POINTER && type_is_arithmetic(t2) ? true : false;
			break;
		default:
			break;
	}
	if (!bvalid){
		compiler_print_error(node->location,"incompatible operands for compound assignment op");
		return;
	}

	switch(node->kind){
		case NODE_VBAR_EQUAL:
		case NODE_CARET_EQUAL:
		case NODE_AMPERSAND_EQUAL:
			type_bitwise_op(node);
			break;
		case NODE_SLASH_EQUAL:    
		case NODE_PERCENT_EQUAL:
		case NODE_ASTERISK_EQUAL:
			type_mult_op(node);
			break;
		case NODE_LESS_LESS_EQUAL:
		case NODE_GREATER_GREATER_EQUAL:
			type_shift_op(node);  
			break;    
		case NODE_PLUS_EQUAL:
		case NODE_MINUS_EQUAL:
			type_add_op(node);
			break;
		default:
			break;
	}
 orig = node_one_operand(NODE_DUMMY, lnode->data.identifier.name, NULL, node->location);
 orig->data.unary.result.type = t1;
	type_usual_assign_conversion(orig, lnode);
	node->data.binary.result.type = t1;
	node->data.binary.result.islvalue = 0; 

}

void type_pre_increment_op(struct node* node){
 struct type* t = NULL;
 struct result* res = NULL;
 struct node* one = NULL;
 struct node* orig = NULL;
 struct node* child = node->data.unary.child_operand;
 
 res = node_get_result(child);
 if (res->islvalue == 0)
  compiler_print_error(node->location,"operand is not a lvalue");
 
 t = res->type;
 if (type_is_arithmetic(t) || t->kind == TYPE_POINTER){
   one =  node_one_operand(NODE_NUMBER, "number", NULL, node->location);
   one->data.number.value = 1;
   one->data.number.result.type = type_basic(false, TYPE_BASIC_INT);

   orig = node_one_operand(NODE_DUMMY, child->data.identifier.name, NULL, node->location);
   orig->data.unary.result.type = t;
   type_usual_binary_conversion(child, one);
   type_usual_assign_conversion(orig, child);

   node->data.unary.result.type = t;
   node->data.unary.result.islvalue = 0;
   free(one);
   free(orig);
   orig = NULL;
   one = NULL;
 }
 else
  compiler_print_error(node->location,"incompatible operand for prefix op");
}

void type_post_increment_op(struct node* node){
 struct type* t = NULL;
 struct result* res = NULL;
 struct node* one = NULL;
 struct node* orig = NULL;
 struct node* child = node->data.unary.child_operand;
 
 res = node_get_result(child);
 if (res->islvalue == 0)
  compiler_print_error(node->location,"operand is not a lvalue");
 
 t = res->type;
 if (type_is_arithmetic(t) || t->kind == TYPE_POINTER){
   one =  node_one_operand(NODE_NUMBER, "number", NULL, node->location);
   one->data.number.value = 1;
   one->data.number.result.type = type_basic(false, TYPE_BASIC_INT);
   orig = node_one_operand(NODE_DUMMY, child->data.identifier.name, NULL, node->location);
   orig->data.unary.result.type = t;

   type_usual_binary_conversion(child, one);
   type_usual_assign_conversion(orig, child);

   node->data.unary.result.type = t;
   node->data.unary.result.islvalue = 0;
   free(one);
   free(orig);
   one = NULL;
 }
 else
  compiler_print_error(node->location,"incompatible operand for postfix op");
}

int type_check_num_params(struct node* node, struct node *parent, struct typelist* params){
  int nparams = 0;
  int index = 0;
  struct location loc;
  struct node* expr = node;
  while(expr){
   expr = expr->data.binary.left_operand;
   nparams++;
  }
  while(params){
   params = params->next;
   index++;
  }
  if (nparams != index){
  	loc = node != NULL ? node->location : parent->location;
   compiler_print_error(loc,"number of parameters do not match");
   return 1;
  }
  return 0;
}

int type_check_param(struct node* node, struct node *parent, struct typelist* params){
  struct node* expr = NULL;
  struct type* t1 = NULL;
  struct type* t2 = NULL;
  struct node* paramnode = NULL;
  struct location loc;
  int index = 0, nparam = 0;;
  
  if (node == NULL) return 0;
  nparam = type_check_param(node->data.binary.left_operand, parent, params);
  
  if (nparam == -1) return -1;
  expr = node->data.binary.right_operand;
  type_eval_expression(expr);
  t1 = node_get_result(expr)->type;
  while (params && index < nparam){
    params = params->next;
    index++;
  }

  t2 = params->curtype;
  paramnode = node_one_operand(NODE_PARAM, "param", NULL, node->location);
  paramnode->data.unary.result.type = t2;
  if (t2->kind == TYPE_POINTER && t1->kind == TYPE_ARRAY){
  	type_usual_unary_conversion(expr);
  }
  	
  type_usual_assign_conversion(paramnode, expr);
  t1 = type_after_conversion(expr);
  t2 = type_after_conversion(paramnode);

  if (!type_is_equal(t1, t2)){
   free(paramnode);
   loc = node != NULL ? node->location : parent->location;
   compiler_print_error(loc, "function parameters do not match prototype");
   return -1;
  }
  free(paramnode);
  paramnode = NULL;
  nparam++;
  return nparam;
}

void type_func_call(struct node* node){
	struct node* postfix = NULL;
	struct node* exprlst = NULL;
  struct type* left = NULL;

  postfix= node->data.binary.left_operand;
  exprlst = node->data.binary.right_operand;

  left = node_get_result(postfix)->type;
  if (left->kind != TYPE_FUNCTION){
  	compiler_print_error(node->location, "something wrong with function call");
  	return;
  }
  if (type_check_num_params(exprlst, node, left->data.functype.params) == 0 &&
      type_check_param( exprlst, node, left->data.functype.params) != -1){
    node->data.binary.result.type = left->data.functype.rvaltype;
    node->data.binary.result.islvalue = false;
  }
    
}



/*****************
 * TYPE CHECKING *
 *****************/
void type_check_return(struct node* node){
  struct type* rettype =  NULL;
  struct node *parent = NULL;
  struct node *fspec = NULL;
  struct node* idnode = NULL;
  struct type* idtype = NULL;
  struct type *newt = NULL;
  struct type *left = NULL;

  rettype = node->data.unary.child_operand != NULL ? node_get_result(node->data.unary.child_operand)->type : NULL;
  parent = node->parent;

  while(parent && parent->kind != NODE_FUNC_DEF){
    parent = parent->parent;
  }
  if (parent->kind != NODE_FUNC_DEF){ 
    compiler_print_error(node->location,"something wrong with parse tree");
  	return;
  }

  fspec = parent->data.binary.left_operand;
  if (fspec && fspec->data.binary.right_operand && fspec->data.binary.right_operand->kind == NODE_FUNC_DECL){
  	 idnode = fspec->data.binary.right_operand->data.binary.left_operand;
     idtype = idnode && idnode->kind == NODE_IDENTIFIER ? node_get_result(idnode)->type : NULL;
     
     left = idtype->data.functype.rvaltype;
     if (left->kind == TYPE_VOID && rettype == NULL) return;

     if (type_is_arithmetic(left) && type_is_arithmetic(rettype)
     		&& !type_is_compatible(left, rettype)){
		 		newt = type_basic(left->data.basic.is_unsigned, left->data.basic.datatype);
		 		type_create_cast_node(node->data.unary.child_operand, newt);

		 }
     else if (idtype->kind != TYPE_FUNCTION || !type_is_compatible(left, rettype)){
       compiler_print_error(node->location, "return type does not match with prototype");
     }
  }
  
}

void type_check_main(struct node *node){
  struct type *t = node_get_result(node)->type;
  struct typelist *params = NULL;
  int index = 0;
  assert (t->kind == TYPE_FUNCTION);

  if (t->data.functype.rvaltype->kind != TYPE_BASIC || t->data.functype.rvaltype->data.basic.datatype != TYPE_BASIC_INT)
   compiler_print_error(node->location, "main return type does not match with standard c");
  
  params = t->data.functype.params;
  while (params){
   if (index == 0){
    if (params->curtype->kind != TYPE_BASIC || params->curtype->data.basic.datatype != TYPE_BASIC_INT)
     compiler_print_error(node->location, "main parameter does not match with standard c");
   }
   else if (index == 1){
    if (params->curtype->kind != TYPE_POINTER || params->curtype->data.ptrtype.target->kind != TYPE_ARRAY)
     compiler_print_error(node->location, "main parameter does not match with standard c");
   }
   else{
    compiler_print_error(node->location, "main parameter does not match with standard c");
    return;
   }
   index++;
   params = params->next;
  }
}

void type_eval_expression(struct node* node){
 static int has_return;
 if (node == NULL) return;
	switch (node->kind) {
		case NODE_BINARY_OPERATION:
			break;
		case NODE_IDENTIFIER:
			break;      
		case NODE_NUMBER:
			break;
		case NODE_STRING:
		  type_usual_unary_conversion(node);
			break;
		case NODE_EXPRESSION_STATEMENT:
			break; 
		case NODE_TR_UNIT:
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);
			break;
		case NODE_ARRAY_DECL:
		case NODE_POINTER_DECL:        
			break;        
		case NODE_FUNC_CALL:
		  type_eval_expression(node->data.binary.left_operand);        
			type_eval_expression(node->data.binary.right_operand);      
		  type_func_call(node);
		  break;
		case NODE_PARAM_LIST:
		case NODE_FUNC_DECL:
			break;              
		case NODE_DECL_LIST:
		case NODE_EXPR_LIST:
		case NODE_COMMA_EXPR:
			type_eval_expression(node->data.binary.left_operand);        
			type_eval_expression(node->data.binary.right_operand);      
			break;
		case NODE_FOR:        
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);
			break;
		case NODE_IF:
		case NODE_WHILE:
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);
			break;
		case NODE_DOWHILE:
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);
			break;
		case NODE_FUNC_DEF:
			{				
		  	struct node *curnode = NULL;
		  	struct type *t = NULL;

		  	has_return = 0;
				type_eval_expression(node->data.binary.left_operand);
				type_eval_expression(node->data.binary.right_operand);
				curnode = node->data.binary.left_operand->data.binary.right_operand;
				while(curnode && curnode->kind != NODE_IDENTIFIER){
					if (curnode->kind == NODE_FUNC_DECL)
						curnode = curnode->data.binary.left_operand;
					else if (curnode->kind == NODE_POINTER_DECL)
						curnode = curnode->data.binary.right_operand;
				}
        if (curnode && curnode->kind == NODE_IDENTIFIER){
           t = node_get_result(curnode)->type;
           if (t->data.functype.rvaltype->kind != TYPE_VOID && has_return == 0){
           	compiler_print_error(node->location, "function expects return value");
           }
        }
			}
			break;
		case NODE_STATEMENT_LIST:
			 type_eval_expression(node->data.binary.left_operand);
			 type_eval_expression(node->data.binary.right_operand);       
			break;   
		case NODE_IF_ELSE:
			type_eval_expression(node->data.ternary.left_operand);
			type_eval_expression(node->data.ternary.middle_operand);
			type_eval_expression(node->data.ternary.right_operand);
			break;
		case NODE_FOR_EXPR:
			type_eval_expression(node->data.ternary.left_operand);
			type_eval_expression(node->data.ternary.middle_operand);
			type_eval_expression(node->data.ternary.right_operand);
			break;
		case NODE_DECL:
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);
			break;
		case NODE_ABSTR_DECL:
			break;
		case NODE_PARAM:      
		case NODE_FUNC_DEF_SPEC:
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);	
			break;
		 case NODE_RETURN:
			type_eval_expression(node->data.unary.child_operand);
			type_check_return(node);
			has_return = 1;
			break;
		case NODE_GOTO:
		case NODE_BREAK:
		case NODE_CONTINUE:
			type_eval_expression(node->data.unary.child_operand);
			break;
		case NODE_LABEL:
			type_eval_expression(node->data.binary.right_operand);
			break;
		case NODE_COMP_STATEMENT:
			type_eval_expression(node->data.unary.child_operand);
			break;        
		case NODE_VOID:
		 node->data.unary.result.type = type_void();
   break;
  case NODE_CHAR:
   node->data.unary.result.type = type_basic(false, TYPE_BASIC_CHAR);
   break;
  case NODE_SIGNED :
   node->data.unary.result.type = type_basic(false, TYPE_BASIC_INT);
   break;
  case NODE_UNSIGNED :
   node->data.unary.result.type = type_basic(true, TYPE_BASIC_INT);
   break;
  case NODE_SIGNED_INT:
   node->data.unary.result.type = type_basic(true, TYPE_BASIC_INT);
   break;
    case NODE_SIGNED_LONG :
     node->data.unary.result.type = type_basic(false, TYPE_BASIC_LONG);
      break;
    case NODE_SIGNED_CHAR :
      node->data.unary.result.type = type_basic(false, TYPE_BASIC_CHAR);
      break;
    case NODE_SIGNED_SHORT :
      node->data.unary.result.type = type_basic(false, TYPE_BASIC_SHORT);
      break;
    case NODE_UNSIGNED_INT :
      node->data.unary.result.type = type_basic(true, TYPE_BASIC_INT);
      break;
    case NODE_UNSIGNED_CHAR :
      node->data.unary.result.type = type_basic(true, TYPE_BASIC_CHAR);
      break;
    case NODE_UNSIGNED_SHORT :
      node->data.unary.result.type = type_basic(true, TYPE_BASIC_SHORT);
      break;
    case NODE_SIGNED_LONG_INT :
      node->data.unary.result.type = type_basic(false, TYPE_BASIC_LONG);
      break;
    case NODE_SIGNED_SHORT_INT :
      node->data.unary.result.type = type_basic(false, TYPE_BASIC_SHORT);
      break;
    case NODE_UNSIGNED_SHORT_INT :
      node->data.unary.result.type = type_basic(true, TYPE_BASIC_SHORT);
      break;
    case NODE_UNSIGNED_LONG :
      node->data.unary.result.type = type_basic(true, TYPE_BASIC_LONG);
      break;
    case NODE_UNSIGNED_LONG_INT :
      node->data.unary.result.type = type_basic(true, TYPE_BASIC_LONG);
      break;
		case NODE_NULL_STATEMENT:
		  node->data.unary.result.type = type_basic(true, TYPE_BASIC_CHAR);
      break;     
		/* */    
  /*binary expression*/
		case NODE_VBAR:
		case NODE_CARET:
		case NODE_AMPERSAND:
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);
			type_bitwise_op(node);
			break;
		case NODE_PLUS:
		case NODE_MINUS:
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);
			type_add_op(node);
			break;            
		case NODE_SLASH:
		case NODE_PERCENT:
		case NODE_ASTERISK:
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);
			type_mult_op(node);
			break;
		case NODE_LESS:
		case NODE_GREATER:
		case NODE_LESS_EQUAL:
		case NODE_GREATER_EQUAL:
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);
			type_relational_op(node);
			break;
		case NODE_VBAR_VBAR:
		case NODE_AMPERSAND_AMPERSAND:
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);
			type_logical_andor_op(node);
			break;
		case NODE_SHIFT_LEFT:
		case NODE_SHIFT_RIGHT:
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);
			type_shift_op(node);
			break;      
		case NODE_NOT_EQUAL:
		case NODE_EQUAL_EQUAL:
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);
			type_equal_op(node);
			break;
		case NODE_EQUAL:
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);
			type_simple_assign_op(node);
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
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);
			type_compound_assign_op(node);
			break;
		case NODE_CAST:
			type_eval_expression(node->data.binary.left_operand);
			type_eval_expression(node->data.binary.right_operand);
			type_cast_op(node);
			break;

		/* */    
		/*unary expression*/     
		case NODE_TILDE:
			type_eval_expression(node->data.unary.child_operand);
			type_bitwise_negation_op(node);
			break;
		case NODE_UNARY_PLUS:        
		case NODE_UNARY_MINUS:
			type_eval_expression(node->data.unary.child_operand);
			type_unary_arithmetic_op(node);
			break;
		case NODE_POST_INCR:
		case NODE_POST_DECR:
			type_eval_expression(node->data.unary.child_operand);
			type_post_increment_op(node);
			break;
		case NODE_PRE_INCR:
		case NODE_PRE_DECR:
			type_eval_expression(node->data.unary.child_operand);
			type_pre_increment_op(node);
			break;
		case NODE_INDIRECT:       
			type_eval_expression(node->data.unary.child_operand);
			type_indirection_op(node);
			break;
		case NODE_ADDRESS_OP:
			type_eval_expression(node->data.unary.child_operand);
			type_address_op(node);
			break;
		case NODE_EXCLAMATION:
			type_eval_expression(node->data.unary.child_operand);
			type_logical_negation_op(node);
			break;
		/* */    
  /*ternary expression*/
		case NODE_TERNARY_OP:
			type_eval_expression(node->data.ternary.left_operand);
			type_eval_expression(node->data.ternary.middle_operand);
			type_eval_expression(node->data.ternary.right_operand);
			type_conditional_op(node);
			break;     

		default:
			break;
	}	 
}



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
static void type_print_array(FILE* output, struct type* t, struct list* lst);
static void type_print_pointer(FILE* output, struct type* t, struct list* lst);
static void type_print_basic(FILE *output, struct type *basic) {
	assert(TYPE_BASIC == basic->kind);

	if (basic->data.basic.is_unsigned) {
		fputs("unsigned", output);
	} else {
		fputs("  signed", output);
	}

	switch (basic->data.basic.datatype) {
		case TYPE_BASIC_CHAR:
			fputs("  char", output);
			break;
		case TYPE_BASIC_SHORT:
			fputs(" short", output);
			break;
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

static struct list* type_add_item(char* item){
	struct list* lst = NULL;

	lst = malloc(sizeof(struct list));
  assert(lst != NULL);

  strcpy(lst->item, item);
  lst->next = NULL;
  return lst;
}

static void type_print_pointer(FILE* output, struct type* t, struct list* lst){
  char item[16];
  struct list* lstitem = NULL;
  struct type* target = t->data.ptrtype.target;
  if (target == NULL) return;

  switch(target->kind) {
   case TYPE_BASIC:
    type_print_basic(output, target);
    break;
   case TYPE_POINTER:
    lstitem = type_add_item("(*)");
    lst->next = lstitem;
    type_print_pointer(output, target, lstitem);
    break;
   case TYPE_ARRAY:
    sprintf(item, "[%d]", target->data.arraytype.arrsize);
    lstitem = type_add_item(item);
    lst->next = lstitem;
    type_print_array(output, target, lstitem);
    break;
   default:
    return;
  }
  
}

static void type_print_array(FILE* output, struct type* t, struct list* lst){
	char item[16];
	struct list* lstitem = NULL;
  struct type* target = t->data.arraytype.target;
  if (target == NULL) return;

  switch(target->kind) {
   case TYPE_BASIC:
    type_print_basic(output, target);
    break;
   case TYPE_POINTER:
    lstitem = type_add_item(item);
    lst->next = lstitem;
    type_print_pointer(output, target, lstitem);
    break;
   case TYPE_ARRAY:
    sprintf(item, "[%d]", target->data.arraytype.arrsize);
    lstitem = type_add_item(item);
    lst->next = lstitem;
    type_print_array(output, target, lstitem);
    break;
   default:
    return;
  }
  /*fprintf(output, "[%d]", t->data.arraytype.arrsize);*/
}




void type_print(FILE *output, struct type *kind) {	
	assert(NULL != kind);

	switch (kind->kind) {
		case TYPE_BASIC:
			type_print_basic(output, kind);
			break;
    case TYPE_POINTER:
	    {
	    	struct list* head = NULL;
	    	struct list* item = NULL;
	      head = type_add_item("(*)");
	      type_print_pointer(output, kind, head);
	      while(head){
	      	item = head;
	      	fputs(item->item, output);
	      	head = item->next;
	      	free(item);
	      }
	    }    
	    break;
    case TYPE_ARRAY:
	    {
	    	char cval[16];
	    	struct list* head = NULL;
	    	struct list* item = NULL;
	    	sprintf(cval, "[%d]", kind->data.arraytype.arrsize);
	      head = type_add_item(cval);
	      type_print_array(output, kind, head);
	      while(head){
	      	item = head;
	      	fputs(item->item, output);
	      	head = item->next;
	      	free(item);
	      }
	    }	      
	    break;
		default:
			assert(0);
			break;
	}
}
