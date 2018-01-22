// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.
@white
M=0
@black
M=-1
@i
M=0
@SCREEN
D=A
@8196 // total bytes
D=D+A
@4
D=D-A // ugly hack to get the numbers right
@limit
M=D
(start)
  @SCREEN
  D=A
  @i
  M=D
(poll)
  @KBD
  D=M
  @paintwhite
  D; JEQ
  @paintblack
  0; JMP
(paintwhite)
  @white
  D=M
  @colour
  M=D
  @paint
  0; JMP
(paintblack)
  @black
  D=M
  @colour
  M=D
  @paint
  0; JMP
(paint)
  @colour
  D=M
  @i
  A=M
  M=D // write to location pointed to by i
  @i
  M=M+1
  @limit
  D=M
  @i
  D=D-M
  @start
  D; JLE
  @paint
  0;JMP
