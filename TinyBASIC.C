/*
 * Tiny BASIC Interpreter
 *
 *  File: TinyBASIC.C
 * Author: Theo P. (theo_pap@otenet.gr)
 * Language used: C
 * Copyright: No copyright. You can do with this software whatever
 * you like.
 * Warranty: No warranty. Use this software at your own risk.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <math.h>
#include <malloc.h>

/*** CONSTANTS ***/

#define PROG_SIZE 20*1024  /* 20K source size */
#define TOK_STR_LEN 256  /* token str len */
#define NUM_LBLS 512  /* num of labels */
#define NUM_FOR_NEST 32  /* num of FOR nesting levels */
#define NUM_WHILE_NEST 32  /* num of WHILE nesting levels */
#define NUM_DO_NEST 32  /* num of DO nesting levels */
#define NUM_GOSUB_NEST 32  /* num of GOSUB nesting levels */
#define NUM_VARS 26  /* num of predefined vars A ... Z */
#define MAX_STACK 100  /* arithmetic stack size */
#define MAX_ERRORS 10  /* num of errors */
#define SCR_LINE_WIDTH 50  /* line width displayed on screen */

/*** ERROR ***/
enum ErrCode  /* error code */
{
  ecEXPR_MISSING,
  ecEQ_MISSING,
  ecCOMMA_MISSING,
  ecVAR_MISSING,
  ecREL_OP_MISSING,
  ecQUOTE_MISSING,
  ecLPAR_MISSING,
  ecRPAR_MISSING,
  ecEND_MISSING,
  ecTO_MISSING,
  ecSTEP_MISSING,
  ecTHEN_MISSING,
  ecNEXT_MISSING,
  ecWEND_MISSING,

  ecUNBAL_PAR,
  ecNOT_VAR,
  ecNOT_REL_OP,
  ecUNREC_TOKEN,
  ecUNEXP_TOKEN,
  ecILL_VAR_NAME,
  ecILL_VAR_NAME_FOR,
  ecRAND_ARG_NEG,
  ecRAND_ARG_INT,
  ecRND_ARG_NEG,
  ecRND_ARG_INT,
  ecRND_WRONG_ARG,
  ecDIV_ZERO,
  ecEXP_NEG,
  ecEXP_NOT_INT,
  ecSQR_ARG_NEG,
  ecLOG_ARG_NEG,
  ecMOD_OPND_NOT_INT,
  ecPREC_ARG_NEG,
  ecPREC_ARG_INT,
  ecON_OFF_MISSING,

  ecTOO_MANY_FOR_NEST,
  ecNEXT_WITHOUT_FOR,
  ecSTEP_ZERO,

  ecTOO_MANY_WHILE_NEST,
  ecWEND_WITHOUT_WHILE,

  ecTOO_MANY_DO_NEST,
  ecUNTIL_WITHOUT_DO,

  ecTOO_MANY_GOSUB_NEST,
  ecRET_WITHOUT_GOSUB,

  ecLBL_FULL,
  ecLBL_EMPTY,
  ecLBL_DUPL,
  ecLBL_UNDEF,
  ecLBL_MISSING,

  ecSTK_FULL,
  ecSTK_EMPTY,

  ecGOSUB_FULL,
  ecGOSUB_EMPTY,

  ecFOR_FULL,
  ecFOR_EMPTY,

  ecWHILE_FULL,
  ecWHILE_EMPTY,

  ecDO_FULL,
  ecDO_EMPTY,

  ecEOT  /* end of table = terminal mark. Do not remove */
};

struct ErrTblItem  /* item of ErrTable */
{
  enum ErrCode Code;  /* error code */
  const char* Msg;  /* error message */
};

struct ErrTblItem ErrTable[] =  /* error table */
{
  ecEXPR_MISSING, "no expression present",
  ecEQ_MISSING, "equal sign = expected",
  ecCOMMA_MISSING, "comma , expected",
  ecVAR_MISSING, "variable expected",
  ecREL_OP_MISSING, "relational operator expected",
  ecQUOTE_MISSING, "closing quote \" missing:",
  ecLPAR_MISSING, "left parenthesis ( missing",
  ecRPAR_MISSING, "right parenthesis ) missing",
  ecEND_MISSING, "END missing",
  ecTO_MISSING, "TO expected",
  ecSTEP_MISSING, "STEP expected",
  ecTHEN_MISSING, "THEN expected",
  ecNEXT_MISSING, "NEXT expected",
  ecWEND_MISSING, "WEND expected",

  ecUNBAL_PAR, "unbalanced parentheses",
  ecNOT_VAR, "not a variable",
  ecNOT_REL_OP, "not a relational operator",
  ecUNREC_TOKEN, "unrecognized token",
  ecUNEXP_TOKEN, "unexpected token",
  ecILL_VAR_NAME, "illegal var name",
  ecILL_VAR_NAME_FOR, "illegal var name in FOR loop",
  ecRAND_ARG_NEG, "RANDOMIZE argument cannot be negative",
  ecRAND_ARG_INT, "RANDOMIZE argument must be integer",
  ecRND_ARG_NEG, "RND() argument canot be negative",
  ecRND_ARG_INT, "RND() argument must be integer",
  ecRND_WRONG_ARG, "wrong RND() arguments: must be a < b",
  ecDIV_ZERO, "division by 0 is illegal",
  ecEXP_NEG, "negative exponent is illegal",
  ecEXP_NOT_INT, "exponent must be integer number",
  ecSQR_ARG_NEG, "SQR() argument cannot be negative",
  ecLOG_ARG_NEG, "LOG() argument must be non-negative",
  ecMOD_OPND_NOT_INT, "% operands must be integers",
  ecPREC_ARG_NEG,  "PRECISION argument cannot be negative",
  ecPREC_ARG_INT,  "PRECISION argument must be integer",
  ecON_OFF_MISSING,  "ON or OFF expected",

  ecTOO_MANY_FOR_NEST, "too many nested FORs",
  ecNEXT_WITHOUT_FOR, "NEXT without FOR",
  ecSTEP_ZERO, "step is zero",

  ecTOO_MANY_WHILE_NEST, "too many nested WHILEs",
  ecWEND_WITHOUT_WHILE, "WEND without WHILE",

  ecTOO_MANY_DO_NEST, "too many nested DOs",
  ecUNTIL_WITHOUT_DO, "UNTIL without DO",

  ecTOO_MANY_GOSUB_NEST, "too many nested GOSUBs",
  ecRET_WITHOUT_GOSUB, "RETURN without GOSUB",

  ecLBL_FULL, "label table full",
  ecLBL_EMPTY, "label table empty",
  ecLBL_DUPL, "duplicate label",
  ecLBL_UNDEF, "undefined label",
  ecLBL_MISSING, "label missing",

  ecSTK_FULL, "cannot push: stack is full",
  ecSTK_EMPTY, "cannot pop: stack is empty",

  ecGOSUB_FULL, "cannot push: GOSUB stack is full",
  ecGOSUB_EMPTY, "cannot pop: GOSUB stack is empty",

  ecFOR_FULL, "cannot push: FOR stack is full",
  ecFOR_EMPTY, "cannot pop: FOR stack is empty",

  ecWHILE_FULL, "cannot push: WHILE stack is full",
  ecWHILE_EMPTY, "cannot pop: WHILE stack is empty",

  ecDO_FULL, "cannot push: DO stack is full",
  ecDO_EMPTY, "cannot pop: DO stack is empty",

  ecEOT,  ""  /* end of table = terminal mark. Do not remove. */
};

/*** DEFINITIONS ***/
enum TokCode  /* token code */
{
/* logical ops */
  tcOR,
  tcAND,
  tcNOT,

/* commands */
  tcIF,
  tcTHEN,
  tcELSE,
  tcENDIF,

  tcFOR,
  tcTO,
  tcSTEP,
  tcNEXT,

  tcWHILE,
  tcWEND,

  tcDO,
  tcUNTIL,

  tcBREAK,
  tcCONTINUE,

  tcGOTO,

  tcGOSUB,
  tcRETURN,

  tcEND,

  tcINPUT,
  tcPRINT,
  tcRANDOMIZE,

/* built-in funcs */
  tcABS,
  tcSGN,
  tcCINT,
  tcFIX,
  tcSQR,
  tcPOW,
  tcEXP,
  tcLOG,
  tcRND,

/* immediate commands */
  tcPRECISION,
  tcDEB_MODE,
  tcON,
  tcOFF,

/* arithmetic ops */
  tcPLUS,  /* + */
  tcMINUS,  /* - */
  tcSTAR,  /* * */
  tcSLASH,  /* / */
  tcPERC,  /* % */

/* parentheses ops */
  tcLPAR,  /* ( */
  tcRPAR,  /* ) */

/* relational ops */
  tcLT,  /* < */
  tcLE,  /* <= */
  tcGT,  /* > */
  tcGE,  /* >= */
  tcEQ,  /* = */
  tcNE,  /* <> */

/* misc */
  tcCOMMA,  /* , */
  tcSEMI,  /* ; */

/* tokens with user-defined content */
  tcVAR,  /* variable */
  tcNUM,  /* number literal */
  tcSTR,  /* string literal */

/* special */
  tcEOL,  /* end of line => ch = '\n' */
  tcEOF,  /* end of file => ch = 0 */

  tcINVALID  /* illegal token */
};

