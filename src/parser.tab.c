/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 12 "parser.y"

  #include <stdio.h>

  #include "compiler.h"
  #include "parser.tab.h"
  #include "scanner.yy.h"
  #include "node.h"

  #define YYERROR_VERBOSE
  static void yyerror(YYLTYPE *loc, YYSTYPE *root,
                      int *error_count, yyscan_t scanner,
                      char const *s);


/* Line 268 of yacc.c  */
#line 86 "parser.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 1
#endif


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


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 209 "parser.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  43
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   676

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  69
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  87
/* YYNRULES -- Number of rules.  */
#define YYNRULES  197
/* YYNRULES -- Number of states.  */
#define YYNSTATES  313

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   323

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,    10,    12,    16,    20,    23,
      28,    32,    34,    38,    42,    46,    50,    54,    58,    62,
      66,    70,    74,    76,    80,    83,    85,    89,    91,    95,
      98,   100,   105,   107,   109,   112,   115,   117,   121,   125,
     128,   130,   136,   138,   140,   142,   144,   146,   148,   151,
     155,   157,   159,   161,   164,   166,   168,   170,   174,   178,
     182,   187,   189,   193,   195,   197,   205,   207,   211,   215,
     217,   219,   223,   226,   234,   241,   247,   252,   259,   266,
     272,   278,   282,   287,   291,   296,   299,   302,   306,   314,
     320,   323,   325,   327,   329,   333,   335,   337,   339,   341,
     343,   345,   347,   349,   353,   355,   359,   362,   364,   368,
     370,   374,   378,   382,   384,   386,   389,   391,   394,   396,
     400,   402,   406,   408,   411,   414,   417,   419,   421,   423,
     425,   427,   430,   433,   436,   438,   440,   442,   444,   448,
     452,   456,   460,   464,   467,   469,   473,   477,   479,   482,
     485,   489,   491,   494,   496,   498,   501,   504,   508,   510,
     512,   514,   516,   518,   520,   522,   524,   526,   528,   530,
     532,   537,   539,   541,   544,   546,   548,   550,   552,   554,
     556,   558,   560,   562,   564,   566,   568,   571,   574,   577,
     581,   583,   586,   590,   595,   596,   602,   604
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     155,     0,    -1,   130,    -1,    95,    -1,   130,    95,    -1,
     123,    -1,    71,    60,   123,    -1,    71,    61,   123,    -1,
      51,    80,    -1,    96,    24,    88,    25,    -1,    96,    24,
      25,    -1,    85,    -1,   149,    44,    74,    -1,   149,    41,
      74,    -1,   149,    43,    74,    -1,   149,    40,    74,    -1,
     149,    42,    74,    -1,   149,    36,    74,    -1,   149,    37,
      74,    -1,   149,    38,    74,    -1,   149,    34,    74,    -1,
     149,    35,    74,    -1,    98,    -1,    75,    51,    98,    -1,
      66,    80,    -1,    78,    -1,    77,    49,    78,    -1,    75,
      -1,    78,    50,    75,    -1,     6,    29,    -1,   149,    -1,
      22,   147,    23,    80,    -1,     4,    -1,     7,    -1,    18,
       7,    -1,    19,     7,    -1,    74,    -1,    83,    33,    74,
      -1,    26,    92,    27,    -1,    26,    27,    -1,   122,    -1,
     122,    46,    99,    45,    85,    -1,   110,    -1,   109,    -1,
     115,    -1,    81,    -1,   144,    -1,    85,    -1,     8,    29,
      -1,    93,   114,    29,    -1,    90,    -1,   143,    -1,    91,
      -1,    92,    91,    -1,   148,    -1,   131,    -1,    96,    -1,
      22,    70,    23,    -1,    24,    88,    25,    -1,    95,    24,
      25,    -1,    95,    24,    88,    25,    -1,   142,    -1,    22,
      94,    23,    -1,   105,    -1,    73,    -1,     9,   143,    21,
      22,    99,    23,    29,    -1,   138,    -1,    98,    53,   138,
      -1,    98,    52,   138,    -1,    83,    -1,    74,    -1,   100,
      33,    74,    -1,    99,    29,    -1,    22,   112,    29,    99,
      29,    99,    23,    -1,    22,    29,    99,    29,    99,    23,
      -1,    22,    29,    29,    99,    23,    -1,    22,    29,    29,
      23,    -1,    22,   112,    29,    29,    99,    23,    -1,    22,
     112,    29,    99,    29,    23,    -1,    22,   112,    29,    29,
      23,    -1,    22,    29,    99,    29,    23,    -1,    11,   102,
     143,    -1,   133,    22,   100,    23,    -1,   133,    22,    23,
      -1,    96,    22,   128,    23,    -1,   107,    84,    -1,    93,
      94,    -1,    12,   124,    29,    -1,    13,    22,    99,    23,
     143,    10,   143,    -1,    13,    22,    99,    23,   143,    -1,
      64,    80,    -1,    99,    -1,    94,    -1,   113,    -1,   114,
      33,   113,    -1,     4,    -1,   141,    -1,   152,    -1,    82,
      -1,   154,    -1,    97,    -1,   103,    -1,   124,    -1,   118,
      45,   143,    -1,    77,    -1,   120,    48,    77,    -1,    65,
      80,    -1,   120,    -1,   122,    47,   120,    -1,    80,    -1,
     123,    64,    80,    -1,   123,    63,    80,    -1,   123,    62,
      80,    -1,     3,    -1,    29,    -1,    93,    94,    -1,    93,
      -1,    93,    70,    -1,   126,    -1,   127,    33,   126,    -1,
     127,    -1,    22,    99,    23,    -1,    64,    -1,    64,   130,
      -1,   130,    96,    -1,   133,    67,    -1,   137,    -1,   145,
      -1,   104,    -1,   134,    -1,   132,    -1,   133,    68,    -1,
      67,   149,    -1,    68,   149,    -1,     3,    -1,    87,    -1,
     129,    -1,   140,    -1,   138,    55,   140,    -1,   138,    57,
     140,    -1,   138,    54,   140,    -1,   138,    56,   140,    -1,
      16,    99,    29,    -1,    16,    29,    -1,    71,    -1,   140,
      59,    71,    -1,   140,    58,    71,    -1,    17,    -1,    17,
      14,    -1,    18,    17,    -1,    18,    17,    14,    -1,    14,
      -1,    18,    14,    -1,    18,    -1,    15,    -1,    15,    14,
      -1,    18,    15,    -1,    18,    15,    14,    -1,     3,    -1,
     101,    -1,   119,    -1,    84,    -1,    86,    -1,   117,    -1,
      79,    -1,    89,    -1,   139,    -1,   108,    -1,   125,    -1,
       5,    -1,   133,    24,    99,    25,    -1,    90,    -1,   106,
      -1,    93,    70,    -1,    93,    -1,   116,    -1,   153,    -1,
     133,    -1,   150,    -1,   151,    -1,   121,    -1,    76,    -1,
      72,    -1,   111,    -1,   136,    -1,   135,    -1,    61,    80,
      -1,    60,    80,    -1,    19,    17,    -1,    19,    17,    14,
      -1,    19,    -1,    19,    14,    -1,    19,    15,    15,    -1,
      19,    15,    15,    14,    -1,    -1,    21,    22,    99,    23,
     143,    -1,   146,    -1,   155,   146,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    54,    54,    55,    56,    59,    60,    62,    73,    76,
      78,    83,    84,    86,    88,    90,    92,    94,    96,    98,
     100,   102,   126,   127,   133,   137,   138,   143,   144,   151,
     157,   158,   163,   167,   168,   169,   173,   174,   179,   180,
     184,   185,   191,   192,   196,   197,   198,   201,   204,   211,
     219,   220,   224,   225,   230,   237,   238,   244,   245,   246,
     247,   251,   252,   253,   254,   257,   262,   263,   265,   276,
     279,   280,   284,   290,   292,   294,   296,   298,   300,   302,
     304,   309,   314,   315,   319,   324,   328,   337,   343,   348,
     356,   360,   364,   368,   369,   375,   379,   380,   381,   384,
     385,   386,   388,   393,   398,   399,   404,   408,   409,   416,
     417,   419,   421,   434,   439,   446,   448,   450,   454,   455,
     460,   464,   469,   470,   474,   479,   483,   484,   485,   486,
     487,   491,   495,   499,   505,   506,   507,   512,   513,   515,
     517,   519,   533,   534,   539,   540,   542,   554,   555,   556,
     557,   558,   559,   560,   561,   562,   563,   564,   569,   574,
     575,   576,   577,   578,   579,   580,   581,   582,   583,   587,
     591,   598,   599,   607,   608,   613,   614,   617,   618,   619,
     620,   621,   622,   623,   624,   625,   629,   633,   638,   639,
     640,   641,   642,   643,   648,   652,   657,   658
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "NUMBER", "STRING",
  "BREAK", "CHAR", "CONTINUE", "DO", "ELSE", "FOR", "GOTO", "IF", "INT",
  "LONG", "RETURN", "SHORT", "SIGNED", "UNSIGNED", "VOID", "WHILE",
  "LEFT_PAREN", "RIGHT_PAREN", "LEFT_SQUARE", "RIGHT_SQUARE", "LEFT_CURLY",
  "RIGHT_CURLY", "PERIOD", "SEMICOLON", "SINGLE_QUOTE", "DOUBLE_QUOTE",
  "BACK_SLASH", "COMMA", "CARET_EQUAL", "VBAR_EQUAL", "LESS_LESS_EQUAL",
  "GREATER_GREATER_EQUAL", "AMPERSAND_EQUAL", "SLASH_EQUAL",
  "ASTERISK_EQUAL", "PLUS_EQUAL", "PERCENT_EQUAL", "MINUS_EQUAL", "EQUAL",
  "COLON", "QUESTION", "VBAR_VBAR", "AMPERSAND_AMPERSAND", "VBAR", "CARET",
  "AMPERSAND", "EXCLAMATION_EQUAL", "EQUAL_EQUAL", "GREATER", "LESS",
  "GREATER_EQUAL", "LESS_EQUAL", "GREATER_GREATER", "LESS_LESS", "PLUS",
  "MINUS", "PERCENT", "SLASH", "ASTERISK", "EXCLAMATION", "TILDE",
  "MINUS_MINUS", "PLUS_PLUS", "$accept", "abstract_declarator",
  "additive_expr", "address_expr", "array_declarator", "assignment_expr",
  "bitwise_and_expr", "bitwise_negation_expr", "bitwise_or_expr",
  "bitwise_xor_expr", "break_statement", "cast_expr", "character_constant",
  "character_type_specifier", "comma_expr", "compound_statement",
  "conditional_expr", "conditional_statement", "constant", "constant_expr",
  "continue_statement", "decl", "declaration_or_statement",
  "declaration_or_statement_list", "declaration_specifiers", "declarator",
  "direct_abstract_declarator", "direct_declarator", "do_statement",
  "equality_expr", "expr", "expression_list", "expression_statement",
  "for_expr", "for_statement", "function_call", "function_declarator",
  "function_definition", "function_def_specifier", "goto_statement",
  "if_else_statement", "if_statement", "indirection_expr",
  "initial_clause", "initialized_declarator",
  "initialized_declarator_list", "integer_constant",
  "integer_type_specifier", "iterative_statement", "label",
  "labeled_statement", "logical_and_expr", "logical_negation_expr",
  "logical_or_expr", "multiplicative_expr", "named_label",
  "null_statement", "parameter_decl", "parameter_list",
  "parameter_type_list", "parenthesized_expr", "pointer",
  "pointer_declarator", "postdecrement_expr", "postfix_expr",
  "postincrement_expr", "predecrement_expr", "preincrement_expr",
  "primary_expr", "relational_expr", "return_statement", "shift_expr",
  "signed_type_specifier", "simple_declarator", "statement",
  "string_constant", "subscript_expr", "top_level_decl", "type_name",
  "type_specifier", "unary_expr", "unary_minus_expr", "unary_plus_expr",
  "unsigned_type_specifier", "void_type_specifier", "while_statement",
  "translation_unit", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    69,    70,    70,    70,    71,    71,    71,    72,    73,
      73,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    75,    75,    76,    77,    77,    78,    78,    79,
      80,    80,    81,    82,    82,    82,    83,    83,    84,    84,
      85,    85,    86,    86,    87,    87,    87,    88,    89,    90,
      91,    91,    92,    92,    93,    94,    94,    95,    95,    95,
      95,    96,    96,    96,    96,    97,    98,    98,    98,    99,
     100,   100,   101,   102,   102,   102,   102,   102,   102,   102,
     102,   103,   104,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   114,   115,   116,   116,   116,   117,
     117,   117,   118,   119,   120,   120,   121,   122,   122,   123,
     123,   123,   123,   124,   125,   126,   126,   126,   127,   127,
     128,   129,   130,   130,   131,   132,   133,   133,   133,   133,
     133,   134,   135,   136,   137,   137,   137,   138,   138,   138,
     138,   138,   139,   139,   140,   140,   140,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   142,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   144,
     145,   146,   146,   147,   147,   148,   148,   149,   149,   149,
     149,   149,   149,   149,   149,   149,   150,   151,   152,   152,
     152,   152,   152,   152,   153,   154,   155,   155
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     3,     3,     2,     4,
       3,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     1,     3,     2,     1,     3,     1,     3,     2,
       1,     4,     1,     1,     2,     2,     1,     3,     3,     2,
       1,     5,     1,     1,     1,     1,     1,     1,     2,     3,
       1,     1,     1,     2,     1,     1,     1,     3,     3,     3,
       4,     1,     3,     1,     1,     7,     1,     3,     3,     1,
       1,     3,     2,     7,     6,     5,     4,     6,     6,     5,
       5,     3,     4,     3,     4,     2,     2,     3,     7,     5,
       2,     1,     1,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     3,     2,     1,     3,     1,
       3,     3,     3,     1,     1,     2,     1,     2,     1,     3,
       1,     3,     1,     2,     2,     2,     1,     1,     1,     1,
       1,     2,     2,     2,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     2,     1,     3,     3,     1,     2,     2,
       3,     1,     2,     1,     1,     2,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     1,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     3,
       1,     2,     3,     4,     0,     5,     1,     2
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
     194,    33,   151,   154,   147,   153,   190,    98,   171,     0,
     172,     0,   175,    96,   196,    54,    97,   176,   194,   155,
     148,    34,   152,   156,   149,    35,   191,     0,   188,   158,
       0,   122,    64,    92,    56,    63,    93,     0,     0,    55,
      61,     0,    85,     1,   197,   157,   150,   192,   189,     0,
     123,   194,     0,    49,     0,   124,   134,    32,   169,     0,
       0,     0,     0,     0,     0,     0,     0,   194,    39,   114,
       0,     0,     0,     0,     0,     0,     0,     0,   144,   182,
      36,    27,   181,   104,    25,   164,   109,    45,    69,   161,
      11,   162,   135,   165,    50,    52,     0,     0,   100,    22,
       0,   159,   101,   128,   167,    43,    42,   183,    44,   163,
       0,   160,   107,   180,    40,     5,   102,   168,   136,   130,
     177,   129,   185,   184,   126,    66,   166,   137,    51,    46,
     127,    30,   178,   179,    99,   193,    62,   116,   118,   120,
       0,   134,    10,    47,     0,    30,    92,    94,    29,    48,
       0,     0,     0,   113,     0,     0,   143,     0,     0,   174,
       0,     0,     8,   187,   186,    90,   106,    24,     0,   132,
     133,     0,     0,     0,     0,     0,     0,    38,    53,     0,
       0,    72,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   125,   131,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   117,   115,     3,     2,   194,    84,     9,     0,     0,
      91,     0,    81,    87,     0,   142,     0,     0,   173,     2,
     121,     0,     6,     7,    23,    26,    28,    37,    68,    67,
     103,   105,     0,   108,   112,   111,   110,    83,    70,     0,
       0,   140,   138,   141,   139,   146,   145,    20,    21,    17,
      18,    19,    15,    13,    16,    14,    12,     0,     0,     0,
       4,   119,     0,     0,     0,     0,     0,     0,    31,     0,
      82,     0,   170,    57,    58,    59,     0,     0,    76,     0,
       0,     0,     0,    89,   195,    41,    71,    60,     0,    75,
      80,     0,    79,     0,     0,     0,    65,    74,    77,    78,
       0,    88,    73
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,   267,    78,    79,    32,    80,    81,    82,    83,    84,
      85,    86,    87,     7,    88,    89,    90,    91,    92,   144,
      93,     8,    95,    96,     9,    49,   213,    34,    98,    99,
     100,   249,   101,   152,   102,   103,    35,    10,    11,   104,
     105,   106,   107,   221,    36,    37,   108,    12,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   138,   139,   140,
     118,    38,    39,   119,   120,   121,   122,   123,   124,   125,
     126,   127,    13,    40,   128,   129,   130,    14,   161,    15,
     131,   132,   133,    16,    17,   134,    18
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -203
static const yytype_int16 yypact[] =
{
     170,  -203,  -203,    57,    69,    37,    83,  -203,  -203,    38,
    -203,    66,  -203,  -203,  -203,  -203,  -203,  -203,   156,  -203,
    -203,  -203,  -203,    71,    80,  -203,  -203,    84,    89,  -203,
      38,    72,  -203,   104,    35,  -203,  -203,   -15,    12,  -203,
    -203,   242,  -203,  -203,  -203,  -203,  -203,   140,  -203,   144,
    -203,   170,    44,  -203,    38,    35,   124,  -203,  -203,   143,
     147,   335,   158,   179,   164,   139,   173,   309,  -203,  -203,
     581,   581,   581,   581,   581,   581,   608,   608,    27,  -203,
    -203,   150,  -203,   149,   152,  -203,  -203,  -203,   181,  -203,
    -203,  -203,  -203,  -203,  -203,  -203,   269,    38,  -203,    -7,
     183,  -203,  -203,  -203,  -203,  -203,  -203,  -203,  -203,  -203,
     172,  -203,   171,  -203,    87,    96,  -203,  -203,  -203,  -203,
     -12,  -203,  -203,  -203,  -203,    59,  -203,   107,  -203,  -203,
    -203,   620,  -203,  -203,  -203,  -203,  -203,     8,  -203,   185,
     197,  -203,  -203,  -203,   199,  -203,  -203,  -203,  -203,  -203,
     201,   362,   335,  -203,   196,   581,  -203,   198,   581,    -3,
     203,   207,  -203,  -203,  -203,  -203,  -203,  -203,   581,  -203,
    -203,   581,   581,   581,   581,   581,   581,  -203,  -203,   581,
     581,  -203,   335,   581,   581,   581,   581,   581,   581,   384,
     581,  -203,  -203,   581,   581,   581,   581,   581,   581,   581,
     581,   581,   581,   581,   581,   581,   581,   581,   581,     8,
     581,  -203,  -203,   208,    62,   170,  -203,  -203,   212,   411,
    -203,   206,  -203,  -203,   213,  -203,   214,    -3,  -203,    46,
    -203,   581,    96,    96,    -7,   152,   150,  -203,    59,    59,
    -203,   149,   193,   171,  -203,  -203,  -203,  -203,  -203,    20,
     215,   107,   107,   107,   107,    27,    27,  -203,  -203,  -203,
    -203,  -203,  -203,  -203,  -203,  -203,  -203,   216,   218,   433,
     208,  -203,   581,   460,   223,   483,   335,   335,  -203,   581,
    -203,   581,  -203,  -203,  -203,  -203,   237,   243,  -203,   244,
     510,   532,   236,   231,  -203,  -203,  -203,  -203,   241,  -203,
    -203,   256,  -203,   266,   559,   335,  -203,  -203,  -203,  -203,
     271,  -203,  -203
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -203,  -117,    -6,  -203,  -203,  -126,   117,  -203,   114,   125,
    -203,   -35,  -203,  -203,  -203,   289,   -46,  -203,  -203,  -202,
    -203,   -32,   205,  -203,   -34,     4,  -198,   -36,  -203,   131,
     -62,  -203,  -203,  -203,  -203,  -203,  -203,  -203,  -203,  -203,
    -203,  -203,  -203,  -203,   251,  -203,  -203,  -203,  -203,  -203,
    -203,   126,  -203,  -203,    22,   252,  -203,   102,  -203,  -203,
    -203,   -30,  -203,  -203,  -203,  -203,  -203,  -203,  -203,    29,
    -203,   -76,  -203,  -203,   -61,  -203,  -203,   300,  -203,  -203,
     -48,  -203,  -203,  -203,  -203,  -203,  -203
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -114
static const yytype_int16 yytable[] =
{
     150,    50,    55,   157,   145,   160,   143,    97,   268,    94,
     189,    29,   190,    33,    53,    29,   270,   137,    54,   227,
     211,   210,   145,   145,   145,   145,   145,   145,   169,   170,
     209,   270,   210,   159,    30,   162,   163,   164,   165,   166,
     167,    29,   228,   280,    21,   179,   180,   141,    57,    58,
     237,    22,    23,   281,    24,   191,   192,    51,   146,    52,
      30,    31,    97,   248,    94,    29,    67,   286,   227,   142,
     210,    19,    31,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,    20,   209,    45,   210,   171,   172,   220,
      25,   222,    41,   224,    46,    70,   226,    26,    27,    47,
      28,   146,    31,    48,    71,    72,   160,   214,    73,    74,
      75,    76,    77,   193,   194,   195,   196,   251,   252,   253,
     254,   240,   242,   145,   145,   145,   145,   145,   250,   229,
     -86,   145,   145,   184,   185,   145,    31,   145,   145,   145,
     145,   212,   141,    57,    58,   145,   145,   145,   145,   145,
     145,   244,   245,   246,   135,   296,    43,   274,   186,   187,
     188,    67,   145,     1,   143,   197,   198,   136,   156,  -113,
       2,     3,   148,     4,     5,     6,   149,     1,    55,   214,
     151,   137,   153,   145,     2,     3,   155,     4,     5,     6,
      70,   255,   256,   232,   233,   158,   278,   229,   174,    71,
      72,   173,   175,    73,    74,    75,    76,    77,   238,   239,
     287,   289,   181,   292,   176,   293,   294,   182,   215,   183,
     216,   145,   218,   143,   217,   223,   230,   225,   301,   303,
     231,   145,   269,   295,   272,   275,   276,   277,   279,   283,
     282,   305,   310,   284,   311,    56,    57,    58,    59,     1,
      60,    61,   290,    62,    63,    64,     2,     3,    65,     4,
       5,     6,   297,    66,    67,   304,   298,   299,    41,    68,
     306,    69,    56,    57,    58,    59,     1,    60,    61,   307,
      62,    63,    64,     2,     3,    65,     4,     5,     6,   308,
      66,    67,   236,    70,   312,    41,   177,   241,    69,   235,
      42,   178,    71,    72,   234,   147,    73,    74,    75,    76,
      77,   243,   141,    57,    58,   154,     1,   271,    44,     0,
      70,     0,     0,     2,     3,     0,     4,     5,     6,    71,
      72,    67,     0,    73,    74,    75,    76,    77,    56,    57,
      58,    59,     0,    60,    61,     0,    62,    63,    64,     0,
       0,    65,     0,     0,     0,     0,    66,    67,     0,     0,
      70,    41,     0,     0,    69,   141,    57,    58,     0,    71,
      72,     0,     0,    73,    74,    75,    76,    77,     0,     0,
       0,     0,     0,     0,    67,     0,    70,   141,    57,    58,
       0,   219,     0,     0,     0,    71,    72,     0,     0,    73,
      74,    75,    76,    77,     0,     0,    67,   247,     0,     0,
       0,     0,     0,    70,   141,    57,    58,     0,     0,     0,
       0,     0,    71,    72,     0,     0,    73,    74,    75,    76,
      77,     0,     0,    67,     0,    70,   141,    57,    58,     0,
     273,     0,     0,     0,    71,    72,     0,     0,    73,    74,
      75,    76,    77,     0,     0,    67,     0,     0,   285,     0,
       0,     0,    70,   141,    57,    58,     0,     0,     0,     0,
       0,    71,    72,     0,     0,    73,    74,    75,    76,    77,
       0,     0,    67,   288,    70,     0,   141,    57,    58,     0,
       0,     0,     0,    71,    72,     0,     0,    73,    74,    75,
      76,    77,     0,     0,     0,    67,     0,     0,     0,     0,
       0,    70,   291,   141,    57,    58,     0,     0,     0,     0,
      71,    72,     0,     0,    73,    74,    75,    76,    77,     0,
       0,     0,    67,   300,    70,   141,    57,    58,     0,     0,
       0,     0,     0,    71,    72,     0,     0,    73,    74,    75,
      76,    77,     0,     0,    67,   302,     0,     0,     0,     0,
       0,    70,   141,    57,    58,     0,     0,     0,     0,     0,
      71,    72,     0,     0,    73,    74,    75,    76,    77,     0,
       0,    67,   309,    70,   141,    57,    58,     0,     0,     0,
       0,     0,    71,    72,     0,     0,    73,    74,    75,    76,
      77,     0,     0,    67,     0,     0,     0,     0,     0,     0,
      70,   141,    57,    58,     0,     0,     0,     0,     0,    71,
      72,     0,     0,    73,    74,    75,    76,    77,     0,     0,
     168,     0,    70,     0,     0,     0,     0,     0,     0,     0,
       0,    71,    72,     0,     0,    73,    74,    75,    76,    77,
       0,     0,     0,     0,   199,   200,   201,   202,   203,    70,
     204,   205,   206,   207,   208,     0,     0,     0,    71,    72,
       0,     0,    73,    74,    75,    76,    77
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-203))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      61,    31,    38,    65,    52,    67,    52,    41,   210,    41,
      22,     3,    24,     9,    29,     3,   214,    51,    33,    22,
     137,    24,    70,    71,    72,    73,    74,    75,    76,    77,
      22,   229,    24,    67,    22,    70,    71,    72,    73,    74,
      75,     3,   159,    23,     7,    52,    53,     3,     4,     5,
     176,    14,    15,    33,    17,    67,    68,    22,    54,    24,
      22,    64,    96,   189,    96,     3,    22,   269,    22,    25,
      24,    14,    64,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,    14,    22,    14,    24,    60,    61,   151,
       7,   152,    26,   155,    14,    51,   158,    14,    15,    15,
      17,    97,    64,    14,    60,    61,   168,   137,    64,    65,
      66,    67,    68,    54,    55,    56,    57,   193,   194,   195,
     196,   182,   184,   171,   172,   173,   174,   175,   190,   159,
      26,   179,   180,    46,    47,   183,    64,   185,   186,   187,
     188,   137,     3,     4,     5,   193,   194,   195,   196,   197,
     198,   186,   187,   188,    14,   281,     0,   219,    62,    63,
      64,    22,   210,     7,   210,    58,    59,    23,    29,    45,
      14,    15,    29,    17,    18,    19,    29,     7,   214,   209,
      22,   215,     3,   231,    14,    15,    22,    17,    18,    19,
      51,   197,   198,   171,   172,    22,   231,   227,    49,    60,
      61,    51,    50,    64,    65,    66,    67,    68,   179,   180,
     272,   273,    29,   275,    33,   276,   277,    45,    33,    48,
      23,   269,    21,   269,    25,    29,    23,    29,   290,   291,
      23,   279,    24,   279,    22,    29,    23,    23,    45,    23,
      25,    10,   304,    25,   305,     3,     4,     5,     6,     7,
       8,     9,    29,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    25,    21,    22,    29,    23,    23,    26,    27,
      29,    29,     3,     4,     5,     6,     7,     8,     9,    23,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    23,
      21,    22,   175,    51,    23,    26,    27,   183,    29,   174,
      11,    96,    60,    61,   173,    54,    64,    65,    66,    67,
      68,   185,     3,     4,     5,    63,     7,   215,    18,    -1,
      51,    -1,    -1,    14,    15,    -1,    17,    18,    19,    60,
      61,    22,    -1,    64,    65,    66,    67,    68,     3,     4,
       5,     6,    -1,     8,     9,    -1,    11,    12,    13,    -1,
      -1,    16,    -1,    -1,    -1,    -1,    21,    22,    -1,    -1,
      51,    26,    -1,    -1,    29,     3,     4,     5,    -1,    60,
      61,    -1,    -1,    64,    65,    66,    67,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    22,    -1,    51,     3,     4,     5,
      -1,    29,    -1,    -1,    -1,    60,    61,    -1,    -1,    64,
      65,    66,    67,    68,    -1,    -1,    22,    23,    -1,    -1,
      -1,    -1,    -1,    51,     3,     4,     5,    -1,    -1,    -1,
      -1,    -1,    60,    61,    -1,    -1,    64,    65,    66,    67,
      68,    -1,    -1,    22,    -1,    51,     3,     4,     5,    -1,
      29,    -1,    -1,    -1,    60,    61,    -1,    -1,    64,    65,
      66,    67,    68,    -1,    -1,    22,    -1,    -1,    25,    -1,
      -1,    -1,    51,     3,     4,     5,    -1,    -1,    -1,    -1,
      -1,    60,    61,    -1,    -1,    64,    65,    66,    67,    68,
      -1,    -1,    22,    23,    51,    -1,     3,     4,     5,    -1,
      -1,    -1,    -1,    60,    61,    -1,    -1,    64,    65,    66,
      67,    68,    -1,    -1,    -1,    22,    -1,    -1,    -1,    -1,
      -1,    51,    29,     3,     4,     5,    -1,    -1,    -1,    -1,
      60,    61,    -1,    -1,    64,    65,    66,    67,    68,    -1,
      -1,    -1,    22,    23,    51,     3,     4,     5,    -1,    -1,
      -1,    -1,    -1,    60,    61,    -1,    -1,    64,    65,    66,
      67,    68,    -1,    -1,    22,    23,    -1,    -1,    -1,    -1,
      -1,    51,     3,     4,     5,    -1,    -1,    -1,    -1,    -1,
      60,    61,    -1,    -1,    64,    65,    66,    67,    68,    -1,
      -1,    22,    23,    51,     3,     4,     5,    -1,    -1,    -1,
      -1,    -1,    60,    61,    -1,    -1,    64,    65,    66,    67,
      68,    -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      51,     3,     4,     5,    -1,    -1,    -1,    -1,    -1,    60,
      61,    -1,    -1,    64,    65,    66,    67,    68,    -1,    -1,
      22,    -1,    51,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    60,    61,    -1,    -1,    64,    65,    66,    67,    68,
      -1,    -1,    -1,    -1,    34,    35,    36,    37,    38,    51,
      40,    41,    42,    43,    44,    -1,    -1,    -1,    60,    61,
      -1,    -1,    64,    65,    66,    67,    68
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     7,    14,    15,    17,    18,    19,    82,    90,    93,
     106,   107,   116,   141,   146,   148,   152,   153,   155,    14,
      14,     7,    14,    15,    17,     7,    14,    15,    17,     3,
      22,    64,    73,    94,    96,   105,   113,   114,   130,   131,
     142,    26,    84,     0,   146,    14,    14,    15,    14,    94,
     130,    22,    24,    29,    33,    96,     3,     4,     5,     6,
       8,     9,    11,    12,    13,    16,    21,    22,    27,    29,
      51,    60,    61,    64,    65,    66,    67,    68,    71,    72,
      74,    75,    76,    77,    78,    79,    80,    81,    83,    84,
      85,    86,    87,    89,    90,    91,    92,    93,    97,    98,
      99,   101,   103,   104,   108,   109,   110,   111,   115,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   129,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   143,   144,
     145,   149,   150,   151,   154,    14,    23,    93,   126,   127,
     128,     3,    25,    85,    88,   149,    94,   113,    29,    29,
     143,    22,   102,     3,   124,    22,    29,    99,    22,    93,
      99,   147,    80,    80,    80,    80,    80,    80,    22,   149,
     149,    60,    61,    51,    49,    50,    33,    27,    91,    52,
      53,    29,    45,    48,    46,    47,    62,    63,    64,    22,
      24,    67,    68,    54,    55,    56,    57,    58,    59,    34,
      35,    36,    37,    38,    40,    41,    42,    43,    44,    22,
      24,    70,    94,    95,   130,    33,    23,    25,    21,    29,
      99,   112,   143,    29,    99,    29,    99,    22,    70,   130,
      23,    23,   123,   123,    98,    78,    75,    74,   138,   138,
     143,    77,    99,   120,    80,    80,    80,    23,    74,   100,
      99,   140,   140,   140,   140,    71,    71,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    70,    88,    24,
      95,   126,    22,    29,    99,    29,    23,    23,    80,    45,
      23,    33,    25,    23,    25,    25,    88,    99,    23,    99,
      29,    29,    99,   143,   143,    85,    74,    25,    23,    23,
      23,    99,    23,    99,    29,    10,    29,    23,    23,    23,
      99,   143,    23
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (&yylloc, root, error_count, scanner, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc, scanner)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, root, error_count, scanner); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, YYSTYPE *root, int *error_count, yyscan_t scanner)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, root, error_count, scanner)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    YYSTYPE *root;
    int *error_count;
    yyscan_t scanner;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (root);
  YYUSE (error_count);
  YYUSE (scanner);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, YYSTYPE *root, int *error_count, yyscan_t scanner)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, root, error_count, scanner)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    YYSTYPE *root;
    int *error_count;
    yyscan_t scanner;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, root, error_count, scanner);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, YYSTYPE *root, int *error_count, yyscan_t scanner)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, root, error_count, scanner)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    YYSTYPE *root;
    int *error_count;
    yyscan_t scanner;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , root, error_count, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, root, error_count, scanner); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, YYSTYPE *root, int *error_count, yyscan_t scanner)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, root, error_count, scanner)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    YYSTYPE *root;
    int *error_count;
    yyscan_t scanner;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (root);
  YYUSE (error_count);
  YYUSE (scanner);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (YYSTYPE *root, int *error_count, yyscan_t scanner);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (YYSTYPE *root, int *error_count, yyscan_t scanner)
