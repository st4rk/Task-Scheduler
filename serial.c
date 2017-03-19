#include "serial.h"

void usart_start() {

 /**
  * Configure the baud rate prescale
  */
 UBRR0H  = (unsigned char)(USART_PRESCALE >> 8);
 UBRR0L  = (unsigned char)(USART_PRESCALE & 0xFF);
 
 /**
  * Enable USART hardware to recv and send data
  */
 UCSR0B |= (1<<RXEN0)  | (1<<TXEN0);
 
 /**
  * This is the packet format (stop bit, data size etc)
  */
 UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);


}

void usart_send(char data) {
	while (!(UCSR0A & (1 << UDRE0))) {}

	UDR0 = data;
}

void usart_print(char *data, unsigned char len) {
	for (int i = 0; i < len; i++) {
		usart_send(data[i]);
	}
}

char usart_recv() {
	while (!(UCSR0A & (1 << RXC0))) {}

	return UDR0;
}