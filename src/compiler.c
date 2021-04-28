#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <limits.h>
#include <stdarg.h>

#include "compiler.h"
#include "parser.h"
#include "scanner.h"
#include "node.h"
#include "symbol.h"
#include "type.h"
#include "ir.h"
#include "mips.h"

extern int errno;

static int nerrors = 0;

void compiler_print_error(YYLTYPE location, const char *format, ...) {
  va_list ap;
  fprintf(stdout, "Error (%d, %d) to (%d, %d): ",
          location.first_line, location.first_column,
          location.last_line, location.last_column);
  va_start(ap, format);
  vfprintf(stdout, format, ap);
  va_end(ap);
  nerrors++;
  fputc('\n', stdout);
}

static void print_errors_from_pass(char *pass, int error_count) {
  fprintf(stdout, "%s encountered %d %s.\n",
          pass, error_count, (error_count == 1 ? "error" : "errors"));
}

/**
 * Launches the compiler.
 * 
 * The following describes the arguments to the program:
 * compiler [-s (scanner|parser|symbol|type|ir|mips)] [-o outputfile] [inputfile|stdin]
 *
 * -s : the name of the stage to stop after. Defaults to
 *      runs all of the stages.
 * -o : the name of the output file. Defaults to "output.s"      
 *
 * You should pass the name of the file to process or redirect stdin.
 */
int main(int argc, char **argv) {
  FILE *output;
  FILE *fpparser = NULL;
  FILE *fpsymbol = NULL;
  struct symbol_table symbol_table;
  char *stage, output_name[NAME_MAX + 1];
  char parse_file[NAME_MAX+1];
  char sym_file[NAME_MAX+1];
  char op_lvl[4];
  int opt;
  yyscan_t scanner;
  struct node *parse_tree;
  int error_count;

  strncpy(output_name, "output.s", NAME_MAX + 1);
  parse_file[0] = '\0';
  sym_file[0] = '\0';
  op_lvl[0] = '\0';
  stage = "mips";

  while (-1 != (opt = getopt(argc, argv, "o:s:l:p:t:"))) {
    switch (opt) {
      case 'o':
        strncpy(output_name, optarg, NAME_MAX);
        break;
      case 'p':
        strncpy(parse_file, optarg, NAME_MAX);
        break;
      case 't':
        strncpy(sym_file, optarg, NAME_MAX);
        break;
      case 's':
        stage = optarg;
        break;
      case 'l':
        strncpy(op_lvl, optarg, 4);
        break;
    }
  }

  /* Figure out whether we're using stdin/stdout or file in/file out. */
  if (optind >= argc) {
    scanner_initialize(&scanner, stdin);
  } else if (optind == argc - 1) {
    scanner_initialize(&scanner, fopen(argv[optind], "r"));
  } else {
    fprintf(stdout, "Expected 1 input file, found %d.\n", argc - optind);
    return 1;
  }

  if (0 == strcmp("scanner", stage)) {
    error_count = 0;
    scanner_print_tokens(stdout, &error_count, scanner);
    scanner_destroy(&scanner);
    if (error_count > 0) {
      print_errors_from_pass("Scanner", error_count);
      return 1;
    } else {
      return 0;
    }
  }

  error_count = 0;
  parse_tree = parser_create_tree(&error_count, scanner);
  scanner_destroy(&scanner);
  if (NULL == parse_tree) {
    print_errors_from_pass("Parser", error_count);
    return 1;
  }

  if (0 == strcmp("parser", stage)) {
    /*fprintf(stdout, "=============== PARSE TREE ===============\n");*/
    node_print_parsetree_ouput(stdout, parse_tree);
    /*
    node_print_statement_list(stdout, parse_tree);
    */
    return 0;
  }

  symbol_initialize_table(&symbol_table);
  strcpy(symbol_table.name, "global");
  symbol_parse_ast(&symbol_table, parse_tree);
  /*error_count = symbol_add_from_statement_list(&symbol_table, parse_tree);*/
  if (nerrors > 0) {
    print_errors_from_pass("Symbol table", nerrors);
    return 1;
  }
  if (sym_file[0] == '\0' || (fpsymbol = fopen(sym_file,"w")) == NULL){
    fpsymbol = stdout;
  }
  if (parse_file[0] == '\0' || (fpparser = fopen(parse_file, "w")) == NULL){
    fpparser = stdout;
  }
  fprintf(fpsymbol, "================= SYMBOLS ================\n");
  symbol_print_table(fpsymbol, &symbol_table);

  if (fpsymbol != stdout) fclose(fpsymbol);
  if (0 == strcmp("symbol", stage)) {
    fprintf(fpparser, "=============== PARSE TREE ===============\n");
    node_print_parsetree_ouput(fpparser, parse_tree);
    if (fpparser != stdout) fclose(fpparser);
    /*node_print_statement_list(stdout, parse_tree);*/
    return 0;
  }

  /*error_count = type_assign_in_statement_list(parse_tree);
  if (error_count > 0) {
    print_errors_from_pass("Type checking", error_count);
    return 1;
  }*/
  type_eval_expression(parse_tree);
  if (nerrors > 0){
    print_errors_from_pass("Type checking", nerrors);
    return 1;
  }
  fprintf(stdout, "=============== PARSE TREE ===============\n");
  node_print_parsetree_ouput(stdout, parse_tree);
  /*node_print_statement_list(stdout, parse_tree);*/
  if (0 == strcmp("type", stage)) {
    return 0;
  }

  /*error_count = ir_generate_for_statement_list(parse_tree);*/
  ir_generate_for_ast(parse_tree);
  if (nerrors > 0) {
    print_errors_from_pass("IR generation", nerrors);
    return 1;
  }


  fprintf(stdout, "=================== IR ===================\n");
  ir_print_section(stdout, parse_tree->ir);
  
  if (0 == strcmp("ir", stage)) { 
    if (strcmp(op_lvl, "lv") == 0){
      fprintf(stdout, "=================== IR Analysis ===================\n");
      ir_generate_live_analysis(parse_tree->ir);
      ir_print_section(stdout, parse_tree->ir);
    }
    return 0;
  }

  if (strcmp(op_lvl, "l1") == 0){
    fprintf(stdout, "=================== IR Optimization ===================\n");
    ir_optimize(parse_tree->ir);
    ir_print_section(stdout, parse_tree->ir);
  }

  fprintf(stdout, "================== MIPS ==================\n");
  mips_print_program(stdout, parse_tree->ir, &symbol_table);
  fputs("\n\n", stdout);

  output = fopen(output_name, "w");
  if (NULL == output) {
    fprintf(stdout, "Could not open output file %s: %s", optarg, strerror(errno));
    return 1;
  }
  mips_print_program(output, parse_tree->ir, &symbol_table);
  fputs("\n\n", output);

  return 0;
}
