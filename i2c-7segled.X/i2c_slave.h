#ifndef _I2C_SLAVE_H_
#define _I2C_SLAVE_H_

void i2cs_interrupt(void);
void i2cs_init(int address);
int i2cs_proc(void);
char *i2cs_read_recv_buff(char *buff, unsigned char size);
void i2cs_set_send_buff(char *buff, unsigned char size);
char *i2cs_get_send_buff(void);

#endif
