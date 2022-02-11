/*----------------------------------------------------------------------------
 * Name:    PB_LCD_Drivers.c
 * Purpose: Low-level driver functions for the LCD on the York Processor Board
 * Note(s): v0.0
 *----------------------------------------------------------------------------
 * Supplied under the terms of the MIT Open-Source License:
 *
 * Copyright (c) 2018 David A.J. Pearce
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *----------------------------------------------------------------------------*/

#include "PB_LCD_Drivers.h"
#include "stm32f4xx.h"

////////////////////////////////////////
// Some general purpose helper routines:
// Prototypes added to avoid warnings

void PB_LCD_Set_As_Output(int bit, GPIO_TypeDef* port) {
  // Configures one bit in the GPIO port as an output suitable for driving
  // any general purpose output. This involves setting the appropriate bits in:
  // MODER set to "01" (general purpose output)
  // OTYPER set to "0" (push-pull output)
  // OSPEEDR set to "10" (high-speed - perhaps not required)
  // PUPDR set to "00" (no pull-up or pull-down required)
  unsigned long bitMask = ~(3UL << 2*bit);
  port->MODER = (port->MODER & bitMask) | (1UL << 2*bit);
  port->OTYPER &= ~(1UL << bit);
  port->OSPEEDR = (port->OSPEEDR & bitMask) | (2UL << 2*bit);
  port->PUPDR = (port->PUPDR & bitMask) | (0UL << 2*bit);
}

enum eTermType { PULLUP = 1, PULLDOWN = 2, PULLBOTH = 3, NOPULL = 0 };
void PB_LCD_Set_As_Input(int bit, GPIO_TypeDef* port, enum eTermType eTT) {
  // Configures one bit in the GPIO port as an input suitable for reading from.  
  // Note, this includes a pull-up resistor for compatibility with open-drain 
  // outputs.  This involves setting the corresponding bits in:
  // MODER set to "00" (general purpose input)
  // PUPDR should be set to NOPULL ("00") by default
  unsigned long bitMask = ~(3UL << 2*bit);
  port->MODER &= bitMask;
  port->PUPDR = (port->PUPDR & bitMask) | ((unsigned int)eTT << 2*bit);
}

// PB_LCD_Microdelay attempts to delay by the requested number of microseconds
// (The factors were determined experimentally for the STM32F discovery
// board running at 16 MHz with no compiler optimisations.)
void PB_LCD_Microdelay (unsigned int delayInMicroSeconds) {
  float compensation = (float)SystemCoreClock / (float)16e6;
  volatile unsigned long x = (unsigned long)(compensation * (36 * delayInMicroSeconds >> 4));
  while (x-- > 0);
}

///////////////////////////////////////////////////////////////////////////////
// LCD Definitions
// I'll slow all the accesses down a bit, since I don't think the LCD
// controller will be able to keep up with the ARM-Cortex.  Experimentally,
// a delay of around 50 us seems to be enough, and it makes writing to the
// LCD so fast that you can't see the individual characters appear.  It's
// also less likely to be interrupted by a reset in the middle of an operation,
// and this can upset it (I'm still not entirely clear why this happens, or
// how to kick it out of whatever random state it gets into at these times).
#define LCD_DELAY_CONST 50

enum eLCD_OP { READ_INSTRUCTION, WRITE_INSTRUCTION, READ_DATA, WRITE_DATA };

uint8_t PB_LCD_IsBusy() {
  // Returns non-zero if the LCD currently reports it is busy, or zero otherwise.
  
  // This involves repeated reads from the instruction register waiting for DB7 
  // to go low.  At this point I don't know whether the LCD is in 8-bit mode or 
  // 4-bit mode, so I'll read it twice and take the MSB of the first reading only.
  uint8_t busy;

  // Set the ports to read mode:
  for (int loop = 4; loop < 8; loop++) {
    PB_LCD_Set_As_Input(loop, GPIOD, NOPULL);
  }
  PB_LCD_Microdelay(LCD_DELAY_CONST);   
  GPIOB->BSRR = 1UL << 16;  // Set to instruction register
  GPIOB->BSRR = 1UL << 1;    // Set to read access
  PB_LCD_Microdelay(LCD_DELAY_CONST); GPIOB->BSRR = 1UL << 2;
  PB_LCD_Microdelay(LCD_DELAY_CONST); busy = GPIOD->IDR; 
  PB_LCD_Microdelay(LCD_DELAY_CONST); GPIOB->BSRR = 1UL << 18;
  PB_LCD_Microdelay(LCD_DELAY_CONST); GPIOB->BSRR = 1UL << 2;
  PB_LCD_Microdelay(LCD_DELAY_CONST); GPIOB->BSRR = 1UL << 18;
  
  // Then set the ports back to write mode:
  for (int loop = 4; loop < 8; loop++) {
    PB_LCD_Set_As_Output(loop, GPIOD);
  }
  GPIOB->BSRR = 1UL << 17;    // Set to write access
  PB_LCD_Microdelay(LCD_DELAY_CONST);

  return busy & 0x80;  
}

