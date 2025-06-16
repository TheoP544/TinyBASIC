# TinyBASIC
A Tiny BASIC interpreter written in C.
A Tiny BASIC Interpreter

Author: Theo P. (theo_pap@otenet.gr)
Language used: C
Copyright: No copyright. You can do with this software whatever you
  like.
Warranty: No warranty. Use this software at your own risk.

1. THE ORIGINS OF THIS PROGRAM
It's based on the book: Herbert Schildt, "C Power User's Guide".
Chapter 7 describes the Small BASIC interpreter.

2. STATEMENTS SUPPORTED

NOTE: <block of statements> is a number of statements, each located on separate line. e.g.:

A = 3
B = A * 2 + 6
C = 1 - A * SQR(4)

2.1 Assignment
var = expression

2.2 IF ... ENDIF
IF expression THEN
  <block of statemments>
ENDIF

2.3 IF ... ELSE ... ENDIF
IF expression THEN
  <block of statemments 1>
ELSE
  <block of statemments 2>
ENDIF

2.4 GOTO

GOTO label
2.5 GOSUB ... RETURN
GOSUB label
<block of statemments>
label
RETURN

2.6 FOR ... NEXT
FOR var = start_value TO end_value [ STEP step_value ]
  <block of statemments>
NEXT

2.7 WHILE ... WEND
WHILE var rel_op expression
  <block of statemments>
WEND

2.8 DO ... UNTIL
DO
  <block of statemments>
UNTIL var rel_op expression

2.9 BREAK
2.10 CONTINUE

2.10 INPUT
INPUT [ prompt_str, ] var

2.11 PRINT

PRINT
PRINT string [, ...]
PRINT expression [, ...]

2.12 RANDOMIZE
RANDOMIZE seed

2.13  PRECISION
PRECISION prec
Sets the precison of displayed numbers, i.e. the number of decimal places diplayed.
By default, presision is 0, i.e. all numbers are displayed as integers.

2.14  DEB_MODE
DEB_MODE ON | OFF
Sets the debug mode toggle to on/off value.
DEB_MODE ON causes the debug info to be diplayed.
DEB_MODE OFF causes the debug info not to be suppressed.
By default, DEB_MODE is OFF.

3. EXPRESSION CALCULATOR
The precedence table with all the operators is as follows:

 --------------------------------------------------------------------
 Operator            Level  Operation
 --------------------------------------------------------------------
 num  var  func       8    Number, variable or function value
 ( )                        7    Parentheses
 un+ un-                6    Unary + -
 NOT                     5    Logical NOT
 * / %                   4    Mult/Div/Mod
 + -                      3    Add/Sub
 < <= > >= = <>  2    Relational
 AND                     1    Logical AND
 OR                       0    Logical OR
 --------------------------------------------------------------------

4. ACCURACY OF CALCULATIONS
All numbers used are of type double, so all calculations are done to the full precision of a double.
The PRECISION statement controls only the way the numbers are displayed on screen. For example, PRECISION 0 will cause the numbers to be displayed as integers.
Round-off is performed before displaying the numbers.

5. KNOWN ISSUES
5.1 Sometimes the DispFloat() function doesn't work correctly. For example, the statements:

A = 2.8
PRINT "A =", A

will display
A = 2.79
instead of the correct:

A = 2.80

5.2 The error handling is, well, non-existing. Which means that if you feed the interpreter a syntactically correct source file, it will be compiled and executed OK. But a source file with errors will probably cause a crash.
Supply your own error handler, if you like.

5.3 The unary operators + - NOT, when used consequtively, cause an error. For example, the expressions:

+-3  -+3  --3  ++3  NOT NOT 0

will cause an error. To solve this problem, use parenthses. The expressions:

+(-3)  -(+3)  -(-3)  +(+3  NOT (NOT 0)

will execute OK.

