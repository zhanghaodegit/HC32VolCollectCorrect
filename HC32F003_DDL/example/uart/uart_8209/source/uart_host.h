#ifndef 	_UART_HOST_H_
#define 	_UART_HOST_H
#include "stdint.h"
void uart_host_init(void);
int host_rs232_send_bytes(uint8_t *bytes,int len);
#define 	HOSTRXBUFSIZE		16
char HostRS232GetChar ( char *ch);
unsigned char HostRS232RXBufferCount (void);
void reset_host_rx_buf(void);

#endif
