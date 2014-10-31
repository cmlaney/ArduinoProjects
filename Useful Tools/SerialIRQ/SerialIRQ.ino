
#include "queue.h"

/**
 * UART0 Rx Queue
 **/
QUEUE_NEW(uart0RxQueue,32);

/**
 * Setup the serial port to the specified baud rate.
 **/
void uartSetup(uint32_t baud){
    uint32_t ubbr = ((8000000/8)/baud)-1;

    UBRR0H=(ubbr>>8)&0xff ;UBRR0L=ubbr & 0xff;
    //Use double speed here
    UCSR0A = (1<<U2X0);
    //Enable tx and rx
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    //Enable receive interrupt
    UCSR0B |= (1<<RXCIE0);
    //Set frame format to 8 data, 1 stop bits
    UCSR0C = (3<<UCSZ00);

    //Set tx to output;
    DDRD |= 1 << 1;
}

/**
 * Interrupt for receving on uart0.  Puts the received data in the
 * uart rx queue.  If the queue is full the oldest data is discarded.
 **/
SIGNAL(USART_RX_vect){
  char t;
  if(!(UCSR0A & (1<<RXC0))) return;
  if(QUEUE_ISFULL(uart0RxQueue)) t = DEQUEUE(uart0RxQueue);
  ENQUEUE(uart0RxQueue,UDR0);
}

/**
 * Non-blocking uart receive.  Returns 1 iff byte returned in c
 **/
int8_t uartReceiveByte(char *c){
/*
  if(!(UCSR0A & (1<<RXC0))) return 0;
    *c = UDR0;
    return 1;
*/
   if(QUEUE_ISEMPTY(uart0RxQueue)) return 0;
   *c = DEQUEUE(uart0RxQueue);
   return 1;
}

/**
 * Send one byte over the serial port
 **/
void uartSendByte(char c){
  while(!(UCSR0A & (1<<UDRE0)));
  UDR0 = c; 
}

void setup() {
  // put your setup code here, to run once:
  uartSetup(57600);
}

void loop() {
  char c;
  // put your main code here, to run repeatedly: 
 if(uartReceiveByte(&c) == 1){
  uartSendByte('g'); uartSendByte('o'); uartSendByte('t'); uartSendByte(':'); 
  uartSendByte(' ');
  uartSendByte(c);
  uartSendByte('\n');
 }
}
