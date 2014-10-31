#include "Arduino.h"
#include "ATTiny_sleep.h"

void ATTiny_sleep::goToSleep(void)
{
    GIMSK |= _BV(INT0);                       //enable INT0
    MCUCR &= ~(_BV(ISC01) | _BV(ISC00));      //INT0 on low level
    ACSR |= _BV(ACD);                         //disable the analog comparator
    ADCSRA &= ~_BV(ADEN);                     //disable ADC
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    //turn off the brown-out detector.
    //must have an ATtiny45 or ATtiny85 rev C or later for software to be able to disable the BOD.
    //current while sleeping will be <0.5uA if BOD is disabled, <25uA if not.
    cli();
    mcucr1 = MCUCR | _BV(BODS) | _BV(BODSE);  //turn off the brown-out detector
    mcucr2 = mcucr1 & ~_BV(BODSE);
    MCUCR = mcucr1;
    MCUCR = mcucr2;
    sei();                         //ensure interrupts enabled so we can wake up again
    sleep_cpu();                   //go to sleep
    cli();                         //wake up here, disable interrupts
    GIMSK = 0x00;                  //disable INT0
    sleep_disable();               
    sei();                         //enable interrupts again (but INT0 is disabled from above)
}