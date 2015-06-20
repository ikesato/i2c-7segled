#include <p18cxxx.h>
#include <htc.h>
#include <string.h>
#include "i2c_slave.h"

#define SEND_BUFF_LEN 1                  // Send buffer size
#define RECV_BUFF_LEN 200                // Receive buffer size//TODO: extend buffer size

unsigned char recv_buff[RECV_BUFF_LEN];  // Send buffer
unsigned char send_buff[SEND_BUFF_LEN];  // Receive bufer

unsigned int  recv_size;     // Receive data size
unsigned char *sdtp;         // A Pointer of send buffer
unsigned char *rdtp;         // A Pointer of receive buffer


/**
 * !@brief Interrupt function for I2C slave
 */
void i2cs_interrupt(void)
{
  char x;

  if (SSPIF == 1) {
    if (SSPSTATbits.R_nW == 0) {
      // Write Command from Master (Slave is recieving data)
      if (SSPSTATbits.D_nA == 0) {
        rdtp = recv_buff;
        x = SSPBUF;        // Skip address buffer
        recv_size = 0;
      } else {
        *rdtp = SSPBUF;    // Read data
        rdtp++;
        recv_size++;
      }
      SSPIF = 0;             // Clear interrupt flag
      SSPCON1bits.CKP = 1;   // Release SCL
    } else {
      // Read Command from Master (Slave is sending data)
      if (SSPSTATbits.BF == 1) {
        sdtp = send_buff;
        x = SSPBUF;        // Skip address buffer
        while(SSPCON1bits.CKP | SSPSTATbits.BF);
        SSPBUF = *sdtp++;  // Set sending data
        SSPIF = 0;
        SSPCON1bits.CKP = 1;
      } else {
        if (SSPCON2bits.ACKSTAT==0) {
          // ACK from Master
          while((SSPCON1bits.CKP)|(SSPSTATbits.BF));
          SSPBUF = *sdtp++;
          SSPIF = 0;
          SSPCON1bits.CKP = 1;
        } else {
          // NACK from Master
          SSPIF = 0;
        }
      }
    }
  }

  // MSSP Bus Collision Interrupt
  if (BCLIF == 1) {
    BCLIF = 0;   // Clear only
  }
}


/**
 * !@brief Initilize function for I2C slave
 *
 * @param[in] address Slave address
 */
void i2cs_init(int address)
{
  RABPU = 0;                  // Use inernal pull up register
  WPUB4 = 1;                  // pull up for SDA
  WPUB6 = 1;                  // pull up for SCL
  TRISB4 = 1;                 // SDA as input
  TRISB6 = 1;                 // SCL as input
  SSPSTAT= 0b10000000;        // normal speed, 100kHz.
  SSPCON1= 0b00100110;        // Slave mode, Use SDA/SCL, 7bit address
  SSPCON2bits.SEN  = 1;       // Enable Clock stretching
  SSPADD = address<<1;        // Set slave address
  SSPMSK = 0b11111110;        // Set address mask
  SSPIE  = 1;                 // Enable interrupt
  BCLIE  = 1;                 // Enable MSSP(I2C) Bus Collision interrupt
  PEIE   = 1;                 // Enable Peripheral interrupt
  GIE    = 1;                 // Enable Global interrupt
  SSPIF  = 0;                 // Clear SSP(I2C) interrupt flag
  BCLIF  = 0;                 // Clear MSSP(I2C) interrupt flag
  recv_size = 0;
}


/**
 * !@brief I2C process function
 *
 * This function should call from main loop
 * @return Receive size of Writing data from Master. 0 means no data received, other means receieve data size.
 */
int i2cs_proc()
{
  int ans;

  ans = 0;
  if (recv_size != 0) {
    if ((SSPSTATbits.S==0) && (SSPSTATbits.P==1)) { // Detected stop condition
      ans = recv_size;
      recv_size = 0;
    }
  }
  return(ans);
}

/**
 * !@brief Read received data
 *
 * @param[out] buff An address of buffer for output. If buff is NULL, return a pointer of receive buff.
 * @param[in] size A size of buff [Byte].
 * @return If buff is not NULL, return buff address. Otherwise buff is NULL, Return a pointer of receive buffer.
 */
char *i2cs_read_recv_buff(char *buff, unsigned char size)
{
  if (buff != NULL) {
    memcpy(buff, recv_buff, size);
    return buff;
  } else {
    return recv_buff;
  }
}

/**
 * !@brief Set send data
 *
 * @param[in] buff An address of buffer for sending.
 * @param[in] size A size of buff [Byte].
 */
void i2cs_set_send_buff(char *buff, unsigned char size)
{
  memcpy(send_buff, buff, size);
}

/**
 * !@brief Get send data buffer
 *
 * @return A buffer of send data.
 */
char *i2cs_get_send_buff(void)
{
  return send_buff;
}
