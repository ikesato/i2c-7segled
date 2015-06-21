#include <p18cxxx.h>
#include <delays.h>

#pragma config MCLRE=OFF	// MCLR PIN ENABLE BIT
#pragma config FOSC = IRC	// Internal clock
#pragma config LVP =   OFF	// Single-Supply ICSP Enable bit
#pragma config WDTEN = OFF	// Watchdog Timer Enable bit


typedef struct {
  unsigned port : 2; // 0:Port-A 1:Port-B 2:Port-C
  unsigned num : 4;  // Port Number 0-7
} PortMap;

static PortMap port_font_map[] = {
  {0, 5}, // A
  {2, 5}, // B
  {2, 3}, // C
  {2, 7}, // D
  {1, 7}, // E
  {0, 4}, // F
  {2, 4}, // G
  {2, 6}, // DP
};

static const PortMap cathodes[] = {
  {2, 0},
  {2, 1},
  {1, 5},
  {2, 2},
};

static volatile unsigned char * const ports[] = {
  &PORTA, &PORTB, &PORTC
};

static unsigned char font_digits[] = {
  // 0     1     2     3     4     5     6     7     8     9
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x6F
};

// *  switch case
// -- most difficult
// -  more iffy
static unsigned char font_alphabet_upper[] = {
  //A  B-   C    d*   E    F    G    H    I    J    K--  L    M--  N-   O
  0x77,0x7F,0x39,0x5E,0x79,0x71,0x3D,0x76,0x06,0x1E,0x7A,0x38,0x15,0x37,0x3F,

  //P  q*   R-   S    T-   U    V-   W--  X-   y*   Z
  0x73,0x67,0x33,0x6D,0x07,0x3E,0x3E,0x7E,0x76,0x6E,0x5B
};

static unsigned char font_alphabet_lower[] = {
  //a  b    c    d    e    F*   g    h    i-   j-   k--  l    m--  n    o
  0x5F,0x7C,0x58,0x5E,0x7B,0x71,0x6F,0x74,0x04,0x0E,0x7A,0x30,0x14,0x54,0x5C,

  //P* q    r    S*   t    u    v-   w--  X*   y    Z*
  0x73,0x67,0x50,0x6D,0x78,0x1C,0x1c,0x2A,0x76,0x6E,0x5B
};

void seg7_off(unsigned char place) {
  *ports[cathodes[place].port] &= ~(1U<<cathodes[place].num);
}

void seg7_off_all() {
  for (unsigned char i=0; i<4; i++) {
    seg7_off(i);
  }
}

void seg7_on(unsigned char place, unsigned char bits) {
  unsigned char i;
  unsigned char port_hi[3] = {0,0,0};
  unsigned char port_lo[3] = {0,0,0};
  unsigned char b;
  for (i=0; i<8; i++) {
    b = (1 << port_font_map[i].num);
    if (bits & (1 << i)) {
      port_hi[port_font_map[i].port] |= b;
    } else {
      port_lo[port_font_map[i].port] |= b;
    }
  }
  for (i=0; i<3; i++) {
    *ports[i] |= port_hi[i];
    *ports[i] &= ~port_lo[i];
  }
  *ports[cathodes[place].port] |= (1<<cathodes[place].num);
}

void seg7_put(unsigned char place, char c) {
  unsigned char dot = c & 0x80;
  c &= 0x7f;
  if (c == ' ' && dot == 0) {
    seg7_off(place);
  } else if ('0' <= c && c <= '9') {
    seg7_on(place, font_digits[c - '0'] | dot);
  } else if ('A' <= c && c <= 'Z') {
    seg7_on(place, font_alphabet_upper[c - 'A'] | dot);
  } else if ('a' <= c && c <= 'z') {
    seg7_on(place, font_alphabet_lower[c - 'a'] | dot);
  } else if (dot == 0x80) {
    seg7_on(place, dot);
  }
}