struct TokTblItem  /* item of TokTbl */
{
  enum TokCode Token;  /* token code */
  char Str[TOK_STR_LEN+1];  /* token str */
};

struct TokTblItem TokTbl[] =  /* token table */
{
/* logical ops */
  tcOR, "OR",
  tcAND, "AND",
  tcNOT, "NOT",

/* commands */
  tcIF, "IF",
  tcTHEN, "THEN",
  tcELSE, "ELSE",
  tcENDIF, "ENDIF",

  tcFOR, "FOR",
  tcTO, "TO",
  tcSTEP, "STEP",
  tcNEXT, "NEXT",

  tcWHILE, "WHILE",
  tcWEND, "WEND",

  tcDO, "DO",
  tcUNTIL, "UNTIL",

  tcBREAK, "BREAK",
  tcCONTINUE, "CONTINUE",

  tcGOTO, "GOTO",

  tcGOSUB, "GOSUB",
  tcRETURN, "RETURN",

  tcEND, "END",  /* end of prog */

  tcINPUT, "INPUT",
  tcPRINT, "PRINT",
  tcRANDOMIZE, "RANDOMIZE",

/* built-in funcs */
  tcABS, "ABS",
  tcSGN, "SGN",
  tcCINT, "CINT",
  tcFIX, "FIX",
  tcSQR, "SQR",
  tcPOW, "POW",
  tcEXP, "EXP",
  tcLOG, "LOG",
  tcRND, "RND",

/* immediate commands */
  tcPRECISION, "PRECISION",
  tcDEB_MODE, "DEB_MODE",
  tcON, "ON",
  tcOFF, "OFF",

/* arithmetic ops */
  tcPLUS, "+",
  tcMINUS, "-",
  tcSTAR, "*",
  tcSLASH, "/",
  tcPERC, "%",

/* parentheses ops */
  tcLPAR, "(",
  tcRPAR, ")",

/* relational ops */
  tcLT, "<",
  tcLE, "<=",
  tcGT, ">",
  tcGE, ">=",
  tcEQ, "=",
  tcNE, "<>",

/* misc */
  tcCOMMA, ",",
  tcSEMI, ";",

  tcINVALID, ""  /* terminal mark. Do not remove. */
};

struct LblTblItem  /* item of label table */
{
  char Name[TOK_STR_LEN+1];  /* label str */
  char* Loc;  /* loc of label in source */
  int Line;  /* line num of label in source */
};

struct ForStkItem  /* item of FOR stack */
{
  char Var;  /* name of var (counter) */
  double EndValue;  /* end value of counter */
  double StepValue;  /* step value of counter */
  char* Loc;  /* loc of FOR command in source */
};

struct WhileStkItem  /* item of WHILE stack */
{
  char Var;  /* var name */
  enum TokCode Op;  /* relational op */
  double Expr;  /* value to compare Var against */
  char* Loc;  /* loc of WHILE command in source */
};

struct DoStkItem  /* item of DO stack */
{
  char Var;  /* var name */
  enum TokCode Op;  /* relational op */
  double Expr;  /* value to compare Var against */
  char* Loc;  /* loc of DO command in source */
};

/*** GLOBAL VARS ***/
char* Source;  /* source buffer */
char* Prog;  /* current loc in source */
int Line;  /* current line num in source */

enum TokCode Token;  /* current token code */
char TokStr[TOK_STR_LEN+1];  /* current token str */

int ErrCounter;  /* error counter = num of errors occurred so far */
int Precision;  /* num of decimal places to display */
int DebMode;  /* debug mode on/off toggle switch */

struct LblTblItem LblTbl[NUM_LBLS];  /* label table */
int LblTblCounter;  /* label table counter */

char* GosubStk[NUM_GOSUB_NEST];  /* GOSUB stack */
int GosubStkTos;  /* GOSUB stack tos */

struct ForStkItem ForStk[NUM_FOR_NEST];  /* FOR stack */
int ForStkTos;  /* FOR stack tos */

struct WhileStkItem WhileStk[NUM_WHILE_NEST];  /* WHILE stack */
int WhileStkTos;  /* WHILE stack tos */

struct DoStkItem DoStk[NUM_DO_NEST];  /* DO stack */
int DoStkTos;  /* DO stack tos */

double Stk[MAX_STACK];  /* arithmetic values stack */
int StkTos;  /* stack tos */

double VarTbl[NUM_VARS];  /* var table = predefined vars A ... Z */

/*** FUNC PROTOTYPES ***/
/*** ERROR ***/
void Error(enum ErrCode ec);

/*** MISC ***/
int IsInt(double num);
int RoundOff(double num);
int Trunc(double num);
void DispCh(char ch, int count);
void DispLogValue(double value);
void DispFloat(double num, int ndp);

/*** LABEL TABLE ***/
void LblTblInit(void);
int LblTblIsEmpty(void);
int LblTblIsFull(void);
void LblTblInsert(const char* str, char* loc, int line);
char* LblTblFindLoc(const char* str);
void LblTblDisplay(void);

/*** GOSUB STACK ***/
void GosubStkInit(void);
int GosubStkIsEmpty(void);
int GosubStkIsFull(void);
void GosubStkPush(char* loc);
char* GosubStkPop(void);

/*** FOR STACK ***/
void ForStkInit(void);
int ForStkIsEmpty(void);
int ForStkIsFull(void);
void ForStkPush(struct ForStkItem* p);
struct ForStkItem* ForStkPop(void);
struct ForStkItem* ForStkPeek(void);

/*** WHILE STACK ***/
void WhileStkInit(void);
int WhileStkIsEmpty(void);
int WhileStkIsFull(void);
void WhileStkPush(struct WhileStkItem* p);
struct WhileStkItem* WhileStkPop(void);
struct WhileStkItem* WhileStkPeek(void);

/*** DO STACK ***/
void DoStkInit(void);
int DoStkIsEmpty(void);
int DoStkIsFull(void);
void DoStkPush(struct DoStkItem* p);
struct DoStkItem* DoStkPop(void);

/*** VAR TABLE ***/
void VarTblInit(void);
void VarTblSet(char var, double value);
double VarTblGet(char var);

/*** STACK ***/
void StkInit(void);
int StkIsEmpty(void);
int StkIsFull(void);
void StkPush(double value);
double StkPop(void);

/*** SCANNER ***/
int IsWhite(char ch);
void SkipWhite(void);
void SkipToEOL(void);
void ReadComment(void);
void ReadEOL(void);
void ReadNum(void);
void ReadStr(void);
void ReadAlpha(void);
void ReadOp1(void);
void ReadOp2(void);
void ReadOp3(void);
enum TokCode ReadToken(void);

/*** PARSER ***/
enum TokCode FindToken(const char* str);
const char* FindTokStr(enum TokCode tok);
int IsRelOp(enum TokCode tok);
int Compare(enum TokCode rel_op, double opnd1, double opnd2);
void SkipUntilToken(enum TokCode tok);
void SkipUntilToken2(enum TokCode tok1, enum TokCode tok2);
void SkipUntilToken3(enum TokCode tok1, enum TokCode tok2,
  enum TokCode tok3);

/*** EXPR CALCULATOR ***/
/*
 Precedence Table
 ------------------------------------------------
 op                     level  func
 ------------------------------------------------
 num  var  func       8   EvalFactor()
 ( )                        7   EvalPar()
 un+ un-                6   EvalUnPlusMinus()
 NOT                     5   EvalNot()
 * / %                   4   EvalMultDivMod()
 + -                      3   EvalAddSub()
 < <= > >= = <>  2   EvalComp()
 AND                     1   EvalAnd()
 OR                       0   EvalOr()
 ------------------------------------------------
*/

double EvalExpr(void);  /* entry point */
void EvalOr(void);  /* level 0 */
void EvalAnd(void);  /* level 1 */
void EvalComp(void);  /* level 2 */
void EvalAddSub(void);  /* level 3 */
void EvalMultDivMod(void);  /* level 4 */
void EvalNot(void);  /* level 5 */
void EvalUnPlusMinus(void);  /* level 6 */
void EvalPar(void);  /* level 7 */
void EvalFactor(void);  /* level 8 */

/* built-in funcs */
double EvalAbs(void);
double EvalSgn(void);
double EvalCInt(void);
double EvalFix(void);
double EvalSqr(void);
double EvalPow(void);
double EvalExp(void);
double EvalLog(void);
double EvalRnd(void);

/*** COMMAND EXECUTOR ***/
void ExecCmd(void);  /* entry point */
void ExecAssign(void);
void ExecIf(void);
void ExecElse(void);
void ExecEndIf(void);
void ExecGoto(void);
void ExecGosub(void);
void ExecReturn(void);
void ExecFor(void);
void ExecNext(void);
void ExecWhile(void);
void ExecWend(void);
void ExecDo(void);
void ExecUntil(void);
void ExecBreak(void);
void ExecContinue(void);
void ExecInput(void);
void ExecPrint(void);
void ExecRandomize(void);
void ExecPrecision(void);
void ExecDebMode(void);

/*** INTERPRETER ***/
void DispSource(void);
void DispTokens(void);
void FilterCR(void);
void LoadProg(const char* fname);
void ScanLabels(void);
void InitInterpreter(const char* fname);
void CloseInterpreter(void);

/*** FUNC DEFINITIONS ***/
/*** ERROR ***/
/*
 * A simple error reporter.
 */
