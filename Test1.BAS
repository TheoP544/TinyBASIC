REM Test program to test all the BASIC statements.

DEB_MODE OFF
PRECISION 2

REM Testing the assignment op.
a = 2.8
PRINT "A =", A
PRINT "This is incorrent. It should be A = 2.80."
PRINT

REM Testing the expression calulator.
DEB_MODE ON
PRINT
PRINT "Calculating arithmetic expression: A = (2+3)*7/2-5"
A = (2+3)*7/2-5
PRINT "A =", A
PRINT

PRINT "Calculating comparison: A = 7 > 2"
A = 7 > 2
PRINT "A =", A
PRINT

PRINT "Calculating logical expression: A = 7 > 2 AND NOT (9 < 1)"
A = 7 > 2 AND NOT (9 < 1)
PRINT "A =", A
PRINT

DEB_MODE OFF

PRINT "Calculating the built-in functions:"
A = ABS(-2)
PRINT "ABS(-2) =", A

A = SGN(-2)
PRINT "SGN(-2) =", A

A = CINT(2.538)
PRINT "CINT(2.538) =", A

A = FIX(-2.538)
PRINT "FIX(-2.538) =", A

A = SQR(9)
PRINT "SQR(9) =", A

A = POW(2, 3)
PRINT "POW(2, 3) =", A

A = EXP(2)
PRINT "EXP(2) =", A

A = LOG(2)
PRINT "LOG(2) =", A

A = RND(1, 10)
PRINT "RND(1, 10) =", A
PRINT

PRINT "Testing the IF ... ENDIF statement:"
A = 3
PRINT "A =", A
IF A > 0 THEN
  PRINT "A > 0"
ENDIF
PRINT

PRINT "Testing the IF ... ELSE ... ENDIF statement:"
A = -3
PRINT "A =", A
IF A >= 0 THEN
  PRINT "A >= 0"
ELSE
  PRINT "A < 0"
ENDIF
PRINT

PRECISION 0

PRINT "Testing the GOTO statement:"
A = 0
PRINT "A =", A
PRINT "Jumping forward to label 20"
GOTO 20
10 PRINT "10"
A = 10
PRINT "A =", A
GOTO 30
20 PRINT "20"
A = 20
PRINT "A =", A
PRINT "Jumping back to label 10"
GOTO 10
30
PRINT

PRINT "Testing the GOSUB ... RETURN statement"
A = 0
PRINT "A =", A
PRINT "Jumping to subroutine at label 50"
GOSUB 50
40 PRINT "40"
A = 40
PRINT "A =", A
GOTO 60
50 PRINT "50"
A = 50
PRINT "A =", A
PRINT "Returning back to label 40"
RETURN
60
PRINT

PRINT "Testing the FOR ... NEXT statement:"
PRINT

PRINT "1) Just a plain vanilla FOR"
FOR A = 1 TO 5
  PRINT "A =", A
NEXT
PRINT

PRINT "2) A FOR with step"
FOR A = 1 TO 7 STEP 2
  PRINT "A =", A
NEXT
PRINT

PRINT "3) A FOR with count-down counter"
FOR A = 7 TO 1 STEP -1
  PRINT "A =", A
NEXT
PRINT

PRINT "4) A nested FOR"
FOR A = 1 TO 3
  FOR B = 2 TO 7 step 2
    PRINT "A =", A, "  B =", B
  NEXT
NEXT
PRINT

PRINT "Testing the WHILE ... WEND statement:"
A = 5
PRINT "A =", A
PRINT
WHILE A > 0    REM A > 0 is the condition to stay in loop
  A = A - 1    REM Decrement A
  PRINT "A =", A
WEND
PRINT
PRINT "A =", A
PRINT

PRINT "Testing the DO ... UNTIL statement:"
A = 5
PRINT "A =", A
PRINT
DO
  A = A - 1    REM Decrement A
  PRINT "A =", A
UNTIL A <= 0    REM A <= 0 is the condilition to terminate loop
PRINT
PRINT "A =", A
PRINT

PRINT "Testing the BREAK statement:"
A = 10
PRINT "A =", A
PRINT "We are entering the WHILE loop"
PRINT
WHILE A > 0
  A = A - 1    REM Decrement A
  PRINT "A =", A
  IF A = 5 THEN    REM Jump out of loop when A = 5
    BREAK
  ENDIF
WEND
PRINT
PRINT "We are now outside the WHILE loop"
PRINT "A =", A
PRINT

PRINT "Testing the CONTINUE statement:"
A = 6
PRINT "A =", A
PRINT "We are entering the WHILE loop"
PRINT
WHILE A > 0
  A = A - 1    REM Decrement A
  PRINT "A =", A
  IF A = 3 THEN    REM The value B = 6 will be skipped.
    CONTINUE
  ENDIF
  B = 2 * A
  PRINT "  B =", B
WEND
PRINT
PRINT "We are now outside the WHILE loop"
PRINT "A =", A
PRINT

PRINT "Testing the PRINT statement:"
PRINT "Mary had a little lamb"    REM Printing a string literal
PRINT 5, 3*2, SQR(9)    REM Printing expressions
PRINT    REM Printing an empty line
PRINT 1; 2; 3; 4    REM Using tabs as separators
PRINT

PRINT "Testing the INPUT statement:"
PRINT "Enter value for A"
INPUT A    REM No user-defined prompt
PRINT "A =", A
INPUT "Enter value for B:", B    REM User-defined prompt supplied
PRINT "B =", B
PRINT

PRINT "That's all, folks."
PRINT

END
