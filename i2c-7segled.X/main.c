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
#define BASE_TIME       (unsigned char)(ONE_SEC*0.1)

static unsigned long gcounter = 0;     // global counter
static unsigned char timer = 0;
static char lcd_buffer[255];
static char current_page = 0;

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
    if (++timer > BASE_TIME) {
      seg7lcd_sync();
      timer = 0;
    }
  }

  // I2C interrupt handler
  i2cs_interrupt();
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

  i2cs_init(0x70);
  seg7lcd_init(lcd_buffer, sizeof(lcd_buffer));
  seg7lcd_clear();
}


void main(void) {
  int ans;
  char send_data;

  init();

  while(1) {
    ans = i2cs_proc();
    if (ans != 0) {
      char *recv_buff = i2cs_read_recv_buff(NULL, 0);
      switch(recv_buff[0]) {
      case 0x00:
        seg7lcd_reset();
        break;
      case 0x01:
        seg7lcd_clear();
        break;
      case 0x02:
        seg7lcd_set_page_num(recv_buff[1]);
        break;
      case 0x03:
        seg7lcd_set_scroll_speed(recv_buff[1]);
        break;
      case 0x04:
        seg7lcd_set_flip_speed(recv_buff[1]);
        break;
      case 0x05:
        seg7lcd_set_blink_speed(recv_buff[1]);
        break;
      case 0x06:
        seg7lcd_enable_scroll(recv_buff[1]);
        break;
      case 0x07:
        seg7lcd_enable_blink(recv_buff[1]);
        break;
      case 0x10:
        current_page = recv_buff[1];
        break;
      case 0x11:
        seg7lcd_puts(current_page, recv_buff);
        break;
      }
    }

    seg7lcd_draw();
  }
}
