#ifndef _SEG7LCD_H_
#define _SEG7LCD_H_

void seg7lcd_init(char *buffer, unsigned char buffer_size);
void seg7lcd_reset(void);
void seg7lcd_clear(void);
void seg7lcd_set_page_num(unsigned char page_num);
void seg7lcd_set_rotate_timer(unsigned char rotate_time);
void seg7lcd_set_scroll_timer(unsigned char scroll_time);
void seg7lcd_set_blink_timer(unsigned char blink_time);
void seg7lcd_enable_blink(unsigned char enabled);
void seg7lcd_enable_scroll(unsigned char enabled);
void seg7lcd_sync(void);
void seg7lcd_draw(void);
void seg7lcd_puts(int page_no, char *str);

#endif//_SEG7LCD_H_