#else
int
yyparse (root, error_count, scanner)
    YYSTYPE *root;
    int *error_count;
    yyscan_t scanner;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 6:

/* Line 1806 of yacc.c  */
#line 61 "parser.y"
    {(yyval) = node_two_operands(NODE_PLUS, "+", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 63 "parser.y"
    {(yyval) = node_two_operands(NODE_MINUS, "-", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 77 "parser.y"
    {(yyval) = node_two_operands(NODE_ARRAY_DECL, "arraydecl", (yyvsp[(1) - (4)]), (yyvsp[(3) - (4)]), yylloc);}
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 79 "parser.y"
    {(yyval) = node_two_operands(NODE_ARRAY_DECL, "arraydecl", (yyvsp[(1) - (3)]), NULL, yylloc);}
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 85 "parser.y"
    {(yyval) = node_two_operands(NODE_EQUAL, "=", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 87 "parser.y"
    {(yyval) = node_two_operands(NODE_PLUS_EQUAL, "+=", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 89 "parser.y"
    {(yyval) = node_two_operands(NODE_MINUS_EQUAL, "-=", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 91 "parser.y"
    {(yyval) = node_two_operands(NODE_ASTERISK_EQUAL, "*=", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 93 "parser.y"
    {(yyval) = node_two_operands(NODE_PERCENT_EQUAL, "%=", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 95 "parser.y"
    {(yyval) = node_two_operands(NODE_LESS_LESS_EQUAL, "<<=", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 97 "parser.y"
    {(yyval) = node_two_operands(NODE_GREATER_GREATER_EQUAL, ">>=", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 99 "parser.y"
    {(yyval) = node_two_operands(NODE_AMPERSAND_EQUAL, "&=", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 101 "parser.y"
    {(yyval) = node_two_operands(NODE_CARET_EQUAL, "^=", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 103 "parser.y"
    {(yyval) = node_two_operands(NODE_VBAR_EQUAL, "|=", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 128 "parser.y"
    {(yyval) =  node_two_operands(NODE_AMPERSAND, "&", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 133 "parser.y"
    {(yyval) = node_one_operand(NODE_TILDE, "~", (yyvsp[(2) - (2)]), yylloc);}
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 139 "parser.y"
    {(yyval) = node_two_operands(NODE_VBAR, "|", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 145 "parser.y"
    {(yyval) = node_two_operands(NODE_CARET, "^", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 151 "parser.y"
    {(yyval) = node_one_operand(NODE_BREAK, "break", NULL, yylloc);}
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 159 "parser.y"
    {(yyval) = node_two_operands(NODE_CAST, "cast", (yyvsp[(2) - (4)]), (yyvsp[(4) - (4)]), yylloc);}
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 167 "parser.y"
    {(yyval) = node_one_operand(NODE_SIGNED_CHAR, "signed char", NULL, yylloc);}
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 168 "parser.y"
    {(yyval) = node_one_operand(NODE_SIGNED_CHAR, "signed char", NULL, yylloc);}
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 169 "parser.y"
    {(yyval) = node_one_operand(NODE_UNSIGNED_CHAR, "unsigned char", NULL, yylloc);}
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 179 "parser.y"
    {(yyval) = (yyvsp[(2) - (3)]);}
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 180 "parser.y"
    {(yyval) = node_null_statement(yylloc);}
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 186 "parser.y"
    { (yyval) = node_three_operands(NODE_TERNARY_OP, "?:", (yyvsp[(1) - (5)]), (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)]), yylloc);}
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 205 "parser.y"
    {(yyval) = node_one_operand(NODE_CONTINUE, "continue", NULL, yylloc);}
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 212 "parser.y"
    {(yyval) = node_two_operands(NODE_DECL, "decl", (yyvsp[(1) - (3)]), (yyvsp[(2) - (3)]), yylloc); printf("parse node: decl\n");}
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 219 "parser.y"
    {printf("parse node: declaration_or_statement\n");}
    break;

  case 51:

/* Line 1806 of yacc.c  */
#line 220 "parser.y"
    {printf("parse node: declaration_or_statement\n");}
    break;

  case 52:

/* Line 1806 of yacc.c  */
#line 224 "parser.y"
    {(yyval) = node_two_operands(NODE_STATEMENT_LIST, "stmtlst", NULL, (yyvsp[(1) - (1)]), yylloc);printf("parse node: declaration_or_statement_list---declaration_or_statement\n");}
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 226 "parser.y"
    { (yyval) = node_two_operands(NODE_STATEMENT_LIST, "stmtlst", (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)]), yylloc); printf("parse node: declaration_or_statement_list---declaration_or_statement_list declaration_or_statement\n");}
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 230 "parser.y"
    {printf("parse node: declaration_specifiers\n");}
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 251 "parser.y"
    {printf("parse node: direct_declarator--simple declarator\n");}
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 253 "parser.y"
    {printf("parse node: direct_declarator--function declarator\n");}
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 264 "parser.y"
    {(yyval) = node_two_operands(NODE_EQUAL_EQUAL, "==", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 266 "parser.y"
    {(yyval) = node_two_operands(NODE_NOT_EQUAL, "!=", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 280 "parser.y"
    { (yyval) = node_two_operands(NODE_EXPR_LIST, "expr_lst", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 284 "parser.y"
    {(yyval) = (yyvsp[(1) - (2)]);}
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 291 "parser.y"
    {(yyval) = node_three_operands(NODE_FOR_EXPR, "", (yyvsp[(2) - (7)]), (yyvsp[(4) - (7)]), (yyvsp[(6) - (7)]), yylloc);}
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 293 "parser.y"
    {(yyval) = node_three_operands(NODE_FOR_EXPR, "", NULL, (yyvsp[(3) - (6)]), (yyvsp[(5) - (6)]), yylloc);}
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 295 "parser.y"
    {(yyval) = node_three_operands(NODE_FOR_EXPR, "", NULL, NULL, (yyvsp[(4) - (5)]), yylloc);}
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 297 "parser.y"
    {(yyval) = node_three_operands(NODE_FOR_EXPR, "", NULL, NULL, NULL, yylloc);}
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 299 "parser.y"
    {(yyval) = node_three_operands(NODE_FOR_EXPR, "", (yyvsp[(2) - (6)]), NULL, (yyvsp[(5) - (6)]), yylloc);}
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 301 "parser.y"
    {(yyval) = node_three_operands(NODE_FOR_EXPR, "", (yyvsp[(2) - (6)]), (yyvsp[(4) - (6)]), NULL, yylloc);}
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 303 "parser.y"
    {(yyval) = node_three_operands(NODE_FOR_EXPR, "", (yyvsp[(2) - (5)]), NULL, NULL, yylloc);}
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 305 "parser.y"
    {(yyval) = node_three_operands(NODE_FOR_EXPR, "", NULL, (yyvsp[(3) - (5)]), NULL, yylloc);}
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 310 "parser.y"
    {(yyval) = node_two_operands(NODE_FOR, "for", (yyvsp[(2) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 314 "parser.y"
    {(yyval) = node_two_operands(NODE_FUNC_CALL, "func_call", (yyvsp[(1) - (4)]), (yyvsp[(3) - (4)]), yylloc);}
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 315 "parser.y"
    {(yyval) = node_two_operands(NODE_FUNC_CALL, "func_call", (yyvsp[(1) - (3)]), NULL, yylloc);}
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 320 "parser.y"
    { (yyval) = node_two_operands(NODE_FUNC_DECL, "funcdecl", (yyvsp[(1) - (4)]), (yyvsp[(3) - (4)]), yylloc); printf("parse node: function_declarator\n");}
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 325 "parser.y"
    {(yyval) = node_two_operands(NODE_FUNC_DEF, "funcdef", (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)]), yylloc); printf("parse node: function_definition\n");}
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 329 "parser.y"
    { (yyval) = node_two_operands(NODE_FUNC_DEF_SPEC, "funcspec", (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)]), yylloc); printf("parse node: function_def_specifier\n");}
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 338 "parser.y"
    {(yyval) = node_one_operand(NODE_GOTO, "goto", (yyvsp[(2) - (3)]), yylloc);}
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 344 "parser.y"
    {(yyval) = node_three_operands(NODE_IF_ELSE, "if else", (yyvsp[(3) - (7)]), (yyvsp[(5) - (7)]), (yyvsp[(7) - (7)]), yylloc);}
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 349 "parser.y"
    {(yyval) = node_two_operands(NODE_IF, "if", (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)]), yylloc);}
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 356 "parser.y"
    {(yyval) = node_one_operand(NODE_INDIRECT, "*", (yyvsp[(2) - (2)]), yylloc);}
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 364 "parser.y"
    { /*printf("parse node:initialized_declarator\n");*/}
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 368 "parser.y"
    { printf("parse node:initialized_declarator_list COMMA initialized_declarator\n");}
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 370 "parser.y"
    {(yyval) = node_two_operands(NODE_DECL_LIST, "decl_list", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc); printf("parse node:initialized_declarator_list COMMA initialized_declarator\n");}
    break;

  case 103:

/* Line 1806 of yacc.c  */
#line 394 "parser.y"
    {(yyval) = node_one_operand(NODE_LABEL, ":", (yyvsp[(1) - (3)]), yylloc);}
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 400 "parser.y"
    {(yyval) = node_two_operands(NODE_AMPERSAND_AMPERSAND, "&&", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 106:

/* Line 1806 of yacc.c  */
#line 404 "parser.y"
    {(yyval) = node_one_operand(NODE_EXCLAMATION, "!", (yyvsp[(2) - (2)]), yylloc);}
    break;

  case 108:

/* Line 1806 of yacc.c  */
#line 410 "parser.y"
    {(yyval) = node_two_operands(NODE_AMPERSAND_AMPERSAND, "&&", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 418 "parser.y"
    {(yyval) = node_two_operands(NODE_ASTERISK, "*", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 420 "parser.y"
    {(yyval) = node_two_operands(NODE_SLASH, "/", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 422 "parser.y"
    {(yyval) = node_two_operands(NODE_PERCENT, "%", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 114:

/* Line 1806 of yacc.c  */
#line 440 "parser.y"
    { (yyval) = node_null_statement(yylloc);}
    break;

  case 115:

/* Line 1806 of yacc.c  */
#line 447 "parser.y"
    {(yyval) = node_two_operands(NODE_PARAM, "param", (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)]), yylloc);}
    break;

  case 116:

/* Line 1806 of yacc.c  */
#line 449 "parser.y"
    {(yyval) = node_two_operands(NODE_PARAM, "param", (yyvsp[(1) - (1)]), NULL, yylloc);}
    break;

  case 119:

/* Line 1806 of yacc.c  */
#line 456 "parser.y"
    {(yyval) = node_two_operands(NODE_PARAM_LIST, "params", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 121:

/* Line 1806 of yacc.c  */
#line 465 "parser.y"
    {(yyval) = (yyvsp[(2) - (3)]);}
    break;

  case 122:

/* Line 1806 of yacc.c  */
#line 469 "parser.y"
    {(yyval) = node_one_operand(NODE_POINTER, "*", NULL, yylloc);}
    break;

  case 123:

/* Line 1806 of yacc.c  */
#line 470 "parser.y"
    {(yyval) = node_one_operand(NODE_POINTER, "*", (yyvsp[(2) - (2)]), yylloc);}
    break;

  case 124:

/* Line 1806 of yacc.c  */
#line 475 "parser.y"
    { (yyval) = node_two_operands(NODE_POINTER_DECL, "pointerdecl", (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)]), yylloc);}
    break;

  case 125:

/* Line 1806 of yacc.c  */
#line 479 "parser.y"
    {(yyval) = node_one_operand(NODE_POST_DECR, "--", (yyvsp[(2) - (2)]), yylloc);}
    break;

  case 131:

/* Line 1806 of yacc.c  */
#line 491 "parser.y"
    {(yyval) = node_one_operand(NODE_POST_INCR, "++", (yyvsp[(2) - (2)]), yylloc);}
    break;

  case 132:

/* Line 1806 of yacc.c  */
#line 495 "parser.y"
    {(yyval) = node_one_operand(NODE_PRE_DECR, "--", (yyvsp[(2) - (2)]), yylloc);}
    break;

  case 133:

/* Line 1806 of yacc.c  */
#line 499 "parser.y"
    {(yyval) = node_one_operand(NODE_PRE_INCR, "++", (yyvsp[(2) - (2)]), yylloc);}
    break;

  case 138:

/* Line 1806 of yacc.c  */
#line 514 "parser.y"
    {(yyval) = node_two_operands(NODE_LESS, "<", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 139:

/* Line 1806 of yacc.c  */
#line 516 "parser.y"
    {(yyval) = node_two_operands(NODE_LESS_EQUAL, "<=", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 140:

/* Line 1806 of yacc.c  */
#line 518 "parser.y"
    {(yyval) = node_two_operands(NODE_GREATER, ">", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 141:

/* Line 1806 of yacc.c  */
#line 520 "parser.y"
    {(yyval) = node_two_operands(NODE_GREATER_EQUAL, ">=", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 142:

/* Line 1806 of yacc.c  */
#line 533 "parser.y"
    {(yyval) = node_one_operand(NODE_RETURN, "return", (yyvsp[(2) - (3)]), yylloc);}
    break;

  case 143:

/* Line 1806 of yacc.c  */
#line 534 "parser.y"
    {(yyval) = node_one_operand(NODE_RETURN, "return", NULL, yylloc);}
    break;

  case 145:

/* Line 1806 of yacc.c  */
#line 541 "parser.y"
    {(yyval) = node_two_operands(NODE_SHIFT_LEFT, "<<", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 146:

/* Line 1806 of yacc.c  */
#line 543 "parser.y"
    {(yyval) = node_two_operands(NODE_SHIFT_RIGHT, ">>", (yyvsp[(1) - (3)]), (yyvsp[(3) - (3)]), yylloc);}
    break;

  case 147:

/* Line 1806 of yacc.c  */
#line 554 "parser.y"
    {(yyval) = node_one_operand(NODE_SIGNED_SHORT, "signed short", NULL, yylloc);}
    break;

  case 148:

/* Line 1806 of yacc.c  */
#line 555 "parser.y"
    {(yyval) = node_one_operand(NODE_SIGNED_SHORT_INT, "signed short int", NULL, yylloc);}
    break;

  case 149:

/* Line 1806 of yacc.c  */
#line 556 "parser.y"
    {(yyval) = node_one_operand(NODE_SIGNED_SHORT, "signed short", NULL, yylloc);}
    break;

  case 150:

/* Line 1806 of yacc.c  */
#line 557 "parser.y"
    {(yyval) = node_one_operand(NODE_SIGNED_SHORT_INT, "signed short int", NULL, yylloc);}
    break;

  case 151:

/* Line 1806 of yacc.c  */
#line 558 "parser.y"
    {(yyval) = node_one_operand(NODE_SIGNED_INT, "signed int", NULL, yylloc);}
    break;

  case 152:

/* Line 1806 of yacc.c  */
#line 559 "parser.y"
    {(yyval) = node_one_operand(NODE_SIGNED_INT, "signed int", NULL, yylloc);}
    break;

  case 153:

/* Line 1806 of yacc.c  */
#line 560 "parser.y"
    {(yyval) = node_one_operand(NODE_SIGNED, "signed", NULL, yylloc);}
    break;

  case 154:

/* Line 1806 of yacc.c  */
#line 561 "parser.y"
    {(yyval) = node_one_operand(NODE_SIGNED_LONG, "signed long", NULL, yylloc);}
    break;

  case 155:

/* Line 1806 of yacc.c  */
#line 562 "parser.y"
    {(yyval) = node_one_operand(NODE_SIGNED_LONG_INT, "signed int", NULL, yylloc);}
    break;

  case 156:

/* Line 1806 of yacc.c  */
#line 563 "parser.y"
    {(yyval) = node_one_operand(NODE_SIGNED_LONG, "signed long", NULL, yylloc);}
    break;

  case 157:

/* Line 1806 of yacc.c  */
#line 564 "parser.y"
    {(yyval) = node_one_operand(NODE_SIGNED_LONG_INT, "signed long int", NULL, yylloc);}
    break;

  case 158:

/* Line 1806 of yacc.c  */
#line 569 "parser.y"
    {printf("parse node: simple_declarator\n");}
    break;

  case 170:

/* Line 1806 of yacc.c  */
#line 592 "parser.y"
    { 
   (yyval) = node_one_operand(NODE_INDIRECT, "*", node_two_operands(NODE_PLUS, "+", (yyvsp[(1) - (4)]), (yyvsp[(3) - (4)]), yylloc), yylloc);
}
    break;

  case 171:

/* Line 1806 of yacc.c  */
#line 598 "parser.y"
    {printf("parse node: top_level_decl--decl\n");}
    break;

  case 172:

/* Line 1806 of yacc.c  */
#line 599 "parser.y"
    {printf("parse node: top_level_decl--function_definition\n");}
    break;

  case 175:

/* Line 1806 of yacc.c  */
#line 613 "parser.y"
    {printf("parse node: type_specifier--integer_type_specifier\n");}
    break;

  case 176:

/* Line 1806 of yacc.c  */
#line 614 "parser.y"
    {printf("parse node: type_specifier--void_type_specifier\n");}
    break;

  case 188:

/* Line 1806 of yacc.c  */
#line 638 "parser.y"
    {(yyval) = node_one_operand(NODE_UNSIGNED_SHORT, "unsigned short", NULL, yylloc);}
    break;

  case 189:

/* Line 1806 of yacc.c  */
#line 639 "parser.y"
    {(yyval) = node_one_operand(NODE_UNSIGNED_SHORT_INT, "unsigned short int", NULL, yylloc);}
    break;

  case 190:

/* Line 1806 of yacc.c  */
#line 640 "parser.y"
    {(yyval) = node_one_operand(NODE_UNSIGNED_INT, "unsigned", NULL, yylloc);}
    break;

  case 191:

/* Line 1806 of yacc.c  */
#line 641 "parser.y"
    {(yyval) = node_one_operand(NODE_UNSIGNED_INT, "unsigned int", NULL, yylloc);}
    break;

  case 192:

/* Line 1806 of yacc.c  */
#line 642 "parser.y"
    {(yyval) = node_one_operand(NODE_UNSIGNED_LONG_LONG, "unsigned long long", NULL, yylloc);}
    break;

  case 193:

/* Line 1806 of yacc.c  */
#line 643 "parser.y"
    {(yyval) = node_one_operand(NODE_UNSIGNED_LONG_LONG_INT, "unsigned long long int", NULL, yylloc);}
    break;

  case 194:

/* Line 1806 of yacc.c  */
#line 648 "parser.y"
    {(yyval) = node_one_operand(NODE_VOID, "void", NULL, yylloc); /*printf("parse node: void_type_specifier\n");*/}
    break;

  case 195:

/* Line 1806 of yacc.c  */
#line 653 "parser.y"
    {(yyval) = node_two_operands(NODE_WHILE, "while", (yyvsp[(3) - (5)]), (yyvsp[(5) - (5)]), yylloc);}
    break;

  case 196:

/* Line 1806 of yacc.c  */
#line 657 "parser.y"
    {printf("parse node: translation_unit ---top_level_decl\n");}
    break;

  case 197:

/* Line 1806 of yacc.c  */
#line 659 "parser.y"
    {
    printf("parse node: translation_unit ---translation_unit top_level_decl\n");
    (yyval) = node_two_operands(NODE_TR_UNIT, "program", (yyvsp[(1) - (2)]), (yyvsp[(2) - (2)]), yylloc);
    *root = (yyval); 
   }
    break;



/* Line 1806 of yacc.c  */
#line 2717 "parser.tab.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, root, error_count, scanner, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, root, error_count, scanner, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc, root, error_count, scanner);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, root, error_count, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, root, error_count, scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, root, error_count, scanner);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, root, error_count, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 666 "parser.y"


static void yyerror(YYLTYPE *loc,
                    YYSTYPE *root __attribute__((unused)),
                    int *error_count,
                    yyscan_t scanner __attribute__((unused)),
                    char const *s)
{
  compiler_print_error(*loc, s);
  (*error_count)++;
}

struct node *parser_create_tree(int *error_count, yyscan_t scanner) {
  struct node *parse_tree;
  int result = yyparse(&parse_tree, error_count, scanner);
  if (result == 1 || *error_count > 0) {
    return NULL;
  } else if (result == 2) {
    fprintf(stdout, "Parser ran out of memory.\n");
    return NULL;
  } else {
    return parse_tree;
  }
}

char const *parser_token_name(int token) {
  return yytname[token - 255];
}


