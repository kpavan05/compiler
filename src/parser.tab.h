/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENTIFIER = 258,
     NUMBER = 259,
     STRING = 260,
     BREAK = 261,
     CHAR = 262,
     CONTINUE = 263,
     DO = 264,
     ELSE = 265,
     FOR = 266,
     GOTO = 267,
     IF = 268,
     INT = 269,
     LONG = 270,
     RETURN = 271,
     SHORT = 272,
     SIGNED = 273,
     UNSIGNED = 274,
     VOID = 275,
     WHILE = 276,
     LEFT_PAREN = 277,
     RIGHT_PAREN = 278,
     LEFT_SQUARE = 279,
     RIGHT_SQUARE = 280,
     LEFT_CURLY = 281,
     RIGHT_CURLY = 282,
     PERIOD = 283,
     SEMICOLON = 284,
     SINGLE_QUOTE = 285,
     DOUBLE_QUOTE = 286,
     BACK_SLASH = 287,
     COMMA = 288,
     CARET_EQUAL = 289,
     VBAR_EQUAL = 290,
     LESS_LESS_EQUAL = 291,
     GREATER_GREATER_EQUAL = 292,
     AMPERSAND_EQUAL = 293,
     SLASH_EQUAL = 294,
     ASTERISK_EQUAL = 295,
     PLUS_EQUAL = 296,
     PERCENT_EQUAL = 297,
     MINUS_EQUAL = 298,
     EQUAL = 299,
     COLON = 300,
     QUESTION = 301,
     VBAR_VBAR = 302,
     AMPERSAND_AMPERSAND = 303,
     VBAR = 304,
     CARET = 305,
     AMPERSAND = 306,
     EXCLAMATION_EQUAL = 307,
     EQUAL_EQUAL = 308,
     GREATER = 309,
     LESS = 310,
     GREATER_EQUAL = 311,
     LESS_EQUAL = 312,
     GREATER_GREATER = 313,
     LESS_LESS = 314,
     PLUS = 315,
     MINUS = 316,
     PERCENT = 317,
     SLASH = 318,
     ASTERISK = 319,
     EXCLAMATION = 320,
     TILDE = 321,
     MINUS_MINUS = 322,
     PLUS_PLUS = 323
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif



#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