void Error(enum ErrCode ec)
{
  int i;

  for (i = 0; ErrTable[i].Code != ecEOT; i++)
    if (ErrTable[i].Code == ec)
    {
      printf("\nERROR: Line = %d, Msg = %s.\n", Line,
        ErrTable[i].Msg);      

      ErrCounter++;

      if (ErrCounter >= MAX_ERRORS)
      {
        printf("\nToo many errors. Program aborted.\n\n");
        exit(1);
      }
    }
}

/*** MISC ***/
/*
 * Round-off a double num to the nearest int.
 * e.g.:
 * 2.4 -> 2, 2.5 -> 3
 * -2.4 -> -2, -2.5 -> -3
 */
int RoundOff(double num)
{
  int is_neg = 0, res;

  if (num < 0.0)
  {
    is_neg = 1;
    num = -num;
  }

  res = (int)(num + 0.5);

  if (is_neg)
    res = -res;

  return res;
}
/*
 * Truncate a double num to the smaller int.
 * e.g.:
 * 2.4 -> 2, 2.5 -> 2
 * -2.4 -> -2, -2.5 -> -2
*/
int Trunc(double num)
{
  int is_neg = 0, res;

  if (num < 0.0)
  {
    is_neg = 1;
    num = -num;
  }

  res = (int)num;

  if (is_neg)
    res = -res;

  return res;
}
/*
 * Return 1 if num is integer.
 */
int IsInt(double num)
{
  return num == (double)(int)num;
}
/*
 * Display a char count times.
 */

void DispCh(char ch, int count)
{
  while (count)
  {
    _putch(ch);
    count--;
  }
}
/*
 * Display a logical value as TRUE or FALSE.
 */
void DispLogValue(double value)
{
  value ? printf("TRUE") : printf("FALSE");
}
/*
 * Display a double num with the given precision ndp.
 * ndp = number of decimal places.
 * Must be 0 <= ndp <= 6
 */
void DispFloat(double num, int ndp)
{
  int i, ip;
  double fp;

  if (ndp > 6)
    ndp = 6;  /* max precision */

  if (num == 0.0)
  {
    printf("0");

    if (ndp == 0)
      return;

    printf(".");

    for (i = 0; i < ndp; i++)
      printf("0");

    return;
  }

  if (num < 0.0)
  {
    printf("-");
    num = -num;
  }

  for (i = 0; i < ndp; i++)
    num *= 10;

  num = (double)(int)(num + 0.5);

  for (i = 0; i < ndp; i++)
    num /= 10;

  ip = (int)num;  /* integer part */
  fp = num - (double)ip;  /* fractional part */
  printf("%d", ip);

  if (ndp == 0)
    return;

  printf(".");

  if (fp == 0.0)
  {
    for (i = 0; i < ndp; i++)
      printf("0");

    return;
  }

  for (i = 0; i < ndp; i++)
    fp *= 10;

  printf("%d", (int)fp);
}

/*** LABEL TABLE ***/
/*
 * Initialize the label table.
 */
void LblTblInit(void)
{
  int i;

  for (i = 0; i < NUM_LBLS; i++)
  {
    LblTbl[i].Name[0] = 0;
    LblTbl[i].Loc = NULL;
    LblTbl[i].Line = 0;
  }

  LblTblCounter = 0;
}
/*
 * Return 1 if label table is empty.
 */
int LblTblIsEmpty(void)
{
  return LblTblCounter == 0;
}
/*
 * Return 1 if label table is full.
 */
int LblTblIsFull(void)
{
  return LblTblCounter == NUM_LBLS;
}
/*
 * Insert a label info into the label table.
 */
void LblTblInsert(const char* name, char* loc, int line)
{
  if (LblTblIsFull())
  {
    Error(ecLBL_FULL);
    return;
  }

  strcpy(LblTbl[LblTblCounter].Name, name);
  LblTbl[LblTblCounter].Loc = loc;
  LblTbl[LblTblCounter++].Line = line;
}
/*
 * Find location of a label.
 */
char* LblTblFindLoc(const char* name)
{
  int i;

  for (i = 0; i < LblTblCounter; i++)
    if (!stricmp(LblTbl[i].Name, name))
      return LblTbl[i].Loc;

  return NULL;  /* no such label */
}
/*
 * Display the label table.
 * Useful for debug purposes.
 */
void LblTblDisplay(void)
{
  int i;

  if (LblTblIsEmpty())
  {
    printf("Label table is empty.\n\n");
    return;
  }

  DispCh('=', SCR_LINE_WIDTH);
  printf("\nLabel Table:\n\n");

  printf("Name  Line   Loc\n");

  DispCh('-', SCR_LINE_WIDTH);
  printf("\n");

  for (i = 0; i < LblTblCounter; i++)
    printf("%s    %3d    0x%p\n", LblTbl[i].Name, LblTbl[i].Line,
      LblTbl[i].Loc);

  DispCh('-', SCR_LINE_WIDTH);
  DispCh('\n', 2);

  printf("Labels = %d\n", LblTblCounter);

  DispCh('=', SCR_LINE_WIDTH);
  DispCh('\n', 2);
}

/*** STACK ***/
/*
 * Initialize the stack.
 */
void StkInit(void)
{
  int i;

  for (i = 0; i < MAX_STACK; i++)
   Stk[i] = 0.0;

  StkTos = 0;
}
/*
 * Return 1 if stack is empty.
 */
int StkIsEmpty(void)
{
  return StkTos == 0;
}
/*
 * Return 1 if stack is full.
 */
int StkIsFull(void)
{
  return StkTos == MAX_STACK;
}
/*
 * Push a number on stack.
 */
void StkPush(double value)
{
  if (StkIsFull())
  {
    Error(ecSTK_FULL);
    return;
  }

  Stk[StkTos++] = value;
}
/*
 * Pop a number from stack.
 */
double StkPop(void)
{
  if (StkIsEmpty())
  {
    Error(ecSTK_EMPTY);
    return 0.0;
  }	

  return Stk[--StkTos];
}

/*** GOSUB STACK ***/
/*
 * Initialize the GOSUB stack.
 */
void GosubStkInit(void)
{
  int i;

  for (i = 0; i < NUM_GOSUB_NEST; i++)
    GosubStk[i] = NULL;

  GosubStkTos = 0;
}
/*
 * Return 1 if GOSUB stack is empty.
 */
int GosubStkIsEmpty(void)
{
  return GosubStkTos == 0;
}
/*
 * Return 1 if GOSUB stack is full.
 */
int GosubStkIsFull(void)
{
  return GosubStkTos == NUM_GOSUB_NEST;
}
/*
 * Push a location on GOSUB stack.
 */
void GosubStkPush(char* loc)
{
  if (GosubStkIsFull())
  {
    Error(ecGOSUB_FULL);
    return;
  }

  GosubStk[GosubStkTos++] = loc;
}
/*
 * Pop a location from GOSUB stack.
 */
char* GosubStkPop(void)
{
  if (GosubStkIsEmpty())
  {
    Error(ecGOSUB_EMPTY);
    return NULL;
  }

  return GosubStk[--GosubStkTos];
}

/*** FOR STACK ***/
/*
 * Initialize the FOR stack.
 */
void ForStkInit(void)
{
  int i;

  for (i = 0; i < NUM_FOR_NEST; i++)
  {
    ForStk[i].Var = 0;
    ForStk[i].EndValue = ForStk[i].StepValue = 0.0;
    ForStk[i].Loc = NULL;
  }

  ForStkTos = 0;
}
/*
 * Return 1 if FOR stack is empty.
 */
int ForStkIsEmpty(void)
{
  return ForStkTos == 0;
}
/*
 * Return 1 if FOR stack is full.
 */
int ForStkIsFull(void)
{
  return ForStkTos == NUM_FOR_NEST;
}
/*
 * Push an item on FOR stack.
 */
void ForStkPush(struct ForStkItem* p)
{
  if (ForStkIsFull())
  {
    Error(ecFOR_FULL);
    return;
  }

  ForStk[ForStkTos++] = *p;
}
/*
 * Pop an item from FOR stack.
 */
struct ForStkItem* ForStkPop(void)
{
  if (ForStkIsEmpty())
  {
    Error(ecFOR_EMPTY);
    return NULL;
  }

  return &ForStk[--ForStkTos];
}
/*
 * Get the top item from FOR stack, without removing it.
 */
struct ForStkItem* ForStkPeek(void)
{
  return &ForStk[ForStkTos-1];
}

/*** WHILE STACK ***/
/*
 * Initialize the WHILE stack.
 */
void WhileStkInit(void)
{
  int i;

  for (i = 0; i < NUM_WHILE_NEST; i++)
  {
    WhileStk[i].Var = 0;
    WhileStk[i].Op = tcINVALID;
    WhileStk[i].Expr = 0.0;
    WhileStk[i].Loc = NULL;
  }

  WhileStkTos = 0;
}
/*
 * Return 1 if WHILE stack is empty.
*/
int WhileStkIsEmpty(void)
{
  return WhileStkTos == 0;
}
/*
 * Return 1 if WHILE stack is full.
 */
int WhileStkIsFull(void)
{
  return WhileStkTos == NUM_WHILE_NEST;
}
/*
 * Push an item on WHILE stack.
 */
void WhileStkPush(struct WhileStkItem* p)
{
  if (WhileStkIsFull())
  {
    Error(ecWHILE_FULL);
    return;
  }

  WhileStk[WhileStkTos++] = *p;
}
/*
 * Pop an item from WHILE stack.
 */
