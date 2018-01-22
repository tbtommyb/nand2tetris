// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Put your code here.

@R0
D=M // load R0
@i
M=D // store R0 in i
@0
D=A
@R2
M=D // initialise R2 to 0
(loop)
  @i
  D=M
  @end
  D; JEQ // quit if i is 0
  @R1
  D=M
  @R2
  M=M+D // add R1 to R2
  @i
  M=M-1 // decrement i
  @loop
  0; JMP
(end)
  @end
  0; JMP
