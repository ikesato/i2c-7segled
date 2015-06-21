#include <delays.h>
#include "seg7.h"
#include "lcd_buffer.h"

void seg7lcd_init(char *buffer, unsigned char buffer_size) {
  lbuf_init(buffer, buffer_size);
  seg7_off_all();
}

void seg7lcd_reset(void) {
  lbuf_reset();
  seg7_off_all();
}

void seg7lcd_clear(void) {
  lbuf_clear();
  seg7_off_all();
}

void seg7lcd_set_page_num(unsigned char page_num) {
  lbuf_set_page_num(page_num);
}

void seg7lcd_set_rotate_speed(unsigned char rotate_speed) {
  lbuf_set_rotate_speed(rotate_speed);
}

void seg7lcd_set_scroll_speed(unsigned char scroll_speed) {
  lbuf_set_scroll_speed(scroll_speed);
}

void seg7lcd_set_blink_speed(unsigned char blink_speed) {
  lbuf_set_blink_speed(blink_speed);
}

void seg7lcd_enable_blink(unsigned char enabled) {
  lbuf_enable_blink(enabled);
}

void seg7lcd_enable_scroll(unsigned char enabled) {
  lbuf_enable_scroll(enabled);
}

void seg7lcd_sync(void) {
  lbuf_sync();
}

void seg7lcd_draw(void) {
  char *lcd;
  unsigned char page_no = LBUF_LCD_LEN - 1;
  lbuf_draw();
  lcd = lbuf_get();
  for (int i=0; i<LBUF_LCD_LEN; i++, page_no--) {
    seg7_put(page_no, lcd[i]);
    Delay1KTCYx(2);
    seg7_off(page_no);
  }
}

void seg7lcd_puts(int page_no, char *str) {
  lbuf_put(page_no, str);
}