struct WhileStkItem* WhileStkPop(void)
{
  if (WhileStkIsEmpty())
  {
    Error(ecWHILE_EMPTY);
    return NULL;
  }

  return &WhileStk[--WhileStkTos];
}
/*
 * Get the top item from WHILE stack, without removing it.
 */
struct WhileStkItem* WhileStkPeek(void)
{
  if (WhileStkIsEmpty())
  {
    Error(ecWHILE_EMPTY);
    return NULL;
  }

  return &WhileStk[WhileStkTos-1];
}

/*** DO STACK ***/
/*
 * Initialize the DO stack.
 */
void DoStkInit(void)
{
  int i;

  for (i = 0; i < NUM_DO_NEST; i++)
  {
    DoStk[i].Var = 0;
    DoStk[i].Op = tcINVALID;
    DoStk[i].Expr = 0.0;
    DoStk[i].Loc = NULL;
  }

  DoStkTos = 0;
}
/*
 * Return 1 if DO stack is empty.
 */
int DoStkIsEmpty(void)
{
  return DoStkTos == 0;
}
/*
 * Return 1 if DO stack is full.
 */
int DoStkIsFull(void)
{
  return DoStkTos == NUM_DO_NEST;
}
/*
 * Push an item on DO stack.
 */
void DoStkPush(struct DoStkItem* p)
{
  if (DoStkIsFull())
  {
    Error(ecDO_FULL);
    return;
  }

  DoStk[DoStkTos++] = *p;
}
/*
 * Pop an item from DO stack.
 */
struct DoStkItem* DoStkPop(void)
{
  if (DoStkIsEmpty())
  {
    Error(ecDO_EMPTY);
    return NULL;
  }

  return &DoStk[--DoStkTos];
}

/*** VAR TABLE ***/
/*
 * Initialize the var table.
 */
void VarTblInit(void)
{
  int i;

  for (i = 0; i < NUM_VARS; i++)
    VarTbl[i] = 0.0;
}
/*
 * Assign value to a var of var table.
 */
void VarTblSet(char var, double value)
{
  if (!isalpha(var))
  {
    Error(ecILL_VAR_NAME);
    return;
  }

  VarTbl[toupper(var) - 'A'] = value;
}
/*
 * Get value of a var of var table.
 */
double VarTblGet(char var)
{
  if (!isalpha(var))
  {
    Error(ecILL_VAR_NAME);
    return 0.0;
  }

  return VarTbl[toupper(var) - 'A'];  
}

/*** SCANNER ***/
/*
 * Return 1 if char is a white char, i.e. space or tab.
 */
int IsWhite(char ch)
{
  return ch == ' ' || ch == '\t';
}
/*
 * Move the Prog pointer over the white chars.
 */
void SkipWhite(void)
{
  while (IsWhite(*Prog))
    Prog++;
}
/*
 * Skip chars to the end of line, then go to the start of next line.
 */
void SkipToEOL(void)
{
  while (*Prog != '\n' && *Prog)
    Prog++;

  if (*Prog == '\n')  /* end of line */
  {
    Prog++;
    Line++;
  }
}
/*
 * Read a comment.
 */
void ReadComment(void)
{
  SkipToEOL();  /* skip everything to the end of line */
  Token = tcEOL;  /* a comment is equivalent to EOL */
}
/*
 *  Read the EOL char.
 */
void ReadEOL(void)
{
  Prog++;
  Line++;
  Token = tcEOL;
}
/*
 * Read a number literal.
 */
void ReadNum(void)
{
  char* p = TokStr;

  while (isdigit(*Prog))  /* read the int part */
    *p++ = *Prog++;

  if (*Prog == '.')  /* we have a decimal point */
  {
    *p++ = *Prog++;

    while (isdigit(*Prog))  /* read the fract part */
      *p++ = *Prog++;
  }

  *p = 0;
  Token = tcNUM;
}
/*
 * Read a str literal.
 */
void ReadStr(void)
{
  char* p = TokStr;

  Prog++;  /* skip " */

  while (*Prog != '"' && *Prog != '\n' && *Prog)
    *p++ = *Prog++;

  *p = 0;

  if (*Prog == '"')  /* str is terminated OK */
  {
    Prog++;  /* skip " */
    Token = tcSTR;
    return;
  }

  Error(ecQUOTE_MISSING);  /* no closing quote */

  if (*Prog == '\n')  /* end of line */
  {
    Prog++;
    Line++;
  }

  Token = tcINVALID;
}
/*
 * Read an identifier, i.e. var name, command, func name.
 * An ID must begin with a alpha char and can contain the _ char.
 */
void ReadAlpha(void)
{
  char* p = TokStr;

  while (isalpha(*Prog) || *Prog == '_')
    *p++ = toupper(*Prog++);  /* make ID uppercase */

  *p = 0;

  if (strlen(TokStr) == 1)  /* 1-char => var name */
  {
    Token = tcVAR;
    return;
  }

  Token = FindToken(TokStr);

  if (Token == tcINVALID)  /* not in table */
    Error(ecUNREC_TOKEN);
}
/*
 * Read an 1-char token.
 */
void ReadOp1(void)
{
  switch (*Prog)
  {
    case '+': Token = tcPLUS; break;
    case '-': Token = tcMINUS; break;
    case '*': Token = tcSTAR; break;
    case '/': Token = tcSLASH; break;
    case '%': Token = tcPERC; break;
    case '(': Token = tcLPAR; break;
    case ')': Token = tcRPAR; break;
    case '=': Token = tcEQ; break;
    case ',': Token = tcCOMMA; break;
    case ';': Token = tcSEMI; break;
  }

  Prog++;
}
/*
 * Read an 1- or 2-char rel op that begins with <.
 */
void ReadOp2(void)
{
  Prog++;  /* skip < */

  switch (*Prog)
  {
    case '=': Token = tcLE; Prog++; break;  /* <= */
    case '>': Token = tcNE; Prog++; break;  /* <> */
    default:  Token = tcLT; break;  /* < */
  }
}
/*
 * Read an 1- or 2-char rel op that begins with >.
 */
void ReadOp3(void)
{
  Prog++;  /* skip > */

  switch (*Prog)
  {
    case '=': Token = tcGE; Prog++; break;  /* >= */
    default:  Token = tcGT; break;  /* > */
  }
}
/*
 * Read a token from the input stream.
 */
enum TokCode ReadToken(void)
{
  SkipWhite();

  if (*Prog == 0)  /* end of file */
    Token = tcEOF;
  else if (!_strnicmp(Prog, "REM", 3))  /* comment */
    ReadComment();
  else if (*Prog == '\n')  /* end of line */
    ReadEOL();
  else if (isdigit(*Prog))  /* num literal */
    ReadNum();
  else if (*Prog == '"')  /* str literal */
    ReadStr();
  else if (isalpha(*Prog))  /* var name, command, func name */
    ReadAlpha();
  else if (strchr("+-*/%()=,;", *Prog) != NULL)  /* 1-char token */
    ReadOp1();
  else if (*Prog == '<')  /* 1- or 2-char rel op, starting with < */
    ReadOp2();
  else if (*Prog == '>')  /* 1- or 2-char rel op, starting with > */
    ReadOp3();
  else
  {
    Error(ecUNREC_TOKEN);  /* unrecognized token */
    Token = tcINVALID;
  }

  return Token;
}

/*** PARSER ***/
/*
 * Return token code corresponding to token string str.
 */
enum TokCode FindToken(const char* str)
{
  int i;

  for (i = 0; TokTbl[i].Token != tcINVALID; i++)
    if (!stricmp(TokTbl[i].Str, str))
      return TokTbl[i].Token;

  return tcINVALID;  /* str is not a valid token string */
}
/*
 * Return token string corresponding to token code tok.
 */
const char* FindTokStr(enum TokCode tok)
{
  int i;

  for (i = 0; TokTbl[i].Token != tcINVALID; i++)
    if (TokTbl[i].Token == tok)
      return TokTbl[i].Str;

  return NULL;  /* tok is not a valid token */
}
/*
 * Return 1 if token tok is a relational op, i.e. < <= > >= = <>.
 */
int IsRelOp(enum TokCode tok)
{
  return tok >= tcLT && tok <= tcNE;
}
/*
 * Compare the numbers opnd1 and opnd2 using the relational
 * operator op and return the logical result (0 or 1).
 */
int Compare(enum TokCode rel_op, double opnd1, double opnd2)
{
  int res;

  switch (rel_op)
  {
    case tcLT: res = opnd1 < opnd2; break;  /* < */
    case tcLE: res = opnd1 <= opnd2; break;  /* <= */
    case tcGT: res = opnd1 > opnd2; break;  /* > */
    case tcGE: res = opnd1 >= opnd2; break;  /* >= */
    case tcEQ: res = opnd1 == opnd2; break;  /* = */
    case tcNE: res = opnd1 != opnd2; break;  /* <> */
  }

  if (DebMode)
  {
    DispFloat(opnd1, Precision);
    printf(" ");
    printf(FindTokStr(rel_op));
    printf(" ");
    DispFloat(opnd2, Precision);
    printf(" = ");
    DispLogValue(res);
    printf("\n");
  }

  return res;
}
/*
 * Skip tokens until tok is reached.
 */
void SkipUntilToken(enum TokCode tok)
{
  do
  {
    ReadToken();
  } while (Token != tok && Token != tcEND && Token != tcEOF);
}
/*
 * Skip tokens until either tok1 or tok2 is reached.
 */
