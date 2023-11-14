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

// High-level:
// Paint(paint_value) {
//   address = SCREEN;
//   while (address < KBD) {
//     M[address] = paint_value;
//     ++address;
//   }
// }
//
// while (true) {
//   if (KBD != 0) {
//     Paint(black);
//   } else {
//     Paint(white);
//   }
// }

// Pseudocode:
// LOOP:
// if (KBD != 0) goto PAINT_BLACK;
// paint_value = 0;
// goto PAINT;
//
// PAINT_BLACK:
// paint_value = -1;
//
// PAINT:
// address = SCREEN;
// PAINT_LOOP:
// if (address >= KBD) goto END;
// M[address] = paint_value;
// address = address + 1;
// goto PAINT_LOOP;
//
// END:
// goto LOOP;

(LOOP)
  @KBD
  D=M
  @PAINT_BLACK
  D;JNE

  @paint_value
  M=0

  @PAINT
  0;JMP
(PAINT_BLACK)
  @paint_value
  M=-1

(PAINT)
  @SCREEN
  D=A
  @address
  M=D
(PAINT_LOOP)
  @address
  D=M
  @KBD
  D=D-A
  @END
  D;JGE

  @paint_value
  D=M
  @address
  A=M
  M=D
  @address
  M=M+1

  @PAINT_LOOP
  0;JMP

(END)
  @LOOP
  0;JMP
