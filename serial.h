#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <avr/io.h>


#define USART_PRESCALE (((F_CPU/(9600 * 16UL))) - 1)


#ifdef __cplusplus
	extern "C" {
#endif
void usart_start();
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
	extern "C" {
#endif
void usart_send(char data);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
	extern "C" {
#endif
void usart_print(char *data, unsigned char len);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
	extern "C" {
#endif
char usart_recv();
#ifdef __cplusplus
}
#endif

#endif