void SkipUntilToken2(enum TokCode tok1, enum TokCode tok2)
{
  do
  {
    ReadToken();
  } while (Token != tok1 && Token != tok2 && Token != tcEND &&
              Token != tcEOF);
}
/*
 * Skip tokens until either tok1, tok2 or tok3 is reached.
 */
void SkipUntilToken3(enum TokCode tok1, enum TokCode tok2,
  enum TokCode tok3)
{
  do
  {
    ReadToken();
  } while (Token != tok1 && Token != tok2 && Token != tok3 && 
              Token != tcEND && Token != tcEOF);
}

/*** EXPR CALCULATOR ***/
/* Entry point to expr calculator */
/*
 * Evaluate an expression.
 * An expression can contain arithmetic, logical and comparison ops.
 */
double EvalExpr(void)
{
  double res;

  EvalOr();  /* start from bottom, i.e. from level 0 */
  res = StkPop();
  return res;
}
/*
 * level 0
 * OR
 * res = opnd1 OR opnd2
 */
void EvalOr(void)
{
  enum TokCode op;
  double opnd1, opnd2, res;

  EvalAnd();

  while ((op = Token) == tcOR)
  {
    ReadToken();
    EvalAnd();
    opnd2 = StkPop();
    opnd1 = StkPop();
    res = opnd1 || opnd2;
    StkPush(res);

    if (DebMode)
    {
      DispLogValue(opnd1);
      printf(" OR ");
      DispLogValue(opnd2);
      printf(" = ");
      DispLogValue(res);
      printf("\n");
    }
  }
}
/*
 * level 1
 * AND
 * res = opnd1 AND opnd2
 */
void EvalAnd(void)
{
  enum TokCode op;
  double opnd1, opnd2, res;

  EvalComp();

  while ((op = Token) == tcAND)
  {
    ReadToken();
    EvalComp();
    opnd2 = StkPop();
    opnd1 = StkPop();
    res = opnd1 && opnd2;
    StkPush(res);

    if (DebMode)
    {
      DispLogValue(opnd1);
      printf(" AND ");
      DispLogValue(opnd2);
      printf(" = ");
      DispLogValue(res);
      printf("\n");
    }
  }
}
/*
 * level 2
 * Comparison
 * res = opnd1 op opnd2
 * op =  < <= > >= = <>
 */
void EvalComp(void)
{
  enum TokCode op;
  double opnd1, opnd2, res;

  EvalAddSub();
  op = Token;

  if (!IsRelOp(op))
    return;

  ReadToken();
  EvalAddSub();
  opnd2 = StkPop();
  opnd1 = StkPop();
  res = Compare(op, opnd1, opnd2);
  StkPush(res);
}
/*
 * level 3
 * Add/Subtract
 * res = opnd1 op opnd2
 * op =  + -
 */
void EvalAddSub(void)
{
  enum TokCode op;
  double opnd1, opnd2, res;
	
  EvalMultDivMod();
	
  while ((op = Token) == tcPLUS || op == tcMINUS)
  {
    ReadToken();
    EvalMultDivMod();
    opnd2 = StkPop();
    opnd1 = StkPop();
		
    switch (op)
    {
      case tcPLUS:
        res = opnd1 + opnd2;
        break;

      case tcMINUS:
        res = opnd1 - opnd2;
        break;
    }

    StkPush(res);

    if (DebMode)
    {
      DispFloat(opnd1, Precision);
      printf(" %s ", FindTokStr(op));
      DispFloat(opnd2, Precision);
      printf(" = ");
      DispFloat(res, Precision);
      printf("\n");
    }
  }
}
/*
 * level 4
 * Multiply/Divide/Modulus
 * res = opnd1 op opnd2
 * op =  * / %
 * The operands of % must be int.
 */
void EvalMultDivMod(void)
{
  enum TokCode op;
  double opnd1, opnd2, res;

  EvalNot();

  while ((op = Token) == tcSTAR || op == tcSLASH || op == tcPERC)
  {
    ReadToken();
    EvalNot();
    opnd2 = StkPop();
    opnd1 = StkPop();
		
    switch (op)
    {
      case tcSTAR:
        res = opnd1 * opnd2;
        break;

      case tcSLASH:
        if (opnd2 == 0.0)
        {
          Error(ecDIV_ZERO);
          res = 0.0;
        }
        else
          res = opnd1 / opnd2;
        break;

      case tcPERC:
        if (!IsInt(opnd1))
        {
           Error(ecMOD_OPND_NOT_INT);
           opnd1 = RoundOff(opnd1);
        }
        if (!IsInt(opnd2))
        {
           Error(ecMOD_OPND_NOT_INT);
           opnd2 = RoundOff(opnd2);
        }
        res = (double)((int)opnd1 % (int)opnd2);
        break;
    }

    StkPush(res);

    if (DebMode)
    {
      DispFloat(opnd1, Precision);
      printf(" %s ", FindTokStr(op));
      DispFloat(opnd2, Precision);
      printf(" = ");
      DispFloat(res, Precision);
      printf("\n");
    }
  }
}
/*
 * level 5
 * NOT
 * res = NOT opnd
 */
void EvalNot(void)
{
  enum TokCode op;
  double opnd, res;

  if ((op = Token) == tcNOT)
    ReadToken();

  EvalUnPlusMinus();

  if (op == tcNOT)
  {
    opnd = StkPop();
    res = !opnd;
    StkPush(res);

    if (DebMode)
    {
      printf("NOT ");
      DispLogValue(opnd);
      printf(" = ");
      DispLogValue(res);
      printf("\n");
    }
  }
}
/*
 * level 6
 * Unary + -
 * res = op opnd
 * op =  + -
 */
void EvalUnPlusMinus(void)
{
  enum TokCode op;
  double opnd, res;

  if ((op = Token) == tcPLUS || op == tcMINUS)
    ReadToken();

  EvalPar();

  if (op == tcPLUS || op == tcMINUS)
  {
    opnd = StkPop();
    res = (op == tcPLUS) ? opnd : -opnd;
    StkPush(res);  

    if (DebMode)
    {
      printf("%s(", FindTokStr(op));
      DispFloat(opnd, Precision);
      printf(") = ");
      DispFloat(res, Precision);
      printf("\n");
    }
  }
}
/*
 * level 7
 * Parentheses
 * ( )
 */
void EvalPar(void)
{
  if (Token != tcLPAR)
  {
    EvalFactor();
    return;
  }

  if (DebMode)
    printf("(\n");

  ReadToken();
  EvalOr();

  if (Token != tcRPAR)
   Error(ecRPAR_MISSING); 
  else
    if (DebMode)
      printf(")\n");

  ReadToken();
}
/*
 * level 8
 * Factor
 * num  var  func()
 */
void EvalFactor()
{
  double res;
	
  switch (Token)
  {
    case tcNUM:
      res = atof(TokStr);
      StkPush(res);
      ReadToken();
      break;

    case tcVAR:
      res = VarTblGet(*TokStr);
      StkPush(res);
      ReadToken();
      break;

    case tcABS: res = EvalAbs(); StkPush(res); break;
    case tcSGN: res = EvalSgn(); StkPush(res); break;
    case tcCINT: res = EvalCInt(); StkPush(res); break;
    case tcFIX: res = EvalFix(); StkPush(res); break;
    case tcSQR: res = EvalSqr(); StkPush(res); break;
    case tcPOW: res = EvalPow(); StkPush(res); break;
    case tcEXP: res = EvalExp(); StkPush(res); break;
    case tcLOG: res = EvalLog(); StkPush(res); break;
    case tcRND: res = EvalRnd(); StkPush(res); break;

    default:
      Error(ecUNEXP_TOKEN);
      res = 0.0;
      StkPush(res);
      ReadToken();
      break;
  }
}
/*
 * ABS function
 * y = ABS(x)
 */
double EvalAbs(void)
{
  double x, y;

  ReadToken();  /* read ( */

  if (Token != tcLPAR)
  {
    Error(ecLPAR_MISSING);
    return 0.0;
  }

  ReadToken();  /* read x */
  x = EvalExpr();

  if (Token != tcRPAR)
  {
    Error(ecRPAR_MISSING);
    return 0.0;
  }

  ReadToken();
  y = (x < 0.0) ? -x : x;

  if (DebMode)
  {
    printf("ABS(");
    DispFloat(x, Precision);
    printf(") = ");
    DispFloat(y, Precision);
    printf("\n");
  }

  return y;
}
/*
 * SGN function
 * y = SGN(x)
 */
double EvalSgn(void)
{
  double x, y;

  ReadToken();  /* read ( */

  if (Token != tcLPAR)
  {
    Error(ecLPAR_MISSING);
    return 0.0;
  }

  ReadToken();  /* read x */
  x = EvalExpr();

  if (Token != tcRPAR)
  {
    Error(ecRPAR_MISSING);
    return 0.0;
  }

  ReadToken();

  if (x < 0.0)
    y = -1.0;
  else if (x > 0.0)
    y = 1.0;
  else
    y = 0.0;

  if (DebMode)
  {
    printf("SGN(");
    DispFloat(x, Precision);
    printf(") = ");
    DispFloat(y, Precision);
    printf("\n");
  }

  return y;
}
/*
 * CINT function
 * Roun-off a double to the nearest int.
 * y = CINT(x)
 */
