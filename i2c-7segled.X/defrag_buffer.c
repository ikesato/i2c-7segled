#include <stdlib.h>
#include <string.h>
#include "defrag_buffer.h"

struct DBuf {
  char *buffer;
  SIZET buffer_size;
} dbuf;

struct DbufPage {
  SIZET offset;
  SIZET size; // string length, not includes '\0'
} dbuf_pages[DBUF_PAGE_SIZE];

void dbuf_init(char *buffer, SIZET buffer_size) {
  dbuf.buffer = buffer;
  dbuf.buffer_size = buffer_size;
  dbuf_clear();
}

void dbuf_clear() {
  memset(dbuf_pages, 0, sizeof(dbuf_pages));
  memset(dbuf.buffer, 0, dbuf.buffer_size);
}

char dbuf_put(unsigned char page_no, char* buffer) {
  SIZET slen = strlen(buffer);
  SIZET pos;
  SIZET i;
  SIZET diff;
  SIZET next_pos;
  SIZET ret = DBUF_SUCCESS;
  if (page_no >= DBUF_PAGE_SIZE) {
    return DBUF_ERR_BUFFER_OVERFLOW;
  }

  pos = dbuf_pages[page_no].offset;
  if (pos + slen >= dbuf.buffer_size) {
    ret = DBUF_ERR_BUFFER_OVERFLOW;
    dbuf.buffer[dbuf.buffer_size - 1] = '\0';
    if (pos >= dbuf.buffer_size) {
      return ret;
    }
    slen = dbuf.buffer_size - pos - 1;
  }
  next_pos = dbuf_pages[page_no].offset + slen + 1;
  if (page_no < DBUF_PAGE_SIZE-1) {
    SIZET next_page = dbuf_pages[page_no+1].offset;
    if (next_page < next_pos) {
      diff = next_pos - next_page;
      // scroll right
      struct DbufPage *plast = &dbuf_pages[DBUF_PAGE_SIZE-1];
      pos = plast->offset + plast->size;
      for (; (SSIZET)next_page <= (SSIZET)pos; pos--) {
        if (pos + diff > dbuf.buffer_size) {
          if (dbuf.buffer[pos] != '\0')
            ret = DBUF_ERR_BUFFER_OVERFLOW;
        } else {
          dbuf.buffer[pos + diff] = dbuf.buffer[pos];
        }
      }
      for (i=page_no+1; i<DBUF_PAGE_SIZE; i++) {
        dbuf_pages[i].offset += diff;
      }
    } else if (next_page > next_pos) {
      // scroll left
      diff = next_page - next_pos;
      struct DbufPage *pnext = &dbuf_pages[page_no+1];
      struct DbufPage *plast = &dbuf_pages[DBUF_PAGE_SIZE-1];
      for (pos = pnext->offset; pos <= plast->offset + plast->size; pos++) {
        if (pos < dbuf.buffer_size) {
          dbuf.buffer[pos - diff] = dbuf.buffer[pos];
        }
      }
      for (i=page_no+1; i<DBUF_PAGE_SIZE; i++) {
        dbuf_pages[i].offset -= diff;
      }
    }
  }
  dbuf_pages[page_no].size = slen;
  pos = dbuf_pages[page_no].offset;
  for (i=0; i<=slen; i++) {
    if (pos + i < dbuf.buffer_size - 1) {
      dbuf.buffer[pos+i] = buffer[i];
    }
  }

  for (i=page_no+1; i<DBUF_PAGE_SIZE; i++) {
    if (dbuf_pages[page_no].offset == dbuf_pages[i].offset) {
      dbuf_pages[i].offset = next_pos;
    }
  }
  return ret;
}

char* dbuf_get(unsigned char page_no) {
  return &dbuf.buffer[dbuf_pages[page_no].offset];
}
