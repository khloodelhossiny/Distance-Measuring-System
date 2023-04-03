/*
 * Distance_Measuring_Sys.c
 *
 * Created: 2/4/2023 11:37:17 AM
 * Author : Ve-ga
 */ 

#include "lcd.h"

uint32_t timer1_init_input_capture_usec(void);

#define CLOCK_CYCLE_USEC (1000000.0/F_CPU)
#define MAX_VALUE_TIMER1 65537
#define Timer1_Enable_interrupt SETBIT(TIMSK,TOIE1);SETBIT(SREG, 7);
#define LED_RED_ON SETBIT(PORTD,0);
#define LED_RED_OFF CLRBIT(PORTD,0);

#define LED_YELLOW_ON SETBIT(PORTD,1);
#define LED_YELLOW_OFF CLRBIT(PORTD,1);
uint32_t ovf_num = 0;

int main(void)
{
	LCD_init();
	Timer1_Enable_interrupt;
	SETBIT(DDRD,7);      // D7 as output
	SETBIT(DDRD,0);      // D7 as output
	SETBIT(DDRD,1);      // D7 as output
	
	SETBIT(DDRB,0);
	SETBIT(PORTB,0);
	
	uint32_t Time, distance = 0;
    while (1) 
    {			
		
		SETBIT(PORTD,7);   // given signal to ultrasonic to work now
		_delay_ms(50);
		CLRBIT(PORTD,7);
		Time = timer1_init_input_capture_usec();	
 		distance = (Time * 34600) / (F_CPU * 2) ;
		LCD_write_command(1);
		if(distance > 80){
			LED_RED_OFF;
			LED_YELLOW_ON
			LCD_write_string("No Object");	
			CLRBIT(PORTB,0);  // Buzzer
		}
		else{
			SETBIT(PORTB,0);
			LED_YELLOW_OFF;
			LED_RED_ON;
			LCD_write_string("Distance = ");
			LCD_write_number(distance);
			LCD_write_string(" cm");
		}
		_delay_ms(500);
    }
}
ISR(TIMER1_OVF_vect){
	ovf_num++;
}
uint32_t timer1_init_input_capture_usec(void){
	uint32_t t1, t2; 	
	ovf_num = 0;
	TCCR1A = 0;
	TCCR1B |= 1<<CS00 | 1<<ICES1;    // detect rising edge
	TIFR |= 1<<ICF1;                 // clear ICU flag bit
	while(READBIT(TIFR,ICF1) == 0 ); // wait until first rising edge detect
	t1 = ICR1;                      // store value of first rising edge
	CLRBIT(TCCR1B,ICES1);           // detect failing edge
	TIFR |= 1<<ICF1;                // clear IC flag
	while(READBIT(TIFR,ICF1) == 0); // wait until first falling edge detect
	t2 = ICR1;                      // store value of first falling edge
	TCCR1B = 0;                     // stop timer 1
	TIFR |= 1<<ICF1;                // clear IC flag
	ICR1 = 0;
	TCNT1 = 0;
	 
	return ((ovf_num * MAX_VALUE_TIMER1)  + (t2 - t1));
}