double EvalCInt(void)
{
  double x, y;

  ReadToken();  /* read ( */

  if (Token != tcLPAR)
  {
    Error(ecLPAR_MISSING);
    return 0.0;
  }

  ReadToken();  /* read x */
  x = EvalExpr();

  if (Token != tcRPAR)
  {
    Error(ecRPAR_MISSING);
    return 0.0;
  }

  ReadToken();
  y = (double)RoundOff(x);

  if (DebMode)
  {
    printf("CINT(");
    DispFloat(x, Precision);
    printf(") = ");
    DispFloat(y, Precision);
    printf("\n");
  }

  return y;
}
/*
 * FIX function
 * Truncate a double to the smallest int.
 * y = FIX(x)
 */
double EvalFix(void)
{
  double x, y;

  ReadToken();  /* read ( */

  if (Token != tcLPAR)
  {
    Error(ecLPAR_MISSING);
    return 0.0;
  }

  ReadToken();  /* read x */
  x = EvalExpr();

  if (Token != tcRPAR)
  {
    Error(ecRPAR_MISSING);
    return 0.0;
  }

  ReadToken();
  y = (double)Trunc(x);

  if (DebMode)
  {
    printf("FIX(");
    DispFloat(x, Precision);
    printf(") = ");
    DispFloat(y, Precision);
    printf("\n");
  }

  return y;
}
/*
 * SQR function
 * Must be x >= 0.
 * y = SQR(x)
 */
double EvalSqr(void)
{
  double x, y;

  ReadToken();  /* read ( */

  if (Token != tcLPAR)
  {
    Error(ecLPAR_MISSING);
    return 0.0;
  }

  ReadToken();  /* read x */
  x = EvalExpr();

  if (x < 0.0)
  {
    Error(ecSQR_ARG_NEG);
    return 0.0;
  }

  if (Token != tcRPAR)
  {
    Error(ecRPAR_MISSING);
    return 0.0;
  }

  ReadToken();
  y = sqrt(x);

  if (DebMode)
  {
    printf("SQR(");
    DispFloat(x, Precision);
    printf(") = ");
    DispFloat(y, Precision);
    printf("\n");
  }

  return y;
}
/*
 * POW() function
 * POW(b, n) = b^n. n must be integer >= 0.
 * y = POW(b, n)
 */
double EvalPow(void)
{
  double b, n, y;

  ReadToken();  /* read ( */

  if (Token != tcLPAR)
  {
    Error(ecLPAR_MISSING);
    return 0.0;
  }

  ReadToken();  /* read b */
  b = EvalExpr();

  if (Token != tcCOMMA)
  {
    Error(ecCOMMA_MISSING);
    return 0.0;
  }

  ReadToken();  /* read n */
  n = EvalExpr();

  if (n < 0.0)
  {
    Error(ecEXP_NEG);
    n = 0.0;
  }

  if (!IsInt(n))
  {
    Error(ecEXP_NOT_INT);
    n = RoundOff(n);
  }

  if (Token != tcRPAR)
  {
    Error(ecRPAR_MISSING);
    return 0.0;
  }

  ReadToken();
  y = pow(b, n);

  if (DebMode)
  {
    printf("POW(");
    DispFloat(b, Precision);
    printf(", ");
    DispFloat(n, 0);  /* n is integer */
    printf(") = ");
    DispFloat(y, Precision);
    printf("\n");
  }

  return y;
}
/*
 * EXP() function
 * EXP(x) = e^x
 * y = EXP(x)
 */
double EvalExp(void)
{
  double x, y;

  ReadToken();  /* read ( */

  if (Token != tcLPAR)
  {
    Error(ecLPAR_MISSING);
    return 0.0;
  }

  ReadToken();  /* read x */
  x = EvalExpr();

  if (Token != tcRPAR)
  {
    Error(ecRPAR_MISSING);
    return 0.0;
  }

  ReadToken();
  y = exp(x);

  if (DebMode)
  {
    printf("EXP(");
    DispFloat(x, Precision);
    printf(") = ");
    DispFloat(y, Precision);
    printf("\n");
  }

  return y;
}
/*
 * LOG() function
 * LOG(x) = ln(x) = natural logarithm of x. Must be x > 0.
 * y = LOG(x)
 */
double EvalLog(void)
{
  double x, y;

  ReadToken();  /* read ( */

  if (Token != tcLPAR)
  {
    Error(ecLPAR_MISSING);
    return 0.0;
  }

  ReadToken();  /* read x */
  x = EvalExpr();

  if (x <= 0.0)
  {
    Error(ecLOG_ARG_NEG);
    return 0.0;
  }

  if (Token != tcRPAR)
  {
    Error(ecRPAR_MISSING);
    return 0.0;
  }

  ReadToken();
  y = log(x);

  if (DebMode)
  {
    printf("LOG(");
    DispFloat(x, Precision);
    printf(") = ");
    DispFloat(y, Precision);
    printf("\n");
  }

  return y;
}
/*
 * RND function
 * Returns a pseudo-random r in the range: a <= r <= b.
 * Must be a < b,  a, b = unsigned int.
 * y = RND(a, b)
 */
double EvalRnd(void)
{
  double a, b, y;

  ReadToken();  /* read ( */

  if (Token != tcLPAR)
  {
    Error(ecLPAR_MISSING);
    return 0.0;
  }

  ReadToken();  /* read a */
  a = EvalExpr();

  if (a < 0.0)
  {
    Error(ecRND_ARG_NEG);
    a = -a;
  }

  if (!IsInt(a))
  {
    Error(ecRND_ARG_INT);
    a = RoundOff(a);
  }

  if (Token != tcCOMMA)
  {
    Error(ecCOMMA_MISSING);
    return 0.0;
  }

  ReadToken();  /* read b */
  b = EvalExpr();

  if (b < 0.0)
  {
    Error(ecRND_ARG_NEG);
    b = -b;
  }

  if (!IsInt(b))
  {
    Error(ecRND_ARG_INT);
    b = RoundOff(b);
  }

  if (a >= b)
  {
    Error(ecRND_WRONG_ARG);
    return 0.0;
  }

  if (Token != tcRPAR)
  {
    Error(ecRPAR_MISSING);
    return 0.0;
  }

  ReadToken();
  y = (double)(int)((double)rand()/RAND_MAX*(b-a)+a+0.5);

  if (DebMode)
  {
    printf("RND(");
    DispFloat(a, 0);
    printf(", ");
    DispFloat(b, 0);  /* n is integer */
    printf(") = ");
    DispFloat(y, 0);
    printf("\n");
  }

  return y;
}

/*** COMMAND EXECUTOR ***/
/* Entry point to command executor */
/*
 * Execute a command.
 */
void ExecCmd()
{
  int done = 0;

  ReadToken();

  while (!done)  /* execution loop */
  {
    switch (Token)
    {
      case tcVAR: ExecAssign(); break;
      case tcIF: ExecIf(); break;
      case tcELSE: ExecElse(); break;
      case tcENDIF: ExecEndIf(); break;
      case tcGOTO: ExecGoto(); break;
      case tcGOSUB: ExecGosub(); break;
      case tcRETURN: ExecReturn(); break;
      case tcFOR: ExecFor(); break;
      case tcNEXT: ExecNext(); break;
      case tcWHILE: ExecWhile(); break;
      case tcWEND: ExecWend(); break;
      case tcDO: ExecDo(); break;
      case tcUNTIL: ExecUntil(); break;
      case tcBREAK: ExecBreak(); break;
      case tcCONTINUE: ExecContinue(); break;
      case tcINPUT: ExecInput(); break;
      case tcPRINT: ExecPrint(); break;
      case tcRANDOMIZE: ExecRandomize(); break;
      case tcPRECISION: ExecPrecision(); break;
      case tcDEB_MODE: ExecDebMode(); break;
      case tcEND: done = 1; break;
      case tcEOF: done = 1; break;
      default: ReadToken(); break;
    }
  }

  if (Token != tcEND)
    Error(ecEND_MISSING);  /* no END at the end of source */
}
/*
 * Assignment command.
 * Assign an expr to a var.
 * var = expr
 */
void ExecAssign(void)
{
  char var;  /* var name */
  double value;  /* valuen of expr */

  var = toupper(*TokStr);
  ReadToken();  /* read = */

  if (Token != tcEQ)
  {
    Error(ecEQ_MISSING);
    return;
  }

  ReadToken();  /* read expr */
  value = EvalExpr();
  VarTblSet(var, value);  /* assign value to var */
}
/*
 * IF command
 *
 * version 1 = simplified IF
 * IF expr THEN
 *   block1
 * ENDIF
 *
 * version 2 = full IF
 * IF expr THEN
 *   block1
 * ELSE
 *   block2
 * ENDIF
 */
void ExecIf(void)
{
  double res;  /* value of expr */

  ReadToken();  /* read expr */
  res = EvalExpr();

  if (Token != tcTHEN)
  {
    Error(ecTHEN_MISSING);
    return;
  }

  ReadToken();

  if (!res)  /* expr is false, so skip block1 */
    SkipUntilToken2(tcELSE, tcENDIF);

  ReadToken();
}
/*
 * ELSE command
 *
 * IF expr THEN
 *   block1
 * ELSE
 *   block2
 * ENDIF
 */
