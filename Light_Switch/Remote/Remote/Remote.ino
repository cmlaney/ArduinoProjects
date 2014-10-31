//This code controls a small, attiny85 based 433MHz remote for the lightswitch code using a 433MHz receiver.


#include <avr/sleep.h>
#include <Manchester.h>

#define BODS 7                     //BOD Sleep bit in MCUCR
#define BODSE 2                    //BOD Sleep enable bit in MCUCR
#define TX_PIN 1  //pin where your transmitter is connected
#define BUTTON 12
#define SENDER_ID 169

int transmit_data = 12;

void setup(void)
{
    //to minimize power consumption while sleeping, output pins must not source
    //or sink any current. input pins must have a defined level; a good way to
    //ensure this is to enable the internal pullup resistors.
 
    for (byte i=0; i<5; i++) {     //make all pins inputs with pullups enabled
        if(i!=1) {
        pinMode(i, INPUT);
        digitalWrite(i, HIGH);
        }
    }
    man.workAround1MhzTinyCore(); //add this in order for transmitter to work with 1Mhz Attiny85/84
    man.setupTransmit(TX_PIN, MAN_1200);
}
 
void loop(void)
{
    goToSleep();
 
    int i;
    for(i=0; i<2; i++) {
      man.transmit(2141);
      delay(100);
    }
    
    delay(3000);
}
 
void goToSleep(void)
{
    byte adcsra, mcucr1, mcucr2;
 
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    MCUCR &= ~(_BV(ISC01) | _BV(ISC00));      //INT0 on low level
    GIMSK |= _BV(INT0);                       //enable INT0
    adcsra = ADCSRA;                          //save ADCSRA
    ADCSRA &= ~_BV(ADEN);                     //disable ADC
    cli();                                    //stop interrupts to ensure the BOD timed sequence executes as required
    mcucr1 = MCUCR | _BV(BODS) | _BV(BODSE);  //turn off the brown-out detector
    mcucr2 = mcucr1 & ~_BV(BODSE);            //if the MCU does not have BOD disable capability,
    MCUCR = mcucr1;                           //  this code has no effect
    MCUCR = mcucr2;
    sei();                                    //ensure interrupts enabled so we can wake up again
    sleep_cpu();                              //go to sleep
    sleep_disable();                          //wake up here
    ADCSRA = adcsra;                          //restore ADCSRA
}
 
//external interrupt 0 wakes the MCU
ISR(INT0_vect)
{
    GIMSK = 0;                     //disable external interrupts (only need one to wake up)
}
