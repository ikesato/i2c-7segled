#ifndef _DEFRAG_BUFFER_H_
#define _DEFRAG_BUFFER_H_

// Memory buffer for string character.
// Buffer always ends '\0'.
// This buffer was always defragmented.

// typedef
typedef unsigned char SIZET;
typedef signed char SSIZET;

// defines
#define DBUF_PAGE_SIZE  8    // max page size

// return code
#define DBUF_SUCCESS                    0
#define DBUF_ERR_BUFFER_OVERFLOW        -1

void dbuf_init(char *buffer, SIZET buffer_size);
void dbuf_clear();
char dbuf_put(unsigned char page_no, char* buffer);
char* dbuf_get(unsigned char page_no);

#endif//_DEFRAG_BUFFER_H_