void ExecElse(void)
{
  SkipUntilToken(tcENDIF);  /* skip block2 */
  ReadToken();
}
/*
 * ENDIF command
 *
 * version 1 = simplified IF
 * IF expr THEN
 *   block1
 * ENDIF
 *
 * version 2 = full IF
 * IF expr THEN
 *   block1
 * ELSE
 *   block2
 * ENDIF
 */
void ExecEndIf(void)
{
  ReadToken();  /* get out of last block, either block1 or block2 */
}
/*
 * GOTO command
 * Jump to label loc.
 *
 * GOTO label
 */
void ExecGoto(void)
{
  char* loc;

  ReadToken();  /* read label */

  if (Token != tcNUM)  /* not a label */
  {
    Error(ecLBL_MISSING);
    return;
  }

  loc = LblTblFindLoc(TokStr);

  if (loc == NULL)  /* not a valid label */
  {
    Error(ecLBL_UNDEF);
    return;
  }

  Prog = loc;  /* jump to loc */
  ReadToken();
}
/*
 * GOSUB command
 * Jump to label loc. Return later with RETURN.
 *
 * GOSUB label
 *   block1
 * label
 *   block2
 * RETURN
 */
void ExecGosub(void)
{
  char* loc;

  ReadToken();  /* read label */

  if (Token != tcNUM)  /* not a label */
  {
    Error(ecLBL_MISSING);
    return;
  }

  loc = LblTblFindLoc(TokStr);

  if (loc == NULL)  /* not a valid lbl */
  {
    Error(ecLBL_UNDEF);
    return;
  }

  /* push current loc on GOSUB stack = return address */
  GosubStkPush(Prog);
  Prog = loc;  /* jump to loc */
  ReadToken();
}
/*
 * RETURN command
 * Return from a GOSUB subroutine. Jump to the 1st loc following the 
 * GOSUB.
 *
 * GOSUB label
 *   block1
 * label
 *   block2
 * RETURN
 */
void ExecReturn(void)
{
  /* pop the return address from the GOSUB stack */
  Prog = GosubStkPop(); 
  ReadToken();
}
/*
 * FOR command
 *
 * FOR var = start_value TO end_value [ STEP step_value ]
 *   block
 * NEXT
 */
void ExecFor(void)
{
  char var;  /* name of var (counter) */
  double start_value, end_value, step_value;
  int skip_loop;
  struct ForStkItem i;

  ReadToken();  /* read var name */

  if (Token != tcVAR)
  {
    Error(ecNOT_VAR);
    return;
  }

  var = toupper(*TokStr);
  ReadToken();  /* read = */

  if (Token != tcEQ)
  {
    Error(ecEQ_MISSING);
    return;
  }

  ReadToken();  /* read start_value */
  start_value = EvalExpr();

  if (Token != tcTO)
  {
    Error(ecTO_MISSING);
    return;
  }

  ReadToken();  /* read end_value */
  end_value = EvalExpr();

  if (Token != tcSTEP)  /* no STEP clause */
    step_value = 1.0;  /* by default, step = 1 */
  else  /* STEP clause present */
  {
    ReadToken();  /* read step_value */
    step_value = EvalExpr();

    if (step_value == 0.0)
    {
      Error(ecSTEP_ZERO);  /* zero step is illegal */
      step_value = 1.0;
    }
  }

  if (step_value > 0.0)  /* counting up */
    skip_loop = start_value > end_value;
  else  /* counting down */
    skip_loop = start_value < end_value;

  if (skip_loop)  /* skip the loop */
  {
    SkipUntilToken(tcNEXT);

    if (Token != tcNEXT)
      Error(ecNEXT_MISSING);
    else
      ReadToken();

    return;
  }

  /* stay in loop */
  VarTblSet(var, start_value);  /* save current var value in VarTbl */
  i.Var = var;  /* save var name on stack */
  i.EndValue = end_value;  /* save end value on stack */
  i.StepValue = step_value;  /* save step value on stack */
  i.Loc = Prog;  /* save loc on stack */
  ForStkPush(&i);
  ReadToken();  /* read the 1st token of block */
}
/*
 * NEXT command
 *
 * FOR var = start_value TO end_value [ STEP step_value ]
 *   block
 * NEXT
 */
void ExecNext(void)
{
  char var;  /* name of var (counter) */
  double var_value, end_value, step_value;
  int skip_loop;
  struct ForStkItem* p;

  if (ForStkIsEmpty())
  {
    Error(ecNEXT_WITHOUT_FOR);  /* too many NEXTs */
    return;
  }

  p = ForStkPeek();
  var = p->Var;
  var_value = VarTblGet(var);  /* current value of var */
  end_value = p->EndValue;
  step_value =  p->StepValue;
  var_value += step_value;
  VarTblSet(var, var_value);

  if (step_value > 0.0)  /* counting up */
    skip_loop = var_value > end_value;
  else  /* counting down */
    skip_loop = var_value < end_value;

  if (skip_loop)  /* skip the loop */
  {
    var_value -= step_value;
    VarTblSet(var, var_value);
    ForStkPop();  /* remove the top item from stack */
    ReadToken();  /* skip NEXT */
    return;
  }

  /* stay in loop */
  Prog = p->Loc;  /* jump back to FOR */
  ReadToken();
}
/*
 * WHILE command
 *
 * WHILE var rel_op expr
 *   block
 * WEND
 */
void ExecWhile(void)
{
  char var;  /* var name */
  double var_value, expr;
  enum TokCode rel_op;
  int res;
  struct WhileStkItem i;

  ReadToken();  /* read var name */

  if (Token != tcVAR)
  {
    Error(ecNOT_VAR);
    return;
  }

  var = toupper(*TokStr);
  var_value = VarTblGet(var);  /* get current value of var */

  rel_op = ReadToken();  /* read op */

  if (!IsRelOp(rel_op))
  {
    Error(ecREL_OP_MISSING);
    return;
  }

  ReadToken();  /* read expr */
  expr = EvalExpr();

  /* compare var_value with expr */
  res = Compare(rel_op, var_value, expr);

  if (!res)  /* res is false, so skip loop */
  {
    SkipUntilToken(tcWEND);

    if (Token == tcWEND)
      ReadToken();
    else
      Error(ecWEND_MISSING);

    return;
  }

  /* res is true, so stay in loop */
  if (WhileStkIsFull())
  {
    Error(ecTOO_MANY_WHILE_NEST);  /* too many WHILEs */
    return;
  }

  i.Var = var;
  i.Op = rel_op;
  i.Expr = expr;
  i.Loc = Prog;
  WhileStkPush(&i);
  ReadToken();  /* read the 1st token of block */
}
/*
 * WEND command
 *
 * WHILE var rel_op expr
 *   block
 * WEND
 */
void ExecWend(void)
{
  char var;  /* var name */
  double var_value, expr;
  enum TokCode rel_op;
  int res;
  struct WhileStkItem* p;

  if (WhileStkIsEmpty())
  {
    Error(ecWEND_WITHOUT_WHILE);  /* too many WENDs */
    return;
  }

  p = WhileStkPeek();
  var = p->Var;
  rel_op = p->Op;
  expr = p->Expr;
  var_value = VarTblGet(var);
  res = Compare(rel_op, var_value, expr);

  if (!res)  /* res is false, so exit loop */
  {
    WhileStkPop();  /* remove the top item from stack */
    ReadToken();  /* skip WEND */
    return;
  }

  /* res is true, so stay in loop */
  Prog = p->Loc;  /* jump back to WHILE */
  ReadToken();
}
/*
 * DO command
 *
 * DO
 *   block
 * UNTIL var rel_op expr
 */
void ExecDo(void)
{
  struct DoStkItem i;

  i.Loc = Prog;
  DoStkPush(&i);
  ReadToken();
}
/*
 * UNTIL command
 *
 * DO
 *   block
 * UNTIL var rel_op expr
 */
void ExecUntil(void)
{
  char var;  /* var name */
  double var_value, expr;
  enum TokCode rel_op;
  int res;
  struct DoStkItem i;

  ReadToken();  /* read var name */

  if (Token != tcVAR)
  {
    Error(ecNOT_VAR);
    return;
  }

  var = toupper(*TokStr);
  var_value = VarTblGet(var);  /* get current value of var */
  rel_op = ReadToken();  /* read op */

  if (!IsRelOp(rel_op))
  {
    Error(ecREL_OP_MISSING);
    return;
  }

  ReadToken();  /* read expr */
  expr = EvalExpr();
  res = Compare(rel_op, var_value, expr);

  if (res)  /* res is true, so exit loop */
  {
    DoStkPop();
    ReadToken();
    return;
  }

  /* res is false, so stay in loop */
  if (DoStkIsFull())
  {
    Error(ecTOO_MANY_DO_NEST);  /* too many DOs */
    return;
  }

  i = *DoStkPop();
  i.Var = var;
  i.Op = rel_op;
  i.Expr = expr;
  DoStkPush(&i);
  VarTblSet(var, var_value);
  Prog = i.Loc;
  ReadToken();  /* read the 1st token of block */
}
/*
 * BREAK command
 * Used in FOR ... NEXT, WHILE ... WEND and DO ... UNTIL loops.
 * Causes immediate exit from loop.
 *
 * BREAK
 */
void ExecBreak(void)
{
  SkipUntilToken3(tcNEXT, tcWEND, tcUNTIL);
  ReadToken();
}
/*
 * CONTINUE command
 * Used in FOR ... NEXT, WHILE ... WEND and DO ... UNTIL loops.
 * Causes jump to the end of loop. No exit from loop.
 *
 * CONTINUE
 */
