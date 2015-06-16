#ifndef _LCD_BUFFER_H_
#define _LCD_BUFFER_H_

#define LBUF_LCD_LEN          4
#define LBUF_TMP_BUFFER_SIZE  100
#define LBUF_PAGE_NUM         8

void lbuf_init(char *buffer, unsigned char buffer_size);
void lbuf_reset(void);
void lbuf_set_page_num(unsigned char page_num);
void lbuf_set_rotate_timer(unsigned char rotate_time);
void lbuf_set_move_timer(unsigned char move_time);
void lbuf_set_blink_timer(unsigned char blink_time);
void lbuf_enable_blink(unsigned char enabled);
void lbuf_enable_move(unsigned char enabled);
void lbuf_sync(void);
void lbuf_put(int page_no, char *str);
char *lbuf_get(void);
void lbuf_draw(void);
void lbuf_print(void);

#endif//_LCD_BUFFER_H_
