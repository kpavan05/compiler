%option yylineno
%option reentrant
%option bison-bridge
%option bison-locations
%option nounput
%option noyyalloc
%option noyyrealloc
%option noyyfree

%{
/*
 * scanner.lex
 *
 * This file contains the specification for the Flex generated scanner
 * for the CSCI E-95 sample language.
 *
 */

  #include <stdlib.h>
  #include <errno.h>
  #include <string.h>

  /* Suppress compiler warnings about unused variables and functions. */
  #define YY_EXIT_FAILURE ((void)yyscanner, 2)
  #define YY_NO_INPUT

  /* Track locations. */
  #define YY_EXTRA_TYPE int
  #define YY_USER_ACTION { yylloc->first_line = yylloc->last_line = yylineno; \
                           yylloc->first_column = yyextra; \
                           yylloc->last_column = yyextra + yyleng - 1; \
                           yyextra += yyleng; }

  #include "compiler.h"
  #include "parser.tab.h"
  #include "node.h"
  #include "type.h"
%}

newline         \n|\r\n
ws              [ \t\v\f]
underscore      _   
graphic         [@$`]
digit           [[:digit:]]
letter          [[:alpha:]]

comment         ("/*")[^/]*("*/")
octal           ("0")([0-7])*
decimal          ([1-9][0-9]*)
number          0|[1-9][[:digit:]]*

octalconst      ("\'")(\\)(([0-7]){0,3})("\'")
charescape      \'(\\[ntvfrba?\"\\\'])\'
/*charconst       ("\'")({number}|{letter}|{underscore}|{graphic})?("\'")*/

charconst       ("\'")(({number}|{letter}|{underscore}|{graphic}){1})("\'")
string          ("\"")(\\.|[^\\"])*("\"")
id              ({letter}|{underscore})({letter}|{digit}|{underscore})*

%%

{newline}   yyextra = 1;
{ws}        {}

  /* operators begin */
\*      return ASTERISK;
\+      return PLUS;
-       return MINUS;
\/      return SLASH;
\=      return EQUAL;
\(      return LEFT_PAREN;
\)      return RIGHT_PAREN;
;       return SEMICOLON;

"["     return LEFT_SQUARE;
"]"     return RIGHT_SQUARE;
"{"     return LEFT_CURLY;
"}"     return RIGHT_CURLY;
"++"    return PLUS_PLUS;
"--"    return MINUS_MINUS;
"!"     return EXCLAMATION;
"~"     return TILDE;
"\'"    return SINGLE_QUOTE;
"\""    return DOUBLE_QUOTE;
"\\"    return BACK_SLASH;
"%"     return PERCENT;
"<<"    return LESS_LESS;
">>"    return GREATER_GREATER;
"<"     return LESS;
"<="    return LESS_EQUAL;
">"     return GREATER;
">="    return GREATER_EQUAL;
"=="    return EQUAL_EQUAL;
"!="    return EXCLAMATION_EQUAL;
"|"     return VBAR;
"^"     return CARET;
"&"     return AMPERSAND;
"||"    return VBAR_VBAR;
"&&"    return AMPERSAND_AMPERSAND;
"?"     return QUESTION;
":"     return COLON;

"+="    return PLUS_EQUAL;
"-="    return MINUS_EQUAL;
"*="    return ASTERISK_EQUAL;
"/="    return SLASH_EQUAL;
"%="    return PERCENT_EQUAL;
"<<="   return LESS_LESS_EQUAL;
">>="   return GREATER_GREATER_EQUAL;
"&="    return AMPERSAND_EQUAL;
"^="    return CARET_EQUAL;
"|="    return VBAR_EQUAL;
","     return COMMA;
"."     return PERIOD;
  /* operators end */


break      return BREAK;
char       return CHAR;
continue   return CONTINUE;
do         return DO;
else       return ELSE;
for        return FOR;
goto       return GOTO;
if         return IF;
int        return INT;
long       return LONG;
return     return RETURN;
short      return SHORT;
signed     return SIGNED;
unsigned   return UNSIGNED;
void       return VOID;
while      return WHILE;


  /* constants begin */
{comment}		     {}  
{string}		     *yylval = node_string(*yylloc, yytext, yyleng);  return STRING;
{octalconst}	   *yylval = node_cconst(*yylloc,yytext, yyleng);   return NUMBER;
{charconst}		   *yylval = node_cconst(*yylloc,yytext, yyleng);   return NUMBER;
{charescape}	   *yylval = node_cconst(*yylloc, yytext, yyleng);  return NUMBER;
{number}		     *yylval = node_number(*yylloc, yytext);          return NUMBER;
  /* constants end */

  /* identifiers */
{id}			       *yylval = node_identifier(*yylloc, yytext, yyleng); return IDENTIFIER;
.				         return -1;


%%

void scanner_initialize(yyscan_t *scanner, FILE *input) {
  yylex_init(scanner);
  yyset_in(input, *scanner);
  yyset_extra(1, *scanner);
}

void scanner_destroy(yyscan_t *scanner) {
  yylex_destroy(*scanner);
  scanner = NULL;
}

void scanner_print_tokens(FILE *output, int *error_count, yyscan_t scanner) {
  YYSTYPE val;
  YYLTYPE loc;
  int token;

  token = yylex(&val, &loc, scanner);
  printf("token is: %d\n", token);
  while (0 != token) {
    /*
     * Print the line number. Use printf formatting and tabs to keep columns
     * lined up.
     */
    fprintf(output, "loc = %04d:%04d-%04d:%04d",
            loc.first_line, loc.first_column, loc.last_line, loc.last_column);

    /*
     * Print the scanned text. Try to use formatting but give up instead of
     * truncating if the text is too long.
     */
    if (yyget_leng(scanner) <= 20) {
      fprintf(output, "     text = %-20s", yyget_text(scanner));
    } else {
      fprintf(output, "     text = %s", yyget_text(scanner));
    }

    if (token <= 0) {
      fputs("     token = ERROR", output);
      (*error_count)++;
    } else {
      fprintf(output, "     token = %-20s", parser_token_name(token));

      switch (token) {
        case NUMBER:
          /* Print the type and value. */
          fputs("     type = ", output);
          type_print(output, val->data.number.result.type);

		  if (val->data.number.ischar)
	        fprintf(output, "     value = %d", (int)(val->data.number.value));
		  else
			fprintf(output, "     value = %-10lu", val->data.number.value);
          if (val->data.number.overflow) {
            fputs("     OVERFLOW", output);
            (*error_count)++;
          }
          break;

        case IDENTIFIER:
          fprintf(output, "     name = %s", val->data.identifier.name);
          break;

		case STRING:
		  fprintf(output, "     name = %s", val->data.string_literal.string);
          break;
      }
    }
    fputs("\n", output);
    token = yylex(&val, &loc, scanner);
  }
}

/* Suppress compiler warnings about unused parameters. */
void *yyalloc (yy_size_t size, yyscan_t yyscanner __attribute__((unused)))
{   
return (void *)malloc(size);
}

/* Suppress compiler warnings about unused parameters. */
void *yyrealloc  (void *ptr, yy_size_t size, yyscan_t yyscanner __attribute__((unused)))
{
  return (void *)realloc((char *)ptr, size );
}

/* Suppress compiler warnings about unused parameters. */
void yyfree (void *ptr, yyscan_t yyscanner __attribute__((unused)))
{
  free((char *)ptr); /* see yyrealloc() for (char *) cast */
}