void ExecContinue(void)
{
  SkipUntilToken3(tcNEXT, tcWEND, tcUNTIL);
}
/*
 * INPUT command
 * Input a var value. The prompt is optional.
 *
 * INPUT [ prompt, ] var
 */
void ExecInput(void)
{
  char var;  /* var name */
  float value;

  ReadToken();  /* read prompt or var name*/

  if (Token == tcSTR)  /* we have a user-defined prompt */
  {
    printf("%s ", TokStr);  /* so display it */
    ReadToken();  /* read , */

    if (Token != tcCOMMA)
    {
      Error(ecCOMMA_MISSING);
      return;
    }

    ReadToken();  /* read var name */
  }
  else  /* no user-defined prompt present */
    printf("? ");  /* display the default prompt ? */

  if (Token != tcVAR)  /* no var name*/
  {
    Error(ecVAR_MISSING);
    return;
  }

  var = toupper(*TokStr);
  scanf("%f", &value);
  VarTblSet(var, (double)value);
  ReadToken();
}
/*
 * PRINT command
 * Print a comma- or tab-separated list of strings or expressions.
 * , causes the display of a space, ; causes the display of a tab.
 *
 * PRINT
 * PRINT str [, ...]
 * PRINT expr [, ...]
 */
void ExecPrint(void)
{
  double value;
  int done = 0;

  ReadToken();

  while (!done)
  {
    switch (Token)
    {
      case tcEOL:  /* terminate loop */
        printf("\n");
        ReadToken();
        done = 1;
        break;

      case tcCOMMA:  /* print a space */
        printf(" ");
        ReadToken();
        break;

      case tcSEMI:  /* print a tab */
        printf("\t");
        ReadToken();
        break;

      case tcSTR:  /* str literal */
        printf(TokStr);
        ReadToken();
        break;

      default:  /* expr */
        value = EvalExpr();
        DispFloat(value, Precision);
        break;
    }
  }
}
/*
 * RANDOMIZE command
 * Set the seed value for the random-number generator.
 * Seed must be unsigned int.
 *
 * RANDOMIZE seed
 */
void ExecRandomize(void)
{
  double value;
  unsigned int seed;

  ReadToken();  /* read seed */
  value = EvalExpr();

  if (value < 0.0)  /* must be >= 0 */
  {
    Error(ecRAND_ARG_NEG);
    value = -value;
  }

  if (!IsInt(value))  /* must be integer */
  {
    Error(ecRAND_ARG_INT);
    value = RoundOff(value);
  }

  seed = (unsigned int)value;
  srand(seed);

  if (DebMode)
  {
    printf("Seed = ");
    DispFloat(seed, 0);
    printf("\n");
  }
}
/*
 * PRECISION command
 * Set the precison of displayed numbers.
 * Must be 0 <= prec <= 6.
 *
 * PRECISION prec
 */
void ExecPrecision(void)
{
  double prec;

  ReadToken();  /* read prec */
  prec = EvalExpr();

  if (prec < 0.0)  /* must be >= 0 */
  {
    Error(ecPREC_ARG_NEG);
    prec = -prec;
  }

  if (!IsInt(prec))  /* must be integer */
  {
    Error(ecPREC_ARG_INT);
    prec = RoundOff(prec);
  }

  Precision = (unsigned int)prec;

  if (DebMode)
  {
    printf("Precision = ");
    DispFloat(prec, 0);
    printf("\n");
  }
}
/*
 * DEB_MODE command
 * Set the debug mode toggle to on/off value.
 *
 * DEB_MODE ON | OFF
 */
void ExecDebMode(void)
{
  ReadToken();  /* read the on/off value */

  if (!(Token == tcON || Token == tcOFF))
  {
    Error(ecON_OFF_MISSING); 
    return;
  }

  DebMode = (Token == tcON);
  ReadToken();

  if (DebMode)
  {
    printf("Debug Mode = ");
    DebMode ? printf("ON") : printf("OFF");
    printf("\n");
  }
}

/*** INTERPRETER ***/
/*
 * Display the source file.
 * Useful for debug purposes.
 */
void DispSource(void)
{
  char* p = Source;
  int ch_count = 0, line = 1;  /* char counter, line counter */

  DispCh('=', SCR_LINE_WIDTH);
  printf("\nSource File:\n\n");

  printf("%3d   ", line);

  while (*p)
  {
    if (*p == '\n')
      printf("\n%3d   ", ++line);  /* EOL char */
    else
      printf("%c", *p);  /* printable char */

    p++;
    ch_count++;
  }

  printf("\n\nLines = %d, Chars = %d\n", line, ch_count);
  DispCh('=', SCR_LINE_WIDTH);
  DispCh('\n', 2);
}
/*
 * Display all the tokens of source.
 * Useful for debug purposes.
 */
void DispTokens(void)
{
  int tok_count = 0;

  Prog = Source;
  Line = 1;

  DispCh('=', SCR_LINE_WIDTH);
  printf("\nTokens:\n\n");

  printf("Line  Token\n");
  DispCh('-', SCR_LINE_WIDTH);
  printf("\n");

  while (ReadToken() != tcEOF)
  {
    tok_count++;

    switch (Token)
    {
      case tcVAR:
        printf("%3d   Token = Variable, Value = %s\n", Line, TokStr);
        break;

      case tcNUM:
        printf("%3d   Token = Number, Value = %s\n", Line, TokStr);
        break;

      case tcSTR:
        printf("%3d   Token = String, Value = %s\n", Line, TokStr);
        break;

       case tcEOL:
        printf("%3d   Token = EOL\n", Line-1);
        break;

       case tcINVALID:
        printf("%3d   Token = Error\n", Line-1);
        break;

      default:  /* all the other tokens */
        printf("%3d   Token = %s\n", Line, FindTokStr(Token));
    }
  }

  DispCh('-', SCR_LINE_WIDTH);
  printf("\n\nTokens = %d\n", tok_count);
  DispCh('=', SCR_LINE_WIDTH);
  DispCh('\n', 2);

  Prog = Source;
  Line = 1;
}
/*
 * Load the source from file into the buffer Source.
 */
void LoadProg(const char* fname)
{
  FILE* fp;
  int ch_count;  /* num of bytes in file */

  if (fname == NULL)
  {
    printf("Error: file name is NULL.\n");
    exit(1);
  }

  if (fname[0] == 0)
  {
    printf("Error: file name is empty.\n");
    exit(1);
  }

  fp = fopen(fname, "rb");

  if (fp == NULL)
  {
    printf("Error: cannot open file %s.\n", fname);
    exit(1);
  }

  ch_count = fread(Source, 1, PROG_SIZE, fp);
  Source[ch_count] = 0;
  fclose(fp);
  FilterCR();
}
/*
 * Filter out the CR chars from the Source buffer.
 */
void FilterCR(void)
{
  char* s = Source;  /* ptr to source char */
  char* d = s;  /* ptr to destination char */

  while (*s)
  {
    if (*s == '\r')  /* CR char => skip it */
      s++;
    else
    *d++ = *s++;  /* copy char from source to destination */
  }

  *d = 0;
}
/*
 * Preprocessor scan.
 *  Scan the source for labels and insert them into the label table.
 */
void ScanLabels(void)
{
  int done = 0;

  Prog = Source;
  Line = 1;

  while (!done)
  {
    ReadToken();

    switch (Token)
    {
      case tcEOF:
        done = 1;
        break;

      case tcEOL:
        break;

      case tcNUM:
        if (LblTblIsFull())
          done = 1;
        /* no such label in lbl table */
        else if (LblTblFindLoc(TokStr) == NULL)
          LblTblInsert(TokStr, Prog, Line);
        else
          Error(ecLBL_DUPL);  /* duplicate lbl, don't insert */

        SkipToEOL();
        break;

      default:  /* not a number => skip line and go to the next line */
        SkipToEOL();
        break;
    }
  }

  Prog = Source;
  Line = 1;
}
/*
 * Initialize the interpreter.
 */
void InitInterpreter(const char* fname)
{
  Source = malloc(PROG_SIZE);

  if (Source == NULL)
  {
    printf("Error: memory allovation failure.\n");
    exit(1);
  }

  LoadProg(fname);
  Prog = Source;
  Token = tcINVALID;
  TokStr[0] = 0;
  Line = 1;
  ErrCounter = 0;
  Precision = 0;  /* by default, display all numbers as int */
  DebMode = 0;  /* by default, no debug info is displayed */

  LblTblInit();
  StkInit();
  GosubStkInit();
  ForStkInit();
  WhileStkInit();
  DoStkInit();
  VarTblInit();

  ScanLabels();
}
/*
 * Close the interpreter.
 */
void CloseInterpreter(void)
{
  free(Source);
  Source = NULL;
}
/*
 * A test program. No execution of statements is done.
 */ 
void main0(void)
{
  InitInterpreter("Test0.bas");
  DispSource();  /* display source file */
  LblTblDisplay();  /* check label table */
  DispTokens();  /* see if there are illegal tokens in source */
  CloseInterpreter();
}
/*
 * A prog to run a BASIC source file.
 */
void main(int argc, const char* argv[])
{
  if (argc != 2)
  {
    printf("Usage: %s <file_name>\n", argv[0]);
    return;
  }

  InitInterpreter(argv[1]);
  ExecCmd();
  CloseInterpreter();
}

