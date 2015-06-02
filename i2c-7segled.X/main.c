#include <p18cxxx.h>
#include <delays.h>

#pragma config MCLRE=OFF	//MCLR PIN ENABLE BIT
#pragma config FOSC = IRC	// 内部クロック
#pragma config LVP =   OFF	// Single-Supply ICSP Enable bit
#pragma config WDTEN = OFF	// Watchdog Timer Enable bit


typedef struct {
    unsigned port : 2; // 0:Port-A 1:Port-B 2:Port-C
    unsigned num : 4;  // Port Number 0-7
} PortMap;

PortMap port_font_map[] = {
    {0, 5}, // A
    {2, 5}, // B
    {2, 3}, // C
    {2, 7}, // D
    {1, 7}, // E
    {0, 4}, // F
    {2, 4}, // G
    {2, 6}, // DP
};

const PortMap cathodes[] = {
    {2, 0},
    {2, 1},
    {1, 5},
    {2, 2},
};

volatile unsigned char * const ports[] = {
    &PORTA, &PORTB, &PORTC
};

//unsigned char fonts[] = {
//  0x
//};

void segment_off(unsigned char place) {
    *ports[cathodes[place].port] &= ~(1U<<cathodes[place].num);
}

void segment_on(unsigned char place, unsigned char bits) {
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

// 1MHz : 1/1,000,000=1us
// 4サイクル必要なので
// 1us*4 = 4us
// 1*1000*1000/4us = 250,000 = 250K
void main(void) {
    OSCCON = 0x52;      // 4MHZ
//	REFCON0 = 0xB0;//FVR*4
//	INTCON2bits.RABPU = 1;

//	UCONbits.USBEN = 0;
//	IOCAbits.IOCA0 = 1;
//	IOCAbits.IOCA1 = 1;

    // ポートA,B,Cを出力に設定する
    TRISA = 0;
    TRISB = 0;
    TRISC = 0;
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
	ANSELH = 0;
	ANSEL = 0;

//    //RC0 = 1;
//    //RC1 = RC2 = RB5 = 1;
//    RC2 = 1;
//
//    while(1) {
//        RA5 = RC5 = RC3 = RC7 = RB7 = RA4 = RC4 = RC6 = 1;
//        Delay10KTCYx(100);
//        RA5 = RC5 = RC3 = RC7 = RB7 = RA4 = RC4 = RC6 = 0;
//        Delay10KTCYx(100);
//    }
    while(1) {
        segment_off(0);
        Delay10KTCYx(100);
        //segment_on(0, 0x3F); // 0
        //segment_on(0, 0x06); // 1
        segment_on(0, 0x5B); // 2
        //segment_on(0, 0x4F); // 3
        //segment_on(0, 0x66); // 4
        //segment_on(0, 0x6D); // 5
        //segment_on(0, 0x7D); // 6
        //segment_on(0, 0x27); // 7
        //segment_on(0, 0x7F); // 8
        //segment_on(0, 0x6F); // 9
        Delay10KTCYx(100);
    }
}
