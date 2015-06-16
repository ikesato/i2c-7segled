#include <p18cxxx.h>
#include <delays.h>
#include "seg7lcd.h"
#include "i2c_slave.h"

#pragma config MCLRE = OFF  // MCLR PIN ENABLE BIT
#pragma config FOSC  = IRC  // Internal clock
#pragma config LVP   = OFF  // Single-Supply ICSP Enable bit
#pragma config WDTEN = OFF  // Watchdog Timer Enable bit

#define T0CNT 4
#define OVERFLOW_MSEC   (8064L)   // Timer overflow frequency [us]
#define ONE_SEC         ((LONG)(1000L * 1000L / OVERFLOW_MSEC))  // one second of WORD value
#define MOVE_TIME       (unsigned char)(ONE_SEC*0.2)

static unsigned long gcounter = 0;     // global counter
static unsigned char timer = 0;
static char lcd_buffer[255];

/**
 * !@brief Interrupt function
 */
void interrupt interrupt_func(void)
{
  // timer interrupt
  if (T0IF == 1) {
    TMR0 = T0CNT;
    T0IF = 0;
    gcounter++;
    if (++timer > MOVE_TIME) {
      seg7lcd_sync();
      timer = 0;
    }
  }

  // I2C interrupt handler
  //i2cs_interrupt();
}



void init(void) {
  OSCCON = 0x52;      // 4MHZ
  TRISA = 0;
  TRISB = 0;
  TRISC = 0;
  PORTA = 0;
  PORTB = 0;
  PORTC = 0;
  ANSELH = 0;
  ANSEL = 0;

  // timer
  // timer cycle => 8064 usec
  // 8064usec is most close to generage 1sec
  // 8064 * 124 = 999,936[usec]
  TMR0ON = 1;
  T08BIT = 1;
  T0CONbits.T0PS = 0b100; // prescaler 1:32
  TMR0 = T0CNT;
  TMR0IP = 1;
  TMR0IE = 1;
  TMR0IF = 0;
  T0CS = 0;
  PSA = 0;
  IPEN = 1;
  GIEH = 1;
  GIEL = 1;

  //i2cs_init(0x20);
}


#if 0
#include "seg7.h"
void main(void) {
  init();
  unsigned char wait = 2;
  seg7_off_all();
  while(1) {
    seg7_put(3, '1');
    Delay1KTCYx(wait);
    seg7_off(3);
    seg7_put(2, '2');
    Delay1KTCYx(wait);
    seg7_off(2);
    seg7_put(1, '3');
    Delay1KTCYx(wait);
    seg7_off(1);
    seg7_put(0, '4');
    Delay1KTCYx(wait);
    seg7_off(0);
  }
}
#endif


void main(void) {
  init();
  seg7lcd_init(lcd_buffer, sizeof(lcd_buffer));
  seg7lcd_clear();
  seg7lcd_puts(0, (char*)"HELLO");
  seg7lcd_enable_move(1);
  while(1) {
    seg7lcd_draw();
  }
}