void PB_LCD_Write (enum eLCD_OP op, uint8_t data) {
  // Writes a byte to the LCD.  This assumes four-bit mode, and that
  // the GPIO outputs are already configured for writing (set to outputs):
  if (op == WRITE_DATA) {
    GPIOB->BSRR = 1UL << 0;
  }
  else if (op == WRITE_INSTRUCTION) {
    GPIOB->BSRR = 1UL << 16;
  }
  else {
    return;
  }

  unsigned int toWrite_High = (GPIOD->ODR & 0xFFFFFF0F) | (data & 0xF0);
  unsigned int toWrite_Low = (GPIOD->ODR & 0xFFFFFF0F) | ((data << 4) & 0xF0);
  GPIOD->ODR = toWrite_High;  
  PB_LCD_Microdelay(LCD_DELAY_CONST); GPIOB->BSRR = 1UL << 2;
  PB_LCD_Microdelay(LCD_DELAY_CONST); GPIOB->BSRR = 1UL << 18;
  GPIOD->ODR = toWrite_Low;
  PB_LCD_Microdelay(LCD_DELAY_CONST); GPIOB->BSRR = 1UL << 2;
  PB_LCD_Microdelay(LCD_DELAY_CONST); GPIOB->BSRR = 1UL << 18;
}
  
void PB_LCD_Init (void) {
  // The LCD uses GPIOs A, B and D, so all these clocks are required:
  RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOAEN;
  RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOBEN;
  RCC->AHB1ENR  |= RCC_AHB1ENR_GPIODEN;
  
  // The control lines are PA15 and PB0, PB1 and PB2, so these are outputs,
  // and the default state is a logic low on RS, RW and E.
  PB_LCD_Set_As_Output(15, GPIOA);
  PB_LCD_Set_As_Output(0, GPIOB);
  PB_LCD_Set_As_Output(1, GPIOB);
  PB_LCD_Set_As_Output(2, GPIOB);
  GPIOA->BSRR = 1UL << 15;    // Enable the buffers on the board
  GPIOB->BSRR = 7UL << 16;    // Set other control lines to low
  
  // The four data lines on GPIOD(7:4) are set as outputs by default (using
  // the LCD in four-bit mode, so these are all that's required:
  for (int loop = 4; loop < 8; loop++) {
    PB_LCD_Set_As_Output(loop, GPIOD);
  }
  
  while (PB_LCD_IsBusy());  // Wait until it's ready to talk to me
  
  // Try and kick the four-bit mode back into sync.
  // (If the processor on the discovery board has just been reset, then the LCD 
  // may still be in four-bit mode, as the LCD driver chip is not also reset, and
  // there is no way to reset the LCD under processor control (unfortunately).  
  // If it is in four-bit mode, then the following eight-bit data-write to put it 
  // into four-bit mode will be interpreted as the first half of a four-bit write, 
  // and all writes after that will be out-of-sync.  However, if I toggle E up-and-down 
  // here, then in four-bit mode the eight-bit instruction to go into four-bit mode 
  // will be interpreted as the second half of a write instruction to set the DDRAM 
  // address to zero, which doesn't do any harm at this stage.  If, however, the chip 
  // really was in eight-bit mode (for example it's the first time it's been powered on), 
  // then this toggle of E will be treated as an instruction to clear the display (and 
  // that's fine).)
  GPIOD->ODR = (GPIOD->ODR & 0xFFFFFF00) + 0x01;
  PB_LCD_Microdelay(10000); GPIOB->BSRR = 1UL << 2;
  PB_LCD_Microdelay(10000); GPIOB->BSRR = 1UL << 18;
  GPIOD->ODR = (GPIOD->ODR & 0xFFFFFF00) + 0x28;
  PB_LCD_Microdelay(10000); GPIOB->BSRR = 1UL << 2;
  PB_LCD_Microdelay(10000); GPIOB->BSRR = 1UL << 18;
  
  // Function Set: 4 bit mode, 1/16 duty, 5x8 font, 2 lines
  while (PB_LCD_IsBusy());
  PB_LCD_Write(WRITE_INSTRUCTION, 0x28);
  
  // Display ON/OFF Control: ON
  while (PB_LCD_IsBusy());
  PB_LCD_Write(WRITE_INSTRUCTION, 0x0c);
  
  // Entry Mode Set: Increment
  while (PB_LCD_IsBusy());
  PB_LCD_Write(WRITE_INSTRUCTION, 0x06);
}
void PB_LCD_Clear (void) {
  while (PB_LCD_IsBusy());
  PB_LCD_Write(WRITE_INSTRUCTION, 0x01);
}
void PB_LCD_GoToXY (int x, int y) {
  while (PB_LCD_IsBusy());
  if( y == 0 )
    PB_LCD_Write(WRITE_INSTRUCTION, 0x80 | (x & 0x3F));
  else if( y == 1 )
    PB_LCD_Write(WRITE_INSTRUCTION, 0xC0 | (x & 0x3F));
}
void PB_LCD_WriteChar (char ch) {
  // Write a character to the data register on the LCD:
  while (PB_LCD_IsBusy());
  PB_LCD_Write(WRITE_DATA, ch);
}
void PB_LCD_WriteString (char *s, int maxLength) {
  while(*s && maxLength-- > 0) {
    while (PB_LCD_IsBusy()){}
    PB_LCD_Write(WRITE_DATA, *s++);
  }
}

