#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcd_buffer.h"
#include "defrag_buffer.h"

struct Lbuf {
  char page_num;
  char rotate_time;
  char rotate_timer;
  char move_enabled;
  char move_time;
  char move_timer;
  char move_pos;
  char blink_enabled;
  char blink_time;
  char blink_timer;
  char blink_on;
  char current_page;
  char tmp_buffer[LBUF_TMP_BUFFER_SIZE];
  char lcd[LBUF_LCD_LEN+1];
};

static struct Lbuf lbuf;

static char *lbuf_format_str(char *in, char *out, unsigned char out_size);

void lbuf_init(char *buffer, unsigned char buffer_size) {
  memset(&lbuf, 0, sizeof(lbuf));
  dbuf_init(buffer, buffer_size);
}

void lbuf_reset(void) {
  memset(&lbuf, 0, sizeof(lbuf));
  dbuf_clear();
}

void lbuf_clear(void) {
  dbuf_clear();
}

void lbuf_set_page_num(unsigned char page_num) {
  lbuf.page_num = page_num;
}

void lbuf_set_rotate_timer(unsigned char rotate_time) {
  lbuf.rotate_time = rotate_time;
}

void lbuf_set_move_timer(unsigned char move_time) {
  lbuf.move_time = move_time;
}

void lbuf_set_blink_timer(unsigned char blink_time) {
  lbuf.blink_time = blink_time;
}

void lbuf_enable_blink(unsigned char enabled) {
  lbuf.blink_enabled = enabled;
}

void lbuf_enable_move(unsigned char enabled) {
  lbuf.move_enabled = enabled;
}

void lbuf_sync(void) {
  if (lbuf.move_enabled) {
    if (lbuf.move_timer >= lbuf.move_time) {
      lbuf.move_timer = 0;
      lbuf.move_pos++;
    } else {
      lbuf.move_timer++;
    }
  }
  if (lbuf.page_num > 1) {
    if (lbuf.rotate_timer >= lbuf.rotate_time) {
      lbuf.rotate_timer = 0;
      lbuf.current_page++;
      if (lbuf.current_page >= lbuf.page_num) {
        lbuf.current_page = 0;
      }
      lbuf.move_timer = 0;
      lbuf.move_pos = -1;
    } else {
      lbuf.rotate_timer++;
    }
  } else {
      lbuf.current_page = 0;
  }
  if (lbuf.blink_enabled) {
    if (lbuf.blink_timer >= lbuf.blink_time) {
      lbuf.blink_timer = 0;
      lbuf.blink_on = !lbuf.blink_on;
    } else {
      lbuf.blink_timer++;
    }
  }
}

void lbuf_put(int page_no, char *str) {
  str = lbuf_format_str(str, lbuf.tmp_buffer, sizeof(lbuf.tmp_buffer));
  dbuf_put(page_no, str);
}

char *lbuf_get(void) {
  return lbuf.lcd;
}

void lbuf_draw(void) {
  char *str = dbuf_get(lbuf.current_page);
  char slen = strlen(str);
  int i,j;
  int pos;
  if (lbuf.blink_enabled && !lbuf.blink_on) {
    for (i=0; i<LBUF_LCD_LEN; i++) {
      lbuf.lcd[i] = ' ';
    }
    return;
  }

  if (lbuf.move_enabled) {
    pos = -(LBUF_LCD_LEN-1);
    pos += lbuf.move_pos;
    if (slen < pos) {
      pos -= lbuf.move_pos;
      lbuf.move_pos = 0;
    }
  } else {
    if (slen > LBUF_LCD_LEN)
      pos = 0;
    else
      pos = -(LBUF_LCD_LEN - slen);
  }
  for (i=0; i<LBUF_LCD_LEN; i++) {
    j = i + pos;
    if (0 <= j && j < slen)
      lbuf.lcd[i] = str[j];
    else
      lbuf.lcd[i] = ' ';
  }
}

static char *lbuf_format_str(char *in, char *out, unsigned char out_size) {
  unsigned char i,j;
  for (i=j=0; in[i] != '\0'; i++, j++) {
    if (in[i] == '.') {
      if (i > 0 && in[i-1] != '.') {
        j--;
      }
      if (j < out_size-1)
        out[j] |= 0x80;
    } else {
      if (j < out_size-1)
        out[j] = in[i];
    }
  }
  if (j < out_size)
    out[j] = '\0';
  else
    out[out_size-1] = '\0';
  return out;
}

#ifdef DEBUG
void lbuf_print(void) {
  unsigned char i;
  unsigned char c;
  printf("|");
  for (i=0; i<LBUF_LCD_LEN; i++) {
    c = (lbuf.lcd[i] & 0x80 ? '.' : ' ');
    printf("%c%c", lbuf.lcd[i] & 0x7f, c);
  }
  printf("|");
  printf("\r");
  printf("\n");
  fflush(stdout);
}
#endif
