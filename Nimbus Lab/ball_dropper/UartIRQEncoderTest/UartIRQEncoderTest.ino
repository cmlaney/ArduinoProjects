
#include "queue.h"
#include <stdlib.h>

#define MOTPIN1 8
#define MOTPIN2 9

#define ENCODE1 2
#define ENCODE2 3
#define FORWARD 1
#define BACKWARD 0

#define LED 13

volatile int counter = 0;
volatile int counterLast = -1;
volatile int prevState = 0;
volatile int currentState = 0;
volatile int fault = 0;

long lastPrint = 0;

/**
 * UART0 Rx Queue
 **/
QUEUE_NEW(uart0RxQueue,32);

/**
 * Setup the serial port to the specified baud rate.
 **/
void uartSetup(uint8_t clock, uint32_t baud){
    uint32_t ubbr = (((clock * 1000000)/8)/baud)-1;

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
  cli();
  char t;
  if(!(UCSR0A & (1<<RXC0))) return;
  if(QUEUE_ISFULL(uart0RxQueue)) t = DEQUEUE(uart0RxQueue);
  ENQUEUE(uart0RxQueue,UDR0);
  sei();
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
  uartSetup(8, 57600);
  
  pinMode(MOTPIN1, OUTPUT);
  pinMode(MOTPIN2, OUTPUT);
  
  digitalWrite(MOTPIN2, LOW);
  
  pinMode(ENCODE1, INPUT);
  pinMode(ENCODE2, INPUT);
  
  attachInterrupt(0, encode, CHANGE);
  attachInterrupt(1, encode, CHANGE);
  
  digitalWrite(MOTPIN1, HIGH);
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
 
 if(millis() - lastPrint > 1000) {
   cli();
   printEncodeInfo();
   sei();
   lastPrint = millis();
 }
 
 if(c=='z') {
   digitalWrite(MOTPIN1, !digitalRead(MOTPIN1));
 }
}

void printEncodeInfo() {
  char buffer[10];
  serialWrite("Counter: ");
  itoa(counter, buffer, 10);
  serialWrite(buffer);
  serialWrite("\nFaults: ");
  itoa(fault, buffer, 10);
  serialWrite(buffer);
  serialWrite("\n\n");
}

void serialWrite(char s[]) {
  int i;
  int j = 0;
  while(s[j] != NULL) {
    j++;
  }
  for(i=0; i<=j; i++) {
    uartSendByte(s[i]);
  }
}


void encode() {
  currentState = getState(digitalRead(ENCODE1), digitalRead(ENCODE2));
  
  if(((prevState +1)%4) == currentState) {
    counter++;
  } else if (((prevState -1 +4)%4) == currentState) {
    counter--;
  } else {
    fault++;
  }
  prevState = currentState;
}

int getState(uint8_t A, uint8_t B) {
  if(A) {
    if(B) {
      return 1;
    } else {
      return 2;
    }
  } else {
    if(B) {
      return 0;
    } else {
      return 3;
    }
  }
}
